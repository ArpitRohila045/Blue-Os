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
    pushl $0
    jmp int_bottom
.endm

handleInterruptRequest 0x00
handleInterruptRequest 0x01
handleInterruptRequest 0x0C

int_bottom:
    # save registers
    #pusha
    #pushl %ds
    #pushl %es
    #pushl %fs
    #pushl %gs
    
    pushl %ebp
    pushl %edi
    pushl %esi

    pushl %edx
    pushl %ecx
    pushl %ebx
    pushl %eax

    # load ring 0 segment register
    # cld
    #mov $0x10, %eax
    #mov %eax, %eds
    #mov %eax, %ees

    # call C++ Handler
    pushl %esp
    push (interruptnumber)
    call _ZN6blueOs21hardwarecommunication16InterruptManager15handleInterruptEjj
    # add %esp, 6
    mov %eax, %esp # switch the stack

    # restore registers
    popl %eax
    popl %ebx
    popl %ecx
    popl %edx

    popl %esi
    popl %edi
    popl %ebp
    #pop %gs
    #pop %fs
    #pop %es
    #pop %ds
    #popa
    
    add $4, %esp


_ZN6blueOs21hardwarecommunication16InterruptManager22ignoreInterruptRequestEv:
    iret

.section .data
    interruptnumber: .byte 0