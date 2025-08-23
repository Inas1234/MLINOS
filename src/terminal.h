#ifndef TERMINAL_H
#define TERMINAL_H

void init_terminal();
void terminal_putchar(char c);
void terminal_write(const char* str);
void terminal_backspace(unsigned int guard_column);
unsigned int terminal_cursor_pos(void);

#endif
