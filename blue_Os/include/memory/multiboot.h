#ifndef __BLUE_OS__MEMORY__MULTIBOOT_H
#define __BLUE_OS__MEMORY__MULTIBOOT_H

#include <common/types.h>

namespace blueOs{
    namespace memory{
        /*
        * ============================================================
        *  File: multiboot.h
        *  Purpose:
        *      Defines the structures provided by the GRUB bootloader
        *      following the Multiboot Specification.
        *
        *  Description:
        *      When GRUB loads your kernel, it passes control to the
        *      kernel entry point with two parameters:
        *          1. The magic number (0x2BADB002)
        *          2. A pointer to a multiboot_info structure
        *
        *      This structure describes:
        *          - Available memory
        *          - Boot modules
        *          - Kernel command line
        *          - Memory map (usable/reserved areas)
        *
        *      The kernel can use this info to detect available memory
        *      and properly initialize its memory manager.
        * ============================================================
        */


        /* ------------------------------------------------------------
        * Multiboot Memory Map Entry
        * ------------------------------------------------------------
        * Each entry describes a continuous region of physical memory.
        *
        * Fields:
        *   size        : The size of the entry (not including this field itself).
        *   base_addr   : The starting physical address of this memory region.
        *   length      : The total length (in bytes) of this region.
        *   type        : The memory type:
        *                    1 = Usable (free) RAM
        *                    2 = Reserved
        *                    3 = ACPI Reclaimable
        *                    4 = ACPI NVS
        *                    5 = Bad memory
        *
        * The GRUB-provided memory map can be accessed from
        * multiboot_info.mmap_addr and iterated over until
        * (mmap_addr + mmap_length).
        * ------------------------------------------------------------
        */
        struct multiboot_mmap_entry {
            common::uint32_t size;        // Size of this entry (excluding 'size' field itself)
            common::__uint64_t base_addr;   // Base physical address of the memory region
            common::__uint64_t length;      // Length of the region in bytes
            common::uint32_t type;        // Type of memory region (1 = free, others reserved)
        } __attribute__((packed));
        // 'packed' ensures no compiler-added padding (important for BIOS structure alignment)



        /* ------------------------------------------------------------
        * Multiboot Information Structure
        * ------------------------------------------------------------
        * This structure is passed to your kernel by GRUB.
        * It contains system information, memory size, boot modules,
        * and the BIOS memory map.
        *
        * Important Fields:
        *   flags       : Bitfield indicating which parts are valid.
        *                 (bit 0 = mem_lower/upper valid,
        *                  bit 6 = memory map valid, etc.)
        *   mem_lower   : Amount of lower memory (< 1MB) in KB.
        *   mem_upper   : Amount of upper memory (> 1MB) in KB.
        *   boot_device : BIOS device the OS was booted from.
        *   cmdline     : Address of kernel command line string.
        *   mods_count  : Number of modules (e.g. initrd) loaded by GRUB.
        *   mods_addr   : Address of the first module structure.
        *   syms[4]     : ELF section header info (if available).
        *   mmap_length : Total size (in bytes) of the memory map.
        *   mmap_addr   : Physical address of the first memory map entry.
        *
        * Many fields are optional depending on which bits in 'flags' are set.
        * ------------------------------------------------------------
        */
        struct multiboot_info {
            common::uint32_t flags;         // Flags indicating which fields are valid
            common::uint32_t mem_lower;     // Conventional memory below 1MB (in KB)
            common::uint32_t mem_upper;     // Extended memory above 1MB (in KB)
            common::uint32_t boot_device;   // BIOS boot device
            common::uint32_t cmdline;       // Physical address of kernel command line string
            common::uint32_t mods_count;    // Number of modules loaded by GRUB
            common::uint32_t mods_addr;     // Address of the first module structure
            common::uint32_t syms[4];       // ELF section headers or a.out symbols
            common::uint32_t mmap_length;   // Length (in bytes) of the memory map
            common::uint32_t mmap_addr;     // Address of the memory map entries
            // Additional optional fields may follow depending on Multiboot version
        };
        
        void meminfo(multiboot_info* mbinfo);
    }
}

#endif