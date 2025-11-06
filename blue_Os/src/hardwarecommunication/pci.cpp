#include <hardwarecommunication/pci.h>
#include <drivers/driver.h>
#include <common/print.h>
#include <common/types.h>

using namespace blueOs::common;
using namespace blueOs::hardwarecommunication;
using namespace blueOs::drivers;

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
        | ((registerOffset & 0xFC));
    
    commandPort.write(id);
    uint32_t result = dataPort.read();
    return result >> (8 * (registerOffset % 4));
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
        | ((registerOffset & 0xFC));
    commandPort.write(id);
    dataPort.write(data);
}

bool PeripheralComponentInterconnectController::hasFunctions(blueOs::common::uint16_t bus, blueOs::common::uint16_t device){
    return read(bus, device, 0, 0x0E) & (1<<7);
}

void PeripheralComponentInterconnectController::selectDrivers(
    blueOs::drivers::DriverManager* driverManager, 
    blueOs::hardwarecommunication::InterruptManager* interrupts
){    

    for(uint16_t bus = 0; bus < 8; bus++){
        for(uint16_t device = 0; device < 32; device++){

            int numFunctions = hasFunctions(bus, device) ? 8 : 1;

            for(uint16_t function = 0; function < numFunctions; function++){

                PeripheralComponentInterconnectDeviceDescriptor deviceDescriptor = getDeviceDescriptor(bus, device, function);
                if(deviceDescriptor.vendor_id == 0x0000 || deviceDescriptor.vendor_id == 0xFFFF)
                    continue;

                for(uint8_t barNum = 0; barNum < 6; barNum++){
                    BaseAddressRegister baseAddressRegister = getBaseAddressRegister(bus, device, function, barNum);

                    if(baseAddressRegister.address && (baseAddressRegister.type == InputOutput))
                        deviceDescriptor.portBase = (uint32_t)baseAddressRegister.address;
                }
                
                print("PCI BUS ");
                print_hex(bus & 0xFF);
                
                print(", DEVICE ");
                print_hex(device & 0xFF);

                print(", FUNCTION ");
                print_hex(function & 0xFF);
                
                print(" = VENDOR ");
                print_hex((deviceDescriptor.vendor_id & 0xFF00) >> 8);
                print_hex(deviceDescriptor.vendor_id & 0xFF);
                print(", DEVICE ");
                print_hex((deviceDescriptor.device_id & 0xFF00) >> 8);
                print_hex(deviceDescriptor.device_id & 0xFF);
                print("\n");
            }
        }
    }
}

PeripheralComponentInterconnectDeviceDescriptor PeripheralComponentInterconnectController::getDeviceDescriptor(
    uint16_t bus, uint16_t device, uint16_t function
)
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

BaseAddressRegister PeripheralComponentInterconnectController::getBaseAddressRegister(
    uint16_t bus, uint16_t device, uint16_t function, uint16_t barNum
){
    BaseAddressRegister result;
    
    uint32_t headertype = read(bus, device, function, 0x0E) & 0x7F;
    int maxBARs = 6 - (4*headertype);
    if(barNum >= maxBARs)
        return result;
    
    uint32_t bar_value = read(bus, device, function, 0x10 + 4*barNum);
    result.type = (bar_value & 0x1) ? InputOutput : MemoryMapping;
    uint32_t temp;
    
    if(result.type == MemoryMapping)
    {
        switch((bar_value >> 1) & 0x3)
        {
            
            case 0: // 32 Bit Mode
            case 1: // 20 Bit Mode
            case 2: // 64 Bit Mode
                break;
        }
    }
    else // InputOutput
    {
        result.address = (uint8_t*)(bar_value & ~0x3);
        result.prefetchable = false;
    }
    return result;
}

Driver* getDriver(
    PeripheralComponentInterconnectDeviceDescriptor descriptor, 
    blueOs::hardwarecommunication::InterruptManager* interruptManager
){
    return 0;
}