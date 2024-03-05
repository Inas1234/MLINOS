#include "./stdio.h"

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
	current_line = 0;
};

unsigned int print_string(char *message) {
    char *vidmem = (char *) VGA_ADDRESS;
    unsigned int i = current_line * VGA_WIDTH * 2;

    while (*message != 0) {
        if (*message == '\n') {
            current_line++;
            i = current_line * VGA_WIDTH * 2;
            message++;
        } else {
            vidmem[i++] = *message++;
            vidmem[i++] = WHITE_TXT;
        }
    }

    current_line++; // Move to the next line for subsequent prints
    return 1;
}

void int_to_hex(unsigned int n, char *dest) {
    int i, j = 0;
    for (i = 7; i >= 0; i--, n <<= 4) {
        char hex_digit = "0123456789ABCDEF"[(n >> 28) & 0xF];
        if (j > 0 || hex_digit != '0' || i == 0) { 
            dest[j++] = hex_digit;
        }
    }
    dest[j] = '\0'; 
}


void print_hex(unsigned int value){
    char buffer[10];
    int_to_hex(value, buffer);

    print_string(buffer);
}