/*
 * D_motor.c
 *
 *  Created on: Aug 26, 2025
 *      Author: kimsj
 */

#include "stdio.h"
#include "stdint.h"
#include "usart.h"
#include "tim.h"
#include "ksj_uart.h"
#include "D_motor.h"
#include "D_joystick.h"

#define PWM_MOTOR_MAX_DUTY 200
#define PWM_MOTOR_INTERVAL_DUTY 100
#define PWM_MOTOR_MIN_DUTY 100//1000//100


static ControlInput_t motor_controls = {0};

void Motors_Init(void)
{
	// 타이머의 기본 속도를 설정 (이 작업은 딱 한 번만 실행)
	htim3.Instance->PSC = 1080-1;

    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);

    htim3.Instance->CCR1 = 100;  // 1.0 ms
    htim3.Instance->CCR2 = 100;
    htim3.Instance->CCR3 = 100;
    htim3.Instance->CCR4 = 100;

    HAL_Delay(3000);  // 3초간 대기 → ESC 삑삑 하고 준비됨

}

void Motors_Set_Control_Input(ControlInput_t controls)
{
    // 조종실에서 보낸 새 명령서를 내부 서류함에 복사
    motor_controls = controls;
}

void Motors_Update_PWM(void)
{
    int16_t motor1_pwm = 0;
	int16_t motor2_pwm = 0;
	int16_t motor3_pwm = 0;
	int16_t motor4_pwm = 0;

    //  Quadcopter X-Type 믹싱 공식
    // 모터 번호: (2) (1)
    //          (3) (4)

    // 1단계: 모든 모터는 스로틀(기본 속도)에서 시작
    motor1_pwm = PWM_MOTOR_MIN_DUTY + motor_controls.throttle;
    motor2_pwm = PWM_MOTOR_MIN_DUTY + motor_controls.throttle;
    motor3_pwm = PWM_MOTOR_MIN_DUTY + motor_controls.throttle;
    motor4_pwm = PWM_MOTOR_MIN_DUTY + motor_controls.throttle;

    // 2단계: 피치(앞/뒤) 명령을 추가
    // 앞으로 가려면(-pitch), 앞쪽(1,2번) 모터는 느려지고, 뒤쪽(3,4번)은 빨라짐
    motor1_pwm = motor1_pwm - motor_controls.pitch;
    motor2_pwm = motor2_pwm - motor_controls.pitch;
    motor3_pwm = motor3_pwm + motor_controls.pitch;
    motor4_pwm = motor4_pwm + motor_controls.pitch;

    // 3단계: 롤(좌/우) 명령을 추가
    // 오른쪽으로 가려면(+roll), 오른쪽(1,4번) 모터는 느려지고, 왼쪽(2,3번)은 빨라짐
    // (※ 믹싱 공식은 FC 설정마다 +/-가 다를 수 있음.)
    motor1_pwm = motor1_pwm - motor_controls.roll;
    motor2_pwm = motor2_pwm + motor_controls.roll;
    motor3_pwm = motor3_pwm + motor_controls.roll;
    motor4_pwm = motor4_pwm - motor_controls.roll;

    // 4단계: 요(제자리 회전) 명령을 추가
    // 왼쪽으로 회전하려면(-yaw), 시계방향(2,4번) 모터는 느려지고, 반시계(1,3번)는 빨라짐
    motor1_pwm = motor1_pwm - motor_controls.yaw;
    motor2_pwm = motor2_pwm + motor_controls.yaw;
    motor3_pwm = motor3_pwm - motor_controls.yaw;
    motor4_pwm = motor4_pwm + motor_controls.yaw;

    // 최종 PWM 값이 100~200 범위를 벗어나지 않도록 안전하게 제한
    if (motor1_pwm < PWM_MOTOR_MIN_DUTY) motor1_pwm = PWM_MOTOR_MIN_DUTY;
    if (motor1_pwm > PWM_MOTOR_MAX_DUTY) motor1_pwm = PWM_MOTOR_MAX_DUTY;
    if (motor2_pwm < PWM_MOTOR_MIN_DUTY) motor2_pwm = PWM_MOTOR_MIN_DUTY;
    if (motor2_pwm > PWM_MOTOR_MAX_DUTY) motor2_pwm = PWM_MOTOR_MAX_DUTY;
    if (motor3_pwm < PWM_MOTOR_MIN_DUTY) motor3_pwm = PWM_MOTOR_MIN_DUTY;
    if (motor3_pwm > PWM_MOTOR_MAX_DUTY) motor3_pwm = PWM_MOTOR_MAX_DUTY;
    if (motor4_pwm < PWM_MOTOR_MIN_DUTY) motor4_pwm = PWM_MOTOR_MIN_DUTY;
    if (motor4_pwm > PWM_MOTOR_MAX_DUTY) motor4_pwm = PWM_MOTOR_MAX_DUTY;

    // 계산된 최종 값을 실제 타이머 하드웨어에 적용
    htim3.Instance->CCR3 = motor1_pwm; // 모터 1, RF
    htim3.Instance->CCR1 = motor2_pwm; // 모터 2, LF
    htim3.Instance->CCR2 = motor3_pwm; // 모터 3, LR
    htim3.Instance->CCR4 = motor4_pwm; // 모터 4, RR

}





/*void pa6_tim3_ch1_pwm_lf_motor_run1(void) //run1 : polling
{
	static uint8_t led_phase=0;
	static uint16_t i=0;
	static uint32_t pwm_period_time = 0;
	static uint8_t pwm_psc_init =0;

		if (pwm_psc_init ==0)
		{
			pwm_psc_init = 1;
			htim3.Instance->PSC = 1080-1;
		}

		if (HAL_GetTick() >= (pwm_period_time+10))
		{
			switch (led_phase)
			{
			case 0:
				htim3.Instance->CCR1 = PWM_MOTOR_MIN_DUTY + i++; //CCR = CH(채널)
				if (i>= PWM_MOTOR_INTERVAL_DUTY)	led_phase = 1;
				break;
			case 1:
				htim3.Instance->CCR1 = PWM_MOTOR_MIN_DUTY + i--;
				if (i == 0) led_phase = 0;
				break;
			}
			pwm_period_time = HAL_GetTick();
		}
}*/

/*void data_pwm_process(void)
{
	uint8_t data = 0;
	static uint8_t arrow_key = 0;
	static uint8_t key = 0;
	static uint8_t pwm_psc_init = 0;

	if (pwm_psc_init == 0)
	{
		pwm_psc_init = 1;
		htim3.Instance->PSC = 1080-1;
	}

	if(rcv1.wr_pos != rcv1.rd_pos)
	{
		data = rcv1.buf[rcv1.rd_pos];
		rcv1.rd_pos++;

		if(rcv1.rd_pos >= 10)
		rcv1.rd_pos = 0;

		switch(arrow_key)
		{
		case 0:
			if(data == 0x1b) // 1. 펏 번째 암호 'ESC' 문자인가?
			{
				arrow_key = 1;
			}
			break;


		case 1: //ESC를 받은 직후
			if(data == 0x5b) // 2. 두 번째 암호 '[' 문자인가?
			{
				arrow_key = 2; // 맞으면 마지막 암호를 기다리는 상태
			}
			else // 다른 문자가 들어오면 암호 실패, 평소 상태로 복귀.
			{
				arrow_key = 0;
			}
			break;

		case 2:
			if(data == 0x41) // 3. 위쪽'A' or 아래쪽'B' 화살표인가?
			{
				if (key < PWM_MOTOR_INTERVAL_DUTY)
				{
					key++;
				}
			}

			else if (data == 0x42)
			{
				if (key > 0)
				{
					key--;
				}
			}

			htim3.Instance->CCR1 = PWM_MOTOR_MIN_DUTY + key;
			printf("\r\nMotor Duty: %d", (int)htim3.Instance->CCR1) ;

			arrow_key = 0;// 암호 해독이 성공하든, 실패하든 원래대로 돌아간다.
			break;
		}
	}
}*/
