#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H

#include <stdint.h>
#include "gdt/gdt.h"
#include "hardware/port.h"


class InterruptManager;

class InterruptHandler{
    protected:
        uint8_t interruptNumber;
        InterruptManager* interruptManager;

        InterruptHandler(uint8_t interruptNumber, InterruptManager* interruptManager);
    public:
        ~InterruptHandler();
        virtual uint32_t handleInterrupt(uint32_t esp);
};

class InterruptManager {
    friend class InterruptHandler;
    protected:
        static InterruptManager* activeInterruptManager;
        InterruptHandler* interruptHandler[256];
        struct GateDescriptor{
            uint16_t handlerAddressLowerBits;
            uint16_t gdt_codeSegmentSelector;
            uint8_t reserved;
            uint8_t access;
            uint16_t handlerAddressHighBits; 
        } __attribute__((packed));

        struct InterruptDescriptorTablePointer {
            uint16_t size;
            unsigned int base;
        } __attribute__((packed));

        static GateDescriptor interruptDescriptorTable[256];
        static InterruptDescriptorTablePointer idt;

        static void setInterruptDescriptorEntry(
            uint8_t interruptNumber,
            uint16_t codeSegmentSelector,
            void (*handler)(),
            uint8_t descriptorPriviledgeLevel,
            uint8_t descriptorType
        );

        Port8bitSlow picMasterCommand;
        Port8bitSlow picMasterData;
        Port8bitSlow picSlaveCommand;
        Port8bitSlow picSlaveData;
        
    public:
        InterruptManager();
        ~InterruptManager();
        static uint32_t handleInterrupt(uint32_t interruptnumber, uint32_t esp);
        uint32_t doHandleInterrupt(uint32_t interruptnmber, uint32_t esp);
        void activate();
        void deactivate();
        static void ignoreInterruptRequest();
        static void handleInterruptRequest0x00();  // timer interrupt
        static void handleInterruptRequest0x01();  // keyboard interrupt
};  


#endif