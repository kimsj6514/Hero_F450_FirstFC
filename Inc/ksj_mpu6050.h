/*
 * ksj_mpu6050.h
 *
 *  Created on: Sep 4, 2025
 *      Author: kimsj
 */

#ifndef INC_KSJ_MPU6050_H_
#define INC_KSJ_MPU6050_H_


#define MPU6050_ADDR 0xD0


#define MPU6050_SMPRT_DIV 0X19
#define MPU6050_WHO_AM_I 0X75
#define MPU6050_CONFIG 0X1A
#define MPU6050_GYRO_CONFIG 0X1B
#define MPU6050_ACCEL_CONFIG 0X1C
#define MPU6050_INT_PIN_CFG 0X37
#define MPU6050_INT_ENABLE 0X38
#define MPU6050_INT_STATUS 0X3A
#define MPU6050_ACCEL_XOUT_H 0X3B
#define MPU6050_ACCEL_XOUT_L 0X3C
#define MPU6050_TEMP_XOUT_H 0X41
#define MPU6050_GYRO_XOUT_H 0X43
#define MPU6050_PWR_MGMT_1 0X6B //most important


#define MPU6050_INT_PORT 	GPIOE
#define MPU6050_INT_PIN 	GPIO_PIN_14

typedef struct
{
	// 가속도 센서
	int16_t ax;
	int16_t ay;
	int16_t az;
	// 각속도 센서
	int16_t gx;
	int16_t gy;
	int16_t gz;
	// 온도 센서
	int16_t temp;
} mpu6050_datatypes;
extern mpu6050_datatypes mpu6050_raw;
extern mpu6050_datatypes mpu6050_offset;
extern mpu6050_datatypes mpu6050_offset_reflect;

typedef struct _MPU6050{
	short acc_x_raw;
	short acc_y_raw;
	short acc_z_raw;
	short temperature_raw;
	short gyro_x_raw;
	short gyro_y_raw;
	short gyro_z_raw;

	float acc_x;
	float acc_y;
	float acc_z;
	float temperature;
	float gyro_x;
	float gyro_y;
	float gyro_z;
}Struct_MPU6050;


typedef struct
{
	// 가속도 센서
	double ax;
	double ay;
	double az;
	// 각속도 센서
	double gx;
	double gy;
	double gz;
	// 온도 센서
	double temp;
} mpu6050_converted_datatypes;
extern mpu6050_converted_datatypes mpu6050_converted;


typedef struct
{
	double x;
	double y;
	double z;
}mpu6050_angle_datatypes;
extern mpu6050_angle_datatypes mpu6050_gyro_angle;
extern mpu6050_angle_datatypes mpu6050_accel_angle;
extern mpu6050_angle_datatypes mpu6050_cf_angle;


extern void MPU6050_Writebyte(uint8_t reg_addr, uint8_t val);
extern void MPU6050_Writebytes(uint8_t reg_addr, uint8_t len, uint8_t* data);
extern void MPU6050_Readbyte(uint8_t reg_addr, uint8_t* data);
extern void MPU6050_Readbytes(uint8_t reg_addr, uint8_t len, uint8_t* data);

extern void MPU6050_Initialization(void);
extern void MPU6050_Get_LSB_Sensitivity(uint8_t FS_SCALE_GYRO, uint8_t FS_SCALE_ACC);
extern void MPU6050_DataConvert(Struct_MPU6050* mpu6050);

extern void MPU6050_ADO_Init(void);
extern void mpu6050_raw_datas_get(void);
extern int MPU6050_DataReady(void);

extern void mpu6050_accel_datas_get(void);
extern void mpu6050_gyro_datas_get(void);
extern void mpu6050_temp_data_get(void);

extern void mpu6050_offset_datas_get(void);


#define MPU6050_OFFSET_AVERAGING_CNT 1000
extern uint8_t mpu6050_offset_get_comleted;

extern void mpu6050_offset_reflect_datas_get(void);
extern void mpu6050_sensor_converted_demo(void);
extern void mpu6050_sensor_angle_demo(void);
extern void mpu6050_sensor_cf_angle_demo(void);





#endif /* INC_KSJ_MPU6050_H_ */
