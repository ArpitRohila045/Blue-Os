# ==== BUILD CONFIGURATION ====
ASPARAMS = --32
GCCPARAMS = -m32 -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -g
LDPARAMS = -m elf_i386

INCLUDES = -I. -Igdt -Iinterrupts -Iprint -Ihardware -Idriver 

SRC_DIR := .
BUILD_DIR := build
ISO_DIR := iso
ISO_FILE := mykernel.iso
KERNEL := mykernel.bin
VM_NAME := MyOS

# ==== SOURCE DISCOVERY ====
# Find all .cpp and .s files recursively from the current directory
CPP_SOURCES := $(shell find $(SRC_DIR) -name '*.cpp')
ASM_SOURCES := $(shell find $(SRC_DIR) -name '*.s')
SOURCES := $(CPP_SOURCES) $(ASM_SOURCES)

# Map source files to build directory object files
OBJECTS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(CPP_SOURCES))
OBJECTS += $(patsubst %.s,$(BUILD_DIR)/%.o,$(ASM_SOURCES))

# ==== BUILD RULES ====
all: $(KERNEL)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	gcc $(GCCPARAMS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/%.o: %.s
	@mkdir -p $(dir $@)
	as $(ASPARAMS) $< -o $@

$(KERNEL): linker.ld $(OBJECTS)
	ld $(LDPARAMS) -T $< -o $@ $(OBJECTS)

# ==== INSTALL & ISO ====
install: $(KERNEL)
	sudo cp $< /boot/$(KERNEL)

$(ISO_FILE): $(KERNEL)
	@mkdir -p $(ISO_DIR)/boot/grub
	cp $< $(ISO_DIR)/boot/
	echo 'set timeout=0' > $(ISO_DIR)/boot/grub/grub.cfg
	echo 'set default=0' >> $(ISO_DIR)/boot/grub/grub.cfg
	echo 'menuentry "$(VM_NAME)" {' >> $(ISO_DIR)/boot/grub/grub.cfg
	echo '	multiboot /boot/$(KERNEL)' >> $(ISO_DIR)/boot/grub/grub.cfg
	echo '	boot' >> $(ISO_DIR)/boot/grub/grub.cfg
	echo '}' >> $(ISO_DIR)/boot/grub/grub.cfg
	grub-mkrescue --output=$@ $(ISO_DIR)

# ==== RUN & DEBUG ====
run: $(ISO_FILE)
	qemu-system-i386 -cdrom $(ISO_FILE)

debug: $(ISO_FILE)
	@echo "Starting QEMU in debug mode..."
	qemu-system-i386 -cdrom $(ISO_FILE) -s -S &
	@echo "------------------------------------------"
	@echo "Run this in another terminal to start GDB:"
	@echo "  gdb $(KERNEL)"
	@echo "Then inside GDB:"
	@echo "  (gdb) target remote localhost:1234"
	@echo "  (gdb) symbol-file $(KERNEL)"
	@echo "  (gdb) continue"
	@echo "------------------------------------------"

# ==== CLEAN ====
clean:
	rm -rf $(BUILD_DIR) $(KERNEL) $(ISO_FILE) $(ISO_DIR)
