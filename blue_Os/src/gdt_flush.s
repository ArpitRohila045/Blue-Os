.global gdt_flush

gdt_flush:
    movl 4(%esp), %eax       # Get pointer to GDT descriptor
    lgdt (%eax)              # Load GDT

    movw $0x10, %ax          # Data segment selector (2nd entry)
    movw %ax, %ds            # Load DS
    movw %ax, %es            # Load ES
    movw %ax, %fs            # Load FS
    movw %ax, %gs            # Load GS
    movw %ax, %ss            # Load SS

    ljmp $0x08, $.next       # Far jump to reload CS

.next:
    ret                      # Return

# End of gdt_flush.s
