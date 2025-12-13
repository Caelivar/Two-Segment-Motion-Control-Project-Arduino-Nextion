#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <stdint.h>
#include "motor.h"

void motor_init(void)
{
    DDRD |= (1 << PD6);

    TCCR0A |= 0b10100011;

    TCCR0B |= 0b00000101;

    OCR0A = 0;
}

void motor_set_duty(uint8_t duty)
{
    OCR0A = duty;
}

uint8_t motor_compute_duty(uint32_t distance, uint32_t time)
{
    if (time == 0) return 0;
    uint32_t speed_scaled = (distance * 100UL) / time;
    if (speed_scaled > 100UL) speed_scaled = 100UL;

    uint32_t duty = (speed_scaled * 255UL) / 100UL;
    if (duty > 255UL) duty = 255UL;

    return (uint8_t)duty;
}

/* =========================
   Calibration table
   speed_cm_s = cm/s
   (calibration firmware speed_mps_x100 == cm/s)
   points должны идти по возрастанию speed_cm_s
   ========================= 

typedef struct
{
    uint8_t  duty;         // 0..255
    uint16_t speed_cm_s;   // cm/s
} MotorCalPoint;

/*
 * Test results
 * Exp.
 * duty 15 -> 12 cm/s  (0.12 m/s)
 * duty 35 -> 25 cm/s  (0.25 m/s)
 * ...
 
static const MotorCalPoint g_cal[] =
{
    {  15,  12 },
    {  35,  25 },
    {  55,  40 },
    {  75,  55 },
    {  95,  70 },
    { 115,  82 },
    { 135,  94 },
    { 155, 105 },
    { 175, 115 },
    { 195, 124 },
    { 215, 132 },
    { 225, 136 },
};

static const uint8_t g_cal_n = (uint8_t)(sizeof(g_cal) / sizeof(g_cal[0]));

// Ilnear inter. (x0,y0) и (x1,y1) для x
static uint8_t lerp_u8(uint16_t x, uint16_t x0, uint8_t y0, uint16_t x1, uint8_t y1)
{
    if (x1 <= x0) return y0;

    // y = y0 + (y1-y0)*(x-x0)/(x1-x0)
    int16_t dy = (int16_t)y1 - (int16_t)y0;
    uint16_t dx = x1 - x0;
    uint16_t t  = x - x0;

    int32_t num = (int32_t)dy * (int32_t)t;
    int32_t y   = (int32_t)y0 + (num / (int32_t)dx);

    if (y < 0)   y = 0;
    if (y > 255) y = 255;
    return (uint8_t)y;
}

uint8_t motor_duty_from_speed_cm_s(uint16_t target_speed_cm_s)
{
    if (g_cal_n == 0) return 0;

    // min
    if (target_speed_cm_s <= g_cal[0].speed_cm_s)
        return g_cal[0].duty;

    // max
    if (target_speed_cm_s >= g_cal[g_cal_n - 1].speed_cm_s)
        return g_cal[g_cal_n - 1].duty;

    // find segment [i-1, i] where speed lays inner
    for (uint8_t i = 1; i < g_cal_n; i++)
    {
        uint16_t s0 = g_cal[i - 1].speed_cm_s;
        uint16_t s1 = g_cal[i].speed_cm_s;

        if (target_speed_cm_s <= s1)
        {
            uint8_t d0 = g_cal[i - 1].duty;
            uint8_t d1 = g_cal[i].duty;
            return lerp_u8(target_speed_cm_s, s0, d0, s1, d1);
        }
    }

    // in case of error
    return g_cal[g_cal_n - 1].duty;
}

uint8_t motor_compute_duty(uint32_t distance_cm, uint32_t time_s)
{
    if (time_s == 0) return 0;

    // target speed в cm/s
    uint16_t target_speed_cm_s = (uint16_t)((distance_cm + (time_s / 2)) / time_s);

    return motor_duty_from_speed_cm_s(target_speed_cm_s);
} */

