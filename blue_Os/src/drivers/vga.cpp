#include<drivers/vga.h>

using namespace blueOs::common;
using namespace blueOs::drivers;



VideoGraphicsArray::VideoGraphicsArray():
        miscPort(0x3c2),
        crtcDataPort(0x3d4),
        crtcIndexPort(0x3d5),
        sequencerDataPort(0x3c4),
        sequencerIndexPort(0x3c5),
        graphicsControllerIndexPort(0x3ce),
        graphicsControllerDataPort(0x3cf),
        attributeControllerIndexPort(0x3c0),
        attributeControllerReadPort(0x3c1),
        attributeControllerWritePort(0x3c0),
        attributeControllerResetPort(0x3da)
        {
            }
            
            
            VideoGraphicsArray::~VideoGraphicsArray(){

            }
            
            void VideoGraphicsArray::WriteRegisters(uint8_t* registers){ //this function is to  initialize the ports.
                // --- Write to VGA Miscellaneous Output Register ---
                // Sets basic video mode parameters: clock, sync polarity, etc.
                miscPort.Write(*(registers++));


                // --- Program VGA Sequencer Registers ---
                // Controls how video memory is accessed and sequenced.
                // There are 5 sequencer registers (indices 0–4).
                for(uint8_t i = 0; i < 5; i++)
                {
                    sequencerIndexPort.write(i);       // Select sequencer register i
                    sequencerDataPort.write(*(registers++)); // Write data from mode table
                }


                // --- Unlock CRTC (Cathode Ray Tube Controller) registers ---
                // Some CRTC registers (like 0x03, 0x11) are write-protected.
                // To modify them, you must disable the protection bits first.
                crtcIndexPort.write(0x03);
                crtcDataPort.write(crtcDataPort.read() | 0x80);   // Set bit 7 to disable protection
                crtcIndexPort.write(0x11);
                crtcDataPort.write(crtcDataPort.read() & ~0x80);  // Clear bit 7 to unlock registers

                // Modify in-memory copy of register data accordingly
                registers[0x03] = registers[0x03] | 0x80;
                registers[0x11] = registers[0x11] & ~0x80;


                // --- Program CRTC Registers ---
                // These control screen timing, cursor position, and memory layout.
                // There are 25 CRTC registers (indices 0–24).
                for(uint8_t i = 0; i < 25; i++)
                {
                    crtcIndexPort.write(i);       // Select CRTC register i
                    crtcDataPort.write(*(registers++));  // Write corresponding data
                }


                // --- Program Graphics Controller Registers ---
                // These define how VRAM data maps to displayed pixels.
                // There are 9 graphics controller registers (indices 0–8).
                for(uint8_t i = 0; i < 9; i++)
                {
                    graphicsControllerIndexPort.write(i);       // Select graphics register i
                    graphicsControllerDataPort.write(*(registers++)); // Write data for mode setup
                }


                // --- Program Attribute Controller Registers ---
                // Controls color palettes, blinking, and display attributes.
                // Must read from 0x3DA before each write to reset flip-flop.
                for(uint8_t i = 0; i < 21; i++)
                {
                    attributeControllerResetPort.read();          // Reset flip-flop before writing
                    attributeControllerIndexPort.write(i);        // Select attribute register i
                    attributeControllerWritePort.write(*(registers++)); // Write data
                }

                // Finalize Attribute Controller setup by enabling video output
                attributeControllerResetPort.read();   // Reset flip-flop once more
                attributeControllerIndexPort.write(0x20); // Enable display (unblank screen)
                
            }

                        
            // Check if the VGA mode is supported (320x200, 256 colors)
            bool VideoGraphicsArray::SupportsMode(uint32_t width, uint32_t height, uint32_t colordepth){
                return width == 320 && height == 200 && colordepth == 8;
            }


            // Set VGA video mode to 320x200x256 (Mode 13h-like)
            bool VideoGraphicsArray::SetMode(uint32_t width, uint32_t height, uint32_t colordepth){

                // Verify that the requested mode is supported
                if(!SupportsMode(width, height, colordepth))
                    return false;

                // VGA register configuration for 320x200, 256-color mode
                unsigned char g_320x200x256[] =
                {
                    /* MISC */
                    0x63,
                    /* SEQ (Sequencer Registers) */
                    0x03, 0x01, 0x0F, 0x00, 0x0E,
                    /* CRTC (Cathode Ray Tube Controller) */
                    0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
                    0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    0x9C, 0x0E, 0x8F, 0x28, 0x40, 0x96, 0xB9, 0xA3,
                    0xFF,
                    /* GC (Graphics Controller) */
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
                    0xFF,
                    /* AC (Attribute Controller) */
                    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                    0x41, 0x00, 0x0F, 0x00, 0x00
                };
                
                // Write all these register values to VGA hardware
                WriteRegisters(g_320x200x256);

                return true;
            }


            // Determine which memory segment the VGA framebuffer is located in
            uint8_t* VideoGraphicsArray::GetFrameBufferSegment(){
                graphicsControllerIndexPort.write(0x06);             // Select Graphics Controller register 6
                uint8_t segmentNumber = graphicsControllerDataPort.read() & (3<<2);  // Read bits 3–2 (segment select)

                // Return the base memory address for the active VGA segment
                switch(segmentNumber)
                {
                    default:
                    case 0<<2: return (uint8_t*)0x00000;  // (usually unused)
                    case 1<<2: return (uint8_t*)0xA0000;  // standard VGA graphics memory
                    case 2<<2: return (uint8_t*)0xB0000;  // monochrome display memory
                    case 3<<2: return (uint8_t*)0xB8000;  // color text mode memory
                }            
            }


            // Draw a single pixel at (x, y) with a specific color index
            void VideoGraphicsArray::PutPixel(uint32_t x, uint32_t y, uint8_t colorIndex){
                // Bounds check: skip if outside the screen
                if(x < 0 || 320 <= x || y < 0 || 200 <= y)
                    return;
                
                // Calculate pixel position in video memory: (y * width + x)
                uint8_t* pixelAddress = GetFrameBufferSegment() + 320 * y + x;

                // Write the color index into video memory
                *pixelAddress = colorIndex;
            }


            // Convert an RGB triplet into a VGA color index (for 256-color palette)
            uint8_t VideoGraphicsArray::GetColorIndex(uint8_t r, uint8_t g, uint8_t b){
                if(r == 0x00 && g == 0x00 && b == 0x00) return 0x00; // black
                if(r == 0x00 && g == 0x00 && b == 0xA8) return 0x01; // blue
                if(r == 0x00 && g == 0xA8 && b == 0x00) return 0x02; // green
                if(r == 0xA8 && g == 0x00 && b == 0x00) return 0x04; // red
                if(r == 0xFF && g == 0xFF && b == 0xFF) return 0x3F; // white
                return 0x00; // default to black if color not found
            }


            // Draw a pixel by specifying RGB color (auto-converted to VGA index)
            void VideoGraphicsArray::PutPixel(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b){
                PutPixel(x, y, GetColorIndex(r, g, b)); // Map RGB → VGA index → write to screen
            }


