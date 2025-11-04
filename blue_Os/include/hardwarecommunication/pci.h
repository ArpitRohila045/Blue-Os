#ifndef __BLUE_OS__HARDWARECOMMUNICATION__PCI_H
#define __BLUE_OS__HARDWARECOMMUNICATION__PCI_H
#include <hardwarecommunication/port.h>
#include <hardwarecommunication/interrupts.h>
#include <drivers/driver.h>
#include <common/print.h>
#include <common/types.h>

namespace blueOs{
    namespace hardwarecommunication{
        
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
                bool hasFunctions(blueOs::common::uint16_t bus, blueOs::common::uint16_t device);
                void selectDrivers(blueOs::drivers::DriverManager* driverManager);
                PeripheralComponentInterconnectDeviceDescriptor GetDeviceDescriptor(blueOs::common::uint16_t bus, blueOs::common::uint16_t device, blueOs::common::uint16_t function);        };
    }
}

#endif