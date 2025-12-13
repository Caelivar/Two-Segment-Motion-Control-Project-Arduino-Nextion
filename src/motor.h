#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>

void motor_init(void);

void motor_set_duty(uint8_t duty);

uint8_t motor_compute_duty(uint32_t distance, uint32_t time);

#endif 
