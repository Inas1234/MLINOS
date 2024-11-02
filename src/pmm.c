#include "../stdlib/bitmap.h"
#include "../stdlib/stdio.h"

#include "pmm.h"

static byte pmm_bitmap[TOTAL_PMM_BLOCKS / BIT];

void pmm_init(){
    // Initialize the bitmap to zero
    memset(pmm_bitmap, 0, sizeof(pmm_bitmap));

    // Mark reserved frames as used
    int reserved_frames = PMM_START_ADDRESS / PMM_BLOCK_SIZE;
    for (int i = 0; i < reserved_frames; i++) {
        bitmap_set(pmm_bitmap, i);
    }
}

void *pmm_alloc() {
    int max_frames = IDENTITY_MAP_LIMIT / PMM_BLOCK_SIZE;
    int frame = bitmap_find_free(pmm_bitmap, max_frames);
    if (frame == -1) {
        return NULL; 
    }
    bitmap_set(pmm_bitmap, frame);
    return (void *)(PMM_START_ADDRESS + frame * PMM_BLOCK_SIZE);
}


void pmm_free(void *p) {
    int i = ((unsigned int)p - PMM_START_ADDRESS) / PMM_BLOCK_SIZE;
    bitmap_clear(pmm_bitmap, i);
}

unsigned int pmm_free_blocks_count(){
    return bitmap_count(pmm_bitmap, TOTAL_PMM_BLOCKS / BIT, False);
}