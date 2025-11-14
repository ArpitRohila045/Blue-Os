#include <drivers/keyboard.h>
#include <common/print.h>

using namespace blueOs::common;
using namespace blueOs::drivers;
using namespace blueOs::hardwarecommunication;

KeyboardEventHandler::KeyboardEventHandler() {}
KeyboardEventHandler::~KeyboardEventHandler() {}

void KeyboardEventHandler::onKeyLeft(char)  {}
void KeyboardEventHandler::onKeyRight(char) {}
void KeyboardEventHandler::onKeyPress(char) {}

// ---------------- KEYBOARD DRIVER ----------------

KeyboardDriver::KeyboardDriver(InterruptManager* interruptManager)
: InterruptHandler(0x21, interruptManager),
  commandPort(0x64),
  dataPort(0x60),
  handler(nullptr)
{
}

KeyboardDriver::~KeyboardDriver() {}

void KeyboardDriver::setEventHandler(KeyboardEventHandler* h) {
    handler = h;
}

void KeyboardDriver::activate() {
    // Clear buffer
    while(commandPort.read() & 0x1)
        dataPort.read();

    commandPort.write(0xAE); // enable keyboard device
    commandPort.write(0x20); // read controller command byte
    uint8_t status = dataPort.read();
    status |= 1;    // enable interrupt from keyboard
    status &= ~0x10; // disable translation (we want raw scancodes)
    commandPort.write(0x60); // write controller command byte
    dataPort.write(status);
    dataPort.write(0xF4); // send "enable scanning" to keyboard
}

// Minimal scancode -> ascii map for common keys (make bigger if needed)
static char scancode_to_ascii(uint8_t sc) {
    switch(sc) {
        case 0x02: return '1';
        case 0x03: return '2';
        case 0x04: return '3';
        case 0x05: return '4';
        case 0x06: return '5';
        case 0x07: return '6';
        case 0x08: return '7';
        case 0x09: return '8';
        case 0x0A: return '9';
        case 0x0B: return '0';
        case 0x0C: return '-';
        case 0x0D: return '=';
        case 0x0E: return '\b';
        case 0x0F: return '\t';
        case 0x10: return 'q';
        case 0x11: return 'w';
        case 0x12: return 'e';
        case 0x13: return 'r';
        case 0x14: return 't';
        case 0x15: return 'y';
        case 0x16: return 'u';
        case 0x17: return 'i';
        case 0x18: return 'o';
        case 0x19: return 'p';
        case 0x1E: return 'a';
        case 0x1F: return 's';
        case 0x20: return 'd';
        case 0x21: return 'f';
        case 0x22: return 'g';
        case 0x23: return 'h';
        case 0x24: return 'j';
        case 0x25: return 'k';
        case 0x26: return 'l';
        case 0x2C: return 'z';
        case 0x2D: return 'x';
        case 0x2E: return 'c';
        case 0x2F: return 'v';
        case 0x30: return 'b';
        case 0x31: return 'n';
        case 0x32: return 'm';
        case 0x1C: return '\n';
        case 0x39: return ' ';
        default: return 0;
    }
}

uint32_t KeyboardDriver::handleInterrupt(uint32_t esp) {
    uint8_t scancode = dataPort.read();

    // ignore break codes (scancode with high bit set)
    if (scancode & 0x80)
        return esp;

    char c = scancode_to_ascii(scancode);
    if (c) {
        char temp[2] = { c, 0 };  // No print_char needed
        print(temp);
                      // uses your existing print(const char*)
        if (handler)
            handler->onKeyPress(c);
        }
    else {
        // Unmapped key: print hex (safe buffer)
        const char hex[] = "0123456789ABCDEF";
        char msg[] = "KEY 0x00\n";
        msg[6] = hex[(scancode >> 4) & 0xF];
        msg[7] = hex[scancode & 0xF];
        print(msg);
    }

    return esp;
}
