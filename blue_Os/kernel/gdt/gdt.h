#ifndef GDT_H
#define GDT_H

#include <stdint.h>

#define SEG_NULL        0x00        // Null descriptor
#define SEG_KCODE       0x08        // Kernel code segment selector (index 1, RPL=0)
#define SEG_KDATA       0x10        // Kernel data segment selector (index 2, RPL=0)
#define SEG_UCODE       0x1B        // User code segment selector (index 3, RPL=3)
#define SEG_UDATA       0x23        // User data segment selector (index 4, RPL=3)

struct GDTEntry {
    uint16_t limit_low;      // Lower 16 bits of limit
    uint16_t base_low;       // Lower 16 bits of base
    uint8_t  base_middle;    // Next 8 bits of base
    uint8_t  access;         // Access flags
    uint8_t  granularity;    // Granularity and upper 4 bits of limit
    uint8_t  base_high;      // Last 8 bits of base
} __attribute__((packed));   // Prevent compiler padding

struct GDTPtr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

static GDTEntry gdt[5];
static GDTPtr gdt_ptr;

void init_gdt();
void set_gdt_entry(uint16_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularuity);

#endif
