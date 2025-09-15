/*
 * ksj_led.c
 *
 *  Created on: Jul 31, 2025
 *      Author: kimsj
 */

#include "gpio.h"
#include "ksj_led.h"

void pe3_led_on(void)
{
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET);
}

void pe3_led_off(void)
{
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_RESET);
}

void pe3_led_blinking(void)
{
	HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_3);
}

void pe3_led_blinking_demol(void)
{
	 pe3_led_on();
		  HAL_Delay(200);//200msec
		  pe3_led_off();
		  HAL_Delay(200);//200msec
}

void pe3_led_blinking_demo2(void)
{
	static uint32_t prev_systick = 0;
	static uint32_t tog = 0;

	if (HAL_GetTick() >= prev_systick + 500)
	{
		if (tog == 0)
		{
			tog = 1;
			pe3_led_on();
		}
		else
		{
			tog = 0;
			pe3_led_off();
		}
		  prev_systick = HAL_GetTick();
	}
}
