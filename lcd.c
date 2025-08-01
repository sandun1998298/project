#include "lcd.h"
#include "timer.h"
#include <avr/io.h>

void lcd_init(void) {
    // Configure LCD pins as outputs
    DDRD |= (1 << LCD_RS_PIN) | (1 << LCD_EN_PIN) | (1 << LCD_D4_PIN) | (1 << LCD_D5_PIN);
    DDRC |= (1 << LCD_D6_PIN) | (1 << LCD_D7_PIN);
    
    // Initialize pins to low
    PORTD &= ~((1 << LCD_RS_PIN) | (1 << LCD_EN_PIN) | (1 << LCD_D4_PIN) | (1 << LCD_D5_PIN));
    PORTC &= ~((1 << LCD_D6_PIN) | (1 << LCD_D7_PIN));
    
    // Wait for LCD to power up
    lcd_delay_us(50000); // 50ms
    
    // Initialize in 4-bit mode
    // Send 0x03 three times
    lcd_write_4bits(0x03);
    lcd_delay_us(4500); // 4.5ms
    
    lcd_write_4bits(0x03);
    lcd_delay_us(4500); // 4.5ms
    
    lcd_write_4bits(0x03);
    lcd_delay_us(150); // 150us
    
    // Switch to 4-bit mode
    lcd_write_4bits(0x02);
    lcd_delay_us(150); // 150us
    
    // Configure LCD
    lcd_command(LCD_FUNCTION_SET);  // 4-bit mode, 2 lines, 5x7 dots
    lcd_command(LCD_DISPLAY_OFF);   // Display off
    lcd_command(LCD_CLEAR);         // Clear display
    lcd_command(LCD_ENTRY_MODE);    // Entry mode: increment cursor, no shift
    lcd_command(LCD_DISPLAY_ON);    // Display on, cursor off, blink off
}

void lcd_command(uint8_t cmd) {
    PORTD &= ~(1 << LCD_RS_PIN); // RS = 0 for command
    
    // Send upper 4 bits
    lcd_write_4bits(cmd >> 4);
    
    // Send lower 4 bits
    lcd_write_4bits(cmd & 0x0F);
    
    if (cmd == LCD_CLEAR || cmd == LCD_HOME) {
        lcd_delay_us(2000); // Clear and home commands take longer
    } else {
        lcd_delay_us(50); // Standard command delay
    }
}

void lcd_write_char(char c) {
    PORTD |= (1 << LCD_RS_PIN); // RS = 1 for data
    
    // Send upper 4 bits
    lcd_write_4bits(c >> 4);
    
    // Send lower 4 bits
    lcd_write_4bits(c & 0x0F);
    
    lcd_delay_us(50); // Character write delay
}

void lcd_write_string(const char* str) {
    while (*str) {
        lcd_write_char(*str++);
    }
}

void lcd_set_cursor(uint8_t row, uint8_t col) {
    uint8_t address;
    
    if (row == 0) {
        address = 0x80 + col; // First row starts at 0x80
    } else {
        address = 0xC0 + col; // Second row starts at 0xC0
    }
    
    lcd_command(address);
}

void lcd_clear(void) {
    lcd_command(LCD_CLEAR);
}

void lcd_home(void) {
    lcd_command(LCD_HOME);
}

void lcd_enable_pulse(void) {
    PORTD |= (1 << LCD_EN_PIN);   // EN = 1
    lcd_delay_us(1);              // Enable pulse width
    PORTD &= ~(1 << LCD_EN_PIN);  // EN = 0
    lcd_delay_us(1);              // Enable cycle time
}

void lcd_write_4bits(uint8_t value) {
    // Clear data pins
    PORTD &= ~((1 << LCD_D4_PIN) | (1 << LCD_D5_PIN));
    PORTC &= ~((1 << LCD_D6_PIN) | (1 << LCD_D7_PIN));
    
    // Set data pins according to value
    if (value & 0x01) PORTD |= (1 << LCD_D4_PIN);
    if (value & 0x02) PORTD |= (1 << LCD_D5_PIN);
    if (value & 0x04) PORTC |= (1 << LCD_D6_PIN);
    if (value & 0x08) PORTC |= (1 << LCD_D7_PIN);
    
    lcd_enable_pulse();
}

void lcd_delay_us(uint16_t us) {
    // Convert microseconds to milliseconds for timer delay
    // For small delays, use a simple loop
    if (us < 1000) {
        volatile uint16_t delay_count = us * 4; // Approximate delay loop
        while (delay_count--) {
            __asm__ __volatile__("nop");
        }
    } else {
        // Use timer for longer delays
        timer_delay_ms(us / 1000);
        
        // Handle remaining microseconds with loop
        volatile uint16_t remaining_us = us % 1000;
        volatile uint16_t delay_count = remaining_us * 4;
        while (delay_count--) {
            __asm__ __volatile__("nop");
        }
    }
}