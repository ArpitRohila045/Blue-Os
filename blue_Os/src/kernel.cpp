#include <stdint.h>
#include <common/print.h>
#include <gdt.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/pci.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/driver.h>
#include <drivers/vga.h>
#include <drivers/ata.h>
#include <multitasking.h>
#include <memory/multiboot.h>
#include <memory/memorymanagment.h>

using namespace blueOs;
using namespace blueOs::hardwarecommunication;
using namespace blueOs::drivers;
using namespace blueOs::common;
using namespace blueOs::memory;


// Short-running task (fast CPU burst)
void fastTask() {
    uint32_t counter = 0;
    while (1) {
        print("FAST: ");
        print_hex32(counter++);
        print("\n");

        // Small delay — simulates short burst
        for (volatile int i = 0; i < 300000; i++);
    }
}

// Long-running task (slow CPU burst)
void slowTask() {
    uint32_t counter = 0;
    while (1) {
        print("SLOW: ");
        print_hex32(counter++);
        print("\n");

        // Larger delay — simulates longer burst
        for (volatile int i = 0; i < 1500000; i++);
    }
}

// Optional third task (medium CPU burst)
void mediumTask() {
    uint32_t counter = 0;
    while (1) {
        print("MEDIUM: ");
        print_hex32(counter++);
        print("\n");

        for (volatile int i = 0; i < 800000; i++);
    }
}

/*
 * ------------------------------
 * Kernel Constructors / Setup
 * ------------------------------
 */
typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;


extern "C" uint32_t _kernel_start;   // Provided by linker script
extern "C" uint32_t _kernel_end;     // Provided by linker script

extern "C" void callConstructors(){
    for(constructor* i = &start_ctors; i != &end_ctors; i++){
        (*i)();
    }
}

MemoryManager setup_memory(multiboot_info* mbi) {
    // 1. Find free region after kernel
    size_t heap_start = (size_t)&_kernel_end;
    heap_start = (heap_start + 0xFFF) & ~0xFFF; // align 4KB

    // 2. Pick size based on free memory (e.g., 10 MB)
    size_t heap_size = 10 * 1024 * 1024;

    // 3. Initialize your heap allocator
    MemoryManager memoryManager(heap_start, heap_size);

    // 4. Optionally test
    void* a = memoryManager.kmalloc(256);
    void* b = memoryManager.kmalloc(512);
    memoryManager.kfree(a);
    return memoryManager;
}


extern "C" void kernelMain(uint32_t magic, multiboot_info* addr) {
    if (magic != 0x2BADB002) {
        print("Invalid multiboot magic!\n");
        while (1);
    }
    // print("Starting MyOS Kernel...\n");
    // print("Hardware Initialization Stage 1\n");
    init_gdt();
    // print("Hardware Initialization Stage 2\n");

    TaskManager taskManager;
    Task t1(&fastTask, 3);    // short burst time
    Task t2(&mediumTask, 6);  // medium burst
    Task t3(&slowTask, 10);   // long burst

    // taskManager.addTask(&t1);
    // taskManager.addTask(&t2);
    // taskManager.addTask(&t3);

    // Initialize interrupts and drivers
    InterruptManager interrupts(&taskManager);
    DriverManager driverManager; 

    KeyboardDriver keyboard(&interrupts);
    MouseDriver mouse(&interrupts);

    driverManager.addDriver(&keyboard);
    driverManager.addDriver(&mouse);

    driverManager.activateAll();
    interrupts.activate();

    print("Hardware Initialization Stage 3\n");
    PeripheralComponentInterconnectController pciDevices;
    // pciDevices.selectDrivers(&driverManager, &interrupts);

    VideoGraphicsArray vga;


    // print("Hardware Initialization Done...\n");

    // vga.SetMode(320,200,8);
    // for(uint32_t y=0; y< 200; y++)
    //     for(uint32_t x=0; x<320; x++)
    //         vga.PutPixel(x,y,0x00,0x00,0xA8);


    print("\nS-ATA primary master: ");
    AdvanceTechnologyAttachment ata0m(true, 0x1F0);
    ata0m.identify();
    
    print("\nS-ATA primary slave: ");
    AdvanceTechnologyAttachment ata0s(false, 0x1F0);
    ata0s.identify();
    ata0s.write28(0, (uint8_t*)"http://www.AlgorithMan.de", 25);
    ata0s.flush();
    ata0s.read28(0, 25);
    
    print("\nS-ATA secondary master: ");
    AdvanceTechnologyAttachment ata1m(true, 0x170);
    ata1m.identify();
    
    print("\nS-ATA secondary slave: ");
    AdvanceTechnologyAttachment ata1s(false, 0x170);
    ata1s.identify();
    // third: 0x1E8
    // fourth: 0x168

    interrupts.activate();

    // meminfo(addr);
    while (1);
}