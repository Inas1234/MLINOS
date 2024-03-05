#define BIT (8 * sizeof(byte))
#define BITMAP_NOT_FOUND -1

typedef unsigned char byte;

static bool get (byte, byte);
static void set (byte *, byte);
static void clear (byte *, byte);

bool bitmap_get(byte *bitmap, int bit);
void bitmap_set(byte *bitmap, int bit);
void bitmap_clear(byte *bitmap, int bit);
int bitmap_find(byte *bitmap, int size, bool value, int start);
