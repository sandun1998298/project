#ifndef MORSE_H
#define MORSE_H

#include <stdint.h>
#include <stdbool.h>

// Timing constants (in milliseconds)
#define DOT_TIME_MS         150    // Duration of a dot
#define DASH_TIME_MS        450    // Duration of a dash (3 * dot time)
#define INTRA_CHAR_GAP_MS   150    // Gap between dots/dashes within character
#define INTER_CHAR_GAP_MS   450    // Gap between characters (3 * dot time)
#define WORD_GAP_MS         1050   // Gap between words (7 * dot time)
#define SYMBOL_GAP_MS       300    // Gap to recognize end of character input

// Decoding thresholds
#define DOT_THRESHOLD_MS    300    // Below this = dot, above = dash
#define CHAR_TIMEOUT_MS     800    // Timeout for character completion

// System modes
#define MODE_ENCODE         0
#define MODE_DECODE         1

// Buffer sizes
#define MAX_BUFFER_SIZE     64
#define MAX_MORSE_LENGTH    8      // Longest morse code sequence

// Morse code structure
typedef struct {
    char character;
    char morse_code[MAX_MORSE_LENGTH];
} morse_entry_t;

// Function declarations
void encode_and_transmit_char(char c);
void transmit_morse_sequence(const char* morse);
void transmit_dot(void);
void transmit_dash(void);
void transmit_gap(uint16_t duration_ms);
void transmit_end_of_message(void);

void process_morse_input(uint32_t duration_ms);
char decode_morse_sequence(const char* morse);
void add_morse_symbol(char symbol);
void complete_character(void);

const char* char_to_morse(char c);
char morse_to_char(const char* morse);

// External variables
extern volatile char current_morse_buffer[MAX_MORSE_LENGTH];
extern volatile uint8_t morse_buffer_index;
extern volatile uint32_t last_input_time;

#endif // MORSE_H