#pragma once
typedef struct {
    unsigned int present    : 1;   // Page present in memory
    unsigned int rw         : 1;   // Read-only if clear, read/write if set
    unsigned int user       : 1;   // Supervisor level only if clear
    unsigned int accessed   : 1;   // Has the page been accessed since last refresh?
    unsigned int dirty      : 1;   // Has the page been written to since last refresh?
    unsigned int unused     : 7;   // Unused and reserved bits
    unsigned int frame      : 20;  // Frame address (shifted right 12 bits)
} __attribute__((packed)) page_table_entry_t;

// Page table structure
typedef struct {
    page_table_entry_t entries[1024];
} __attribute__((packed)) page_table_t;

// Page directory entry structure
typedef struct {
    unsigned int present    : 1;
    unsigned int rw         : 1;
    unsigned int user       : 1;
    unsigned int write_through : 1;
    unsigned int cache_disabled : 1;
    unsigned int accessed   : 1;
    unsigned int reserved   : 1;
    unsigned int page_size  : 1;
    unsigned int global     : 1;
    unsigned int unused     : 3;
    unsigned int table_addr : 20;  // Address of the page table
} __attribute__((packed)) page_directory_entry_t;

// Page directory structure
typedef struct {
    page_directory_entry_t entries[1024];
} __attribute__((packed)) page_directory_t;

void initialize_paging();
void load_page_directory(page_directory_t *dir);
void enable_paging();
void map_page(void *virtual_addr, void *physical_addr, page_directory_t *dir);
void unmap_page(void *virtual_addr, page_directory_t *dir);
void test_paging();
void page_fault_handler();
