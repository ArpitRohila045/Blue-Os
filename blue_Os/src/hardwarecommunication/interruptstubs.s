.set IRQ_BASE, 0x20

.section .text

.extern _ZN6blueOs21hardwarecommunication16InterruptManager15handleInterruptEjj
.global _ZN16InterruptManager22ignoreInterruptRequestEv

.macro handleException number
.global _ZN6blueOs21hardwarecommunication16InterruptManager16handleException\number\()Ev
_ZN6blueOs21hardwarecommunication16InterruptManager16handleException\number\()Ev:
    movb $\number, interruptnumber
    jmp int_bottom
.endm

.macro handleInterruptRequest number
.global _ZN6blueOs21hardwarecommunication16InterruptManager26handleInterruptRequest\number\()Ev
_ZN6blueOs21hardwarecommunication16InterruptManager26handleInterruptRequest\number\()Ev:
    movb $\number + IRQ_BASE, interruptnumber
    jmp int_bottom
.endm

handleInterruptRequest 0x00
handleInterruptRequest 0x01
handleInterruptRequest 0x0C

int_bottom:
    # Store the current state of the CPU registers
    pusha
    push %ds
    push %es
    push %fs
    push %gs

    pushl %esp
    pushl interruptnumber
    call _ZN6blueOs21hardwarecommunication16InterruptManager15handleInterruptEjj

    movl %eax, %esp

    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa

_ZN6blueOs21hardwarecommunication16InterruptManager22ignoreInterruptRequestEv:
    iret

.section .data
    interruptnumber: .byte 0