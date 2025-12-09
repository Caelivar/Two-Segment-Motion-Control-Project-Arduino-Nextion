#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>

// Инициализация PWM на PD6 (EnB L298)
void motor_init(void);

// Установить скважность PWM (0..255)
void motor_set_duty(uint8_t duty);

// Посчитать duty из distance/time
// (упрощённая модель: чем больше distance/time, тем больше duty,
// максимум при "скорости" 100 условных единиц)
uint8_t motor_compute_duty(uint32_t distance, uint32_t time);

#endif // MOTOR_H
