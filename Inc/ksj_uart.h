/*
 * ksj_uart.h
 *
 *  Created on: Jul 31, 2025
 *      Author: kimsj
 */

#ifndef INC_KSJ_UART_H_
#define INC_KSJ_UART_H_

//
//#define __RX_CHAR_NOBUF_USE__
#define __RX_CHAR_BUF_USE__
//#define __RX_STR_BUF_USE__
#define __RX_CMD_BUF_USE__

//
typedef struct
{
	uint8_t buf[10];
	uint8_t wr_pos;
	uint8_t rd_pos;
}uart_rcv_datatype1;
extern uart_rcv_datatype1 rcv1;

typedef struct
{
	uint8_t buf[5][10];
	uint8_t wr_pos;
	uint8_t rd_pos;
	uint8_t ch_pos;
}uart_rcv_datatype2;
extern uart_rcv_datatype2 rcv2;

//
extern int __io_putchar(int ch);

extern uint8_t rx_data1;

//
extern void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
extern void Keyboard_Input_Process(void);
//extern void rcv1_data_process(void);
extern void rcv2_str_process(void);
extern void cmd2_exec_process(void);



#endif /* INC_KSJ_UART_H_ */
