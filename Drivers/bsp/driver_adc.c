/* USER CODE BEGIN Header */

/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "driver_adc.h"
#include "tim.h"
#include "adc.h"
#include "math.h"

float R_rnt[CHANNELS] = {0};
float temp[CHANNELS] = {0};
float voltage[CHANNELS] = {0};
uint16_t ADC_Value[CHANNELS] = {0};
float V_source = 0;

void bsp_adc_init(void)
{
    HAL_ADCEx_Calibration_Start(&hadc1);     
}

void ADC_timer_callback(void)
{
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ADC_Value, sizeof(ADC_Value) / sizeof(ADC_Value[0]));
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    voltage[2] = (ADC_Value[2]+1)*3.3/4096;
    R_rnt[2] = 100/((5/voltage[2])-1);
    temp[2] = Get_Temp(R_rnt[2], R25_2, B_2);
    
    voltage[1] = (ADC_Value[1]+1)*3.3/4096*4;
    
    voltage[0] = (ADC_Value[0]+1)*3.3/4096;
    R_rnt[0] = 10/((5/voltage[0])-1);
    temp[0] = Get_Temp(R_rnt[0], R25_1, B_1);
}

float Get_Temp(float rnt, float R25, float B)
{
	float temp;
	//like this R=5000, T2=273.15+25,B=3470, RT=5000*EXP(3470*(1/T1-1/(273.15+25)),  
	temp = rnt / R25;
	temp = log(temp);//ln(Rt/Rp)
	temp /= B;//ln(Rt/Rp)/B
	temp += (1/T25);
	temp = 1 / temp;
	temp -= Ka;
	return temp;
} 
