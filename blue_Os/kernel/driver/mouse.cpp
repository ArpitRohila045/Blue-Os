#include "mouse.h"
#include "utils/print.h"
#include "interrupts/interrupts.h"

static int8_t mouseX;
static int8_t mouseY;

MouseDriver::MouseDriver(InterruptManager* interruptManager)
: InterruptHandler(0x2C, interruptManager),
    dataPort(0x60),
    commandPort(0x64)
{
    offset = 0;
    button = 0;

    mouseX = 40;
    mouseY = 12;

    // Enable auxiliary mouse device
    commandPort.write(0xA8);
    
    // Enable mouse interrupts
    commandPort.write(0x20);
    uint8_t status = dataPort.read() | 2;
    commandPort.write(0x60);
    dataPort.write(status);
    
    // Tell mouse to start sending data
    commandPort.write(0xD4);
    dataPort.write(0xF4);
    dataPort.read(); // Acknowledge (0xFA)
}

MouseDriver::~MouseDriver(){
}

static void drawMouseCursor(const int8_t mouseX, const int8_t mouseY){
    putCharAt(mouseX, mouseY, '#', 0x0E);
}

static void clearMouseCursor(const int8_t mouseX, const int8_t mouseY){
    putCharAt(mouseX, mouseY, ' ', 0x0F);
}

uint32_t MouseDriver::handleInterrupt(uint32_t esp){
    uint8_t status = commandPort.read();

    if (!(status & 0x20)) return esp;

    buffer[offset] = dataPort.read();
    offset = (offset + 1) % 3;

    if(!offset){

        static uint16_t* vga = (uint16_t*)0xb8000;
        vga[80*mouseY + mouseX] = ((vga[80*mouseY + mouseX] & 0xF000) >> 4)
                        |((vga[80*mouseY + mouseX] & 0x0F000) << 4)
                        |((vga[80*mouseY + mouseX] & 0x00FF) >> 4);

        mouseX = (mouseX + buffer[1]);
        mouseY = (mouseY - buffer[2]);

        if (mouseX >= 80) mouseX = 79;
        if (mouseY >= 25) mouseY = 24;
        if (mouseX < 0)   mouseX = 0;
        if (mouseY < 0)   mouseY = 0;

        vga[80*mouseY + mouseX] = ((vga[80*mouseY + mouseX] & 0xF000) >> 4)
                        |((vga[80*mouseY + mouseX] & 0x0F00) << 4)
                        |((vga[80*mouseY + mouseX] & 0x00FF) >> 4);

        button = buffer[0];
    }

    return esp;

}