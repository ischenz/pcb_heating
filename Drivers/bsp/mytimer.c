#include "mytimer.h"
#include "driver_adc.h"
#include "pid.h"
/*
 * 100ms
*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    static uint32_t i = 0;
    if(htim->Instance == TIM1){
        i++;
        if( i%5 == 0 ){//500ms
            ADC_timer_callback();   
            pid_calculate(&pid, temp[HEATING_PLATE]);
            load_pwm(pid.PID_out);
        }     
    }
}

void load_pwm(float pwm)
{
    /*limit*/
    float load_pwm = pwm;
    if(load_pwm > 1000){
        load_pwm = 1000;
    } else if(load_pwm < 0){
        load_pwm = 0;
    }
    TIM2->CCR3 = load_pwm;
}
    
