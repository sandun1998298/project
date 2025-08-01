#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <stdbool.h>
#include <string.h>

#include "morse.h"
#include "uart.h"
#include "lcd.h"
#include "timer.h"

// Function declarations
void gpio_init(void);
void clear_buffers(void);
void process_button_input(void);

// Global state variables
volatile uint8_t system_mode = MODE_ENCODE;
volatile char input_buffer[MAX_BUFFER_SIZE];
volatile uint8_t input_index = 0;
volatile char decoded_message[MAX_BUFFER_SIZE];
volatile uint8_t decoded_index = 0;
volatile bool message_ready = false;

// Button state for decoding
volatile uint8_t button_state = 0;
volatile uint32_t button_press_start = 0;
volatile uint32_t button_release_time = 0;
volatile bool button_pressed = false;

int main(void) {
    // Initialize all subsystems
    uart_init();
    lcd_init();
    timer_init();
    
    // Configure GPIO pins
    gpio_init();
    
    // Enable global interrupts
    sei();
    
    // Display welcome message
    lcd_clear();
    lcd_write_string("Morse Transceiver");
    lcd_set_cursor(1, 0);
    lcd_write_string("Mode: ENCODE");
    
    uart_send_string("Morse Code Transceiver Ready\r\n");
    uart_send_string("Commands: 'e' = encode, 'd' = decode\r\n");
    
    while (1) {
        // Check for mode change commands
        if (uart_data_available()) {
            char cmd = uart_receive_char();
            
            if (cmd == 'e' || cmd == 'E') {
                system_mode = MODE_ENCODE;
                lcd_set_cursor(1, 6);
                lcd_write_string("ENCODE  ");
                uart_send_string("Switched to ENCODE mode\r\n");
                clear_buffers();
            }
            else if (cmd == 'd' || cmd == 'D') {
                system_mode = MODE_DECODE;
                lcd_set_cursor(1, 6);
                lcd_write_string("DECODE  ");
                uart_send_string("Switched to DECODE mode\r\n");
                clear_buffers();
            }
            else if (system_mode == MODE_ENCODE && cmd >= 32 && cmd <= 126) {
                // Printable character for encoding
                if (input_index < MAX_BUFFER_SIZE - 1) {
                    input_buffer[input_index++] = cmd;
                    input_buffer[input_index] = '\0';
                    
                    // Echo character
                    uart_send_char(cmd);
                    
                    // Encode and transmit the character
                    encode_and_transmit_char(cmd);
                }
            }
            else if (cmd == '\r' || cmd == '\n') {
                if (system_mode == MODE_ENCODE && input_index > 0) {
                    uart_send_string("\r\nTransmitting: ");
                    uart_send_string((char*)input_buffer);
                    uart_send_string("\r\n");
                    
                    // Transmit end of message signal
                    transmit_end_of_message();
                    
                    // Display on LCD
                    lcd_clear();
                    lcd_write_string("Sent: ");
                    lcd_write_string((char*)input_buffer);
                    
                    clear_buffers();
                }
            }
        }
        
        // Handle decoded message display
        if (message_ready && system_mode == MODE_DECODE) {
            lcd_clear();
            lcd_write_string("Received:");
            lcd_set_cursor(1, 0);
            lcd_write_string((char*)decoded_message);
            
            uart_send_string("Decoded: ");
            uart_send_string((char*)decoded_message);
            uart_send_string("\r\n");
            
            message_ready = false;
        }
        
        // Process button input for decoding mode
        if (system_mode == MODE_DECODE) {
            process_button_input();
        }
    }
    
    return 0;
}

void gpio_init(void) {
    // Configure LED pin (PB5 - Arduino pin 13) as output
    DDRB |= (1 << PB5);
    
    // Configure buzzer pin (PD6 - Arduino pin 6) as output  
    DDRD |= (1 << PD6);
    
    // Configure button pin (PD2 - Arduino pin 2) as input with pull-up
    DDRD &= ~(1 << PD2);
    PORTD |= (1 << PD2);
    
    // Enable external interrupt for button (INT0)
    EICRA |= (1 << ISC00); // Any logical change
    EIMSK |= (1 << INT0);
}

void clear_buffers(void) {
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        input_index = 0;
        decoded_index = 0;
        input_buffer[0] = '\0';
        decoded_message[0] = '\0';
        message_ready = false;
    }
}

// External interrupt for button press/release
ISR(INT0_vect) {
    static uint8_t last_button_state = 1; // Pull-up, so idle is high
    
    uint8_t current_state = (PIND & (1 << PD2)) ? 1 : 0;
    
    if (current_state != last_button_state) {
        if (current_state == 0) { // Button pressed (active low)
            button_press_start = get_system_time_ms();
            button_pressed = true;
            // Turn on LED
            PORTB |= (1 << PB5);
            // Turn on buzzer
            PORTD |= (1 << PD6);
        } else { // Button released
            if (button_pressed) {
                button_release_time = get_system_time_ms();
                uint32_t press_duration = button_release_time - button_press_start;
                
                // Process the morse signal
                process_morse_input(press_duration);
                
                button_pressed = false;
                // Turn off LED and buzzer
                PORTB &= ~(1 << PB5);
                PORTD &= ~(1 << PD6);
            }
        }
        last_button_state = current_state;
    }
}

void process_button_input(void) {
    static uint32_t last_input_time = 0;
    uint32_t current_time = get_system_time_ms();
    
    // Check for gaps between symbols and words
    if (last_input_time > 0 && (current_time - last_input_time) > WORD_GAP_MS) {
        // Word gap detected
        if (decoded_index > 0 && decoded_message[decoded_index - 1] != ' ') {
            decoded_message[decoded_index++] = ' ';
            decoded_message[decoded_index] = '\0';
        }
        last_input_time = 0;
    }
    
    if (button_pressed || (last_input_time > 0 && (current_time - last_input_time) > SYMBOL_GAP_MS)) {
        last_input_time = current_time;
    }
}