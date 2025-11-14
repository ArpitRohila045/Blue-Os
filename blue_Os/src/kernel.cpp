#include <stdint.h>
#include <common/print.h>
#include <gdt.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/pci.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/driver.h>
#include <drivers/vga.h>
#include <multitasking.h>
#include <memory/multiboot.h>

using namespace blueOs;
using namespace blueOs::hardwarecommunication;
using namespace blueOs::drivers;
using namespace blueOs::common;
using namespace blueOs::memory;

/*
 * ------------------------------
 * Example Tasks for Scheduling
 * ------------------------------
 */

// Short-running task (fast CPU burst)
void fastTask() {
    uint32_t counter = 0;
    while (1) {
        print("FAST: ");
        printHex(counter++);
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
        printHex(counter++);
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
        printHex(counter++);
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

extern "C" void callConstructors() {
    for (constructor* i = &start_ctors; i != &end_ctors; i++) {
        (*i)();
    }
}

/*
 * ------------------------------
 * Kernel Main
 * ------------------------------
 */
extern "C" void kernelMain(uint32_t magic, multiboot_info* addr) {
    if (magic != 0x2BADB002) {
        print("Invalid multiboot magic!\n");
        while (1);
    }

    // Initialize GDT (Global Descriptor Table)
    init_gdt();

    // Initialize task manager and add tasks
    TaskManager taskManager;
    Task t1(&fastTask, 3);    // short burst time
    Task t2(&mediumTask, 6);  // medium burst
    Task t3(&slowTask, 10);   // long burst

    taskManager.addTask(&t1);
    taskManager.addTask(&t2);
    taskManager.addTask(&t3);

    // Initialize interrupts and drivers
    InterruptManager interrupts(&taskManager);
    DriverManager driverManager;

    KeyboardDriver keyboard(&interrupts);
    MouseDriver mouse(&interrupts);
    driverManager.addDriver(&keyboard);
    driverManager.addDriver(&mouse);
    driverManager.activateAll();

    PeripheralComponentInterconnectController pciDevices;
    // pciDevices.selectDrivers(&driverManager, &interrupts);

    VideoGraphicsArray vga;
    // vga.SetMode(320,200,8); // optional if you want graphics mode

    interrupts.activate(); // enable interrupts

    // Display memory info (optional)
    meminfo(addr);

    // Keep kernel running forever
    while (1);
}