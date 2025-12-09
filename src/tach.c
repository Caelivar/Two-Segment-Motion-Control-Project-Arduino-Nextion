#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <stdint.h>
#include "tach.h"

// Дырки
#define NO_HOLES 10

// Радиус
#define WHEEL_RADIUS_CM 13.56f

// Длина дуги на одно отверстие , тобиш s = R * theta, theta = 2*pi/NO_HOLES.
#define DISTANCE_PER_HOLE_CM 1.92f

// послед измеренная скорость в см/с
static float g_speed_cm_s = 0.0f;

void tach_init(void)
{
    // Timer1 норм
    TCCR1A = 0x00;

    // ICNC1: noise canceller
    // ICES1: захват по фронту (rising edge)
    // CS12 + CS10: предделитель 1024
    TCCR1B |= (1 << ICNC1) | (1 << ICES1) | (1 << CS12) | (1 << CS10);

    // PB0 (ICP1) как вход + pull-up
    DDRB &= ~(1 << PB0);
    PORTB |= (1 << PB0);

    TCNT1 = 0;
}

// Проверяем, был ли новый импульс от оптодатчика
void tach_update(void)
{
    if (TIFR1 & (1 << ICF1))
    {
        uint16_t pulseWidth = ICR1;

        TCNT1 = 0;
        TIFR1 |= (1 << ICF1);  // сбрасываем флаг

        // 1 тик = 1024 / 16e6 = 0.000064 с
        float time_s = 0.000064f * pulseWidth;

        if (time_s > 0.0f)
        {
            // v = s / t, где s ≈ 8.52 см на одну дырку
            g_speed_cm_s = DISTANCE_PER_HOLE_CM / time_s;
        }
        else
        {
            g_speed_cm_s = 0.0f;
        }
    }
}

float tach_get_speed_cm_s(void)
{
    return g_speed_cm_s;
}
