#include "bitmap.h"

static Bool get(byte bitmap, byte bit){
    return (bitmap >> bit) & 1;
}

static void set(byte *bitmap, byte bit){
    *bitmap |= 1 << bit;
}

static void clear(byte *bitmap, byte bit){
    *bitmap &= ~(1 << bit);
}

Bool bitmap_get(byte *bitmap, int bit){
    return get(bitmap[bit / BIT], bit % BIT);
}

void bitmap_set(byte *bitmap, int bit){
    set(&bitmap[bit / BIT], bit % BIT);
}

void bitmap_clear(byte *bitmap, int bit){
    clear(&bitmap[bit / BIT], bit % BIT);
}

int bitmap_find(byte *bitmap, int size, Bool value, int start){
    int i;
    for (i = start + 1, size *= BIT; i < size; i++)
    {
        if (bitmap_get(bitmap, i) == value)
        {
            return i;
        }
    }
    return BITMAP_NOT_FOUND;
}