/*
 * ksj_rssi_led.c
 *
 *  Created on: Jul 31, 2025
 *      Author: kimsj
 */

#include "gpio.h"
#include "ksj_rssi_led.h"


// PD1 = LED1, LED on, off, LED Blinking
void pd1_rssi_led1_on(void)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_RESET);
}

void pd1_rssi_led1_off(void)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_SET);
}

void pd1_rssi_led1_blinking(void)
{
	static uint32_t prev_systick = 0;
	static uint32_t tog = 0;

	if (HAL_GetTick() >= prev_systick + 500)
	{
		if (tog == 0)
		{
			tog = 1;
			pd1_rssi_led1_on();
		}
		else
		{
			tog = 0;
			pd1_rssi_led1_off();
		}
		  prev_systick = HAL_GetTick();
	}
}


// PD2 = LED2, LED on, off, LED Blinking
void pd2_rssi_led2_on(void)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
}

void pd2_rssi_led2_off(void)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
}

void pd2_rssi_led2_blinking(void)
{
	static uint32_t prev_systick = 250;
	static uint32_t tog = 0;

	if (HAL_GetTick() >= prev_systick + 500)
	{
		if (tog == 0)
		{
			tog = 1;
			pd2_rssi_led2_on();
		}
		else
		{
			tog = 0;
			pd2_rssi_led2_off();
		}
		  prev_systick = HAL_GetTick();
	}
}


// PD3 = LED3, LED on, off, LED Blinking
void pd3_rssi_led3_on(void)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_RESET);
}

void pd3_rssi_led3_off(void)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_SET);
}

void pd3_rssi_led3_blinking(void)
{
	static uint32_t prev_systick = 500;
	static uint32_t tog = 0;

	if (HAL_GetTick() >= prev_systick + 500)
	{
		if (tog == 0)
		{
			tog = 1;
			pd3_rssi_led3_on();
		}
		else
		{
			tog = 0;
			pd3_rssi_led3_off();
		}
		  prev_systick = HAL_GetTick();
	}
}
