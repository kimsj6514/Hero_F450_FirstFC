/*
 * ksj_adc.h
 *
 *  Created on: Aug 7, 2025
 *      Author: kimsj
 */

#ifndef INC_KSJ_ADC_H_
#define INC_KSJ_ADC_H_

#include "stm32f7xx_ll_adc.h"
#define STM32_BUILTIN_ADC_MAX_VALUE	4095 //because of 12bit
#define ADC_BUF_LEN	2
#define VREFINT_CAL	*VREFINT_CAL_ADDR

extern uint16_t adc_buf[ADC_BUF_LEN];
extern uint8_t adc_half_flag;
extern uint8_t adc_full_flag;

extern void adc_polling(void);
extern void adc3ch0_demo_using_normal_DMA(void);

#endif /* INC_KSJ_ADC_H_ */
