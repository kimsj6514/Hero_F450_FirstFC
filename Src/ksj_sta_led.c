/*
 * ksj_sta_led.c
 *
 *  Created on: Aug 4, 2025
 *      Author: kimsj
 */

#include "gpio.h"
#include "ksj_sta_led.h"


// PD4 = Status_Red_LED, LED on, off
void ksj_sta_led_r_on(void)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4,GPIO_PIN_RESET);
}

void ksj_sta_led_r_off(void)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4,GPIO_PIN_SET);
}


// PD5 = Status_Yellow_LED, LED on, off
void ksj_sta_led_y_on(void)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5,GPIO_PIN_RESET);
}

void ksj_sta_led_y_off(void)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5,GPIO_PIN_SET);
}


// PD6 = Status_Green_LED, LED on, off
void ksj_sta_led_g_on(void)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6,GPIO_PIN_RESET);
}

void ksj_sta_led_g_off(void)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6,GPIO_PIN_SET);
}


// PD7 = Status_White_LED, LED on, off
void ksj_sta_led_w_on(void)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7,GPIO_PIN_RESET);
}

void ksj_sta_led_w_off(void)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7,GPIO_PIN_SET);
}


// PD4,5,6,7 = Status_LED, Blinking
void ksj_sta_led_blinking(void)

{
	static uint32_t tog_time = 0;
	static uint32_t state = 0;

	if (HAL_GetTick() >= tog_time + 500)
	{
		tog_time = HAL_GetTick();

		switch(state)
		{
		case 0:
			ksj_sta_led_r_on();
			break;
		case 1:
			ksj_sta_led_y_on();
			break;
		case 2:
			ksj_sta_led_g_on();
			break;
		case 3:
			ksj_sta_led_w_on();
			break;
		case 4:
			ksj_sta_led_w_off();
			break;
		case 5:
			ksj_sta_led_g_off();
			break;
		case 6:
			ksj_sta_led_y_off();
			break;
		case 7:
			ksj_sta_led_r_off();
			break;
		}

		state++;
		if (state >= 8) state = 0;
	}
}

void all_led_off(void)
{
	ksj_sta_led_w_off();
	ksj_sta_led_g_off();
	ksj_sta_led_y_off();
	ksj_sta_led_r_off();
}

void all_led_on(void)
{
	ksj_sta_led_w_on();
	ksj_sta_led_g_on();
	ksj_sta_led_y_on();
	ksj_sta_led_r_on();
}

////////////////////////////////////////////////////////////////////
