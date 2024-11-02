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
    while (*message != '\0') {
        put_char(*message++);
    }
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
        current_column = 0;
        return;
    }

    char *vidmem = (char *) VGA_ADDRESS;
    unsigned int offset = (current_line * VGA_WIDTH + current_column) * 2;

    vidmem[offset] = c;
    vidmem[offset + 1] = WHITE_TXT;

    current_column++;

    if (current_column >= VGA_WIDTH) {
        current_column = 0;
        current_line++;
    }
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

void *memset(void *dest, int val, unsigned int len) {
    unsigned char *ptr = dest;
    while (len-- > 0) {
        *ptr++ = (unsigned char)val;
    }
    return dest;
}

void *memcpy(void *dest, const void *src, unsigned int len) {
    unsigned char *d = dest;
    const unsigned char *s = src;
    while (len-- > 0) {
        *d++ = *s++;
    }
    return dest;
}
