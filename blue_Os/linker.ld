/*
 * =============================================================
 *  BlueOS Kernel Linker Script
 *  Target Architecture: i386 (32-bit)
 *  Output Format: ELF32
 *
 *  Purpose:
 *      Defines how kernel sections (.text, .data, .bss, etc.)
 *      are laid out in physical memory when GRUB loads the kernel.
 *
 *      GRUB loads the kernel into memory at 1MB (0x00100000),
 *      so this script aligns all sections accordingly.
 * =============================================================
 */

ENTRY(loader)                  /* The entry symbol where execution begins (your loader or kernel_main) */
OUTPUT_FORMAT(elf32-i386)      /* Generate a 32-bit ELF file compatible with GRUB */
OUTPUT_ARCH(i386:i386)         /* Specify i386 architecture */


/* -------------------------------------------------------------
 * Section Layout in Memory
 * -------------------------------------------------------------
 * Notes:
 *   - GRUB loads your kernel starting at 1MB (0x00100000).
 *   - The '.' symbol is the current memory address location counter.
 *   - _kernel_start and _kernel_end are linker symbols you can use
 *     in your C/C++ kernel to know where the kernel resides in memory.
 * -------------------------------------------------------------
 */
SECTIONS
{
    /* ---------------------------------------------------------
     * Start placing sections at 1 MB (0x00100000)
     * ---------------------------------------------------------
     * This is the conventional load address for kernels.
     * It keeps us above the real-mode memory area (below 1MB)
     * which is reserved for BIOS, IVT, and BDA.
     * ---------------------------------------------------------
     */
    . = 0x0100000;
    . = 1M;

    /* Mark the physical start of the kernel in memory */
    _kernel_start = .;

    /* ---------------------------------------------------------
     * TEXT SECTION (.text, .rodata)
     * ---------------------------------------------------------
     * Contains executable code and read-only data.
     * Includes:
     *   - Multiboot header (so GRUB recognizes the kernel)
     *   - Kernel code (.text)
     *   - Read-only constants (.rodata)
     * ---------------------------------------------------------
     */
    .text :
    {
        *(.multiboot)   /* Multiboot header must be first */
        *(.text*)       /* All executable code */
        *(.rodata)      /* Read-only data, strings, const vars */
    }

    /* ---------------------------------------------------------
     * DATA SECTION (.data)
     * ---------------------------------------------------------
     * Contains initialized global/static variables.
     * Also keeps C++ constructors (.init_array).
     * ---------------------------------------------------------
     */
    .data :
    {
        /* Constructor section markers (for C++) */
        start_ctors = .;
        KEEP(*(.init_array))                     /* Keep global constructors */
        KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*))) /* Sort by priority */
        end_ctors = .; 

        *(.data)    /* All initialized global/static variables */
    }

    /* ---------------------------------------------------------
     * BSS SECTION (.bss)
     * ---------------------------------------------------------
     * Contains uninitialized data (automatically zeroed at startup).
     * Includes static variables without explicit initializers.
     * ---------------------------------------------------------
     */
    .bss :
    {
        *(.bss)     /* All zero-initialized variables */
    }

    /* ---------------------------------------------------------
     * DISCARD SECTION
     * ---------------------------------------------------------
     * Discards unneeded sections to keep kernel size minimal.
     * Common discarded sections:
     *   - .fini_array* : C++ destructors (not needed in kernel)
     *   - .comment     : Compiler version strings
     * ---------------------------------------------------------
     */
    /DISCARD/ :
    {
        *(.fini_array*)
        *(.comment)
    }

    /* ---------------------------------------------------------
     * Kernel End Symbol
     * ---------------------------------------------------------
     * Marks the end of the kernel image in memory.
     * Useful for:
     *   - Determining kernel size
     *   - Allocating free memory region after kernel
     * ---------------------------------------------------------
     */
    _kernel_end = .;
}
