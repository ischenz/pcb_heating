#ifndef __CONTROL_H__
#define __CONTROL_H__

#include "main.h"

void show_main_win(void);
void show_heating(void);
void clear_heating(void);
void EncoderAction(void);
void showMainTemperature(void);
void sysMonitor(void);
void Hello(void);
void controlShowHeating(void);
void showBattery(uint8_t level, uint8_t stauts);
void led_control(void);

#endif /* __CONTROL_H__ */

