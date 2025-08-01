#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

// UART receive buffer
static volatile char uart_rx_buffer[UART_BUFFER_SIZE];
static volatile uint8_t uart_rx_head = 0;
static volatile uint8_t uart_rx_tail = 0;
static volatile uint8_t uart_rx_count = 0;

void uart_init(void) {
    // Calculate baud rate register value
    uint16_t ubrr = F_CPU / 16 / UART_BAUD_RATE - 1;
    
    // Set baud rate
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;
    
    // Enable receiver, transmitter, and receive interrupt
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
    
    // Set frame format: 8 data bits, 1 stop bit, no parity
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
    
    // Initialize buffer variables
    uart_rx_head = 0;
    uart_rx_tail = 0;
    uart_rx_count = 0;
}

// UART receive interrupt service routine
ISR(USART_RX_vect) {
    char received_char = UDR0;
    
    // Add character to buffer if there's space
    if (uart_rx_count < UART_BUFFER_SIZE) {
        uart_rx_buffer[uart_rx_head] = received_char;
        uart_rx_head = (uart_rx_head + 1) % UART_BUFFER_SIZE;
        uart_rx_count++;
    }
}

void uart_send_char(char c) {
    // Wait for empty transmit buffer
    while (!(UCSR0A & (1 << UDRE0)));
    
    // Send character
    UDR0 = c;
}

void uart_send_string(const char* str) {
    while (*str) {
        uart_send_char(*str++);
    }
}

char uart_receive_char(void) {
    char c = 0;
    
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        if (uart_rx_count > 0) {
            c = uart_rx_buffer[uart_rx_tail];
            uart_rx_tail = (uart_rx_tail + 1) % UART_BUFFER_SIZE;
            uart_rx_count--;
        }
    }
    
    return c;
}

bool uart_data_available(void) {
    bool available;
    
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        available = (uart_rx_count > 0);
    }
    
    return available;
}

void uart_flush(void) {
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        uart_rx_head = 0;
        uart_rx_tail = 0;
        uart_rx_count = 0;
    }
}