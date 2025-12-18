#ifndef NEXTION_H
#define NEXTION_H

#include <stdint.h>

void nextion_init(void);

uint8_t nextion_check_abort(void);

void nextion_wait_for_values(uint32_t *d1, uint32_t *t1,
                             uint32_t *d2, uint32_t *t2);

void nextion_goto_page(uint8_t page);

void nextion_set_progress(uint8_t percent);

void nextion_set_speed(uint16_t speed_cm_s);

void nextion_set_target_speed(uint16_t speed_cm_s);

void nextion_set_distance(uint16_t dist_cm);

void nextion_set_time(uint16_t seconds);

void nextion_set_voltage(uint16_t mv);

void nextion_set_wall_e(uint8_t percent);


void nextion_reset_start_button(void);




#endif 
