#include "../stdlib/stdio.h"
#include "pmm.h"


void kernel_main() 
{
	clear_screen();
	pmm_init();
	print_string("Initializing Physical Memory Manager...");
	
	print_string("Welcome to MlinOS.");
};