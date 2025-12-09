#ifndef NEXTION_H
#define NEXTION_H

#include <stdint.h>

void nextion_init(void);

void nextion_wait_for_values(uint32_t *d1, uint32_t *t1,
                             uint32_t *d2, uint32_t *t2);

// Установить прогресс маршрута в процентах (0..100)
// nProg – Progress Bar на странице 0
void nextion_set_progress(uint8_t percent);

// Показать скорость (целое число) в компоненте nSpeed
void nextion_set_speed(uint16_t speed);

// Показать напряжение (целое число) в компоненте tVolt
void nextion_set_voltage(uint16_t mv);

#endif // NEXTION_H
