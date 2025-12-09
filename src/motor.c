#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <stdint.h>
#include "motor.h"

void motor_init(void)
{
    // PD6 как выход (OC0A = EnB)
    DDRD |= (1 << PD6);

    // Fast PWM, clear OC0A on compare match
    // COM0A1=1, COM0B1=1, WGM01=1, WGM00=1
    TCCR0A |= 0b10100011;

    // Предделитель 1024: CS02=1, CS00=1
    TCCR0B |= 0b00000101;

    // Мотор выключен
    OCR0A = 0;
}

void motor_set_duty(uint8_t duty)
{
    OCR0A = duty;
}

uint8_t motor_compute_duty(uint32_t distance, uint32_t time)
{
    if (time == 0) return 0;

    // "Скорость" = distance / time (без единиц)
    // Масштабируем так, чтобы 100 -> максимум.
    // speed_scaled = (distance * 100) / time  (0..~100)
    uint32_t speed_scaled = (distance * 100UL) / time;
    if (speed_scaled > 100UL) speed_scaled = 100UL;

    // Переводим 0..100 -> 0..255
    uint32_t duty = (speed_scaled * 255UL) / 100UL;
    if (duty > 255UL) duty = 255UL;

    return (uint8_t)duty;
}
