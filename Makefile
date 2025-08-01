# AVR Makefile for Morse Code Transceiver
# Target: Arduino UNO (ATmega328P)

# Project name
PROJECT = morse_transceiver

# Target microcontroller
MCU = atmega328p

# Clock frequency (Arduino UNO uses 16MHz crystal)
F_CPU = 16000000UL

# Programmer (Arduino UNO uses Arduino bootloader)
PROGRAMMER = arduino

# Serial port (adjust as needed)
# Linux: usually /dev/ttyUSB0 or /dev/ttyACM0
# Windows: usually COM3, COM4, etc.
# macOS: usually /dev/cu.usbmodem* or /dev/cu.usbserial*
PORT = /dev/ttyACM0

# Baud rate for programming (Arduino UNO bootloader)
BAUD = 115200

# Compiler and tools
CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size
AVRDUDE = avrdude

# Compiler flags
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -Wextra -std=c99
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -ffunction-sections -fdata-sections

# Linker flags
LDFLAGS = -mmcu=$(MCU) -Wl,--gc-sections

# Source files
SOURCES = main.c morse.c uart.c lcd.c timer.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Target files
TARGET_ELF = $(PROJECT).elf
TARGET_HEX = $(PROJECT).hex
TARGET_MAP = $(PROJECT).map

# Default target
all: $(TARGET_HEX) size

# Compile source files to object files
%.o: %.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files to create ELF file
$(TARGET_ELF): $(OBJECTS)
	@echo "Linking $(TARGET_ELF)..."
	$(CC) $(LDFLAGS) -Wl,-Map,$(TARGET_MAP) -o $@ $^

# Create HEX file from ELF file
$(TARGET_HEX): $(TARGET_ELF)
	@echo "Creating $(TARGET_HEX)..."
	$(OBJCOPY) -O ihex -R .eeprom $< $@

# Display size information
size: $(TARGET_ELF)
	@echo "Size information:"
	$(SIZE) --format=avr --mcu=$(MCU) $<

# Upload program to microcontroller
upload: $(TARGET_HEX)
	@echo "Uploading to $(MCU) via $(PORT)..."
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -b $(BAUD) -U flash:w:$<:i

# Upload with verbose output
upload-verbose: $(TARGET_HEX)
	@echo "Uploading to $(MCU) via $(PORT) (verbose)..."
	$(AVRDUDE) -v -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -b $(BAUD) -U flash:w:$<:i

# Erase the microcontroller
erase:
	@echo "Erasing $(MCU)..."
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -b $(BAUD) -e

# Read fuses
fuses:
	@echo "Reading fuses from $(MCU)..."
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -b $(BAUD) -U lfuse:r:-:h -U hfuse:r:-:h -U efuse:r:-:h

# Disassemble the ELF file
disasm: $(TARGET_ELF)
	$(OBJDUMP) -d $< > $(PROJECT).asm

# Clean build files
clean:
	@echo "Cleaning build files..."
	rm -f $(OBJECTS) $(TARGET_ELF) $(TARGET_HEX) $(TARGET_MAP) $(PROJECT).asm

# Print help
help:
	@echo "Available targets:"
	@echo "  all          - Build the project (default)"
	@echo "  upload       - Upload program to microcontroller"
	@echo "  upload-verbose - Upload with verbose output"
	@echo "  erase        - Erase microcontroller flash"
	@echo "  fuses        - Read fuse settings"
	@echo "  size         - Display size information"
	@echo "  disasm       - Create disassembly file"
	@echo "  clean        - Remove build files"
	@echo "  help         - Show this help"
	@echo ""
	@echo "Configuration:"
	@echo "  MCU          = $(MCU)"
	@echo "  F_CPU        = $(F_CPU)"
	@echo "  PROGRAMMER   = $(PROGRAMMER)"
	@echo "  PORT         = $(PORT)"
	@echo "  BAUD         = $(BAUD)"
	@echo ""
	@echo "To change the serial port, use:"
	@echo "  make upload PORT=/dev/ttyUSB0"

# Monitor serial output
monitor:
	@echo "Monitoring serial output on $(PORT) at 9600 baud..."
	@echo "Press Ctrl+C to exit"
	screen $(PORT) 9600

# Alternative monitor using minicom
monitor-minicom:
	@echo "Monitoring serial output on $(PORT) at 9600 baud with minicom..."
	minicom -D $(PORT) -b 9600

# Phony targets
.PHONY: all upload upload-verbose erase fuses size disasm clean help monitor monitor-minicom