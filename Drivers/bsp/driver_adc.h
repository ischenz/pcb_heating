#ifndef __DRIVER_ADC
#define __DRIVER_ADC

#include "main.h"

/*
 ***CHANNELS*****NAME****
 ***   0    *****PCB****
 ***   1    *****VCC****
 ***   2    *****heating plate****
*/
#define CHANNELS        3
#define PCB             0
#define VCC             1
#define HEATING_PLATE   2

#define T25             298.15
#define Ka              273.15
#define R25_1           10.0   //pcb
#define B_1	            3950.0 //pcb
#define R25_2           100.0  //main
#define B_2	            3950.0 //main

extern uint16_t ADC_Value[CHANNELS];
extern float V_source;
extern float R_rnt[CHANNELS];
extern float voltage[CHANNELS];
extern float temp[CHANNELS];

void bsp_adc_init(void);
float Get_Temp(float rnt, float R25, float B);
void ADC_timer_callback(void);

#endif /* __DRIVER_ADC */
