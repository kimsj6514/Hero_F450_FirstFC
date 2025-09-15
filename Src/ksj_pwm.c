/*
 * ksj_pwm.c
 *
 *  Created on: Aug 14, 2025
 *      Author: kimsj
 */

#include "stdint.h"
#include "tim.h"
#include "ksj_pwm.h"

#if !defined PWM_DMA_MODE_EN
void pa6_pwm_polling(void)
{
	static uint8_t led_phase=0;
	static uint16_t i=0;
	static uint32_t pwm_period_time = 0;
	static uint8_t pwm_psc_init =0;

	if (pwm_psc_init ==0)
	{
		pwm_psc_init = 1;
		htim3.Instance->PSC = 108-1;
	}

	if (HAL_GetTick() >= (pwm_period_time+10))
	{
		switch (led_phase)
		{
		case 0:
			htim3.Instance->CCR1 = i++; //CCR = CH(채널)
			if (i>= USER_DUTY)	led_phase = 1;
			break;
		case 1:
			htim3.Instance->CCR1 = i--;
			if (i == 0) led_phase = 0;
			break;
		}
		pwm_period_time = HAL_GetTick();
	}
}
#else

static uint8_t inc_pwm_finished = 0;
static uint16_t inc_pwm_buf[USER_DUTY]={0,};
static uint16_t dec_pwm_buf[USER_DUTY]={0,};

void pa6_pwm_dma_run(void)
{
	static uint16_t i=0;
	static uint8_t pwm_data_ready = 0;

	if (pwm_data_ready ==0)
	{
		//PSC for DMA 설정
		htim3.Instance->PSC = (1*100*108)-1; //(1*108)-1;
		//ARR 설정
		htim3.Instance->ARR = USER_DUTY-1;
		//CRR 설정
		for(i=0; i<USER_DUTY; i++)
		{
			inc_pwm_buf[i] = i;
			dec_pwm_buf[i] = USER_DUTY-i;
		}
		pwm_data_ready = 1;
	}

	if (inc_pwm_finished == 0)
	{
		HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_1, (uint32_t *)&inc_pwm_buf[0], USER_DUTY);
	}
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM3)
	{
		if (inc_pwm_finished == 0)
		{
			inc_pwm_finished = 1;

			HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_1, (uint32_t *)&dec_pwm_buf[0], USER_DUTY);
		}
		else
		{
			inc_pwm_finished = 0;

			HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_1, (uint32_t *)&inc_pwm_buf[0], USER_DUTY);
		}
	}
	// rununblock below if want to stio pwm
	//HAL_TIM_PWM_Stio_DMA(htim, TIM_CHANNEL_1);
}
#endif
