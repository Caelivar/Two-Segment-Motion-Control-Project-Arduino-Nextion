#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <stdint.h>
#include "tach.h"

#define NO_HOLES           10U
#define WHEEL_DIAMETER_CM  6.1f
#define PI                 3.1415926f

#define DIST_PER_PULSE_CM  ((PI * WHEEL_DIAMETER_CM) / (float)NO_HOLES)

#define T1_TICK_S          (1024.0f / (float)F_CPU)


#define MIN_TICKS          80U

static volatile float    g_speed_cm_s   = 0.0f;
static volatile uint32_t g_pulse_count  = 0;

void tach_init(void)
{
    DDRB  &= ~(1 << PB0);
    PORTB |=  (1 << PB0);
    TCCR1A = 0;
    TCCR1B = (1 << ICNC1) | (1 << ICES1) | (1 << CS12) | (1 << CS10);
    TCNT1 = 0;
    TIFR1 = (1 << ICF1);

    g_speed_cm_s  = 0.0f;
    g_pulse_count = 0;
}

void tach_update(void)
{
    if (TIFR1 & (1 << ICF1))
    {
        uint16_t ticks = ICR1;

        TCNT1 = 0;

        TIFR1 = (1 << ICF1);

        if (ticks < MIN_TICKS)
        {
            return;
        }

        g_pulse_count++;

        float dt_s = (float)ticks * T1_TICK_S;
        if (dt_s > 0.0f)
            g_speed_cm_s = DIST_PER_PULSE_CM / dt_s;
        else
            g_speed_cm_s = 0.0f;
    }
}

float tach_get_speed_cm_s(void)
{
    return g_speed_cm_s;
}

float tach_get_distance_cm(void)
{
    return (float)g_pulse_count * DIST_PER_PULSE_CM;
}

void tach_reset_distance(void)
{
    g_pulse_count = 0;
}

void tach_reset(void)
{
    g_pulse_count = 0;
    g_speed_cm_s  = 0.0f;

    TCNT1 = 0;
    TIFR1 = (1 << ICF1);   
}
