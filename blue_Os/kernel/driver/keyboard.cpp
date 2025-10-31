#include "keyboard.h"
#include "utils/print.h"
#include "interrupts/interrupts.h"

KeyboardDriver::KeyboardDriver(InterruptManager* interruptManager)
: InterruptHandler(0x21, interruptManager),
    dataPort(0x60),
    commandPort(0x64)
{
    while(commandPort.read() & 0x1)
        dataPort.read();

    commandPort.write(0xAE);    // Activate interrupt
    commandPort.write(0x20);    // get current state
    uint8_t status = (dataPort.read() | 1) & ~0x10;
    commandPort.write(0x60);
    dataPort.write(status);
    dataPort.write(0xF4);
}

KeyboardDriver::~KeyboardDriver(){

}

uint32_t KeyboardDriver::handleInterrupt(uint32_t esp){
    uint8_t key = dataPort.read();
    if(key < 0x80){
        switch(key){

            /*
                Pending task 
                Map every interruppt code to its specific character;
            */
           
            case 0xFA: break;
            case 0x45: case 0xC5: break;
            default:
                char* msg = "KEYBOARD 0x00\t";
                char* hex = "0123456789ABCDEF";
                msg[11] = hex[(key >> 4) & 0x0F];
                msg[12] = hex[key & 0x0F];
                print(msg);
        }
    }
    return esp;
}