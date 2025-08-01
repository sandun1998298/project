# Morse Code Transceiver - AVR/Arduino UNO

A low-level AVR C implementation of a bidirectional Morse code transceiver for Arduino UNO using interrupt-driven timing and non-blocking operations.

## Features

- **Bidirectional Communication**: Encode text to Morse code and decode received Morse signals
- **Interrupt-Driven Design**: Uses timer interrupts for precise timing without blocking delays
- **UART Interface**: Serial communication for text input/output at 9600 baud
- **LCD Display**: 16x2 LCD for visual feedback and message display
- **Visual/Audio Output**: LED and buzzer for Morse code transmission
- **Button Input**: Manual Morse code input via push button
- **Automatic Mode Switching**: Switch between encode and decode modes via serial commands

## Hardware Requirements

### Components
- Arduino UNO (ATmega328P)
- 16x2 LCD Display
- Push Button
- LED
- Piezo Buzzer
- Breadboard and jumper cables
- Resistors (for LED current limiting and button pull-up)

### Pin Connections

| Component | Arduino Pin | AVR Pin | Description |
|-----------|-------------|---------|-------------|
| **LCD Display** | | | |
| RS | 7 | PD7 | Register Select |
| EN | 5 | PD5 | Enable |
| D4 | 4 | PD4 | Data bit 4 |
| D5 | 3 | PD3 | Data bit 5 |
| D6 | A2 | PC2 | Data bit 6 |
| D7 | A3 | PC3 | Data bit 7 |
| **Input/Output** | | | |
| Button | 2 | PD2 | Morse input (with external interrupt) |
| LED | 13 | PB5 | Visual Morse output |
| Buzzer | 6 | PD6 | Audio Morse output |
| **Communication** | | | |
| UART TX | 1 | PD1 | Serial transmission |
| UART RX | 0 | PD0 | Serial reception |

### Circuit Diagram Notes
- Connect LCD power (VCC to 5V, GND to ground)
- Add 10kΩ potentiometer for LCD contrast control (V0 pin)
- Use current-limiting resistor (~220Ω) for LED
- Button should be connected with pull-up resistor or use internal pull-up
- Buzzer can be connected directly (low current piezo type)

## Software Architecture

### Timing System
- **Timer0**: Configured for 1ms interrupts using CTC mode
- **Non-blocking delays**: Implemented using timer comparisons
- **Precise timing**: All Morse code timing based on interrupt-driven counters

### Morse Code Timing Standards
- **Dot duration**: 150ms
- **Dash duration**: 450ms (3 × dot time)
- **Intra-character gap**: 150ms (between dots/dashes)
- **Inter-character gap**: 450ms (between letters)
- **Word gap**: 1050ms (7 × dot time)

### Module Structure

```
├── main.c          # Main program loop and system coordination
├── morse.h/c       # Morse code encoding/decoding logic
├── uart.h/c        # UART communication with interrupt buffer
├── lcd.h/c         # LCD display interface (4-bit mode)
├── timer.h/c       # Interrupt-based timing system
└── Makefile        # AVR compilation and flashing
```

## Build and Flash Instructions

### Prerequisites
Install AVR development tools:

```bash
# Ubuntu/Debian
sudo apt-get install gcc-avr avr-libc avrdude

# macOS (with Homebrew)
brew tap osx-cross/avr
brew install avr-gcc avrdude

# Arch Linux
sudo pacman -S avr-gcc avr-libc avrdude
```

### Building
```bash
# Build the project
make

# Display size information
make size

# Clean build files
make clean
```

### Flashing to Arduino UNO
```bash
# Upload via default port (/dev/ttyACM0)
make upload

# Upload with specific port
make upload PORT=/dev/ttyUSB0

# Upload with verbose output for debugging
make upload-verbose
```

### Serial Monitoring
```bash
# Monitor serial output using screen
make monitor

# Alternative: using minicom
make monitor-minicom

# Manual serial connection (9600 baud)
screen /dev/ttyACM0 9600
```

## Usage Instructions

### Serial Commands
Connect to the device via serial terminal (9600 baud, 8N1):

- **`e` or `E`**: Switch to ENCODE mode
- **`d` or `D`**: Switch to DECODE mode
- **Type text**: In encode mode, type characters to transmit as Morse code
- **Press Enter**: Complete message transmission

### Operation Modes

#### Encode Mode
1. Switch to encode mode: Send `e` via serial
2. Type your message in the serial terminal
3. Each character is immediately encoded and transmitted via LED/buzzer
4. Press Enter to send end-of-message signal (AR: `.-.-.-`)
5. Message appears on LCD display

#### Decode Mode
1. Switch to decode mode: Send `d` via serial
2. Use the button to input Morse code:
   - **Short press** (< 300ms): Dot (.)
   - **Long press** (≥ 300ms): Dash (-)
3. Character timeout (800ms) automatically completes each letter
4. Word gaps (1050ms) add spaces between words
5. Decoded message appears on LCD and via serial

### Morse Code Reference

| Letter | Code | Letter | Code | Number | Code |
|--------|------|--------|------|--------|------|
| A | .- | N | -. | 0 | ----- |
| B | -... | O | --- | 1 | .---- |
| C | -.-. | P | .--. | 2 | ..--- |
| D | -.. | Q | --.- | 3 | ...-- |
| E | . | R | .-. | 4 | ....- |
| F | ..-. | S | ... | 5 | ..... |
| G | --. | T | - | 6 | -.... |
| H | .... | U | ..- | 7 | --... |
| I | .. | V | ...- | 8 | ---.. |
| J | .--- | W | .-- | 9 | ----. |
| K | -.- | X | -..- | | |
| L | .-.. | Y | -.-- | | |
| M | -- | Z | --.. | | |

## Technical Implementation Details

### Interrupt Service Routines
- **Timer0 Compare A**: 1ms system tick, timeout handling
- **UART RX**: Receive buffer management
- **External INT0**: Button press/release detection

### Memory Management
- **Atomic operations**: Used for shared variable access
- **Buffer sizes**: Optimized for ATmega328P memory constraints
- **Stack usage**: Minimized through careful function design

### Power Considerations
- **Sleep modes**: Can be added for battery operation
- **Pin configurations**: Optimized for low power consumption
- **Timer prescaling**: Balanced for accuracy vs. power usage

## Troubleshooting

### Common Issues

**Upload fails:**
- Check USB connection and port permissions
- Verify correct port in Makefile: `make upload PORT=/dev/ttyUSB0`
- Ensure Arduino UNO bootloader is present

**No serial output:**
- Verify baud rate (9600)
- Check TX/RX connections
- Ensure UART initialization is working

**LCD not displaying:**
- Verify all 6 data connections (RS, EN, D4-D7)
- Check power connections (VCC, GND)
- Adjust contrast potentiometer
- Verify timing delays in LCD initialization

**Morse timing issues:**
- Check timer frequency calculation
- Verify F_CPU definition (16000000UL)
- Ensure timer interrupts are enabled

**Button not responding:**
- Check pull-up resistor or enable internal pull-up
- Verify external interrupt configuration
- Test button debouncing

### Debug Tips
- Use `make upload-verbose` for detailed flash information
- Add debug output via UART for troubleshooting
- Check memory usage with `make size`
- Use `make disasm` to examine generated assembly code

## License

This project is provided as educational material for learning low-level AVR programming techniques.

## Contributing

Suggestions for improvements:
- Add EEPROM storage for message history
- Implement different speed settings
- Add support for prosigns and special characters
- Battery power management features
- Wireless transmission capabilities