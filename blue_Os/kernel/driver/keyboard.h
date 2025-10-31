#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <stdint.h>
#include "interrupts/interrupts.h"
#include "hardware/port.h"


class KeyboardDriver : public InterruptHandler{
    private:
        Port8bit commandPort;
        Port8bit dataPort;

    public :
        KeyboardDriver(InterruptManager* interruptManager);
        ~KeyboardDriver();
        virtual uint32_t handleInterrupt(uint32_t esp) override;
}; 

#endif