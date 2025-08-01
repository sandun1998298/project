# Circuit Connections for Morse Code Transceiver

## Arduino UNO Pin Mapping

```
     Arduino UNO                    Components
   ________________               ________________
  |                |             |                |
  | Digital Pins:  |             | LCD (16x2):    |
  | 0  (PD0) - RX  |------------>| (USB to PC)    |
  | 1  (PD1) - TX  |------------>| (USB to PC)    |
  | 2  (PD2) - INT0|------------>| Button Input   |
  | 3  (PD3) - D5  |------------>| LCD D5         |
  | 4  (PD4) - D4  |------------>| LCD D4         |
  | 5  (PD5) - EN  |------------>| LCD Enable     |
  | 6  (PD6) - PWM |------------>| Buzzer +       |
  | 7  (PD7) - RS  |------------>| LCD RS         |
  | 8  (PB0)       |             |                |
  | 9  (PB1) - PWM |             |                |
  | 10 (PB2) - PWM |             |                |
  | 11 (PB3) - PWM |             |                |
  | 12 (PB4)       |             |                |
  | 13 (PB5) - LED |------------>| External LED   |
  |                |             |                |
  | Analog Pins:   |             |                |
  | A0 (PC0)       |             |                |
  | A1 (PC1)       |             |                |
  | A2 (PC2) - D6  |------------>| LCD D6         |
  | A3 (PC3) - D7  |------------>| LCD D7         |
  | A4 (PC4) - SDA |             |                |
  | A5 (PC5) - SCL |             |                |
  |                |             |                |
  | Power:         |             |                |
  | 5V             |------------>| LCD VCC, Pullups|
  | 3.3V           |             |                |
  | GND            |------------>| Common Ground  |
  | VIN            |             |                |
  |________________|             |________________|
```

## Detailed Component Connections

### 1. LCD Display (16x2)
```
LCD Pin | Function | Arduino Pin | Wire Color (Suggested)
--------|----------|-------------|----------------------
1  VSS  | Ground   | GND         | Black
2  VDD  | Power    | 5V          | Red
3  V0   | Contrast | 10kΩ pot    | Green (to pot wiper)
4  RS   | Register | 7 (PD7)     | Yellow
5  RW   | Read/Write| GND        | Black (tie to ground)
6  EN   | Enable   | 5 (PD5)     | Orange
7  D0   | Data 0   | (unused)    | -
8  D1   | Data 1   | (unused)    | -
9  D2   | Data 2   | (unused)    | -
10 D3   | Data 3   | (unused)    | -
11 D4   | Data 4   | 4 (PD4)     | Blue
12 D5   | Data 5   | 3 (PD3)     | Purple
13 D6   | Data 6   | A2 (PC2)    | Gray
14 D7   | Data 7   | A3 (PC3)    | White
15 A    | Backlight| 5V + 220Ω   | Red (via resistor)
16 K    | Backlight| GND         | Black
```

### 2. Push Button (Morse Input)
```
Component | Connection
----------|------------------------------------------
Button Pin 1 | Arduino Pin 2 (PD2/INT0)
Button Pin 2 | GND
Pull-up      | 10kΩ resistor between Pin 2 and 5V
             | (or enable internal pull-up in code)
```

### 3. LED (Visual Morse Output)
```
Component | Connection
----------|------------------------------------------
LED Anode (+) | 220Ω resistor to Arduino Pin 13 (PB5)
LED Cathode (-) | GND
```

### 4. Piezo Buzzer (Audio Morse Output)
```
Component | Connection
----------|------------------------------------------
Buzzer + | Arduino Pin 6 (PD6)
Buzzer - | GND
```

## Breadboard Layout Suggestion

```
     5V Rail                          GND Rail
     +++++++                          -------
     
Row 1: [LCD VCC] ---- 5V
Row 2: [LCD VSS] ---- GND
Row 3: [LCD V0]  ---- 10kΩ Potentiometer
Row 4: [LCD RS]  ---- Arduino Pin 7
Row 5: [LCD EN]  ---- Arduino Pin 5
Row 6: [LCD D4]  ---- Arduino Pin 4
Row 7: [LCD D5]  ---- Arduino Pin 3
Row 8: [LCD D6]  ---- Arduino Pin A2
Row 9: [LCD D7]  ---- Arduino Pin A3

Row 15: [Button] ---- Arduino Pin 2 + 10kΩ to 5V
Row 16: [Button] ---- GND

Row 20: [LED +]  ---- 220Ω ---- Arduino Pin 13
Row 21: [LED -]  ---- GND

Row 25: [Buzzer +] ---- Arduino Pin 6
Row 26: [Buzzer -] ---- GND
```

## Power Supply Notes

- **Total Current**: ~50mA typical (LCD backlight is main consumer)
- **USB Power**: Sufficient for this project
- **External Power**: 7-12V via barrel jack if needed
- **Battery Option**: 9V battery with voltage regulator for portable use

## Component Specifications

### LCD Display
- Type: HD44780 compatible 16x2 character LCD
- Operating Voltage: 5V
- Current: ~20mA (without backlight), ~150mA (with backlight)

### Push Button
- Type: Momentary tactile switch
- Rating: 50mA @ 12V DC minimum
- Recommended: 6mm x 6mm through-hole tactile switch

### LED
- Type: Standard 5mm LED (any color)
- Forward Voltage: ~2V (red), ~3V (blue/white)
- Current: 20mA maximum, 10-15mA recommended

### Buzzer
- Type: Piezo buzzer (passive or active)
- Operating Voltage: 3-5V
- Current: 10-30mA typical

### Resistors
- LED current limiting: 220Ω (for 5V, ~15mA LED current)
- Button pull-up: 10kΩ (can use internal pull-up instead)
- LCD backlight limiting: 220Ω (if using LED backlight)

## Troubleshooting Hardware

### LCD Issues
1. **No display**: Check power connections (VCC, GND)
2. **Garbled text**: Verify data line connections (D4-D7)
3. **No contrast**: Adjust 10kΩ potentiometer
4. **Timing issues**: Check EN and RS connections

### Button Issues
1. **No response**: Check connection to Pin 2 (PD2)
2. **Erratic behavior**: Add/check pull-up resistor
3. **Bouncing**: Software debouncing implemented in code

### LED/Buzzer Issues
1. **LED not lighting**: Check polarity and current-limiting resistor
2. **Buzzer not working**: Verify connection to Pin 6 (PD6)
3. **Weak output**: Check power supply current capability

### General Debug
1. **Nothing works**: Check 5V and GND connections
2. **Intermittent issues**: Check breadboard connections
3. **Programming issues**: Verify USB connection and drivers