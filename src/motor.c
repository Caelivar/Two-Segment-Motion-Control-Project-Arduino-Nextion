#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <stdint.h>
#include "motor.h"

void motor_init(void)
{
    DDRD |= (1 << PD6);

    TCCR0A = (1 << WGM01) | (1 << WGM00) | (1 << COM0A1);

    TCCR0B = (1 << CS01) | (1 << CS00);

    OCR0A = 0;
}

void motor_set_duty(uint8_t duty)
{
    OCR0A = duty;
}
