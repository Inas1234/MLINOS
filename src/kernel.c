#include "kernel.h"


void kernel_main() 
{
	clear_screen();
	print_string("Hello, world! Welcome to my kernel.", 0);
};

void clear_screen()
{
	char *vidmem = (char *) VGA_ADDRESS;
	unsigned int i=0;
	while(i < (VGA_MAX_INDEX*2))
	{
		vidmem[i]=' ';
		i++;
		vidmem[i]=WHITE_TXT;
		i++;
	};
};

unsigned int print_string(char *message, unsigned int line)
{
	char *vidmem = (char *) VGA_ADDRESS;
	unsigned int i=0;

	i=(line*VGA_WIDTH*2);

	while(*message!=0)
	{
		if(*message=='\n') 
		{
			line++;
			i=(line*VGA_WIDTH*2);
			*message++;
		} else {
			vidmem[i]=*message;
			*message++;
			i++;
			vidmem[i]=WHITE_TXT;
			i++;
		};
	};

	return(1);
}
