#pragma once
#define PMM_BLOCK_SIZE 4096
#define MEMORY_SIZE 0x1000000
#define TOTAL_PMM_BLOCKS MEMORY_SIZE / PMM_BLOCK_SIZE

void pmm_init();
void *pmm_alloc();
void pmm_free(void *block);