/*
 * D_joystick.c
 *
 *  Created on: Sep 3, 2025
 *      Author: kimsj
 */

#include "stdio.h"
#include "stdint.h"
#include "usart.h"
#include "ksj_sta_led.h"
#include "ksj_timer.h"
#include "ksj_uart.h"
#include "D_joystick.h"
#include "D_motor.h"


void Keyboard_Input_Process(void)
{
	uint8_t data = 0;
	// 조종 명령서를 작성하기 위한 변수
	static ControlInput_t keyboard_controls = {0};

	// '데드맨 스위치'를 위한 변수
	#define RC_TIMEOUT 200 // 0.5초 동안 입력이 없으면 중립으로
	static uint32_t prev_tick = 0;


	// 1. 새로운 키 입력이 있는지 확인
	if(rcv1.wr_pos != rcv1.rd_pos)
		{
			data = rcv1.buf[rcv1.rd_pos];
			rcv1.rd_pos++;

			if(rcv1.rd_pos >= 10)
			rcv1.rd_pos = 0;

		prev_tick = HAL_GetTick(); // 키가 눌렸으니 현재 시간을 기록합니다.

		// --- 키를 해석해서 명령서 내용을 채웁니다 ---
		// 왼쪽 스틱 (스로틀/요)
		if (data == 'w' && keyboard_controls.throttle < 100)
			{
			keyboard_controls.throttle += 1;
			}
		else if (data == 's' && keyboard_controls.throttle > 0)
			{
			keyboard_controls.throttle -= 1;
			}
		else if (data == 'a')
			{
			keyboard_controls.yaw = -1;
			}
		else if (data == 'd')
			{
			keyboard_controls.yaw = 1;
			}

		// 오른쪽 스틱 (피치/롤)
		else if (data == 'i')
			{
			keyboard_controls.pitch = 1;
			}
		else if (data == 'k')
			{
			keyboard_controls.pitch = -1;
			}
		else if (data == 'j')
			{
			keyboard_controls.roll = -1;
			}
		else if (data == 'l')
			{
			keyboard_controls.roll = 1;
			}
		printf("\r\nThrottle:%d, Roll:%d, Pitch:%d, Yaw:%d",
		       keyboard_controls.throttle,
		       keyboard_controls.roll,
		       keyboard_controls.pitch,
		       keyboard_controls.yaw);

	}

	// 2. 데드맨 스위치: 일정 시간 입력이 없었는지 확인
	if (HAL_GetTick() - prev_tick > RC_TIMEOUT)
	{
		// 0.2초간 입력이 없었다면 Roll, Pitch, Yaw는 0(중립)으로 되돌림
		keyboard_controls.roll = 0;
		keyboard_controls.pitch = 0;
		keyboard_controls.yaw = 0;
		// 스로틀은 마지막 값을 유지
	}

	// 3. 작성된 최종 명령서를 엔진실로 전달
	Motors_Set_Control_Input(keyboard_controls);
}

