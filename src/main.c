#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>

#include "tach.h"
#include "usart.h"
#include "nextion.h"
#include "motor.h"
#include "volt.h"

// ===================== Settings =====================

// 1 = meters convert to centimeters
// 0 = centimiters
#define DIST_INPUT_IN_METERS 0

// steps
#define STEPS_PER_SEGMENT 50

// proportional gain for speed control
#define KP_SPEED 0.5f

// ===================== INNER FUNCTIONS =====================

static uint8_t delay_ms_until_distance(uint32_t ms,
                                       float dist_start_cm,
                                       float dist_limit_cm,
                                       uint32_t *elapsed_ms)
{
    while (ms--)
    {
        tach_update();

        float seg_dist = tach_get_distance_cm() - dist_start_cm;
        if (seg_dist >= dist_limit_cm)
            return 1;

        _delay_ms(1);
        (*elapsed_ms)++;
    }
    return 0;
}

// distance_cm, time_s 
// duty              
// progress_start/end  
// elapsed_ms          
static void run_segment(uint32_t distance_cm, uint32_t time_s,
                        uint8_t duty,
                        uint8_t progress_start,
                        uint8_t progress_end,
                        uint32_t *elapsed_ms)
{
    if (distance_cm == 0 || time_s == 0)
        return;

    float dist_start_cm = tach_get_distance_cm();

    uint32_t segment_ms = time_s * 1000UL;

    uint8_t  steps   = STEPS_PER_SEGMENT;                     
    uint32_t step_ms = segment_ms / steps;
    if (step_ms == 0) step_ms = 1;

    float target_speed_cm_s = (float)distance_cm / (float)time_s;

    motor_set_duty(duty);

    for (uint8_t i = 1; i <= steps; i++)
    {
        tach_update();
        float speed_cm_s = tach_get_speed_cm_s();
        float dist_cm_total    = tach_get_distance_cm();
        float seg_dist_cm = dist_cm_total - dist_start_cm;

        // --- UI: speed, dist, volt ---
        nextion_set_speed((uint16_t)(speed_cm_s));
        nextion_set_target_speed((uint16_t)(target_speed_cm_s));
        nextion_set_distance((uint16_t)(dist_cm_total));


        uint16_t mv = volt_read_mv();
        nextion_set_voltage(mv);    
        
        // --- Progress, по реальной дистанции сегмента
        uint8_t seg_percent = 0;
        if (distance_cm > 0)
        {
            float frac = seg_dist_cm / (float)distance_cm;
            if (frac < 0.0f) frac = 0.0f;
            if (frac > 1.0f) frac = 1.0f;
            seg_percent = (uint8_t)(frac * 100.0f + 0.5f);
        }

        uint8_t progress = progress_start +
            (uint8_t)((uint32_t)(progress_end - progress_start) * seg_percent / 100UL);

        nextion_set_progress(progress);
        nextion_set_wall_e(progress);

        // --- Регулятор скорости (P) ---
        float error = target_speed_cm_s - speed_cm_s;
        int16_t delta = (int16_t)(KP_SPEED * error);

        int16_t new_duty = (int16_t)duty + delta;
        if (new_duty < 0)   new_duty = 0;
        if (new_duty > 255) new_duty = 255;

        duty = (uint8_t)new_duty;
        motor_set_duty(duty);

        uint8_t reached = delay_ms_until_distance(step_ms, dist_start_cm, (float)distance_cm, elapsed_ms);

        nextion_set_time((uint16_t)(*elapsed_ms / 1000UL));

        if (reached)
            break;
    }

    float final_seg_dist = tach_get_distance_cm() - dist_start_cm;
    if (final_seg_dist >= (float)distance_cm)
    {
        nextion_set_progress(progress_end);
        nextion_set_wall_e(progress_end);
    }
}

int main(void)
{
    uart_init();
    io_redirect();

    nextion_init();
    motor_init();
    tach_init();
    volt_init();

    while (1)
    {
        uint32_t d1, t1, d2, t2;

        nextion_wait_for_values(&d1, &t1, &d2, &t2);

#if DIST_INPUT_IN_METERS
        d1 *= 100UL;
        d2 *= 100UL;
#endif

        nextion_reset_start_button();

        if ((t1 + t2) == 0)
        {
            nextion_goto_page(0);
            continue;
        }

        nextion_goto_page(1);

        tach_reset_distance();

        uint8_t duty1 = motor_compute_duty(d1, t1);
        uint8_t duty2 = motor_compute_duty(d2, t2);

        uint32_t total_distance = d1 + d2;
        if (total_distance == 0) total_distance = 1;

        uint8_t p1_end = (uint8_t)((uint32_t)d1 * 100UL / total_distance);

        uint32_t elapsed_ms = 0;

        run_segment(d1, t1, duty1, 0, p1_end, &elapsed_ms);

        run_segment(d2, t2, duty2, p1_end, 100, &elapsed_ms);

        motor_set_duty(0);
        nextion_set_speed(0);
        nextion_set_progress(100);
        nextion_set_wall_e(100);

        nextion_goto_page(0);
    }

    return 0;
}