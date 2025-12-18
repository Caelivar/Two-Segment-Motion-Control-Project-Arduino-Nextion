#ifndef TACH_H
#define TACH_H

#include <stdint.h>

void  tach_init(void);
void  tach_update(void);

float tach_get_speed_cm_s(void);
float tach_get_distance_cm(void);

void  tach_reset_distance(void);
void  tach_reset(void);


#endif
