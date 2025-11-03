#include <stdint.h>
#include <common/print.h>
#include <gdt.h>
#include <hardwarecommunication/interrupts.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/driver.h>

using namespace blueOs;
using namespace blueOs::hardwarecommunication;
using namespace blueOs::drivers;
using namespace blueOs::common;



typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;

extern "C" void callConstructors(){
    for(constructor* i = &start_ctors; i != &end_ctors; i++){
        (*i)();
    }
}

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
    DriverManager driverManager; 

    KeyboardDriver keyboard(&interrupts);
    MouseDriver mouse(&interrupts);

    driverManager.addDriver(&keyboard);
    driverManager.addDriver(&mouse);

    driverManager.activateAll();
    interrupts.activate();
    
    blueOs::common::print("Interrupt Descriptor Table initialized.\n\n");

    while (1);
}