#include "driver_ec11.h"
#include "tim.h"
#include "pid.h"

TypeDef_EC11 ec11;

void ec11_init(void)
{
    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
    __HAL_TIM_ENABLE_IT(&htim3, TIM_IT_UPDATE);
    
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == EC11_BTN_Pin){
        set_pid_target(&pid, ec11.cnt);
    }
}

void Get_Temp_cnt(void)
{
    int16_t cnt = 0;
    cnt = TIM3->CNT * 5;
    if(cnt > 300){
        cnt = 300;
        TIM3->CNT = 60;
    } else if(cnt < 0){
        cnt = 0;
        TIM3->CNT = 0;
    }
    ec11.cnt = cnt;
}
