#include "keyboard.h"
#include "io.h"
#include "../stdlib/stdio.h"


const char scancode_to_ascii[] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0, 'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, '-', 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static volatile char kb_buf[KEYBOARD_BUFFER_SIZE];
static volatile unsigned int kb_head = 0;  // write index (ISR)
static volatile unsigned int kb_tail = 0;  // read index (main loop)

static inline unsigned int kb_next(unsigned int v) {
    return (v + 1U) % KEYBOARD_BUFFER_SIZE;
}

void init_keyboard(void) {
    kb_head = kb_tail = 0;
}

void keyboard_buffer_add(char c) {
    unsigned int next = kb_next(kb_head);
    if (next == kb_tail) {
        kb_tail = kb_next(kb_tail);   
    }
    kb_buf[kb_head] = c;
    kb_head = next;
}

char keyboard_get_char(void) {
    if (kb_head == kb_tail)
        return '\0';
    char c = kb_buf[kb_tail];
    kb_tail = kb_next(kb_tail);
    return c;
}

char get_ascii_from_scancode(unsigned char scancode) {
    if (scancode >= sizeof(scancode_to_ascii) / sizeof(scancode_to_ascii[0])) {
        return 0;  // Return 0 for invalid scancode
    }
    return scancode_to_ascii[scancode];
}

