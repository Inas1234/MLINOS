#pragma once
#include "../stdlib/stdio.h"
#include "./paging.h"
#include "./pmm.h"

void kheap_init(void);

void *kmalloc(unsigned int size);
void  kfree(void *ptr);
void *kcalloc(unsigned int n, unsigned int size);
void *krealloc(void *ptr, unsigned int size);