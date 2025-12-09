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


// Вспомогательная функция: один участок движения
// progress_start/progress_end – границы прогресса (например 0..60, 60..100)

static void delay_ms_variable(uint32_t ms)
{
    while (ms--)
    {
        // Проверяем, не пришёл ли новый импульс от оптодатчика
        tach_update();
        _delay_ms(1);
    }
}

// Участок движения:
// distance, time      – целевые параметры участка (из Nextion),
// duty                 – начальный duty,
// progress_start/end   – границы прогресса для этого участка (0..100).
static void run_segment(uint32_t distance, uint32_t time,
                        uint8_t duty,
                        uint8_t progress_start,
                        uint8_t progress_end)
{
    if (time == 0) return;

    // 1 единица времени = 100 мс
    uint32_t segment_ms = time * 100UL;

    uint8_t steps = 20;
    uint32_t step_ms = segment_ms / steps;
    if (step_ms == 0) step_ms = 1;

    // Целевая скорость в см/с: distance (см) / time (с)
    float target_speed_cm_s = 0.0f;
    if (time > 0)
        target_speed_cm_s = (float)distance / (float)time;

    motor_set_duty(duty);

    for (uint8_t i = 1; i <= steps; i++)
    {
        tach_update();
        float speed_cm_s = tach_get_speed_cm_s();


        // Для отображения просто берём целую часть см/с
        uint16_t speed_display = (uint16_t)(speed_cm_s);
        nextion_set_speed(speed_display);

        // diff in speed
        float error = target_speed_cm_s - speed_cm_s;
        float Kp = 0.5f;   // arbitrary

        int16_t delta = (int16_t)(Kp * error);

        int16_t new_duty = (int16_t)duty + delta;
        if (new_duty < 0)   new_duty = 0;
        if (new_duty > 255) new_duty = 255;

        duty = (uint8_t)new_duty;
        motor_set_duty(duty);

        uint8_t progress = progress_start +
            (uint8_t)((uint32_t)(progress_end - progress_start) * i / steps);

        nextion_set_progress(progress);

        delay_ms_variable(step_ms);
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

        // Ждём ввод с экрана и нажатие Start
        nextion_wait_for_values(&d1, &t1, &d2, &t2);

        printf("Frame received from Nextion!\n");
        printf("D1 = %lu, T1 = %lu\n", (unsigned long)d1, (unsigned long)t1);
        printf("D2 = %lu, T2 = %lu\n", (unsigned long)d2, (unsigned long)t2);

        if ((t1 + t2) == 0) {
            printf("Total time is zero, skipping run.\n");
            continue;
        }

        // Считаем duty для каждого участка
        uint8_t duty1 = motor_compute_duty(d1, t1);
        uint8_t duty2 = motor_compute_duty(d2, t2);

        printf("Duty1 = %u, Duty2 = %u\n", duty1, duty2);

        uint32_t total_distance = d1 + d2;
        if (total_distance == 0) total_distance = 1;

        // Какую долю прогресса занимает первый участок
        uint8_t p1_end = (uint8_t)((uint32_t)d1 * 100UL / total_distance);

        // --- Участок 1: едем дистанцию D1 за время T1 ---
        printf("Segment 1 start\n");
        run_segment(d1, t1, duty1, 0, p1_end);

        // --- Участок 2: не останавливаясь, меняем duty и едем D2 ---
        printf("Segment 2 start\n");
        run_segment(d2, t2, duty2, p1_end, 100);

        // Стоп
        motor_set_duty(0);
        nextion_set_speed(0);
        nextion_set_progress(100);
        printf("Run finished. Car stopped.\n");
        printf("Waiting for next data frame from Nextion...\n");
    }

    return 0;
}
