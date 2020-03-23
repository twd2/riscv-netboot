ARCH=riscv
PREFIX=riscv64-unknown-linux-gnu-
GCC=$(PREFIX)gcc
OBJCOPY=$(PREFIX)objcopy
OBJDUMP=$(PREFIX)objdump

DEFINES=-DPRINTF_DISABLE_SUPPORT_FLOAT -DPRINTF_DISABLE_SUPPORT_EXPONENTIAL \
        -DPRINTF_DISABLE_SUPPORT_LONG_LONG
CFLAGS=-fno-builtin -nostdlib -static -Wl,--gc-sections,--print-gc-sections -O2 -Wall \
       -mcmodel=medany -I./arch/$(ARCH)/ -I. $(DEFINES)

-include Makefile.config

HEADERS=$(wildcard *.h) $(wildcard arch/$(ARCH)/*.h)
SOURCES=$(wildcard *.c *.S) $(wildcard arch/$(ARCH)/*.c arch/$(ARCH)/*.S)
OBJECTS=$(patsubst %.c,%.o,$(wildcard *.c)) $(patsubst %.S,%.o,$(wildcard *.S)) \
        $(patsubst %.c,%.o,$(wildcard arch/$(ARCH)/*.c)) $(patsubst %.S,%.o,$(wildcard arch/$(ARCH)/*.S))

.PHONY: all
all: $(OBJECTS) firmware.elf firmware.bin firmware.coe

.PHONY: asm
asm: firmware.elf
	$(OBJDUMP) -xd $< | vi -

%.o: %.c $(HEADERS)
	$(GCC) $(CFLAGS) -c $< -o $@

%.o: %.S $(HEADERS)
	$(GCC) $(CFLAGS) -c $< -o $@

%.elf: $(OBJECTS) linker.ld
	$(GCC) -Tlinker.ld $(CFLAGS) $^ -o $@

%.bin: %.elf
	$(OBJCOPY) -S --adjust-vma -0x60000000 -O binary $< $@

%.coe: %.bin
	python3 bin2coe.py $< $@

.PHONY: clean
clean:
	-rm *.o *.elf *.bin *.coe arch/*/*.o
