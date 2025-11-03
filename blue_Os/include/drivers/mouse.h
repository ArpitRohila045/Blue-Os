#ifndef __BLUE_OS__DRIVERS__MOUSE_H
#define __BLUE_OS__DRIVERS__MOUSE_H

#include <common/types.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/port.h>
#include <drivers/driver.h>

namespace blueOs{
    namespace drivers{

        class MouseDriver;
        // --------- Abstraction for Handlers-----------
        class MouseEventHandler {
            public:
                MouseEventHandler(MouseDriver*);
                virtual void onMouseUp(uint8_t);
                virtual void onMouseDown(uint8_t);
                virtual void onMouseMove(int, int);
        };

        class MouseDriver : public blueOs::hardwarecommunication::InterruptHandler, public blueOs::drivers::Driver{
            private:
                blueOs::hardwarecommunication::Port8Bit commandPort;
                blueOs::hardwarecommunication::Port8Bit dataPort;

                uint8_t buffer[3];      
                uint8_t offset;
                uint8_t button;
                MouseEventHandler mouseEventHandler;
            public :
                MouseDriver(blueOs::hardwarecommunication::InterruptManager* interruptManager);
                ~MouseDriver();
                virtual uint32_t handleInterrupt(uint32_t esp);
                virtual void activate();
        }; 
    }
}


#endif