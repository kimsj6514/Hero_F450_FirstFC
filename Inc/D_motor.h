/*
 * D_motor.h
 *
 *  Created on: Aug 26, 2025
 *      Author: kimsj
 */

#ifndef DRONE_INC_D_MOTOR_H_
#define DRONE_INC_D_MOTOR_H_

#include "stdint.h"


typedef struct
{
	int16_t throttle;   //0 ~ 100
	int16_t roll;		//-50 ~ 50 (예시)
	int16_t pitch;		//-50 ~ 50 (예시)
	int16_t yaw;		//-50 ~ 50 (예시)
}ControlInput_t;


extern void Motors_Init(void);
extern void Motors_Set_Control_Input(ControlInput_t controls);
extern void Motors_Update_PWM(void);

//extern void pa6_tim3_ch1_pwm_lf_motor_run1(void);
//extern void data_pwm_process(void);

#endif /* DRONE_INC_D_MOTOR_H_ */
