#include "terminal.h"
#include "../stdlib/stdio.h"
#include "io.h"

extern unsigned int current_line;     // Track the current line in terminal
extern unsigned int current_column;   // Track the current column in terminal

char *video_memory = (char *)VGA_ADDRESS;

// Moves the cursor to the current `current_line` and `current_column` position
void move_cursor() {
    unsigned short pos = current_line * VGA_WIDTH + current_column;

    outb(0x3D4, 14);                  // Send high byte of position to VGA
    outb(0x3D5, (pos >> 8) & 0xFF);
    outb(0x3D4, 15);                  // Send low byte of position to VGA
    outb(0x3D5, pos & 0xFF);
}

// Prints a character to the terminal and handles new lines
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
        clear_screen();  // Clear screen and reset cursor if overflow
        current_column = 0;
        current_line = 0;
    }

    move_cursor();
}

// Prints a string to the terminal
void terminal_write(const char* str) {
    while (*str) {
        terminal_putchar(*str++);
    }
}

// Initializes the terminal with a welcome message and prompt
void init_terminal() {
    clear_screen(); 
    current_line = 0;
    current_column = 0;
    move_cursor();
    terminal_write("MlinOS Terminal\n> ");
}
