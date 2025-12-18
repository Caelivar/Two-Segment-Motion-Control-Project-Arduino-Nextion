#ifndef NEXTION_CALIB_H
#define NEXTION_CALIB_H

#include <stdint.h>

void nextion_goto_page(uint8_t page);
void nextion_wait_for_start(void);

void nextion_set_duty(uint8_t duty);
void nextion_set_speed_mps_x100(uint16_t speed_mps_x100); // m/s * 100
void nextion_set_voltage_mv(uint16_t mv);
void nextion_set_status(const char *msg);

void nextion_log_clear(void);
void nextion_log_add(uint8_t duty, uint16_t speed_mps_x100, uint16_t mv);

#endif
