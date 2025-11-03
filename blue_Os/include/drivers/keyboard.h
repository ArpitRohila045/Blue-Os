#ifndef __BLUE_OS__DRIVERS__KEYBOARD_H
#define __BLUE_OS__DRIVERS__KEYBOARD_H

#include <common/types.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/port.h>
#include <drivers/driver.h>

namespace blueOs{
    namespace drivers{

        class KeyboardDriver;
        // --------- Abstraction for Handlers-----------
        class KeyboardEventHandler {
            public:
                KeyboardEventHandler(KeyboardDriver*);
                ~KeyboardEventHandler();
                virtual void onKeyLeft(char);
                virtual void onKeyRight(char);
                virtual void onKeyPress(char);
        };


        class KeyboardDriver : public blueOs::hardwarecommunication::InterruptHandler, public blueOs::drivers::Driver{
            private:
                blueOs::hardwarecommunication::Port8Bit commandPort;
                blueOs::hardwarecommunication::Port8Bit dataPort;
                KeyboardEventHandler keyboardEventHandler;
            public :
                KeyboardDriver(blueOs::hardwarecommunication::InterruptManager* interruptManager);
                ~KeyboardDriver();
                virtual uint32_t handleInterrupt(uint32_t esp);
                virtual void activate();
        };
    }
}
#endif