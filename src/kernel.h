#define VGA_ADDRESS 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MAX_INDEX VGA_WIDTH * VGA_HEIGHT
#define WHITE_TXT 0x07 

unsigned short *vga_buffer;
unsigned int vga_index;

void clear_screen();
unsigned int print_string(char *message, unsigned int line);
void kernel_main();