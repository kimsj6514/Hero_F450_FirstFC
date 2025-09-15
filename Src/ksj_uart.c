/*
 * ksj_uart.c
 *
 *  Created on: Jul 31, 2025
 *      Author: kimsj
 */
#include "string.h"
#include "stdint.h"
#include "stdio.h"
#include "usart.h"
#include "ksj_uart.h"
#include "ksj_sta_led.h"
#include "D_motor.h"


int __io_putchar(int ch)
{
	(void)HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 100);
	return ch;
}

////////////////////////////////////////////////////////////////////

uint8_t rx_data1 = 0;

#if defined __RX_CHAR_NOBUF_USE__
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		printf("\r\nHello, I'm in uart ISR. : %c", rx_data1);

		HAL_UART_Receive_IT(&huart1, &rx_data1, 1);
	}
}
#elif defined __RX_CHAR_BUF_USE__

/*
typedef struct
{
	uint8_t buf[10];
	uint8_t wr_pos;
	uint8_t rd_pos;
}uart_rcv_datatype1;
uart_rcv_datatype1 rcv1;
*/
uart_rcv_datatype1 rcv1;


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		rcv1.buf[rcv1.wr_pos] = rx_data1;
		rcv1.wr_pos++;

		if(rcv1.wr_pos >= 10)
		rcv1.wr_pos = 0;

		HAL_UART_Receive_IT(&huart1, &rx_data1, 1);
		//printf("\r\nrx:%c", rx_data1);
	}
}

/*void Keyboard_Input_Process(void)
{
	uint8_t data = 0;
	// 조종 명령서를 작성하기 위한 변수
	static ControlInput_t keyboard_controls = {0};

	// '데드맨 스위치'를 위한 변수
	#define RC_TIMEOUT 200 // 0.5초 동안 입력이 없으면 중립으로
	static uint32_t last_rc_time = 0;

	// 1. 새로운 키 입력이 있는지 확인
	if(rcv1.wr_pos != rcv1.rd_pos)
		{
			data = rcv1.buf[rcv1.rd_pos];
			rcv1.rd_pos++;

			if(rcv1.rd_pos >= 10)
			rcv1.rd_pos = 0;

		last_rc_time = HAL_GetTick(); // 키가 눌렸으니 현재 시간을 기록합니다.

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
		printf("Throttle:%d, Roll:%d, Pitch:%d, Yaw:%d\r\n",
		       keyboard_controls.throttle,
		       keyboard_controls.roll,
		       keyboard_controls.pitch,
		       keyboard_controls.yaw);
	}

	// 2. 데드맨 스위치: 일정 시간 입력이 없었는지 확인
	if (HAL_GetTick() - last_rc_time > RC_TIMEOUT)
	{
		// 0.2초간 입력이 없었다면 Roll, Pitch, Yaw는 0(중립)으로 되돌림
		keyboard_controls.roll = 0;
		keyboard_controls.pitch = 0;
		keyboard_controls.yaw = 0;
		// 스로틀은 마지막 값을 유지
	}

	// 3. 작성된 최종 명령서를 엔진실로 전달
	Motors_Set_Control_Input(keyboard_controls);
}*/


void rcv1_data_process(void)
{
	uint8_t data = 0;
	static uint8_t arrow_key = 0;

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
			else
			{
				if(data == 'w')// up
				{
					printf("\r\nPD4_RLED_ON_UP (w Key)");
					ksj_sta_led_r_on();
					ksj_sta_led_y_off();
					ksj_sta_led_g_off();
					ksj_sta_led_w_off();
					//printf("\r\nrcv data : %c", data);
				}
				else if (data == 'z')//down
				{
					printf("\r\nPD5_YLED_ON_DOWN (z Key)");
					ksj_sta_led_r_off();
					ksj_sta_led_y_on();
					ksj_sta_led_g_off();
					ksj_sta_led_w_off();
					//printf("\r\nrcv data : %c", data);
				}
				else if (data == 'a')//left
				{
					printf("\r\nPD6_GLED_ON_LEFT (a Key)");
					ksj_sta_led_r_off();
					ksj_sta_led_y_off();
					ksj_sta_led_g_on();
					ksj_sta_led_w_off();
					//printf("\r\nrcv data : %c", data);
				}
				else if (data == 's')//right
				{
					printf("\r\nPD7_WLED_ON_RIGHT (s Key)");
					ksj_sta_led_r_off();
					ksj_sta_led_y_off();
					ksj_sta_led_g_off();
					ksj_sta_led_w_on();
					//printf("\r\nrcv data : %c", data);
				}
				printf("\r\nrcv data : %c", data);
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
			if(data == 0x41) // 3. 위쪽'A' 화살표인가?
			{
				printf("\r\nPD4_RLED_ON_UP Arrow)");
				ksj_sta_led_r_on();
				ksj_sta_led_y_off();
				ksj_sta_led_g_off();
				ksj_sta_led_w_off();
			}
			else if(data == 0x42) // 아래쪽'B' 화살표인가?
			{
				printf("\r\nPD5_YLED_ON_DOWN Arrow)");
				ksj_sta_led_r_off();
				ksj_sta_led_y_on();
				ksj_sta_led_g_off();
				ksj_sta_led_w_off();
			}
			else if(data == 'C') // 왼쪽'C' 화살표인가?
			{
				printf("\r\nPD6_GLED_ON_RIGHT Arrow)");
				ksj_sta_led_r_off();
				ksj_sta_led_y_off();
				ksj_sta_led_g_on();
				ksj_sta_led_w_off();
			}
			else if(data == 'D') // 오른쪽'D' 화살표인가?
			{
				printf("\r\nPD7_WLED_ON_LEFT Arrow)");
				ksj_sta_led_r_off();
				ksj_sta_led_y_off();
				ksj_sta_led_g_off();
				ksj_sta_led_w_on();
			}

			//timer set

			arrow_key = 0;// 암호 해독이 성공하든, 실패하든 원래대로 돌아간다.
			break;
			}
		}
}


#elif defined __RX_STR_BUF_USE__

/*
typedef struct
{
	uint8_t buf[5][10];
	uint8_t wr_pos;
	uint8_t rd_pos;
	uint8_t ch_pos;
}uart_rcv_datatype2;
uart_rcv_datatype2 rcv2;
*/
uart_rcv_datatype2 cmd2;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		if((rx_data1 != 0x0d) && (rx_data1 != 0x0a))
		{
			rcv2.buf[rcv2.wr_pos][rcv2.ch_pos] = rx_data1;
			rcv2.ch_pos++;

			if(rcv2.ch_pos >= 9)
				rcv2.ch_pos = 9;
		}
		else//if (rx_data1 == 0x0d) || (rx_data1 == 0x0a)
		{
			rx_data1 = 0;
			rcv2.buf[rcv2.wr_pos][rcv2.ch_pos] = rx_data1;
			rcv2.ch_pos = 0;
			rcv2.wr_pos++;

			if (rcv2.wr_pos >= 5)
				rcv2.wr_pos = 0;
		}

		HAL_UART_Receive_IT(&huart1, &rx_data1, 1);
	}
}

void rcv2_str_process(void)
{
	uint8_t *str = 0;

	if(rcv2.rd_pos != rcv2.wr_pos)
	{
		str = &rcv2.buf[rcv2.rd_pos][0];
		rcv2.rd_pos++;

		if(rcv2.rd_pos >= 5)
		rcv2.rd_pos = 0;

		printf("\r\nrcv str : %s", str);
	}
}
#elif defined __RX_CMD_BUF_USE__
uart_rcv_datatype2 cmd2;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		if((rx_data1 != 0x0d) && (rx_data1 != 0x0a))
		{
			printf("%c",rx_data1);
			cmd2.buf[cmd2.wr_pos][cmd2.ch_pos] = rx_data1;
			cmd2.ch_pos++;

			if(cmd2.ch_pos >= 9)
				cmd2.ch_pos = 9;
		}
		else//if (rx_data1 == 0x0d) || (rx_data1 == 0x0a)
		{
			rx_data1 = 0;
			cmd2.buf[cmd2.wr_pos][cmd2.ch_pos] = rx_data1;
			cmd2.ch_pos = 0;
			cmd2.wr_pos++;

			if (cmd2.wr_pos >= 5)
				cmd2.wr_pos = 0;
		}

		HAL_UART_Receive_IT(&huart1, &rx_data1, 1);
	}
}

void cmd2_exec_process(void)
{
	uint8_t *str = 0;

	if(cmd2.rd_pos != cmd2.wr_pos)
	{
		str = &cmd2.buf[cmd2.rd_pos][0];
		cmd2.rd_pos++;

		if(cmd2.rd_pos >= 5)
			cmd2.rd_pos = 0;

		//명령어 해독
		if(strcmp((char *)str, "pe1ledon") == 0)
		{
			printf("\r\nPE1 LED ON ..");
			pe1_gled_on();
		}
		else if (strcmp((char *)str, "pe1ledoff") == 0)
		{
			printf("\r\nPE1 LED OFF ..");
			pe1_gled_off();
		}
		else if (strcmp((char *)str, "pd6ledon") == 0)
		{
			printf("\r\nPD6 LED ON ..");
			pd6_bled_on();
		}
		else if (strcmp((char *)str, "pd6ledoff") == 0)
		{
			printf("\r\nPD6 LED OFF ..");
			pd6_bled_off();
		}
		else if (strcmp((char *)str, "pd5ledon") == 0)
		{
			printf("\r\nPD5 LED ON ..");
			pd5_wled_on();
		}
		else if (strcmp((char *)str, "pd5ledoff") == 0)
		{
			printf("\r\nPD5 LED OFF ..");
			pd5_wled_off();
		}
		else if (strcmp((char *)str, "pd4ledon") == 0)
		{
			printf("\r\nPD4 LED ON ..");
			pd4_yled_on();
		}
		else if (strcmp((char *)str, "pd4ledoff") == 0)
		{
			printf("\r\nPD4 LED OFF ..");
			pd4_yled_off();
		}
		else
		{
			printf("\r\nWrong Command !!");
		}
		printf("\r\n");

	}
}


#endif
