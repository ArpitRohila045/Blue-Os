#include <stdint.h>
#include "utils/print.h"
#include "gdt/gdt.h"
#include "interrupts/interrupts.h"
#include "driver/keyboard.h"

typedef void (*constructor)();
// these symbols are defined in the linker script
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;

extern "C" void callConstructors(){
    for(constructor* i = &start_ctors; i != &end_ctors; i++){
        (*i)();
    }
}
/*
    “This thing exists, but its definition is not here — someone else will provide it.”
    So the compiler won’t complain when you reference it; the linker later connects it to the actual definition

    C++ allows function overloading (same function name with different parameters).
    To support that, the C++ compiler changes function names internally into unique symbols — this process is called name mangling.
    So we're the compiler complile this function using "c-linkage", not c++ linkage
*/

typedef struct multiboot_info {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;
} __attribute__((packed)) multiboot_info_t;


typedef struct multiboot_mmap_entry {
    uint32_t size;
    uint64_t addr;
    uint64_t len;
    uint32_t type;
} __attribute__((packed)) multiboot_mmap_entry_t;


extern "C" void kernelMain(uint32_t magic, uint32_t addr) {
    if (magic != 0x2BADB002) {
        print("Invalid multiboot magic!\n");
        while (1);
    }
    print("Starting MyOS Kernel...\n\n");
    init_gdt();
    print("Global Descriptor Table initialized.\n\n");

    InterruptManager interrupts;

    KeyboardDriver keyboard(&interrupts);

    interrupts.activate();
    print("Interrupt Descriptor Table initialized.\n\n");


    while (1);
}