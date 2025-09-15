/*
 * ksj_timer.h
 *
 *  Created on: Aug 7, 2025
 *      Author: kimsj
 */

#ifndef INC_KSJ_TIMER_H_
#define INC_KSJ_TIMER_H_


extern void timer_value_decrease_function(void);

extern void set_power_on_led_duration_timer(uint32_t TimerTick32);
extern uint32_t get_power_on_led_duration_timer(void);

////////////////////////////////////////////////////////////////////
extern void DWT_Init(void);
extern void DWT_Init_Set_Cyccnt(void);
extern uint32_t DWT_Get_Cyccnt(void);
extern void DWT_Delay_us(uint32_t us);
extern uint32_t DWT_Get_Time_Interval(void);
extern void inc_lt_systick64(void);
extern uint32_t get_lt_systick64(void);


#endif /* INC_KSJ_TIMER_H_ */
