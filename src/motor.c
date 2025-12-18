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

typedef struct {
    uint8_t  duty;
    uint16_t speed_x10;
} MotorCalPoint;

static const MotorCalPoint g_cal[] =
{
    {   0,   0   },   
    {  15,   0   },
    {  35,   0   },
    {  55,  0   },
    {  75,  20   },
    {  95,  60   },
    { 115, 60   },
    { 135, 60   },
    { 155, 80   },
    { 175, 80   }, 
    { 195, 80   },
    { 225, 90   },
};


static const uint8_t g_cal_n =
    sizeof(g_cal) / sizeof(g_cal[0]);

static uint16_t clamp_u16(uint16_t v, uint16_t lo, uint16_t hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

MotorMixPlan motor_plan_from_speed_x10(uint16_t target)
{
    MotorMixPlan plan = {0, 0, 0};

    if (target <= g_cal[0].speed_x10)
    {
        plan.duty_low = g_cal[0].duty;
        plan.duty_high = g_cal[0].duty;
        plan.alpha_x1000 = 0;
        return plan;
    }

    if (target >= g_cal[g_cal_n - 1].speed_x10)
    {
        plan.duty_low = g_cal[g_cal_n - 1].duty;
        plan.duty_high = g_cal[g_cal_n - 1].duty;
        plan.alpha_x1000 = 0;
        return plan;
    }

    for (uint8_t i = 1; i < g_cal_n; i++)
    {
        uint16_t v0 = g_cal[i - 1].speed_x10;
        uint16_t v1 = g_cal[i].speed_x10;

        if (target <= v1 && v1 > v0)
        {
            plan.duty_low = g_cal[i - 1].duty;
            plan.duty_high = g_cal[i].duty;

            uint16_t num = (target - v0) * 1000UL;
            uint16_t den = (v1 - v0);

            plan.alpha_x1000 = clamp_u16(num / den, 0, 1000);
            return plan;
        }
    }

    return plan;
}
