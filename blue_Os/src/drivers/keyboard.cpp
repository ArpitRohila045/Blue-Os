#include <drivers/keyboard.h>
#include <common/print.h>

using namespace blueOs::common;
using namespace blueOs::drivers;
using namespace blueOs::hardwarecommunication;


KeyboardEventHandler::KeyboardEventHandler(KeyboardDriver* keyboardDriver){
    print("KeyboardEventHandler is initialized on the KeyboardDriver");
}

KeyboardEventHandler::~KeyboardEventHandler(){
    print("KeyboardEventHandler is deleted from KeyboardDriver");
}

void KeyboardEventHandler::onKeyLeft(char){}
void KeyboardEventHandler::onKeyRight(char){}
void KeyboardEventHandler::onKeyPress(char){}


KeyboardDriver::KeyboardDriver(InterruptManager* interruptManager)
: InterruptHandler(0x21, interruptManager),
    dataPort(0x60),
    commandPort(0x64),
    keyboardEventHandler(this)
{}

KeyboardDriver::~KeyboardDriver(){}

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
            case 0x10: print("q"); break;
            case 0x11: print("w"); break;
            case 0x1E: print("a"); break;
            case 0x1F: print("s"); break;
            case 0x20: print("d"); break;
            case 0x21: print("f"); break;
            case 0x22: print("g"); break;       
            case 0x23: print("h"); break;
            case 0x24: print("j"); break;
            case 0x25: print("k"); break;
            case 0x26: print("l"); break; 
            case 0x2C: print("z"); break;
            case 0x2D: print("x"); break;
            case 0x2E: print("c"); break;
            case 0x2F: print("v"); break;
            case 0x30: print("b"); break;
            case 0x31: print("n"); break;
            case 0x32: print("m"); break;
            case 0x12: print("e"); break;
            case 0x13: print("r"); break;
            case 0x14: print("t"); break;
            case 0x15: print("y"); break;
            case 0x16: print("u"); break;
            case 0x17: print("i"); break;
            case 0x18: print("o"); break;
            case 0x19: print("p"); break;
            case 0x02: print("1"); break;
            case 0x03: print("2"); break;    
            case 0x04: print("3"); break;
            case 0x05: print("4"); break;
            case 0x06: print("5"); break;
            case 0x07: print("6"); break;
            case 0x08: print("7"); break;
            case 0x09: print("8"); break;
            case 0x0A: print("9"); break;
            case 0x0B: print("0"); break;




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

void KeyboardDriver::activate(){
    while(commandPort.read() & 0x1)
        dataPort.read();

    commandPort.write(0xAE);    // Activate interrupt
    commandPort.write(0x20);    // get current state
    uint8_t status = (dataPort.read() | 1) & ~0x10;
    commandPort.write(0x60);
    dataPort.write(status);
    dataPort.write(0xF4);
}