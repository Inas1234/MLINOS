#ifndef IO_H
#define IO_H

#include "../stdlib/stdio.h"
#include "../stdlib/stdtypes.h"

void out8(short port, char byte);
void out32(short port, int word);
char in8(short port);
int in32(short port);

#endif
