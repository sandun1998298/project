#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdbool.h>

// UART configuration
#define UART_BAUD_RATE      9600
#define UART_BUFFER_SIZE    64

// Function declarations
void uart_init(void);
void uart_send_char(char c);
void uart_send_string(const char* str);
char uart_receive_char(void);
bool uart_data_available(void);
void uart_flush(void);

#endif // UART_H