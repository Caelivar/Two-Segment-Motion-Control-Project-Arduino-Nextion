#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#include "usart.h"
#include "motor.h"
#include "tach.h"
#include "volt.h"
#include "nextion_calib.h"

static void delay_ms_with_tach(uint16_t ms)
{
    while (ms--)
    {
        tach_update();
        _delay_ms(1);
    }
}

int main(void)
{
    uart_init();
    io_redirect();

    motor_init();
    tach_init();
    volt_init();

    nextion_goto_page(0);

    const uint8_t duties[] = {15, 35, 55, 75, 95, 115, 135, 155, 175, 195, 215, 225, 235, 255};
    const uint8_t N = sizeof(duties) / sizeof(duties[0]);

    while (1)
    {
        // (page0.bStart -> printh A5)
        nextion_wait_for_start();

        nextion_goto_page(1);
        nextion_log_clear();
        nextion_set_status("Starting...");

        for (uint8_t i = 0; i < N; i++)
        {
            uint8_t duty = duties[i];

            nextion_set_duty(duty);
            nextion_set_status("Stabilizing...");
            motor_set_duty(duty);

            delay_ms_with_tach(2000);
            tach_reset();

            nextion_set_status("Measuring...");
            delay_ms_with_tach(1000);
            float dist_cm = tach_get_distance_cm();
            uint16_t speed_mps_x100 = (uint16_t)(dist_cm + 0.5f);

            uint16_t mv = volt_read_mv();

            nextion_set_speed_mps_x100(speed_mps_x100);
            nextion_set_voltage_mv(mv);
            nextion_log_add(duty, speed_mps_x100, mv);

            delay_ms_with_tach(300);
        }

        motor_set_duty(0);
        nextion_set_status("DONE");
        delay_ms_with_tach(1500);

       // nextion_goto_page(0);
    }

    return 0;
}
