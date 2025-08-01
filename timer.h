#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <stdbool.h>

// Timer configuration
#define TIMER_PRESCALER     64
#define TIMER_FREQ_HZ       (F_CPU / TIMER_PRESCALER)
#define TIMER_PERIOD_US     (1000000UL / TIMER_FREQ_HZ)

// Function declarations
void timer_init(void);
uint32_t get_system_time_ms(void);
void timer_delay_ms(uint16_t delay_ms);
bool timer_delay_complete(void);

// Timer callback function type
typedef void (*timer_callback_t)(void);
void timer_set_callback(timer_callback_t callback, uint16_t interval_ms);
void timer_clear_callback(void);

#endif // TIMER_H