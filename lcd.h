#ifndef LCD_H
#define LCD_H

#include <stdint.h>

// LCD pin definitions (using Arduino digital pins)
// Connect to PORTD pins
#define LCD_RS_PIN      PD7    // Arduino pin 7
#define LCD_EN_PIN      PD5    // Arduino pin 5
#define LCD_D4_PIN      PD4    // Arduino pin 4
#define LCD_D5_PIN      PD3    // Arduino pin 3
#define LCD_D6_PIN      PC2    // Arduino pin A2
#define LCD_D7_PIN      PC3    // Arduino pin A3

// LCD commands
#define LCD_CLEAR           0x01
#define LCD_HOME            0x02
#define LCD_ENTRY_MODE      0x06
#define LCD_DISPLAY_OFF     0x08
#define LCD_DISPLAY_ON      0x0C
#define LCD_CURSOR_OFF      0x0C
#define LCD_CURSOR_ON       0x0E
#define LCD_BLINK_ON        0x0F
#define LCD_FUNCTION_SET    0x28  // 4-bit mode, 2 lines, 5x7 dots

// Function declarations
void lcd_init(void);
void lcd_command(uint8_t cmd);
void lcd_write_char(char c);
void lcd_write_string(const char* str);
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_clear(void);
void lcd_home(void);

// Low-level functions
void lcd_enable_pulse(void);
void lcd_write_4bits(uint8_t value);
void lcd_delay_us(uint16_t us);

#endif // LCD_H