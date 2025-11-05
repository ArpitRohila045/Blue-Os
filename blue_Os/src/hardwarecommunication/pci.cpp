#include <hardwarecommunication/pci.h>
#include <common/print.h>
#include <common/types.h>

using namespace blueOs::common;
using namespace blueOs::hardwarecommunication;

PeripheralComponentInterconnectDeviceDescriptor::PeripheralComponentInterconnectDeviceDescriptor(){}

PeripheralComponentInterconnectDeviceDescriptor::~PeripheralComponentInterconnectDeviceDescriptor(){}

PeripheralComponentInterconnectController::PeripheralComponentInterconnectController()
:   dataPort(0xCFC),
    commandPort(0xCF8)
{}

PeripheralComponentInterconnectController::~PeripheralComponentInterconnectController(){}

uint32_t PeripheralComponentInterconnectController::read(
    uint16_t bus,
    uint16_t device,
    uint16_t function,
    uint32_t registerOffset
){
    /*
        The function reads first 32 bit configuration space of a device
        with some offset register, which is byte specific. 
        (0xCF8) is named CONFIG_ADDRESS
    */

    uint32_t id =
        0x1 << 31
        | ((bus & 0xFF) << 16)
        | ((device & 0x1F) << 11)
        | ((function & 0x0F) << 8)
        | (registerOffset);
    
    commandPort.write(id);
    blueOs::common::uint32_t result = dataPort.read();
    return result;
}     

uint32_t PeripheralComponentInterconnectController::readByte(
    blueOs::common::uint16_t bus,
    blueOs::common::uint16_t device,
    blueOs::common::uint16_t function,
    blueOs::common::uint32_t registerOffset
){
    uint32_t id =
        0x1 << 31
        | ((bus & 0xFF) << 16)
        | ((device & 0x1F) << 11)
        | ((function & 0x0F) << 8)
        | ((registerOffset & 0xFC));
    
    commandPort.write(id);
    uint32_t result = dataPort.read();
    return result >> (8 * (registerOffset % 4));
}

void PeripheralComponentInterconnectController::write(
    blueOs::common::uint16_t bus,
    blueOs::common::uint16_t device,
    blueOs::common::uint16_t function,
    blueOs::common::uint32_t registerOffset,
    blueOs::common::int32_t data
){  
    /*
        This function writes 32 bit data on the port (0xCFC) is called CONFIG_DATA
    */
    uint32_t id =
        0x1 << 31
        | ((bus & 0xFF) << 16)
        | ((device & 0x1F) << 11)
        | ((function & 0x0F) << 8)
        | ((registerOffset << 0xFC));
    commandPort.write(id);
    dataPort.write(data);
}

bool PeripheralComponentInterconnectController::hasFunctions(blueOs::common::uint16_t bus, blueOs::common::uint16_t device){
    return read(bus, device, 0, 0x0E) & (1<<7);
}

void PeripheralComponentInterconnectController::selectDrivers(blueOs::drivers::DriverManager* driverManager){    
    for(int bus = 0; bus < 8; bus++)
    {
        for(int device = 0; device < 32; device++)
        {
            int numFunctions = hasFunctions(bus, device) ? 8 : 1;
            for(int function = 0; function < numFunctions; function++)
            {
                PeripheralComponentInterconnectDeviceDescriptor dev = GetDeviceDescriptor(bus, device, function);
                
                if(dev.vendor_id == 0x0000 || dev.vendor_id == 0xFFFF)
                    continue;

                
                print("PCI BUS ");
                print_hex(bus & 0xFF);
                
                print(", DEVICE ");
                print_hex(device & 0xFF);

                print(", FUNCTION ");
                print_hex(function & 0xFF);
                
                print(" = VENDOR ");
                print_hex((dev.vendor_id & 0xFF00) >> 8);
                print_hex(dev.vendor_id & 0xFF);
                print(", DEVICE ");
                print_hex((dev.device_id & 0xFF00) >> 8);
                print_hex(dev.device_id & 0xFF);
                print("\n");
            }
        }
    }
}

PeripheralComponentInterconnectDeviceDescriptor PeripheralComponentInterconnectController::GetDeviceDescriptor(uint16_t bus, uint16_t device, uint16_t function)
{
    PeripheralComponentInterconnectDeviceDescriptor result;
    
    result.bus = bus;
    result.device = device;
    result.function = function;
    
    result.vendor_id = read(bus, device, function, 0x00);
    result.device_id = read(bus, device, function, 0x02);

    result.class_id = read(bus, device, function, 0x0b);
    result.subclass_id = read(bus, device, function, 0x0a);
    result.interface_id = read(bus, device, function, 0x09);

    result.revision = read(bus, device, function, 0x08);
    result.interrupt = read(bus, device, function, 0x3c);
    
    return result;
}

