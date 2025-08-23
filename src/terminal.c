#include "terminal.h"
#include "../stdlib/stdio.h"
#include "io.h"

extern unsigned int current_line;     // Track the current line in terminal
extern unsigned int current_column;   // Track the current column in terminal

char *video_memory = (char *)VGA_ADDRESS;

void move_cursor() {
    unsigned short pos = current_line * VGA_WIDTH + current_column;

    outb(0x3D4, 14);                 
    outb(0x3D5, (pos >> 8) & 0xFF);
    outb(0x3D4, 15);                  
    outb(0x3D5, pos & 0xFF);
}

void terminal_putchar(char c) {
    if (c == '\n') {
        current_column = 0;
        current_line++;
    } else {
        int offset = (current_line * VGA_WIDTH + current_column) * 2;
        video_memory[offset] = c;
        video_memory[offset + 1] = WHITE_TXT;  
        current_column++;
        if (current_column >= VGA_WIDTH) {
            current_column = 0;
            current_line++;
        }
    }

    if (current_line >= VGA_HEIGHT) {
        clear_screen();  
        current_column = 0;
        current_line = 0;
    }

    move_cursor();
}

void terminal_write(const char* str) {
    while (*str) {
        terminal_putchar(*str++);
    }
}

void init_terminal() {
    clear_screen(); 
    current_line = 0;
    current_column = 0;
    move_cursor();
    terminal_write("MlinOS Terminal\n ");
}

unsigned int terminal_cursor_pos(void) {
    return current_line * VGA_WIDTH + current_column;
}

void terminal_backspace(unsigned int guard_pos) {
    unsigned int pos = current_line * VGA_WIDTH + current_column;

    if (pos <= guard_pos) {
        move_cursor();
        return;
    }

    if (current_column == 0) {
        move_cursor();
        return; 
    }

    current_column--;
    int offset = (current_line * VGA_WIDTH + current_column) * 2;
    video_memory[offset]     = ' ';
    video_memory[offset + 1] = WHITE_TXT;
    move_cursor();
}