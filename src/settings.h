#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdint.h>

typedef struct {
    uint8_t  units_m;    // 0 = cm, 1 = m
    uint16_t steps;      // 10..200
    uint16_t kp_x100;    // 0..500  (0.00..5.00)
} Settings;

extern Settings g_settings;

void settings_set_defaults(void);
void settings_apply(uint32_t units, uint32_t steps, uint32_t kp_x100);

float settings_get_kp(void);

#endif
