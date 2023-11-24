#include "mytimer.h"
#include "driver_adc.h"
#include "pid.h"

void load_pwm(uint8_t channel, float pwm)
{
    /*limit*/
    float load_pwm = pwm;
    if(load_pwm > 1000){
        load_pwm = 1000;
    } else if(load_pwm < 0){
        load_pwm = 0;
    }
    if(channel == main_pwm)
        TIM2->CCR3 = load_pwm;
    else if(channel == fan_pwm)
        TIM2->CCR4 = load_pwm;
    else if(channel == led_pwm){
        TIM2->CCR1 = load_pwm;
    }
}
    
