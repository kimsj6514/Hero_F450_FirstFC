/*
 * ksj_pwm.h
 *
 *  Created on: Aug 14, 2025
 *      Author: kimsj
 */

#ifndef INC_KSJ_PWM_H_
#define INC_KSJ_PWM_H_

#define PWM_DMA_MODE_EN

#define USER_DUTY 100//1000//100

extern void pa6_pwm_polling(void);
extern void pa6_pwm_dma_run(void);

#endif /* INC_KSJ_PWM_H_ */
