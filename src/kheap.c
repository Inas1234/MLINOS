#include "./kheap.h"
#include "./paging.h"
#include "./pmm.h"
#include "../stdlib/stdio.h"

#define KHEAP_BASE  0xC1000000u
#define KHEAP_LIMIT (16u * 1024u * 1024u)
#define KHEAP_INIT_PAGES  4u

#define HEAP_MAGIC 0xC0FFEE11u
#define ALIGN      8u

extern page_directory_t *current_directory;

typedef struct block_header {
    unsigned int size;           // payload size (bytes)
    unsigned int magic;          // HEAP_MAGIC (integrity)
    unsigned char free;          // 1 = free, 0 = used
    struct block_header *next;   // next block in list
} block_header;

static unsigned char *heap_base = (unsigned char *)KHEAP_BASE;
static unsigned char *heap_mapped_end = (unsigned char *)KHEAP_BASE; // end of mapped VA range
static block_header *free_list = 0; // head of block list (free/used mixed)

// ---- Helpers ----
static inline unsigned int align_up(unsigned int x, unsigned int a) {
    unsigned int r = (x + (a - 1)) & ~(a - 1);
    return r;
}

static void map_one_page(void *va) {
    void *frame = pmm_alloc();
    if (!frame) {
        kprintf("kheap: out of physical frames!\n");
        asm volatile("cli; hlt");
    }
    // zero the new page to avoid leaking data into users
    // We zero after mapping; but we need a temporary identity mapping or
    // just zero via the virtual address once mapped. We'll zero after map_page.
    map_page(va, frame, current_directory);
}

static void ensure_mapped_bytes(unsigned int more_bytes) {
    unsigned int need = (unsigned int)(heap_mapped_end - heap_base) + more_bytes;
    unsigned int mapped = (unsigned int)(heap_mapped_end - heap_base);
    while (mapped < need) {
        if ((unsigned int)(heap_mapped_end - heap_base) >= KHEAP_LIMIT) {
            kprintf("kheap: exceeded KHEAP_LIMIT\n");
            asm volatile("cli; hlt");
        }
        map_one_page(heap_mapped_end);
        // zero the newly mapped page
        for (unsigned int i = 0; i < PMM_BLOCK_SIZE; ++i) {
            heap_mapped_end[i] = 0;
        }
        heap_mapped_end += PMM_BLOCK_SIZE;
        mapped += PMM_BLOCK_SIZE;
    }
}

static block_header *split_block(block_header *b, unsigned int want) {
    // Assume b->size >= want and b is free
    unsigned int total = b->size;
    unsigned int remain = total - want;
    if (remain <= sizeof(block_header) + ALIGN) {
        // not worth splitting
        return b;
    }
    // New header starts after payload of first block
    unsigned char *new_hdr_addr = (unsigned char *)b + sizeof(block_header) + want;
    block_header *nb = (block_header *)new_hdr_addr;
    nb->size  = remain - sizeof(block_header);
    nb->magic = HEAP_MAGIC;
    nb->free  = 1;
    nb->next  = b->next;

    b->size = want;
    b->next = nb;
    return b;
}

static void coalesce(block_header *b) {
    while (b && b->next && b->next->free && b->next->magic == HEAP_MAGIC) {
        block_header *n = b->next;
        // merge n into b
        b->size = b->size + sizeof(block_header) + n->size;
        b->next = n->next;
    }
}

// ---- Public API ----
void kheap_init(void) {
    // Map initial pages
    ensure_mapped_bytes(KHEAP_INIT_PAGES * PMM_BLOCK_SIZE);

    // Create a single big free block over the mapped range
    free_list = (block_header *)heap_base;
    free_list->size  = (unsigned int)(heap_mapped_end - heap_base) - sizeof(block_header);
    free_list->magic = HEAP_MAGIC;
    free_list->free  = 1;
    free_list->next  = 0;

    kprintf("Heap: base=0x%x mapped=%u KB\n", (unsigned int)heap_base,
            (unsigned int)(heap_mapped_end - heap_base) / 1024u);
}

void *kmalloc(unsigned int size) {
    if (size == 0) return 0;
    unsigned int need = align_up(size, ALIGN);

    // First-fit
    block_header *prev = 0;
    block_header *cur  = free_list;
    while (cur) {
        if (cur->magic != HEAP_MAGIC) {
            kprintf("kheap: header magic corrupt at %x\n", (unsigned int)cur);
            asm volatile("cli; hlt");
        }
        if (cur->free && cur->size >= need) {
            // maybe split
            cur = split_block(cur, need);
            cur->free = 0;
            return (unsigned char *)cur + sizeof(block_header);
        }
        prev = cur;
        cur  = cur->next;
    }

    // No fit: grow the heap by mapping more pages and append a free block.
    unsigned int grow = align_up(need + sizeof(block_header), PMM_BLOCK_SIZE);
    unsigned char *old_end = heap_mapped_end;
    ensure_mapped_bytes(grow);

    // Create a new free block in the new area and try again
    block_header *nb = (block_header *)old_end;
    nb->size  = grow - sizeof(block_header);
    nb->magic = HEAP_MAGIC;
    nb->free  = 1;
    nb->next  = 0;

    // Link it
    if (prev) prev->next = nb;
    else      free_list  = nb;

    // Allocate from it
    nb = split_block(nb, need);
    nb->free = 0;
    return (unsigned char *)nb + sizeof(block_header);
}

void kfree(void *ptr) {
    if (!ptr) return;
    block_header *b = (block_header *)((unsigned char *)ptr - sizeof(block_header));
    if (b->magic != HEAP_MAGIC) {
        kprintf("kheap: free bad ptr %x\n", (unsigned int)ptr);
        return;
    }
    b->free = 1;

    // Coalesce with next
    coalesce(b);

    // Optional: coalesce with previous by scanning (simple, O(n))
    block_header *cur = free_list;
    block_header *prev = 0;
    while (cur && cur != b) { prev = cur; cur = cur->next; }
    if (prev && prev->free) {
        coalesce(prev);
    }
}

void *kcalloc(unsigned int n, unsigned int size) {
    unsigned int total = n * size;
    void *p = kmalloc(total);
    if (!p) return 0;
    // zero it
    unsigned char *c = (unsigned char *)p;
    for (unsigned int i = 0; i < total; ++i) c[i] = 0;
    return p;
}

void *krealloc(void *ptr, unsigned int size) {
    if (!ptr) return kmalloc(size);
    if (size == 0) { kfree(ptr); return 0; }

    block_header *b = (block_header *)((unsigned char *)ptr - sizeof(block_header));
    if (b->magic != HEAP_MAGIC) {
        kprintf("kheap: realloc bad ptr %x\n", (unsigned int)ptr);
        return 0;
    }

    unsigned int need = align_up(size, ALIGN);
    if (need <= b->size) {
        // shrink in place (optional split)
        split_block(b, need);
        return ptr;
    }

    // Try to grow into the next free neighbor
    if (b->next && b->next->free && b->next->magic == HEAP_MAGIC &&
        (b->size + sizeof(block_header) + b->next->size) >= need) {
        // merge with next and then split
        b->size = b->size + sizeof(block_header) + b->next->size;
        b->next = b->next->next;
        split_block(b, need);
        return ptr;
    }

    // Fallback: allocate new, copy, free old
    void *np = kmalloc(size);
    if (!np) return 0;
    unsigned int copy = (b->size < need) ? b->size : need;
    unsigned char *dst = (unsigned char *)np;
    unsigned char *src = (unsigned char *)ptr;
    for (unsigned int i = 0; i < copy; ++i) dst[i] = src[i];
    kfree(ptr);
    return np;
}