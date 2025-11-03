#include <stdint.h>
#include <hardwarecommunication/port.h>

using namespace blueOs::hardwarecommunication;
using namespace blueOs::common;

Port::Port(uint16_t portnumber) {
    this->portnumber = portnumber;
}

Port::~Port() {}


// -------- 8-bit Port --------
Port8Bit::Port8Bit(uint16_t portnumber)
    : Port(portnumber) {}

Port8Bit::~Port8Bit() {}

void Port8Bit::write(uint8_t data) {
    asm volatile("outb %0, %1" : : "a"(data), "Nd"(portnumber));
}

uint8_t Port8Bit::read() {
    uint8_t result;
    asm volatile("inb %1, %0" : "=a"(result) : "Nd"(portnumber));
    return result;
}


// -------- 8-bit Slow Port --------
Port8BitSlow::Port8BitSlow(uint16_t portnumber)
    : Port8Bit(portnumber) {}

Port8BitSlow::~Port8BitSlow() {}

void Port8BitSlow::write(uint8_t data) {
    asm volatile("outb %0, %1\njmp 1f\n1: jmp 1f\n1:"
                 : : "a"(data), "Nd"(portnumber));
}


// -------- 16-bit Port --------
Port16Bit::Port16Bit(uint16_t portnumber)
    : Port(portnumber) {}

Port16Bit::~Port16Bit() {}

void Port16Bit::write(uint16_t data) {
    asm volatile("outw %0, %1" : : "a"(data), "Nd"(portnumber));
}

uint16_t Port16Bit::read() {
    uint16_t result;
    asm volatile("inw %1, %0" : "=a"(result) : "Nd"(portnumber));
    return result;
}


// -------- 32-bit Port --------
Port32Bit::Port32Bit(uint16_t portnumber)
    : Port(portnumber) {}

Port32Bit::~Port32Bit() {}

void Port32Bit::write(uint32_t data) {
    asm volatile("outl %0, %1" : : "a"(data), "Nd"(portnumber));
}

uint32_t Port32Bit::read() {
    uint32_t result;
    asm volatile("inl %1, %0" : "=a"(result) : "Nd"(portnumber));
    return result;
}
