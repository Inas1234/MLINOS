#pragma once
#define VGA_ADDRESS 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MAX_INDEX VGA_WIDTH * VGA_HEIGHT
#define WHITE_TXT 0x07 
#define NULL 0

static unsigned int current_line = 0;

void clear_screen();
unsigned int print_string(char *message);
void int_to_hex(unsigned int n, char *dest);
void print_hex(unsigned int value);