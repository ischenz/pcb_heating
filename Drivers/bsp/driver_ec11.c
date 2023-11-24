#include "driver_ec11.h"
#include "tim.h"
#include "pid.h"
#include "delay.h"

TypeDef_EC11 ec11 = {
    .cnt = 0,
    .button_status = 0
};

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


/*
 ******************** beep ***************************
 *****************************************************
**/
void rotary_beep(void)
{
    HAL_GPIO_WritePin(beep_GPIO_Port, beep_Pin, GPIO_PIN_RESET);
    delay_ms(1);
    HAL_GPIO_WritePin(beep_GPIO_Port, beep_Pin, GPIO_PIN_SET);
}

