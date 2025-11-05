#ifndef _BLUE_OS_DRIVERS_VGS_H
#define _BLUE_OS_DRIVERS_VGS_H

#include<common/type.h>
#include<hardwawrecommunication/port.h>
#include<drivers/driver.h>

namesapce myos{
    namespace drivers{
        class VideoGraphicsArray{
        protected:
            //these are initillaziation ports
            hardwarecommunication::Port8Bit miscPort;
            hardwarecommunication::Port8Bit crtcIndexPort;
            hardwarecommunication::Port8Bit crtcDataPort;
            hardwarecommunication::Port8Bit sequencerIndexPort;
            hardwarecommunication::Port8Bit sequencerDataPort;
            hardwarecommunication::Port8Bit graphicsControllerIndexPort;
            hardwarecommunication::Port8Bit graphicsControllerDataPort;
            hardwarecommunication::Port8Bit attributeControllerIndexPort;
            hardwarecommunication::Port8Bit attributeControllerReadPort;
            hardwarecommunication::Port8Bit attributeControllerWritePort;
            hardwarecommunication::Port8Bit attributeControllerResetPort;

            //it will send these initiallization ports to the specific Ports
            void WriteRegisters(common::uint8_t* registers);
            //method to give correct offset for the segment
            common::uint8_t* GetFrameBufferSegment();

            virtual void PutPixel(common::uint32_t x, common::uint32_t y, common::uint8_t colorIndex); //8 bit color mode uses 256 entries of diff colors and colorIndex-> will get certain colors for the x,y index
            virtual common::uint8_t GetColorIndex(common::uint8_t r, common::uint8_t g, common::uint8_t b); //called by PutPixel to get colors

        public:
            VideoGraphicsArray();
            virtual ~VideoGraphicsArray();
            
            virtual bool SupportsMode(common::uint32_t width, common::uint32_t height, common::uint32_t colordepth);  //method to tell that the certain mode is supported or not
            virtual bool SetMode(common::uint32_t width, common::uint32_t height, common::uint32_t colordepth); //method to set the modes
            virtual void PutPixel(common::uint32_t x, common::uint32_t y, common::uint8_t r, common::uint8_t g, common::uint8_t b); //r,g,b colors to put on x and y co-ordinates 
            
        };
    }
}

#endif