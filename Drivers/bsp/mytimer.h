#ifndef __MYTIMER_H
#define __MYTIMER_H

#include "main.h"

#define     fan_pwm     0
#define     main_pwm    1
#define     led_pwm     2

void load_pwm(uint8_t channel, float pwm);

#endif /* __MYTIMER_H */
