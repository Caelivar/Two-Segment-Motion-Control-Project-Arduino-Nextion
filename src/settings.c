#include "settings.h"

Settings g_settings;

static uint16_t clamp_u16(uint32_t v, uint16_t lo, uint16_t hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return (uint16_t)v;
}

void settings_set_defaults(void)
{
    g_settings.units_m = 0;     // cm
    g_settings.steps   = 50;    // default
    g_settings.kp_x100 = 50;    // 0.50
}

void settings_apply(uint32_t units, uint32_t steps, uint32_t kp_x100)
{
    g_settings.units_m = (units != 0) ? 1 : 0;

    g_settings.steps   = clamp_u16(steps,   10, 200);
    g_settings.kp_x100 = clamp_u16(kp_x100,  0, 500);
}

float settings_get_kp(void)
{
    return ((float)g_settings.kp_x100) / 100.0f;
}
