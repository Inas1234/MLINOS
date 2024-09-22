#include "../stdlib/stdio.h"
#include "pmm.h"


void kernel_main() 
{
	clear_screen();
	pmm_init();
	print_string("Initializing Physical Memory Manager...");
	kprintf("Free blocks: %d\n", pmm_free_blocks_count() * PMM_BLOCK_SIZE / 1024);
	print_string("Welcome to MlinOS.");
};