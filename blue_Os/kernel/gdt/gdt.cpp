#include "gdt.h"


void printd(const char* str) {
    static uint16_t* video = (uint16_t*)0xb8000;
    static int row = 0, col = 0;
    while (*str) {
        if (*str == '\n') {
            row++;
            col = 0;
        } else {
            video[row * 80 + col] = (0x07 << 8) | *str;
            col++;
        }
        str++;
    }
}


extern "C" void gdt_flush(uint32_t);


void init_gdt() {
    gdt_ptr.limit = (sizeof(GDTEntry) * 5) - 1;
    gdt_ptr.base  = (uint32_t)&gdt;

    // Null Segment
    set_gdt_entry(0, 0, 0, 0, 0);
    // Kernel Code Segment
    set_gdt_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    // Kernel Data Segment
    set_gdt_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    // User Mode Code Segment
    set_gdt_entry(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    // User Mode Data Segment   
    set_gdt_entry(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    gdt_flush((uint32_t)&gdt_ptr);
}

void set_gdt_entry(uint16_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularuity){

    // Set the base address
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;

    // set the limit
    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = (limit >> 16) & 0x0F;

    // Set the granularity and access flags
    gdt[num].granularity |= (granularuity & 0xF0);
    gdt[num].access = access;
}