#include "./stdio.h"
#include "./stdarg.h"
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

unsigned int print_string(const char *message) {
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

    current_line++; 
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

unsigned int kprintf(const char *fmt, ...){
    va_list args;
    va_start(args, fmt);
    while (*fmt != '\0') {
        if (*fmt == '%') {
            fmt++;
            if (*fmt == 'x') {
                unsigned int value = va_arg(args, unsigned int);
                print_hex(value);
            }
            if (*fmt == 'd') {
                unsigned int value = va_arg(args, unsigned int);
                print_int(value);
            }
            if (*fmt == 's') {
                const char *value = va_arg(args, const char *);
                print_string(value);
            }
        } else {
            put_char(*fmt);
        }
        fmt++;
    }
}
void put_char(char c) {
    if (c == '\n') {
        current_line++;
        return;
    }
    
    char *vidmem = (char *) VGA_ADDRESS;
    
    unsigned int offset = current_line * VGA_WIDTH * 2;
    
    while (vidmem[offset] != 0 && (offset < (VGA_WIDTH * VGA_HEIGHT * 2))) {
        offset += 2;
    }
    
    if ((offset - (current_line * VGA_WIDTH * 2)) >= (VGA_WIDTH * 2)) {
        current_line++;
        offset = current_line * VGA_WIDTH * 2;  
    }
    
    vidmem[offset] = c;
    vidmem[offset + 1] = WHITE_TXT;
}

void print_int(unsigned int value) {
    char buffer[10];
    int i = 0;
    while (value > 0) {
        buffer[i++] = (value % 10) + '0';
        value /= 10;
    }
    buffer[i] = '\0';
    char reversed[10];
    int j = 0;
    while (i > 0) {
        reversed[j++] = buffer[--i];
    }
    reversed[j] = '\0';
    print_string(reversed);
}