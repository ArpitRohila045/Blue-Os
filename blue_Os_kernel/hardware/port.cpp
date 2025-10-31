#include<stdint.h>
#include "port.h"

Port::Port(uint16_t portnumber){
    this->portnumber = portnumber;
}

Port::~Port(){}

Port8bit::Port8bit(uint16_t portnumber) : Port(portnumber){}

Port8bit::~Port8bit(){}

void Port8bit::write(uint8_t data){
    asm volatile("outb %0, %1" : : "a"(data), "Nd"(portnumber));
}

uint8_t Port8bit::read(){
    uint8_t result;
    asm volatile("inb %1, %0 ": "=a"(result) : "Nd"(portnumber));
    return result;
}


Port8bitSlow::Port8bitSlow(uint16_t portnumber) : Port8bit(portnumber){}

Port8bitSlow::~Port8bitSlow(){}

void Port8bitSlow::write(uint8_t data){
    asm volatile("outb %0, %1" : : "a"(data), "Nd"(portnumber));
}



Port16bit::Port16bit(uint16_t portnumber) : Port(portnumber){}

Port16bit::~Port16bit(){}

void Port16bit::write(uint16_t data){
    asm volatile("outb %0, %1" : : "a"(data), "Nd"(portnumber));
}

uint16_t Port16bit::read(){
    uint16_t result;
    asm volatile("inb %1, %0 ": "=a"(result) : "Nd"(portnumber));
    return result;
}



Port32bit::Port32bit(uint16_t portnumber) : Port(portnumber){}

Port32bit::~Port32bit(){}

void Port32bit::write(uint32_t data){
    asm volatile("outb %0, %1" : : "a"(data), "Nd"(portnumber));
}

uint32_t Port32bit::read(){
    uint32_t result;
    asm volatile("inb %1, %0 ": "=a"(result) : "Nd"(portnumber));
    return result;
}
