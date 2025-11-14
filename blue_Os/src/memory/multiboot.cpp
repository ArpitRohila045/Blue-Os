#include <memory/multiboot.h>
#include <common/types.h>
#include <common/print.h>


using namespace blueOs::memory;
using namespace blueOs::common;

extern uint32_t _kernel_start;   // Provided by linker script
extern uint32_t _kernel_end;     // Provided by linker script

/*
 * Function: meminfo
 * -----------------
 *  Displays a complete memory layout on screen, showing
 *  BIOS, GRUB, Kernel, and usable (free) memory.
 *
 *  Parameters:
 *      mb_info - Pointer to the Multiboot information structure
 *                 provided by GRUB at kernel entry.
 *
 *  Description:
 *      GRUB provides a memory map that lists all physical
 *      memory regions along with their type (usable/reserved/etc.).
 *
 *      This function:
 *          1. Displays known memory regions like BIOS and VGA.
 *          2. Displays the kernel memory range.
 *          3. Iterates through the GRUB-provided memory map.
 *          4. Calculates total free and reserved memory.
 *          5. Prints everything in a readable table format.
 */

void blueOs::memory::meminfo(multiboot_info* mb_info){
    print("\n================= MEMORY INFO =================\n");
    print("Region Name         | Start Addr | End Addr   | Status\n");
    print("--------------------------------------------------------\n");

    // --------------------------------------------------------------------
    // 1. BIOS Area (0x00000000 - 0x0009FFFF)
    // --------------------------------------------------------------------
    // This region is reserved by the BIOS and contains:
    //  - Interrupt Vector Table (IVT)
    //  - BIOS Data Area (BDA)
    //  - Real-mode structures
    //  It’s always marked as “used”.
    // --------------------------------------------------------------------
    print("BIOS Area           | 0x00000000 | 0x0009FFFF | Used (BIOS)\n");



    // --------------------------------------------------------------------
    // 2. Video & ROM Area (0x000A0000 - 0x000FFFFF)
    // --------------------------------------------------------------------
    // This area typically includes:
    //  - VGA framebuffer memory
    //  - BIOS ROM and option ROMs
    //  Also permanently reserved.
    // --------------------------------------------------------------------
    print("Video/ROM           | 0x000A0000 | 0x000FFFFF | Used (ROM/VGA)\n");



    // --------------------------------------------------------------------
    // 3. Kernel Memory Region
    // --------------------------------------------------------------------
    // The kernel is loaded by GRUB usually at 1MB (0x00100000)
    // until the symbol `_kernel_end` from the linker script.
    // --------------------------------------------------------------------
    print("Kernel              | ");  
    print_hex32((uint32_t)&_kernel_start);
    print(" | ");
    print_hex32((uint32_t)&_kernel_end);
    print(" | Used (kernel)\n");


    // --------------------------------------------------------------------
    // 4. GRUB Memory Map Iteration
    // --------------------------------------------------------------------
    // The Multiboot structure contains a list of memory regions
    // reported by the BIOS. Each entry gives:
    //      - Base address
    //      - Length
    //      - Type (1 = usable, 2+ = reserved)
    //
    // Here, we’ll iterate over the list to:
    //   a) Print all regions.
    //   b) Calculate total free and used memory.
    // --------------------------------------------------------------------
    __uint64_t total_free = 0;
    __uint64_t total_used = 0;

    struct multiboot_mmap_entry* mmap = (struct multiboot_mmap_entry*)mb_info->mmap_addr;

    while((uint32_t)mmap < mb_info->mmap_addr + mb_info->mmap_length){
        uint32_t start = (uint32_t)mmap->base_addr;
        uint32_t end = start + (uint32_t)mmap->length;

        print("Memory Region        | ");
        print_hex32(start);
        print(" | ");
        print_hex32(end);
        print(" | ");

        // Decode type
        switch (mmap->type)
        {
            case 1: // Usable (Free RAM)
                print("Free\n");
                total_free += mmap->length;
                break;

            case 2: // Reserved (BIOS, device memory, etc.)
                print("Reserved\n");
                total_used += mmap->length;
                break;

            case 3: // ACPI reclaimable memory
                print("ACPI reclaim\n");
                total_used += mmap->length;
                break;

            case 4: // ACPI NVS (Non-Volatile Sleep memory)
                print("ACPI NVS\n");
                total_used += mmap->length;
                break;

            default: // Anything else
                print("Other\n");
                break;
        }

        // move to next entry
        mmap = (struct multiboot_mmap_entry*)((uint32_t)mmap + mmap->size + sizeof(mmap->size));
    }

    // --------------------------------------------------------------------
    // 5. Display Summary
    // --------------------------------------------------------------------
    print("--------------------------------------------------------\n");

    // Show kernel size (difference between start & end)
    print("Kernel Size: ");
    print_dec((&_kernel_end - &_kernel_start) / 1024);
    print(" KB\n");

    // Show free and used totals (converted from bytes to KB)
    print("Free Memory: ");
    print_dec(total_free / 1024);
    print(" KB\n");

    print("Used/Reserved: ");
    print_dec(total_used / 1024);
    print(" KB\n");

    print("========================================================\n");
}
