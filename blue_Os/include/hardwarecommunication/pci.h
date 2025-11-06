#ifndef __BLUE_OS__HARDWARECOMMUNICATION__PCI_H
#define __BLUE_OS__HARDWARECOMMUNICATION__PCI_H
#include <hardwarecommunication/port.h>
#include <hardwarecommunication/interrupts.h>
#include <drivers/driver.h>
#include <common/print.h>
#include <common/types.h>

namespace blueOs{
    namespace hardwarecommunication{

        enum BaseAddressRegisterType
        {
            MemoryMapping = 0,
            InputOutput = 1
        };

        class BaseAddressRegister{
            public:
                bool prefetchable;
                blueOs::common::uint8_t* address;
                blueOs::common::uint32_t size;
                /*
                    The Type field of the Memory Space BAR Layout specifies the size of the base register and where in memory it can be mapped.
                    If it has a value of 0x0 then the base register is 32-bits wide and can be mapped anywhere in the 32-bit Memory Space.
                    A value of 0x2 means the base register is 64-bits wide and can be mapped anywhere in the 64-bit Memory Space 
                        (A 64-bit base address register consumes 2 of the base address registers available).
                    A value of 0x1 is reserved as of revision 3.0 of the PCI Local Bus Specification.
                    In earlier versions it was used to support memory space below 1MB (16-bit wide base register that can be mapped anywhere in the 16-bit Memory Space).
                */
                BaseAddressRegisterType type;
        };
        
        class PeripheralComponentInterconnectDeviceDescriptor{
            public:
                blueOs::common::uint32_t portBase;
                blueOs::common::uint32_t interrupt;

                blueOs::common::uint16_t bus;
                blueOs::common::uint16_t device;
                blueOs::common::uint16_t function;

                blueOs::common::uint16_t vendor_id;
                blueOs::common::uint16_t device_id;

                blueOs::common::uint8_t class_id;
                blueOs::common::uint8_t subclass_id;
                blueOs::common::uint8_t interface_id;

                blueOs::common::uint8_t revision;

                PeripheralComponentInterconnectDeviceDescriptor();
                ~PeripheralComponentInterconnectDeviceDescriptor();
        };

        class PeripheralComponentInterconnectController{
                blueOs::hardwarecommunication::Port32Bit dataPort;
                blueOs::hardwarecommunication::Port32Bit commandPort;
            public:
                PeripheralComponentInterconnectController();
                ~PeripheralComponentInterconnectController();

                blueOs::common::uint32_t read(
                    blueOs::common::uint16_t bus,
                    blueOs::common::uint16_t device,
                    blueOs::common::uint16_t function,
                    blueOs::common::uint32_t registerOffset
                );   

                blueOs::common::uint32_t readByte(
                    blueOs::common::uint16_t bus,
                    blueOs::common::uint16_t device,
                    blueOs::common::uint16_t function,
                    blueOs::common::uint32_t registerOffset
                );

                void write(
                    blueOs::common::uint16_t bus,
                    blueOs::common::uint16_t device,
                    blueOs::common::uint16_t function,
                    blueOs::common::uint32_t registerOffset,
                    blueOs::common::int32_t data
                );

                bool hasFunctions(
                    blueOs::common::uint16_t bus, blueOs::common::uint16_t device
                );

                void selectDrivers(
                    blueOs::drivers::DriverManager* driverManager,
                    blueOs::hardwarecommunication::InterruptManager* interrupts
                );

                PeripheralComponentInterconnectDeviceDescriptor getDeviceDescriptor(
                    blueOs::common::uint16_t bus, blueOs::common::uint16_t device, blueOs::common::uint16_t function
                );

                BaseAddressRegister getBaseAddressRegister(
                    blueOs::common::uint16_t bus,
                    blueOs::common::uint16_t device,
                    blueOs::common::uint16_t function,
                    blueOs::common::uint16_t barNum
                );

                blueOs::drivers::Driver* getDriver(
                    PeripheralComponentInterconnectDeviceDescriptor descriptor, 
                    blueOs::hardwarecommunication::InterruptManager* interruptManager
                );
            };
    }   
}

#endif