#pragma once

#include "stdtypes.h"

#define VGA_ADDRESS 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MAX_INDEX VGA_WIDTH * VGA_HEIGHT
#define WHITE_TXT 0x07 
#define NULL 0

static unsigned int current_line = 0;
static unsigned int current_column = 0;

void clear_screen();
unsigned int print_string(const char *message);
void int_to_hex(unsigned int n, char *dest);
void print_hex(unsigned int value);
unsigned int kprintf(const char *fmt, ...);
void print_int(unsigned int value);
void put_char(char c);
void *memset(void *dest, int val, unsigned int len);
void *memcpy(void *dest, const void *src, unsigned int len);
void outb(short port, char byte);
void outw(short port, short word);
void outd(short port, int dword);
