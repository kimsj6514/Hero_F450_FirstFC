/*
 * ksj_booting.c
 *
 *  Created on: Aug 7, 2025
 *      Author: kimsj
 */
#include "stdint.h"
#include "stdio.h"
#include "stm32f7xx_hal.h"
#include "ksj_timer.h"
#include "ksj_sta_led.h"
#include "ksj_booting.h"

void Power_on_led_running(void)
{
	set_power_on_led_duration_timer(3000); //3초 설정

	while(1)
	{
		 if (get_power_on_led_duration_timer() == 0)
		 {
			 ksj_sta_led_w_off();
			 break;
		 }

		 else
			 ksj_sta_led_w_on();
	}
}


#define INIT_LED_PERIOD 250
void all_drone_peripheral_init(void)
{
	static uint32_t prev_systick = 0;
	static uint8_t init_phase = 0;
	static uint8_t led_phase = 0;
	static uint32_t wait_cnt = 0;

	while (1)
	{
		switch(init_phase)
		{
		case 0:
			printf("\r\nSensor 1 Init..");
			//드론 초기화 모의 시간1
			init_phase++;
			break;
		case 1:
			wait_cnt++;
			if (wait_cnt > 5000000)
			{
				wait_cnt = 0;
				init_phase++;
			}
			break;
		case 2:
			printf("\r\nSensor 1 Init..");
			//드론 초기화 모의 시간2
			init_phase++;
			break;
		case 3:
			wait_cnt++;
			if (wait_cnt > 5000000)
			{
				wait_cnt = 0;
				init_phase++;
			}
			break;
		case 4:
			ksj_sta_led_r_off();
			ksj_sta_led_y_off();
			ksj_sta_led_g_off();
			init_phase=0;
			printf("\r\nSensor 1 Init Completed!!!");
			return;
		}

		if (HAL_GetTick() >= prev_systick + INIT_LED_PERIOD)
		{
			switch(led_phase)
			{
			case 0:
				ksj_sta_led_r_on();
				led_phase++;
				break;
			case 1:
				ksj_sta_led_y_on();
				led_phase++;
				break;
			case 2:
				ksj_sta_led_g_on();
				led_phase++;
				break;
			case 3:
				ksj_sta_led_r_off();
				ksj_sta_led_y_off();
				ksj_sta_led_g_off();
				led_phase=0;
				break;
			}
			prev_systick = HAL_GetTick();
		}
	}
}
