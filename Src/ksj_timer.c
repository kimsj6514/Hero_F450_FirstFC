/*
 * ksj_timer.c
 *
 *  Created on: Aug 7, 2025
 *      Author: kimsj
 */

/*
 * 왜 굳이 이렇게 분리했나?
코드 읽기 쉽고 명확하게 → set / get 이름만 봐도 “넣는지, 꺼내는지” 알 수 있음.

안전하게 접근 → 두 함수 다 __disable_irq() / __enable_irq()로 인터럽트 잠깐 막고 값 읽거나 쓰기.

직접 변수에 접근하지 않음
→ power_on_led_duration_timer_value = 3000; 이렇게 직접 건드리지 않고,
반드시 set()과 get()을 거치게 하면, 나중에 동작을 바꾸거나 보호 로직을 넣기 쉬움.
 */

#include "stdint.h"
#include "cmsis_gcc.h"
#include "stm32f7xx.h"
#include "ksj_timer.h"


uint32_t power_on_led_duration_timer_value = 0;


void timer_value_decrease_function(void)
{
	// lt systick64 increase
	inc_lt_systick64();

	//
	if (power_on_led_duration_timer_value > 0) power_on_led_duration_timer_value--;
}

void set_power_on_led_duration_timer(uint32_t TimerTick32)
{
	__disable_irq();

	power_on_led_duration_timer_value = TimerTick32;

	__enable_irq();
}

uint32_t get_power_on_led_duration_timer(void)
{
	uint32_t V32;

	__disable_irq();

	V32 = power_on_led_duration_timer_value;

	__enable_irq();

	return V32;
}

///////////////////////////////////////////////////////////////
void DWT_Init(void)
{
	DWT->LAR = 0xC5ACCE55;// for CM7 **이게 핵심

	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->CYCCNT = 0;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}


void DWT_Init_Set_Cyccnt(void)
{
	DWT->CYCCNT = 0;
}


void DWT_Delay_us(uint32_t us)
{
  uint32_t start = DWT->CYCCNT;
  uint32_t cycles = us * (SystemCoreClock / 1000000);

  while ((DWT->CYCCNT - start) < cycles);
}


uint32_t DWT_Get_Cyccnt(void)
{
	uint32_t us = (DWT->CYCCNT)/(SystemCoreClock / 1000000);

	return us;
}


uint32_t DWT_Get_Time_Interval(void)
{
	uint32_t dt=0;

	dt = DWT_Get_Cyccnt();
	DWT->CYCCNT = 0;

	return dt;
}

static uint64_t lt_systick_value64 = 0;
void inc_lt_systick64(void)
{
	lt_systick_value64++;
}

uint32_t get_lt_systick64(void)
{
	uint32_t tmp = 0;

	__disable_irq();
	tmp = lt_systick_value64;
	__enable_irq();

	return tmp;
}
