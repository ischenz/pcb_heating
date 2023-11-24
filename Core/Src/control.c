#include "control.h"
#include "oled.h"
#include "driver_adc.h"
#include "driver_ec11.h"
#include "pid.h"
#include "tim.h"
#include "mytimer.h"
#include "delay.h"
#include "bmp.h"

int16_t show_heartbeat = 0, showSetTemperatureTime = 0;
volatile int16_t temperature_setValue = 0; //温度设置值
    
/*
 * timer 1 100ms callback
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM1){
        show_heartbeat++;
        showMainTemperature();
        if( show_heartbeat % 5 == 0 ){//500ms
            //ADC
            ADC_timer_callback(); 
            //PID
            pid_calculate(&pid, temp[HEATING_PLATE]);
            load_pwm(main_pwm, pid.PID_out);
            //加热标识
            controlShowHeating();
            //电量标识
            showBattery((voltage[VCC] - 11)/(1.6/11), 1);
            //主板温度
            OLED_ShowFNum(0, 3, temp[PCB], 4, 8, 1);
            //系统监测
            sysMonitor();
            //led
            led_control();
        }     
    }
}

/* 
 * level:0-10
 */
void showBattery(uint8_t level, uint8_t stauts)
{
    uint8_t i;
    if(stauts == 1){
        OLED_ShowPicture(107, 3, 16, 8, BMP3, 1);
        if(level > 10)
            return;
        for(i = 0; i < level; i++){
            OLED_DrawLine(109 + i, 5, 109 + i, 9, 1);
        }        
    }else{
        OLED_ShowPicture(107, 3, 16, 8, BMP4, 1);
    }
}

void showMainTemperature(void)
{
    if(show_heartbeat < showSetTemperatureTime){/* 在设置温度显示的时候（设置温度变化时） */
        OLED_ShowNum(45, 29, temperature_setValue, 3, 24, 1);
    }else{
        OLED_ShowNum(45, 29, temp[2], 3, 24, 1);
    }
}

void EncoderAction(void) //获取设置温度
{
    static int16_t last_cnt;//上次的编码器值
    int16_t changeValue = 0;
    
    ec11.cnt = TIM3->CNT;
    if(last_cnt == ec11.cnt){ //编码器状态没有变化
        return;
    } else {                   //编码器有变动
        //判断正反转
        changeValue = ec11.cnt - last_cnt;
        temperature_setValue += changeValue * 5;
        if(temperature_setValue > 300){
            temperature_setValue = 300;
        } else if(temperature_setValue < 0){
            temperature_setValue = 0;
        }   
        set_pid_target(&pid, temperature_setValue);
        last_cnt = ec11.cnt;    
        rotary_beep();  
        /* 显示固定时间的设置温度 */
        showSetTemperatureTime = 10 + show_heartbeat;
    }
}

//周期500ms
void sysMonitor(void)
{
    static uint8_t beep_status = 0;
    /* 系统散热风扇控制 */
    if(temp[PCB] >= 45){
        load_pwm(fan_pwm, 1000);
    }else if(temp[PCB] >= 40){
        load_pwm(fan_pwm, 600);
    }else if(temp[PCB] >= 35){
        load_pwm(fan_pwm, 400);
    }else{
        load_pwm(fan_pwm, 0);
    }
    
    /* 输入电压过低报警 */
    if(voltage[VCC] < 11.0){
        beep_status = !beep_status;
        if(beep_status == 0){
            showBattery(0, 0);
            HAL_GPIO_WritePin(beep_GPIO_Port, beep_Pin, GPIO_PIN_RESET);
        }else{
            HAL_GPIO_WritePin(beep_GPIO_Port, beep_Pin, GPIO_PIN_SET); 
            showBattery(0, 1);            
        }
    }else{//避免beep不停止
        HAL_GPIO_WritePin(beep_GPIO_Port, beep_Pin, GPIO_PIN_SET); 
    }
}

void led_control(void)
{
    if(show_heartbeat % 10 == 0){
        load_pwm(led_pwm, 1000);
    }else{   
        load_pwm(led_pwm, 0);
    }
}

void Hello(void)
{
    int i;
    for(i = 0; i < 200; i++){
        HAL_GPIO_WritePin(beep_GPIO_Port, beep_Pin, GPIO_PIN_RESET);
        delay_us(500);
        HAL_GPIO_WritePin(beep_GPIO_Port, beep_Pin, GPIO_PIN_SET);   
        delay_us(1);        
    }
    delay_ms(50);    
    HAL_GPIO_WritePin(beep_GPIO_Port, beep_Pin, GPIO_PIN_RESET);
    delay_ms(100);
    HAL_GPIO_WritePin(beep_GPIO_Port, beep_Pin, GPIO_PIN_SET);   

//    HAL_GPIO_WritePin(beep_GPIO_Port, beep_Pin, GPIO_PIN_RESET);    
//    delay_ms(500);
//    HAL_GPIO_WritePin(beep_GPIO_Port, beep_Pin, GPIO_PIN_SET);   
}

void show_main_win(void)
{
//    OLED_ShowNum(45, 29, temp[2], 3, 24, 1);
//    OLED_ShowFNum(0, 30, voltage[2], 4, 8, 1);
//    OLED_ShowNum(110, 0, temp[0], 3, 8, 1);
//    OLED_ShowFNum(0, 56, voltage[1], 4, 8, 1);
//    OLED_ShowNum(58, 20, ec11.cnt, 3, 8, 1);
//    OLED_ShowSNum(98, 56, pid.PID_out, 5, 8, 1);
//    OLED_ShowNum(98, 40, pid.Target, 3, 8, 1);
    //show_heating();
    OLED_Refresh();
}

void controlShowHeating(void)
{
    if(temp[HEATING_PLATE] < temperature_setValue){ //如果温度小于设置温度，显示加热图标
        if(show_heartbeat % 2){
            show_heating();
        }else{
            clear_heating();
        }
    }else{
        if(show_heartbeat % 3 == 0){
            clear_heating();
        }
    }
}

/* void show_heating(void)
 * 显示加热图标
 */
void show_heating(void)
{
    OLED_ShowString(48, 4, "S S S", 12, 1);
    OLED_DrawLine(45, 16, 81, 16, 1);               /* OLED_DrawLine()不包含最后一个点 */
    OLED_DrawLine(45, 20, 81, 20, 1);
    OLED_DrawLine(45, 16, 45, 20, 1);
    OLED_DrawLine(81, 16, 81, 21, 1); 
    
}

void clear_heating(void)
{
    OLED_ShowString(48, 4, "     ", 12, 1);
    OLED_DrawLine(45, 16, 81, 16, 0);               /* OLED_DrawLine()不包含最后一个点 */
    OLED_DrawLine(45, 20, 81, 20, 0);
    OLED_DrawLine(45, 16, 45, 20, 0);
    OLED_DrawLine(81, 16, 81, 21, 0); 
}
