#ifndef __BLUE_OS__DRIVERS__VGA_H
#define __BLUE_OS__DRIVERS__VGA_H

#include<common/types.h>
#include<hardwarecommunication/port.h>
#include<drivers/driver.h>

namespace blueOs{
    namespace drivers{
        class VideoGraphicsArray{
        protected:
            //these are initillaziation ports
            hardwarecommunication::Port8Bit miscPort;                     // Controls VGA misc output (clock select, sync polarity)
            hardwarecommunication::Port8Bit crtcIndexPort;                // Selects which CRT Controller register to access
            hardwarecommunication::Port8Bit crtcDataPort;                 // Reads/writes data to selected CRT Controller register
            hardwarecommunication::Port8Bit sequencerIndexPort;           // Selects which Sequencer register to access (for VGA timing)
            hardwarecommunication::Port8Bit sequencerDataPort;            // Reads/writes data to selected Sequencer register
            hardwarecommunication::Port8Bit graphicsControllerIndexPort;  // Selects which Graphics Controller register to access
            hardwarecommunication::Port8Bit graphicsControllerDataPort;   // Reads/writes data to selected Graphics Controller register
            hardwarecommunication::Port8Bit attributeControllerIndexPort; // Selects Attribute Controller register (color/attribute control)
            hardwarecommunication::Port8Bit attributeControllerReadPort;  // Reads data from Attribute Controller
            hardwarecommunication::Port8Bit attributeControllerWritePort; // Writes data to Attribute Controller
            hardwarecommunication::Port8Bit attributeControllerResetPort; // Resets flip-flop inside Attribute Controller (sync control)


            //it will send these initiallization ports to the specific Ports
            void WriteRegisters(common::uint8_t* registers);
            //method to give correct offset for the segment
            common::uint8_t* GetFrameBufferSegment();

            virtual common::uint8_t GetColorIndex(common::uint8_t r, common::uint8_t g, common::uint8_t b); //called by PutPixel to get colors
            
            public:
            VideoGraphicsArray();
            ~VideoGraphicsArray();
            
            virtual bool SupportsMode(common::uint32_t width, common::uint32_t height, common::uint32_t colordepth);  //method to tell that the certain mode is supported or not
            virtual bool SetMode(common::uint32_t width, common::uint32_t height, common::uint32_t colordepth); //method to set the modes
            virtual void PutPixel(common::uint32_t x, common::uint32_t y, common::uint8_t r, common::uint8_t g, common::uint8_t b); //r,g,b colors to put on x and y co-ordinates 
            virtual void PutPixel(common::uint32_t x, common::uint32_t y, common::uint8_t colorIndex); //8 bit color mode uses 256 entries of diff colors and colorIndex-> will get certain colors for the x,y index
            
            virtual void fillRectangle(common::uint32_t x, common::uint32_t y,common::uint32_t w, common::uint32_t h, common::uint8_t r, common::uint8_t g, common::uint8_t b);  
        };
    }
}

#endif