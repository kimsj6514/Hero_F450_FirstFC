/*
 * ksj_i2c.c
 *
 *  Created on: Aug 14, 2025
 *      Author: kimsj
 */

#include "stdint.h"
#include "stdio.h"
#include "i2c.h"
#include "ksj_i2c.h"


void lt_eeprom_demo2(void)
{
	static uint32_t prev_tick = 0;
	uint8_t tbuf[5] = {1,2,3,4,5};
	static uint8_t eeprom_write_run = 0;
	static uint8_t i2c_phase = 0;
	static uint32_t i2c_wait_cnt = 0;

	if (HAL_GetTick() >= (prev_tick+10))
	{
		switch (i2c_phase)
		{
		case 0:
			if (++i2c_wait_cnt >= 250)
			{
				i2c_wait_cnt = 0;
				if (eeprom_write_run == 0)
				{
					lt_eeprom_write(&tbuf[0]);
					printf("\r\n1_eeprom write run..");
					i2c_phase++;
					eeprom_write_run = 1;
				}
			}
			break;
		case 1:
			if (++i2c_wait_cnt >= 5)
			{
				printf("\r\n2_i2c_waited..");
				i2c_phase++;
				i2c_wait_cnt = 0;
			}
			break;
		case 2:
			//lt_eerprom_write(&tbuf[0]);
			//HAL_Delay(50);	//delay is very critical if there
			printf("\r\n3_eeprom read..");
			lt_eeprom_read();
			eeprom_write_run = 0;
			i2c_phase = 0;
			break;
		}
		prev_tick = HAL_GetTick();
	}
}


HAL_StatusTypeDef lt_eeprom_write(uint8_t *data)
{
	HAL_StatusTypeDef ret;

	ret = HAL_I2C_Mem_Write(&hi2c2, 0xA0, 0x0001, 1, data, 5, 1000);
	return ret;
}

HAL_StatusTypeDef lt_eeprom_read(void)
{
	uint8_t rx_Buffer[5] = {0x00,}, i=0;
	HAL_StatusTypeDef ret;

	ret = HAL_I2C_Mem_Read(&hi2c2, 0xA1, 0x0001, 1, rx_Buffer, 5, 1000);

	printf("\r\neeprom rd : ");

	for (i=0; i<5; i++)
	{
		printf("%d", rx_Buffer[i]);
	}
	return ret;
}
