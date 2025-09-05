#pragma once
#include "../stdlib/stdio.h"

#define PANIC(...) do {                         \
    kprintf(__VA_ARGS__);                       \
    kprintf("\n--- KERNEL PANIC --- Halting.\n"); \
    asm volatile("cli");                        \
    for (;;) { asm volatile("hlt"); }           \
} while (0)

#define ASSERT(x) do {                                      \
    if (!(x)) {                                             \
        PANIC("Assertion failed: %s (%s:%d)",               \
              #x, __FILE__, __LINE__);                      \
    }                                                       \
} while (0)