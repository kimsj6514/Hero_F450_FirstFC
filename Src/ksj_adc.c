/*
 * ksj_adc.c
 *
 *  Created on: Aug 7, 2025
 *      Author: kimsj
 */

#include "stdio.h"
#include "adc.h"
#include "ksj_adc.h"



void adc_polling(void)
{
	uint16_t value = 0;
	static uint8_t adc_cnt=0;
	uint32_t Vadc1=0, Vadc21=0, Vadc22=0, droneV=0, droneV1=0, droneV2=0;
	static uint32_t prev_systick = 0;

	if (HAL_GetTick() >= (prev_systick + 2500))
	{
		HAL_ADC_Start(&hadc3);
		HAL_ADC_PollForConversion(&hadc3, 10);
		value = HAL_ADC_GetValue(&hadc3);

		//real measure

		Vadc1 = (3.3*value*1000)/STM32_BUILTIN_ADC_MAX_VALUE;	//4096	:3.3v = adc:Vx = 3.3v*adc / 4096

		Vadc21 = Vadc1/1000;
		Vadc22 = Vadc1%1000;

		droneV = (Vadc1*(100+36)/36);
		droneV1 = droneV/1000;
		droneV2 = droneV%1000;

		HAL_ADC_Stop(&hadc3);

		adc_cnt++;
		printf("\r\nadc_cnt2 : %d, adc_value : %d, Vadc: %d.%d[V]",
					(int)adc_cnt, (int)value, (int)Vadc21, (int)Vadc22);
		printf("\r\ndroneV: %d.%d[V]",
					(int)droneV1, (int)droneV2);
		prev_systick = HAL_GetTick();
	}
}


// using DMA
uint16_t adc_buf[ADC_BUF_LEN]={0,};
/* commented by LT
At * .ioc file,
1. Scan Conversion Mode       --> Disabled
   Continuous Conversion Mode --> Enabled
   DMA Continuous Request	  --> Enabled
2. Number of Conversion		  --> 1
3. Rank adc3-ch0 only
 */
void adc3ch0_demo_using_normal_DMA(void)  // using normal DMA
{
	uint16_t value = 0;
	uint32_t Vadc1=0, Vadc21=0, Vadc22=0;
	static uint32_t prev_systick = 0;
	static uint32_t adc_cnt =0;

	if (HAL_GetTick() >= (prev_systick + 2500))
	{
		printf("\r\n\n>> ADC Normal DMA Processing using VDDA[%d] <<", (int)++adc_cnt);
		if (adc_half_flag == 1)
		{
			adc_half_flag = 0;

			value = adc_buf[0];

			Vadc1 = (3.3*value*1000)/STM32_BUILTIN_ADC_MAX_VALUE;
			Vadc21 = Vadc1/1000;
			Vadc22 = Vadc1%1000;

			printf("\r\n\n1_half vrefint-cal : %d", (int)VREFINT_CAL);
			printf("\r\n  adc_raw_value : %d, Vadc : %d.%d[V]",
					(int)value, (int)Vadc21, (int)Vadc22);
			adc_buf[0] = 0;
		}

		if (adc_full_flag ==1)
		{
			adc_full_flag = 0;

			value = adc_buf[0]; // <- 이거 물어보기 value = adc_buf[1]이 아니라 왜 0인지?

			Vadc1 = (3.3*value*1000)/STM32_BUILTIN_ADC_MAX_VALUE;
			Vadc21 = Vadc1/1000;
			Vadc22 = Vadc1%1000;

			printf("\r\n\n2_full vrefint-cal : %d",(int)(VREFINT_CAL));
			printf("\r\n   adc_raw_value : %d, Vadc : %d.%d[V]",
					(int)value, (int)Vadc21, (int)Vadc22);
			adc_buf[1] = 0;

			HAL_ADC_Start_DMA(&hadc3, (uint32_t *)adc_buf, ADC_BUF_LEN);
		}
		prev_systick = HAL_GetTick();
	}
}

// 밑에 정의된 이유는 부가적인 콜백함수는 이미 헤더파일에 존재를 알고 있고 컴파일러가 미리 알고 있기 때문에 구현부가 어디에 있는 상관이 없음.
// 정리를 쉽고 구분하기 쉽게 정리함
uint8_t adc_half_flag = 0;
uint8_t adc_full_flag = 0;
void HAL_ADC_ConvHalfCpltfcallback(ADC_HandleTypeDef* hadc)
{
	adc_half_flag = 0;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	adc_full_flag = 1;
}
