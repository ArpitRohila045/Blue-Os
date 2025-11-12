#include <hardwarecommunication/interrupts.h>
#include <common/print.h>
#include <gdt.h>
#include <multitasking.h>


using namespace blueOs::hardwarecommunication;
using namespace blueOs::common;
using namespace blueOs;

InterruptHandler::InterruptHandler(uint8_t interruptNumber, InterruptManager* interruptManager){
    this->interruptNumber = interruptNumber;
    this->interruptManager = interruptManager;

    interruptManager->interruptHandler[interruptNumber] = this;
}

InterruptHandler::~InterruptHandler(){    
    if(interruptManager->interruptHandler[interruptNumber] == this){
        interruptManager->interruptHandler[interruptNumber] = 0;
    }
}

uint32_t InterruptHandler::handleInterrupt(uint32_t esp){
    return esp;
}


InterruptManager::GateDescriptor InterruptManager::interruptDescriptorTable[256];
InterruptManager::InterruptDescriptorTablePointer InterruptManager::idt;
InterruptManager* InterruptManager::activeInterruptManager = 0;


void InterruptManager::setInterruptDescriptorEntry(
    uint8_t interruptNumber,
    uint16_t codeSegmentSelector,
    void (*handler)(),
    uint8_t descriptorPriviledgeLevel,
    uint8_t descriptorType
){
    uint32_t handlerAddr = (uint32_t) handler;
    interruptDescriptorTable[interruptNumber].handlerAddressLowerBits = handlerAddr & 0xFFFF;
    interruptDescriptorTable[interruptNumber].gdt_codeSegmentSelector = codeSegmentSelector;   // weather the interrupt handles in kernel or user mode
    interruptDescriptorTable[interruptNumber].reserved = 0;
    // descriptorType should already encode present (0x80), privilege and gate type (0x0E for 32-bit interrupt gate).
    interruptDescriptorTable[interruptNumber].access = (0x80) | ((descriptorPriviledgeLevel & 0x3) << 5) | (descriptorType & 0x1F);
    interruptDescriptorTable[interruptNumber].handlerAddressHighBits = (handlerAddr >> 16) & 0xFFFF;
}

InterruptManager::InterruptManager(TaskManager* taskManager)
:   picMasterCommand(0x20),  
    picMasterData(0x21),
    picSlaveCommand(0xA0),
    picSlaveData(0xA1)
{
    this->taskManager = taskManager;
    uint16_t codeSegment = 0x08;
    const uint8_t IDT_INTERRUPT_GATE = 0xE;

    for(uint16_t interrupt = 0; interrupt < 256; interrupt++){
        interruptHandler[interrupt] = 0;
        setInterruptDescriptorEntry(interrupt, codeSegment, &ignoreInterruptRequest, 0, IDT_INTERRUPT_GATE);
    }

    setInterruptDescriptorEntry(0x20, codeSegment, &handleInterruptRequest0x00, 0, IDT_INTERRUPT_GATE);
    setInterruptDescriptorEntry(0x21, codeSegment, &handleInterruptRequest0x01, 0, IDT_INTERRUPT_GATE);
    setInterruptDescriptorEntry(0x2C, codeSegment, &handleInterruptRequest0x0C, 0, IDT_INTERRUPT_GATE);    

    picMasterCommand.write(0x11);   // start initialization for master
    picSlaveCommand.write(0x11);    // start initialization for slave

    picMasterData.write(0x20);      // Remap master IRQs to 0x20–0x27
    picSlaveData.write(0x28);       // Remap slave IRQs to 0x28–0x2F

    picMasterData.write(0x04);      // Tell master there is a slave at IRQ2
    picSlaveData.write(0x02);       // Tell slave there is cascade identity

    picMasterData.write(0x01);      // set 8086/88 mode for master
    picSlaveData.write(0x01);       // set 8086/88 mode for the slave

    picMasterData.write(0x00);      // Unmask all IRQs (optional)
    picSlaveData.write(0x00);       // Unmask all IRQs (optional)

    idt.size = sizeof(interruptDescriptorTable) - 1;
    idt.base = (unsigned int)&interruptDescriptorTable;

    asm volatile ("lidt %0": :"m" (idt));  
}

InterruptManager::~InterruptManager(){

}

void InterruptManager::activate(){
    if(activeInterruptManager != 0)
        activeInterruptManager->deactivate();
    
    activeInterruptManager = this;
    asm("sti");
}

void InterruptManager::deactivate(){
    if(activeInterruptManager == this){
        activeInterruptManager = 0;
        asm("cli");
    }
}

uint32_t InterruptManager::handleInterrupt(uint32_t interruptnumber, uint32_t esp){
    if(activeInterruptManager != 0){
        esp = activeInterruptManager->doHandleInterrupt(interruptnumber, esp);
    }
    return esp;
}

uint32_t InterruptManager::doHandleInterrupt(uint32_t interruptNumber, uint32_t esp){

    if(interruptHandler[interruptNumber] != 0){
        esp = interruptHandler[interruptNumber]->handleInterrupt(esp);
    }else if(interruptNumber != 0x20){
        char* msg = "UNHANDLED INTERRUPT 0x00\n\n";
        char* hex = "0123456789ABCDEF";
        msg[22] = hex[(interruptNumber >> 4) & 0x0F];
        msg[23] = hex[interruptNumber & 0x0F];
        print(msg);
    }

    if (interruptNumber == 0x20) {
        // multitasking timer interrupt
        // we can perform task switching here
        esp = (uint32_t)taskManager->schedule((CPUState*)esp);
    }

    if(0x20 <= interruptNumber && 0x30 >= interruptNumber){
        picMasterCommand.write(0x20);
        if(interruptNumber >= 0x28){
            picSlaveCommand.write(0x20);
        }
    }
    return esp;
}

void InterruptManager::ignoreInterruptRequest(){
    
}