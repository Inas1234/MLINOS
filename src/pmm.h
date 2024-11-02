#pragma once
#define PMM_BLOCK_SIZE 4096
#define MEMORY_SIZE 0x1000000
#define TOTAL_PMM_BLOCKS MEMORY_SIZE / PMM_BLOCK_SIZE
#define PMM_START_ADDRESS 0x00100000 // Start allocating after 1MB
#define IDENTITY_MAP_LIMIT 0x01000000 // 16MB


void pmm_init();
void *pmm_alloc();
void pmm_free(void *block);
unsigned int pmm_free_blocks_count();