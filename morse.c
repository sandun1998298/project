#include "morse.h"
#include "timer.h"
#include <avr/io.h>
#include <string.h>
#include <ctype.h>

// Morse code lookup table
static const morse_entry_t morse_table[] = {
    {'A', ".-"},    {'B', "-..."},  {'C', "-.-."},  {'D', "-.."},
    {'E', "."},     {'F', "..-."},  {'G', "--."},   {'H', "...."},
    {'I', ".."},    {'J', ".---"},  {'K', "-.-"},   {'L', ".-.."},
    {'M', "--"},    {'N', "-."},    {'O', "---"},   {'P', ".--."},
    {'Q', "--.-"},  {'R', ".-."},   {'S', "..."},   {'T', "-"},
    {'U', "..-"},   {'V', "...-"},  {'W', ".--"},   {'X', "-..-"},
    {'Y', "-.--"},  {'Z', "--.."},
    {'0', "-----"}, {'1', ".----"}, {'2', "..---"}, {'3', "...--"},
    {'4', "....-"}, {'5', "....."}, {'6', "-...."}, {'7', "--..."},
    {'8', "---.."}, {'9', "----."},
    {'.', ".-.-.-"}, {',', "--..--"}, {'?', "..--.."},
    {'\'', ".----."}, {'!', "-.-.--"}, {'/', "-..-."},
    {'(', "-.--."}, {')', "-.--.-"}, {'&', ".-..."},
    {':', "---..."}, {';', "-.-.-."}, {'=', "-...-"},
    {'+', ".-.-."}, {'-', "-....-"}, {'_', "..--.-"},
    {'"', ".-..-."}, {'$', "...-..-"}, {'@', ".--.-."},
    {'\0', ""}  // End marker
};

// Global variables for decoding
volatile char current_morse_buffer[MAX_MORSE_LENGTH];
volatile uint8_t morse_buffer_index = 0;
volatile uint32_t last_input_time = 0;

// External variables from main.c
extern volatile char decoded_message[MAX_BUFFER_SIZE];
extern volatile uint8_t decoded_index;
extern volatile bool message_ready;

const char* char_to_morse(char c) {
    char upper_c = toupper(c);
    
    for (uint8_t i = 0; morse_table[i].character != '\0'; i++) {
        if (morse_table[i].character == upper_c) {
            return morse_table[i].morse_code;
        }
    }
    return NULL; // Character not found
}

char morse_to_char(const char* morse) {
    for (uint8_t i = 0; morse_table[i].character != '\0'; i++) {
        if (strcmp(morse_table[i].morse_code, morse) == 0) {
            return morse_table[i].character;
        }
    }
    return '?'; // Unknown morse code
}

void encode_and_transmit_char(char c) {
    if (c == ' ') {
        // Space between words
        transmit_gap(WORD_GAP_MS);
        return;
    }
    
    const char* morse = char_to_morse(c);
    if (morse != NULL) {
        transmit_morse_sequence(morse);
        // Add inter-character gap
        transmit_gap(INTER_CHAR_GAP_MS);
    }
}

void transmit_morse_sequence(const char* morse) {
    for (uint8_t i = 0; morse[i] != '\0'; i++) {
        if (morse[i] == '.') {
            transmit_dot();
        } else if (morse[i] == '-') {
            transmit_dash();
        }
        
        // Add intra-character gap between symbols (except after last symbol)
        if (morse[i + 1] != '\0') {
            transmit_gap(INTRA_CHAR_GAP_MS);
        }
    }
}

void transmit_dot(void) {
    // Turn on LED and buzzer
    PORTB |= (1 << PB5);  // LED on
    PORTD |= (1 << PD6);  // Buzzer on
    
    // Use timer for precise non-blocking delay
    timer_delay_ms(DOT_TIME_MS);
    
    // Turn off LED and buzzer
    PORTB &= ~(1 << PB5); // LED off
    PORTD &= ~(1 << PD6); // Buzzer off
}

void transmit_dash(void) {
    // Turn on LED and buzzer
    PORTB |= (1 << PB5);  // LED on
    PORTD |= (1 << PD6);  // Buzzer on
    
    // Use timer for precise non-blocking delay
    timer_delay_ms(DASH_TIME_MS);
    
    // Turn off LED and buzzer
    PORTB &= ~(1 << PB5); // LED off
    PORTD &= ~(1 << PD6); // Buzzer off
}

void transmit_gap(uint16_t duration_ms) {
    // Ensure LED and buzzer are off during gap
    PORTB &= ~(1 << PB5); // LED off
    PORTD &= ~(1 << PD6); // Buzzer off
    
    // Use timer for precise non-blocking delay
    timer_delay_ms(duration_ms);
}

void transmit_end_of_message(void) {
    // Transmit AR (end of message) sequence: .-.-.-
    transmit_morse_sequence(".-.-.-");
    transmit_gap(WORD_GAP_MS);
}

void process_morse_input(uint32_t duration_ms) {
    char symbol;
    
    // Determine if it's a dot or dash based on duration
    if (duration_ms < DOT_THRESHOLD_MS) {
        symbol = '.';
    } else {
        symbol = '-';
    }
    
    add_morse_symbol(symbol);
    last_input_time = get_system_time_ms();
}

void add_morse_symbol(char symbol) {
    if (morse_buffer_index < MAX_MORSE_LENGTH - 1) {
        current_morse_buffer[morse_buffer_index++] = symbol;
        current_morse_buffer[morse_buffer_index] = '\0';
    }
}

void complete_character(void) {
    if (morse_buffer_index > 0) {
        char decoded_char = morse_to_char((char*)current_morse_buffer);
        
        if (decoded_char != '?' && decoded_index < MAX_BUFFER_SIZE - 1) {
            decoded_message[decoded_index++] = decoded_char;
            decoded_message[decoded_index] = '\0';
        }
        
        // Check for end of message (AR sequence)
        if (strcmp((char*)current_morse_buffer, ".-.-.-") == 0) {
            message_ready = true;
        }
        
        // Clear morse buffer for next character
        morse_buffer_index = 0;
        current_morse_buffer[0] = '\0';
    }
}