#pragma once
#include "io.h"

#define KEYBOARD_BUFFER_SIZE 128

// Init (ok if called once at boot)
void init_keyboard(void);

// Ring buffer API used by your kernel_main loop
void  keyboard_buffer_add(char c);
char  keyboard_get_char(void);

// You already have this — keep your mapping:
char get_ascii_from_scancode(unsigned char sc);