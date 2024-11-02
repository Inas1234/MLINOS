#include "../stdlib/stdio.h"
#include "pmm.h"
#include "paging.h"
#include "isr.h"
#include "idt.h"

void kernel_main() 
{
	clear_screen();
	pmm_init();
	if (pmm_free_blocks_count() == 0) {
		print_string("Error: No free blocks available.");
		return;
	}
	print_string("Initializing Physical Memory Manager...\n");
	kprintf("Free blocks: %d\n", pmm_free_blocks_count() * PMM_BLOCK_SIZE / 1024);
		
	// idt_install();
	// isr_install();
	// asm volatile("sti");

	initialize_paging();
	kprintf("Paging initialized.\n");
	
	print_string("Welcome to MlinOS.");
};