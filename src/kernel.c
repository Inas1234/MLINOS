#include "../stdlib/stdio.h"
#include "pmm.h"
#include "paging.h"
#include "descriptorTables.h"
#include "irq.h"


void kernel_main() 
{
	clear_screen();
	pmm_init();
	if (pmm_free_blocks_count() == 0) {
		kprintf("Error: No free blocks available.");
		return;
	}
	print_string("Initializing Physical Memory Manager: OK\n");

	init_gdt();
	kprintf("GDT: OK\n");

	init_idt();
	kprintf("IDT: OK\n");

	remap_irq();
	kprintf("IRQs remapping: OK\n");

	init_timer(20);
	kprintf("Timer (20 Hz): OK\n");

	initialize_paging();
	kprintf("Initializing paging: OK.\n");

	
	kprintf("Welcome to MlinOS.\n");
};