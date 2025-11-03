Blue Os

=============================================
Project structure:

blue_Os/
│
├── boot/
│   └── loader.s
│
├── build/
│   └── kernel/
│       └── ...
│
├── iso/
│   └── boot/
│       ├── grub/
│       └── mykernel.bin
│
├── kernel/
│   ├── gdt/
│   │   ├── gdt.cpp
│   │   ├── gdt.h
│   │   └── gdt_flush.s
│   ├── hardware/
│   │   ├── port.cpp
│   │   └── port.h
│   ├── interrupts/
│   │   ├── interrupts.cpp
│   │   ├── interrupts.h
│   │   └── interrupttubs.s
│   ├── utils/
│   │   └── kernel.cpp
│   └── driver/
│       └── keyboard.cpp
│
├── linker.ld
├── Makefile
├── mykernel.bin
└── mykernel.iso

=======================================================

Requirements

# sudo apt-get install g++ binutils libc6-dev-i386
# sudo apt-get install VirtualBox grub-legacy xorriso