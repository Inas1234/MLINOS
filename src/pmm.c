#include "../stdlib/bitmap.h"
#include "pmm.h"

static byte pmm_bitmap[TOTAL_PMM_BLOCKS / BIT];

void pmm_init(){
    int i;
    for (i = 0; i < sizeof(pmm_bitmap); i++)
    {
        pmm_bitmap[i] = 0;
    }
}

void *pmm_alloc(){
    int i = bitmap_find(pmm_bitmap, TOTAL_PMM_BLOCKS / BIT, False, 0);
    if (i == BITMAP_NOT_FOUND)
    {
        return 0;
    }
    bitmap_set(pmm_bitmap, i);
    return (void *)(i * PMM_BLOCK_SIZE);
}

void pmm_free(void *p){
    int i = (int)p / PMM_BLOCK_SIZE;
    bitmap_clear(pmm_bitmap, i);
}