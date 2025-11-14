#ifndef __BLUE_OS__DRIVERS__ATA_H
#define __BLUE_OS__DRIVERS__ATA_H

#include <hardwarecommunication/port.h>

/*
    Advance Technology Attachment (ATA) driver class.

    This class abstracts low-level ATA PIO communication.
    It works with both Primary and Secondary channels,
    and can address either Master or Slave devices.

    In ATA PIO mode, the CPU communicates with the HDD/SSD
    through a set of 8-bit and 16-bit I/O ports:
    
    - Data port          (16-bit) : sector data transfer
    - Error / Features   (8-bit)
    - Sector Count       (8-bit)
    - LBA Low            (8-bit)
    - LBA Mid            (8-bit)
    - LBA High           (8-bit)
    - Device Select      (8-bit)
    - Command / Status   (8-bit)
    - Control port       (8-bit)

    The class supports:
      - IDENTIFY command (0xEC)
      - LBA28 read / write (up to 128GB)
      - Flush command (0xE7)
*/

namespace blueOs {
    namespace drivers {

        class AdvanceTechnologyAttachment {
        protected:

            /* 
                True  → Master device (0xE0)
                False → Slave device  (0xF0)

                The Master/Slave selection determines bit 4 of the
                Drive/Head register (sent to devicePort).
            */
            bool master;

            /*
                Port mappings (offsets from base I/O port):

                base + 0  → Data port (16-bit)
                base + 1  → Error / Features
                base + 2  → Sector count
                base + 3  → LBA low byte  (bits 0–7)
                base + 4  → LBA mid byte  (bits 8–15)
                base + 5  → LBA high byte (bits 16–23)
                base + 6  → Device select / LBA[24–27]
                base + 7  → Command / Status
                control   → usually base + 0x206 (alternate status)

                These ports allow the CPU to control the ATA drive
                entirely via PIO (Programmed I/O).
            */

            blueOs::hardwarecommunication::Port16Bit dataPort;
            blueOs::hardwarecommunication::Port8Bit  errorPort;
            blueOs::hardwarecommunication::Port8Bit  sectorCountPort;
            blueOs::hardwarecommunication::Port8Bit  lbaLowPort;
            blueOs::hardwarecommunication::Port8Bit  lbaMidPort;
            blueOs::hardwarecommunication::Port8Bit  lbaHighPort;
            blueOs::hardwarecommunication::Port8Bit  devicePort;
            blueOs::hardwarecommunication::Port8Bit  commandPort;
            blueOs::hardwarecommunication::Port8Bit  controlPort;

        public:

            /*
                Constructor:

                'master'  → choose Master (true) or Slave (false)
                'portBase' → base I/O port of channel:
                    Primary   = 0x1F0
                    Secondary = 0x170

                Initializes all ports based on hardware offsets.
            */
            AdvanceTechnologyAttachment(bool master, common::uint16_t portBase);

            ~AdvanceTechnologyAttachment();

            /*
                IDENTIFY (0xEC):

                Queries the drive for:
                    - capabilities
                    - total sectors (LBA28)
                    - model number
                    - supported modes

                The drive returns 256 words (512 bytes).
                Must wait for BSY=0 and DRQ=1 before reading.
            */
            void identify();

            /*
                LBA28 READ:

                - 'sector' = starting LBA sector number (0–2^28).
                - 'count' = how many bytes to read (default = 512 bytes = 1 sector)

                Performs:
                    - Write sector count
                    - Write LBA bytes
                    - Select device (Master/Slave + LBA mode)
                    - Issue command 0x20 (READ SECTORS)
                    - Wait for drive to become ready
                    - Read 256 words from data port

                Uses polling-based PIO read.
            */
            void read28(common::uint32_t sector, int count = 512);

            /*
                LBA28 WRITE:

                - 'sector' = starting LBA sector
                - 'data' = pointer to 512 bytes
                - 'count' = number of bytes to write (normally 512)

                Sequence:
                    - Set sector count
                    - Set LBA registers
                    - Set device register
                    - Command 0x30 (WRITE SECTORS)
                    - Wait for DRQ=1
                    - Write 256 words to data port

                Note: real hardware requires FLUSH after writes.
            */
            void write28(common::uint32_t sector, common::uint8_t* data, common::uint32_t count);

            /*
                FLUSH (0xE7):

                Ensures all buffered data in the drive is physically
                written to the platters (or flash, for SSDs).

                Required after write operations.
            */
            void flush();
        };
    }
}

#endif
