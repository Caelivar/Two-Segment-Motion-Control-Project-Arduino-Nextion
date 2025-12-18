#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>

void motor_init(void);

void motor_set_duty(uint8_t duty);

typedef struct {
    uint8_t  duty_low;
    uint8_t  duty_high;
    uint16_t alpha_x1000; // 0..1000
} MotorMixPlan;
MotorMixPlan motor_plan_from_speed_x10(uint16_t target_speed_cm_s_x10);

#endif
