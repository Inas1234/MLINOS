#include "../stdlib/stdio.h"
#include "pmm.h"
#include "paging.h"
#include "descriptorTables.h"



void kernel_main() 
{
	clear_screen();
	pmm_init();
	if (pmm_free_blocks_count() == 0) {
		kprintf("Error: No free blocks available.");
		return;
	}
	print_string("Initializing Physical Memory Manager...\n");
	kprintf("Free blocks: %d\n", pmm_free_blocks_count() * PMM_BLOCK_SIZE / 1024);

	init_gdt();
	kprintf("GDT installed\n");

	init_idt();
	kprintf("IDT installed\n");

	initialize_paging();
	kprintf("Paging initialized.\n");

	
	kprintf("Welcome to MlinOS.\n");
};