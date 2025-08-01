#include "timer.h"
#include "morse.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <stddef.h>

// Global timer variables
volatile uint32_t system_time_ms = 0;
volatile uint32_t delay_start_time = 0;
volatile uint16_t delay_duration = 0;
volatile bool delay_active = false;

// Callback variables
volatile timer_callback_t timer_callback = NULL;
volatile uint32_t callback_last_time = 0;
volatile uint16_t callback_interval = 0;

// External variables for morse decoding timeout
extern volatile char current_morse_buffer[MAX_MORSE_LENGTH];
extern volatile uint8_t morse_buffer_index;
extern volatile uint32_t last_input_time;
extern volatile char decoded_message[MAX_BUFFER_SIZE];
extern volatile uint8_t decoded_index;

void timer_init(void) {
    // Configure Timer0 for 1ms interrupts
    // Timer0: 8-bit timer
    // With 16MHz clock and prescaler 64: 16,000,000 / 64 = 250,000 Hz
    // For 1ms interrupt: 250,000 / 1000 = 250 counts
    // But Timer0 is only 8-bit (0-255), so we use 250 counts
    
    // Set CTC mode (Clear Timer on Compare)
    TCCR0A = (1 << WGM01);
    
    // Set prescaler to 64
    TCCR0B = (1 << CS01) | (1 << CS00);
    
    // Set compare value for 1ms interrupt
    OCR0A = 249; // 250 counts (0-249) = 1ms
    
    // Enable Timer0 Compare A interrupt
    TIMSK0 = (1 << OCIE0A);
    
    // Initialize variables
    system_time_ms = 0;
    delay_active = false;
}

// Timer0 Compare A interrupt service routine (called every 1ms)
ISR(TIMER0_COMPA_vect) {
    system_time_ms++;
    
    // Handle callback timer
    if (timer_callback != NULL && callback_interval > 0) {
        if ((system_time_ms - callback_last_time) >= callback_interval) {
            timer_callback();
            callback_last_time = system_time_ms;
        }
    }
    
    // Handle morse decoding timeout
    if (last_input_time > 0 && morse_buffer_index > 0) {
        if ((system_time_ms - last_input_time) >= CHAR_TIMEOUT_MS) {
            // Character timeout - complete the current character
            complete_character();
            last_input_time = 0;
        }
    }
}

uint32_t get_system_time_ms(void) {
    uint32_t time;
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        time = system_time_ms;
    }
    return time;
}

void timer_delay_ms(uint16_t delay_ms) {
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        delay_start_time = system_time_ms;
        delay_duration = delay_ms;
        delay_active = true;
    }
    
    // Wait for delay to complete
    while (delay_active) {
        // Check if delay is complete
        if ((system_time_ms - delay_start_time) >= delay_duration) {
            delay_active = false;
        }
    }
}

bool timer_delay_complete(void) {
    if (!delay_active) {
        return true;
    }
    
    uint32_t current_time = get_system_time_ms();
    if ((current_time - delay_start_time) >= delay_duration) {
        delay_active = false;
        return true;
    }
    
    return false;
}

void timer_set_callback(timer_callback_t callback, uint16_t interval_ms) {
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        timer_callback = callback;
        callback_interval = interval_ms;
        callback_last_time = system_time_ms;
    }
}

void timer_clear_callback(void) {
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        timer_callback = NULL;
        callback_interval = 0;
    }
}