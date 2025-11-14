#include <drivers/ata.h>
#include <hardwarecommunication/port.h>
#include <common/print.h>
#include <common/types.h>

using namespace blueOs::common;
using namespace blueOs::drivers;
using namespace blueOs::hardwarecommunication;


/*
 * Constructor for ATA device.
 * portBase = 0x1F0 (Primary) OR 0x170 (Secondary)
 */
AdvanceTechnologyAttachment::AdvanceTechnologyAttachment(bool master, common::uint16_t portBase)
:   dataPort(portBase),
    errorPort(portBase + 1),
    sectorCountPort(portBase + 2),
    lbaLowPort(portBase + 3),
    lbaMidPort(portBase + 4),
    lbaHighPort(portBase + 5),
    devicePort(portBase + 6),
    commandPort(portBase + 7),

    // Control port for resetting device, alternate status register
    controlPort(portBase + 0x206)
{
    this->master = master;
}



AdvanceTechnologyAttachment::~AdvanceTechnologyAttachment()
{
}



/*
 * IDENTIFY COMMAND (0xEC)
 *
 * This checks:
 *  - Whether a drive exists
 *  - Whether it’s ATA or ATAPI
 *  - Drive parameters (model, serial, firmware)
 *
 * IDENTIFY returns 512 bytes = 256 words.
 */
void AdvanceTechnologyAttachment::identify(){
    
    // Select master/slave and enable LBA mode (0xA0 or 0xB0)
    devicePort.write(master ? 0xA0 : 0xB0);

    // Clear control port (disable interrupts)
    controlPort.write(0);

    // Read status to detect if no device exists
    uint32_t status = commandPort.read();
    if(status == 0xFF){
        // No device detected (floating bus)
        return;
    }

    // Send IDENTIFY command
    sectorCountPort.write(0);
    lbaLowPort.write(0);
    lbaMidPort.write(0);
    lbaHighPort.write(0);
    commandPort.write(0xEC);  // IDENTIFY

    status = commandPort.read();
    if(status == 0x00)
        return; // Device does not exist

    // Busy wait until BSY=0 and ERR=0
    while(((status & 0x80) == 0x80)  // BSY bit
       && ((status & 0x01) != 0x01)) // ERR bit
        status = commandPort.read();
        
    if(status & 0x01) // ERR bit
    {
        print("ERROR");
        return;
    }
    
    // Read 256 words of IDENTIFY data (512 bytes)
    for(int i = 0; i < 256; i++)
    {
        uint16_t data = dataPort.read();

        // ATA strings are transmitted in swapped byte order
        char *text = "  ";
        text[0] = (data >> 8) & 0xFF;
        text[1] = data & 0xFF;

        print(text);
    }
    print("\n");
}



/*
 * READ SECTOR (LBA28 + PIO)
 *
 * Reads 1 sector (512 bytes)
 */
void AdvanceTechnologyAttachment::read28(common::uint32_t sectorNum, int count)
{
    // LBA28 supports max 28 bits = 0x0FFFFFFF sectors
    if(sectorNum > 0x0FFFFFFF)
        return;
    
    // Set device + highest 4bits of LBA
    devicePort.write( (master ? 0xE0 : 0xF0) |
                     ((sectorNum & 0x0F000000) >> 24) );

    errorPort.write(0);     // Clear errors
    sectorCountPort.write(1); // Read 1 sector

    // Write LBA low/mid/high bytes
    lbaLowPort.write(  sectorNum & 0xFF );
    lbaMidPort.write( (sectorNum >> 8) & 0xFF );
    lbaHighPort.write((sectorNum >> 16) & 0xFF);

    // Issue READ command
    commandPort.write(0x20);

    // Wait while device is busy
    uint8_t status = commandPort.read();
    while(((status & 0x80) == 0x80)   // BSY
       && ((status & 0x01) != 0x01)) // ERR
        status = commandPort.read();
        
    if(status & 0x01)
    {
        print("ERROR");
        return;
    }
    
    print("Reading ATA Drive: ");
    
    // Read 512 bytes = 256 words
    for(int i = 0; i < count; i += 2)
    {
        uint16_t wdata = dataPort.read();

        // Convert 16-bit word to 2 chars
        char *text = "  \0";
        text[0] = wdata & 0xFF;

        if(i+1 < count)
            text[1] = (wdata >> 8) & 0xFF;
        else
            text[1] = '\0';

        print(text);
    }    
    
    // If user requested fewer bytes, still drain FIFO
    for(int i = count + (count%2); i < 512; i += 2)
        dataPort.read();
}



/*
 * WRITE SECTOR (LBA28)
 */
void AdvanceTechnologyAttachment::write28(common::uint32_t sector, common::uint8_t* data, common::uint32_t count)
{
    if(sector > 0x0FFFFFFF)
        return;
    if(count > 512)         // Can't exceed 1 sector
        return;
    
    
    // Select drive and upper LBA bits
    devicePort.write( (master ? 0xE0 : 0xF0) | 
                     ((sector & 0x0F000000) >> 24) );

    errorPort.write(0);
    sectorCountPort.write(1);

    lbaLowPort.write(  sector & 0xFF );
    lbaMidPort.write( (sector >> 8) & 0xFF);
    lbaHighPort.write((sector >> 16) & 0xFF);

    // Issue WRITE SECTOR command
    commandPort.write(0x30);
    
    print("Writing to ATA Drive: ");

    // Write 512 bytes = 256 words
    for(int i = 0; i < count; i += 2)
    {
        uint16_t wdata = data[i];

        if(i+1 < count)
            wdata |= ((uint16_t)data[i+1]) << 8;

        dataPort.write(wdata);
        
        char *text = "  \0";
        text[0] = (wdata >> 8) & 0xFF;
        text[1] = wdata & 0xFF;

        print(text);
    }
    
    // Fill rest of sector with 0
    for(int i = count + (count%2); i < 512; i += 2)
        dataPort.write(0x0000);
} 



void AdvanceTechnologyAttachment::flush()
{
    // Select the device (Master/Slave)
    devicePort.write(master ? 0xE0 : 0xF0);

    // Send FLUSH CACHE command (0xE7)
    commandPort.write(0xE7);

    // Poll status
    uint8_t status = commandPort.read();

    // Wait for BSY=0 (bit 7 cleared)
    while (status & 0x80)   // BSY bit
        status = commandPort.read();

    // Check for ERR
    if (status & 0x01)      // ERR bit
    {
        print("ATA FLUSH ERROR\n");
        return;
    }

    print("ATA FLUSH Completed\n");
}
