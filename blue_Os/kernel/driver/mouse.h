#ifndef __MOUSE_H
#define __MOUSE_H

#include "interrupts/interrupts.h"

#include <stdint.h>
#include "interrupts/interrupts.h"
#include "hardware/port.h"


class MouseDriver : public InterruptHandler{
    private:
        Port8bit commandPort;
        Port8bit dataPort;

        uint8_t buffer[3];
        uint8_t offset;
        uint8_t button;
    public :
        MouseDriver(InterruptManager* interruptManager);
        ~MouseDriver();
        virtual uint32_t handleInterrupt(uint32_t esp) override;
}; 


#endif