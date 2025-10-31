# ===== Compiler & Tools =====
ASPARAMS = --32
GCCPARAMS = -m32 -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -g
LDPARAMS = -m elf_i386

# ===== Project Directories =====
SRC_DIR := kernel
BOOT_DIR := boot
BUILD_DIR := build
ISO_DIR := iso
KERNEL_BIN := $(BUILD_DIR)/mykernel.bin
ISO_FILE := mykernel.iso

# ===== Includes =====
INCLUDES = -I$(SRC_DIR) -I$(SRC_DIR)/gdt -I$(SRC_DIR)/hardware -I$(SRC_DIR)/interrupts -I$(SRC_DIR)/utils -I$(SRC_DIR)/driver

# ===== Source File Discovery =====
CPP_SOURCES := $(shell find $(SRC_DIR) -name '*.cpp')
ASM_SOURCES := $(shell find $(BOOT_DIR) -name '*.s' -o -name '*.asm' -o -name '*.S')
ASM_SOURCES += $(shell find $(SRC_DIR) -name '*.s' -o -name '*.asm' -o -name '*.S')

OBJECTS := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(CPP_SOURCES))
OBJECTS += $(patsubst %.s, $(BUILD_DIR)/%.o, $(ASM_SOURCES))

# ===== Default Target =====
all: $(KERNEL_BIN)

# ===== Compilation Rules =====
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	gcc $(GCCPARAMS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/%.o: %.s
	@mkdir -p $(dir $@)
	as $(ASPARAMS) $< -o $@

# ===== Linking =====
$(KERNEL_BIN): linker.ld $(OBJECTS)
	ld $(LDPARAMS) -T $< -o $@ $(OBJECTS)

# ===== ISO Creation =====
$(ISO_FILE): $(KERNEL_BIN)
	mkdir -p $(ISO_DIR)/boot/grub
	cp $(KERNEL_BIN) $(ISO_DIR)/boot/mykernel.bin
	echo 'set timeout=0' > $(ISO_DIR)/boot/grub/grub.cfg
	echo 'set default=0' >> $(ISO_DIR)/boot/grub/grub.cfg
	echo 'menuentry "My Operating System" {' >> $(ISO_DIR)/boot/grub/grub.cfg
	echo '	multiboot /boot/mykernel.bin' >> $(ISO_DIR)/boot/grub/grub.cfg
	echo '	boot' >> $(ISO_DIR)/boot/grub/grub.cfg
	echo '}' >> $(ISO_DIR)/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO_FILE) $(ISO_DIR)

# ===== Run on QEMU =====
run: $(ISO_FILE)
	qemu-system-i386 -cdrom $(ISO_FILE)

# ===== Clean =====
clean:
	rm -rf $(BUILD_DIR) $(ISO_DIR) $(ISO_FILE)
