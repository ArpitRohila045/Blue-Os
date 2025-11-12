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

void taskA(){
    while(1)
        print("A");
}

void taskB(){
    while(1)
        print("B");
}

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;

extern "C" void callConstructors(){
    for(constructor* i = &start_ctors; i != &end_ctors; i++){
        (*i)();
    }
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
    /*
    Task task1(&taskA);
    Task task2(&taskB);
    taskManager.addTask(&task1);
    taskManager.addTask(&task2);
    */

    InterruptManager interrupts(&taskManager);
    DriverManager driverManager; 

    KeyboardDriver keyboard(&interrupts);
    MouseDriver mouse(&interrupts);

    driverManager.addDriver(&keyboard);
    driverManager.addDriver(&mouse);

    driverManager.activateAll();

    // print("Hardware Initialization Stage 3\n");
    PeripheralComponentInterconnectController pciDevices;
    // pciDevices.selectDrivers(&driverManager, &interrupts);

    VideoGraphicsArray vga;


    // print("Hardware Initialization Done...\n");

    // vga.SetMode(320,200,8);
    // for(uint32_t y=0; y< 200; y++)
    //     for(uint32_t x=0; x<320; x++)
    //         vga.PutPixel(x,y,0x00,0x00,0xA8);


        
    interrupts.activate();

    meminfo(addr);
    while (1);
}