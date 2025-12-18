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

static void delay_ms_track(uint32_t ms,
                           float dist_start_cm,
                           float dist_limit_cm,
                           uint32_t *elapsed_ms,
                           uint8_t *reached)
{
    while (ms--)
    {
        tach_update();

        if (!(*reached))
        {
            float seg = tach_get_distance_cm() - dist_start_cm;
            if (seg >= dist_limit_cm)
            {
                *reached = 1;
                motor_set_duty(0);
            }
        }

        _delay_ms(1);
        (*elapsed_ms)++;
    }
}

static void run_segment(uint32_t distance_cm, uint32_t time_s,
                        uint8_t progress_start, uint8_t progress_end,
                        uint32_t *elapsed_ms,
                        uint8_t keep_running)

{
    if (!elapsed_ms || time_s == 0 || distance_cm == 0)
        return;

    const float    dist_start = tach_get_distance_cm();
    const uint32_t segment_ms = time_s * 1000UL;
    const uint8_t  steps      = 50;
    const uint32_t step_ms    = segment_ms / steps;

    uint16_t acc_x1000 = 0;
    uint8_t  reached   = 0;
    uint32_t seg_start = *elapsed_ms;

    for (uint8_t i = 0; i <= steps; i++)
    {
        if ((*elapsed_ms - seg_start) >= segment_ms)
            break;

        delay_ms_track(step_ms, dist_start,
                       (float)distance_cm,
                       elapsed_ms, &reached);

        float seg_dist = tach_get_distance_cm() - dist_start;
        if (seg_dist < 0) seg_dist = 0;

        uint32_t seg_elapsed = *elapsed_ms - seg_start;
        if (seg_elapsed >= segment_ms) seg_elapsed = segment_ms;

        float remain_d = (float)distance_cm - seg_dist;
        if (remain_d < 0) remain_d = 0;

        uint32_t remain_ms = segment_ms - seg_elapsed;
        if (remain_ms == 0) remain_ms = 1;

        float v_need = remain_d * 1000.0f / remain_ms;
        uint16_t v_need_x10 = (uint16_t)(v_need * 10.0f + 0.5f);

        if (!reached)
        {
            MotorMixPlan p = motor_plan_from_speed_x10(v_need_x10);
            acc_x1000 += p.alpha_x1000;

            motor_set_duty(
                (acc_x1000 >= 1000)
                ? (acc_x1000 -= 1000, p.duty_high)
                : p.duty_low
            );
        }

        uint8_t prog = reached ? progress_end :
            progress_start +
            (uint8_t)((uint32_t)(progress_end - progress_start) * i / steps);

        nextion_set_progress(prog);
        nextion_set_wall_e(prog);
        nextion_set_speed((uint16_t)(tach_get_speed_cm_s() + 0.5f));
        nextion_set_target_speed((uint16_t)(v_need + 0.5f));
        nextion_set_distance((uint16_t)(tach_get_distance_cm() + 0.5f));
        nextion_set_time((uint16_t)(*elapsed_ms / 1000UL));
        nextion_set_voltage(volt_read_mv());
    }

    motor_set_duty(0);
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

        tach_reset();
        for (uint16_t i = 0; i < 150; i++) {
            tach_update();
            _delay_ms(1);
        }
        tach_reset(); 


        uint32_t total_distance = d1 + d2;
        if (total_distance == 0) total_distance = 1;

        uint8_t p1_end = (uint8_t)((uint32_t)d1 * 100UL / total_distance);

        uint32_t elapsed_ms = 0;

        // duty теперь НЕ передаём – внутри run_segment будет микс + стоп
        run_segment(d1, t1, 0, p1_end, &elapsed_ms, 1);
        
        run_segment(d2, t2, p1_end, 100, &elapsed_ms, 0);

        motor_set_duty(0);
        nextion_set_speed(0);
        nextion_set_progress(100);
        nextion_set_wall_e(100);

        nextion_goto_page(0);
    }

    return 0;
}