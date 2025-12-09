#ifndef TACH_H
#define TACH_H

#include <stdint.h>

// Инициализация таймера/входа для оптодатчика (Timer1, ICP1 = PB0)
void tach_init(void);

// Вызывать часто (в основном цикле / при задержках), а если пришёл новый импульс от оптодатчика – обновит внутреннюю скорость.
void tach_update(void);

// Получить последнюю измеренную скорость в см/с
float tach_get_speed_cm_s(void);

#endif // TACH_H
