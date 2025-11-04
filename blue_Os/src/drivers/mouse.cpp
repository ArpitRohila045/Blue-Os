#include <drivers/mouse.h>
#include <common/types.h>
#include <common/print.h>

using namespace blueOs::drivers;
using namespace blueOs::hardwarecommunication;
using namespace blueOs::common;

static int8_t mouseX;
static int8_t mouseY;
static int16_t data;
static uint16_t* vga = (uint16_t*)0xb8000;


MouseEventHandler::MouseEventHandler(MouseDriver* mouseDriver){
}


void MouseEventHandler::onMouseUp(uint8_t button)
{}


void MouseEventHandler::onMouseDown(uint8_t button)
{}


void MouseEventHandler::onMouseMove(int x, int y)
{}


MouseDriver::MouseDriver(InterruptManager* interruptManager)
: InterruptHandler(0x2C, interruptManager),
    dataPort(0x60),
    commandPort(0x64),
    mouseEventHandler(this)
{}


MouseDriver::~MouseDriver()
{}


static int16_t drawMouseCursor(const int8_t mouseX, const int8_t mouseY){
    data = vga[80*mouseY + mouseX];
    vga[80*mouseY + mouseX] = ((vga[80*mouseY + mouseX] & 0xF000) >> 4)
                        |((vga[80*mouseY + mouseX] & 0x0F00) << 4)
                        |((vga[80*mouseY + mouseX] & 0x00FF) >> 4);
}


static void clearMouseCursor(const int8_t mouseX, const int8_t mouseY){
    vga[80*mouseY + mouseX] = data;
}


uint32_t MouseDriver::handleInterrupt(uint32_t esp){
    uint8_t status = commandPort.read();

    if (!(status & 0x20)) 
        return esp;

    buffer[offset] = dataPort.read();
    offset = (offset + 1) % 3;

    if(!offset){
        clearMouseCursor(mouseX, mouseY);

        mouseX = (mouseX + buffer[1]);
        mouseY = (mouseY - buffer[2]);

        if (mouseX >= 80) mouseX = 79;
        if (mouseY >= 25) mouseY = 24;
        if (mouseX < 0)   mouseX = 0;
        if (mouseY < 0)   mouseY = 0;

        drawMouseCursor(mouseX, mouseY);
        button = buffer[0];
    }
    return esp;
}


void MouseDriver::activate(){
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