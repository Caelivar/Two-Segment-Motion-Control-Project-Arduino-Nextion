#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <stdint.h>
#include "tach.h"

// define
#define NO_HOLES 10
#define WHEEL_DIAMETER_CM 6.1f
#define PI 3.1415926f

#define DISTANCE_PER_HOLE_CM ((PI * WHEEL_DIAMETER_CM) / (float)NO_HOLES)
//#define DISTANCE_PER_HOLE_CM 1.92f
    
//global
static float g_speed_cm_s     = 0.0f;   
static uint32_t g_pulse_count = 0;    

void tach_init(void)
{
    TCCR1A = 0x00;

    TCCR1B |= (1 << ICNC1) | (1 << ICES1) | (1 << CS12) | (1 << CS10);

    DDRB  &= ~(1 << PB0);
    PORTB |=  (1 << PB0);

    TCNT1 = 0;
    g_speed_cm_s = 0.0f;
    g_pulse_count = 0;
}

void tach_update(void)
{
    if (TIFR1 & (1 << ICF1))
    {
        uint16_t pulseWidth = ICR1;

        TCNT1 = 0;
        TIFR1 |= (1 << ICF1);  

        float time_s = 0.000064f * pulseWidth;

        if (time_s > 0.0f)
        {
            g_speed_cm_s = DISTANCE_PER_HOLE_CM / time_s;
        }
        else
        {
            g_speed_cm_s = 0.0f;
        }

        g_pulse_count++;
    }
}

float tach_get_speed_cm_s(void)
{
    return g_speed_cm_s;
}

float tach_get_distance_cm(void)
{
    return g_pulse_count * DISTANCE_PER_HOLE_CM;
}

void tach_reset_distance(void)
{
    g_pulse_count = 0;
}
