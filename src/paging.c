#include "./paging.h"
#include "./pmm.h"
#include "../stdlib/stdio.h"

page_directory_t *kernel_directory;
page_directory_t *current_directory;

void print_page_directory(page_directory_t *dir) {
    for (int i = 0; i < 1024; i++) {
        if (dir->entries[i].present) {  // Only print present entries
            unsigned int table_addr = dir->entries[i].table_addr << 12;
            kprintf("Entry %d: Table at 0x%x, RW=%d, User=%d\n", i, table_addr, dir->entries[i].rw, dir->entries[i].user);
        }
    }
}


void initialize_paging() {
    print_string("Allocating page directory...\n");
    kernel_directory = (page_directory_t *)pmm_alloc();
    if (!kernel_directory) {
        print_string("Failed to allocate kernel directory!\n");
        return;
    } else {
        kprintf("Allocated kernel_directory at: 0x%x\n", (unsigned int)kernel_directory);
    }

    memset(kernel_directory, 0, sizeof(page_directory_t));


    print_string("Mapping page tables...\n");
    int num_tables = IDENTITY_MAP_LIMIT / (PMM_BLOCK_SIZE * 1024);

    for (int i = 0; i < num_tables; i++) {
        page_table_t *table = (page_table_t *)pmm_alloc();
        if (!table) {
            print_string("Failed to allocate page table!\n");
            return;
        }
        memset(table, 0, sizeof(page_table_t));

        if (((unsigned int)table & 0xFFF) != 0) {
            kprintf("Error: Page table %d is not 4KB aligned!\n", i);
            return;
        }

        kernel_directory->entries[i].present = 1;
        kernel_directory->entries[i].rw = 1;
        kernel_directory->entries[i].user = 0;
        kernel_directory->entries[i].table_addr = ((unsigned int)table) >> 12;

        for (int j = 0; j < 1024; j++) {
            unsigned int addr = (i * 1024 + j) * PMM_BLOCK_SIZE;
            table->entries[j].present = 1;
            table->entries[j].rw = 1;
            table->entries[j].user = 0;
            table->entries[j].frame = addr >> 12;
        }
    }

    print_string("Loading page directory...\n");
    load_page_directory(kernel_directory);

    print_string("Enabling paging...\n");
    enable_paging();
    print_string("Paging enabled.\n");
}

void load_page_directory(page_directory_t *dir) {
    current_directory = dir;
    asm volatile("mov %0, %%cr3":: "r"(dir));
}

void enable_paging() {
    unsigned int cr0;
    asm volatile("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000000; // Set the paging bit
    asm volatile("mov %0, %%cr0":: "r"(cr0));
}

void map_page(void *virtual_addr, void *physical_addr, page_directory_t *dir) {
    unsigned int addr = (unsigned int)virtual_addr;
    unsigned int pd_index = addr >> 22;                 // Page directory index
    unsigned int pt_index = (addr >> 12) & 0x03FF;      // Page table index

    page_table_t *table;

    // If the page table doesn't exist, create it
    if (dir->entries[pd_index].present != 1) {
        table = (page_table_t *)pmm_alloc();
        if (!table) {
            // Handle allocation failure
        }
        // Zero out the page table
        memset(table, 0, sizeof(page_table_t));

        dir->entries[pd_index].present = 1;
        dir->entries[pd_index].rw = 1;
        dir->entries[pd_index].user = 0;
        dir->entries[pd_index].table_addr = ((unsigned int)table) >> 12;
    } else {
        table = (page_table_t *)(dir->entries[pd_index].table_addr << 12);
    }

    table->entries[pt_index].present = 1;
    table->entries[pt_index].rw = 1;
    table->entries[pt_index].user = 0;
    table->entries[pt_index].frame = ((unsigned int)physical_addr) >> 12;

    // Invalidate the TLB for the virtual address
    asm volatile("invlpg (%0)"::"r" (virtual_addr) : "memory");
}

void unmap_page(void *virtual_addr, page_directory_t *dir) {
    unsigned int addr = (unsigned int)virtual_addr;
    unsigned int pd_index = addr >> 22;
    unsigned int pt_index = (addr >> 12) & 0x03FF;

    if (dir->entries[pd_index].present != 1) {
        // Page table not present
        return;
    }

    page_table_t *table = (page_table_t *)(dir->entries[pd_index].table_addr << 12);
    table->entries[pt_index].present = 0;

    // Invalidate the TLB for the virtual address
    asm volatile("invlpg (%0)"::"r" (virtual_addr) : "memory");
}

void test_paging() {
    unsigned int *ptr = (unsigned int *)0x400000; // Some virtual address (e.g., 4MB)
    unsigned int physical_addr = 0x100000;        // Some physical address (e.g., 1MB)

    // Map the virtual address to the physical address
    map_page(ptr, (void *)physical_addr, kernel_directory);

    // Write to the virtual address
    *ptr = 0xDEADBEEF;

    // Read back and check
    if (*ptr == 0xDEADBEEF) {
        // Success: paging works
        kprintf("Paging test passed!\n");
    } else {
        // Failure: paging does not work as expected
        kprintf("Paging test failed!\n");
    }
}
