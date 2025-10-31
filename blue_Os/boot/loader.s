.set MAGIC, 0x1badb002
.set FLAGS, (1<<0 | 1<<1)
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot
    .long MAGIC
    .long FLAGS
    .long CHECKSUM

.section .text
.global loader
.extern kernelMain

loader:
    mov $kernel_stack, %esp
    push %ebx            # multiboot info structure pointer
    push %eax            # magic number
    call kernelMain

halt:
    cli
    hlt
    jmp halt

.section .bss
.space 2*1024*1024
kernel_stack:
