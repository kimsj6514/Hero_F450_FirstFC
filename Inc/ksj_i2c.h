/*
 * ksj_i2c.h
 *
 *  Created on: Aug 14, 2025
 *      Author: kimsj
 */

#ifndef INC_KSJ_I2C_H_
#define INC_KSJ_I2C_H_


extern void lt_eeprom_demo2(void);
extern HAL_StatusTypeDef lt_eeprom_write(uint8_t *data);
extern HAL_StatusTypeDef lt_eeprom_read(void);

#endif /* INC_KSJ_I2C_H_ */
