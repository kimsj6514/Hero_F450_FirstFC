/*
 * ksj_mpu6050.c
 *
 *  Created on: Sep 4, 2025
 *      Author: kimsj
 */

#include "stdio.h"
#include "stdint.h"
#include "math.h"
#include "i2c.h"
#include "ksj_mpu6050.h"
#include "ksj_timer.h"


Struct_MPU6050 MPU6050;

static float LSB_Sensitivity_ACC = 0.0;
static float LSB_Sensitivity_GYRO = 0.0;

//////////////////////////////////////////////////////////////////

mpu6050_datatypes mpu6050_raw;
//

mpu6050_angle_datatypes mpu6050_cf_angle={0.0, 0.0, 0.0};
void mpu6050_sensor_cf_angle_demo(void)// cf : complement filter
{
	if (mpu6050_offset_get_comleted == 1)
	{
	// 1. sensor raw data read
	mpu6050_accel_datas_get();
	mpu6050_gyro_datas_get();
	mpu6050_temp_data_get();

	// 2. get measure time interval
	float dt = DWT_Get_Time_Interval()/1000000.0;
	//--float dt = get_time_interval();

	// 3. reflect sensor offset data
	mpu6050_offset_reflect.ax = mpu6050_raw.ax - mpu6050_offset.ax;
	mpu6050_offset_reflect.ay = mpu6050_raw.ay - mpu6050_offset.ay;
	mpu6050_offset_reflect.az = mpu6050_raw.az - mpu6050_offset.az;
	mpu6050_offset_reflect.az += LSB_Sensitivity_ACC;//16384; //
	// for gyro
	mpu6050_offset_reflect.gx = mpu6050_raw.gx - mpu6050_offset.gx;
	mpu6050_offset_reflect.gy = mpu6050_raw.gy - mpu6050_offset.gy;
	mpu6050_offset_reflect.gz = mpu6050_raw.gz - mpu6050_offset.gz;

	// 4. convert
	/*mpu6050_converted.ax = (float)mpu6050_offset_reflect.ax/LSB_Sensitivity_ACC;
	mpu6050_converted.ay = (float)mpu6050_offset_reflect.ay/LSB_Sensitivity_ACC;
	mpu6050_converted.az = (float)mpu6050_offset_reflect.az/LSB_Sensitivity_ACC;

	mpu6050_converted.gx = (float)mpu6050_offset_reflect.gx/LSB_Sensitivity_GYRO;
	mpu6050_converted.gy = (float)mpu6050_offset_reflect.gy/LSB_Sensitivity_GYRO;
	mpu6050_converted.gz = (float)mpu6050_offset_reflect.gz/LSB_Sensitivity_GYRO;

	mpu6050_converted.temp = (float)((mpu6050_raw.temp/340)+36.53);*/

	// LSB_Sensitivity_GYRO : default 131
	mpu6050_converted.gx = (double)mpu6050_offset_reflect.gx/LSB_Sensitivity_GYRO;
	mpu6050_converted.gy = (double)mpu6050_offset_reflect.gy/LSB_Sensitivity_GYRO;
	mpu6050_converted.gz = (double)mpu6050_offset_reflect.gz/LSB_Sensitivity_GYRO;

	mpu6050_converted.temp = (float)((mpu6050_raw.temp/340)+36.53);


	// 5-1. angle using gyro
	mpu6050_gyro_angle.x += mpu6050_converted.gx * dt;
	mpu6050_gyro_angle.y += mpu6050_converted.gy * dt;
	mpu6050_gyro_angle.z += mpu6050_converted.gz * dt;

	// 5-2. angle using accelarator
	const float Radian_To_Degree = 180/3.14159;
	// for angle_y
	double denominator_yz = sqrt(pow(mpu6050_raw.ay, 2)+pow(mpu6050_raw.az, 2));
	mpu6050_accel_angle.y = atan(-mpu6050_offset_reflect.ax/denominator_yz)*Radian_To_Degree;
	// for angle_x
	double denominator_xz = sqrt(pow(mpu6050_raw.ax, 2)+pow(mpu6050_raw.az, 2));
	mpu6050_accel_angle.x = atan(-mpu6050_offset_reflect.ay/denominator_xz)*Radian_To_Degree;
	// for angle z
	mpu6050_accel_angle.z = 0.0;

	// 6. get angle using cf(complementary filter)
	const double CF_ALPHA = 0.96;//96%

	mpu6050_cf_angle.x = CF_ALPHA * (mpu6050_cf_angle.x + mpu6050_converted.gx * dt) + (1.0 - CF_ALPHA)*mpu6050_accel_angle.x;
	mpu6050_cf_angle.y = CF_ALPHA * (mpu6050_cf_angle.y + mpu6050_converted.gx * dt) + (1.0 - CF_ALPHA)*mpu6050_accel_angle.y;
	mpu6050_cf_angle.z = mpu6050_gyro_angle.z; // 정확한 값을 위하여는 바로미터가 필요하나 현재 사용치 않아 자이로 센서값 활용


#if 1 //unblock for test
	static uint64_t prev_systick64 = 0;
	static uint32_t measure_cnt = 0;

	if (get_lt_systick64() >= prev_systick64 + 1000)
	{
		++measure_cnt;
		printf("\r\n\n");
		printf("\r\n>> MPU6050 Sensor Measure : %u <<", (int)measure_cnt);
		//printf("\r\ntime interval : %u[us]", (unsigned int)dt);
		printf("\r\ntime interval : %f[sec], %f[msec]", dt, dt*1000);
		//printf("\r\ntime interval : %f", dt);
#if 1//0//---
		printf("\r\n>> Angle by Gyroscope Sensor <<");
		printf("\r\ngyro_angle.x=%8.2f[D]	| gyro_angle.y=%8.2f[D]	| gyro_angle.z=%8.2f[D]"
				,mpu6050_gyro_angle.x, mpu6050_gyro_angle.y, mpu6050_gyro_angle.z);
		printf("\r\n>> Angle by Accelaration Sensor <<");
					printf("accel_angle.x=%8.2f[D]	| accel_angle.y=%8.2f[D]	| accel_angle.z=%8.2f[D]"
							, mpu6050_accel_angle.x, mpu6050_accel_angle.y, mpu6050_accel_angle.z);
		printf("\r\n>> Angle by Complementary Filter <<");
					printf("\r\ncf_angle.x=%8.2f[D]	| cf_angle.y=%8.2f[D]	| cf_angle.z=%8.2f[D]"
							, mpu6050_cf_angle.x, mpu6050_cf_angle.y, mpu6050_cf_angle.z);

#endif//---

	prev_systick64 = get_lt_systick64();
	}
#endif
#if 1// for accruate time interval
	DWT_Init_Set_Cyccnt();
#endif
	}
}



//
mpu6050_angle_datatypes mpu6050_gyro_angle={0.0, 0.0, 0.0};
mpu6050_angle_datatypes mpu6050_accel_angle={0.0, 0.0, 0.0};
void mpu6050_sensor_angle_demo(void)
{
	if (mpu6050_offset_get_comleted == 1)
	{
	// 1. sensor raw data read
	mpu6050_accel_datas_get();
	mpu6050_gyro_datas_get();
	mpu6050_temp_data_get();

	// 2. get measure time interval
	float dt = DWT_Get_Time_Interval()/1000000.0;
	//--float dt = get_time_interval();

	// 3. reflect sensor offset data
	mpu6050_offset_reflect.ax = mpu6050_raw.ax - mpu6050_offset.ax;
	mpu6050_offset_reflect.ay = mpu6050_raw.ay - mpu6050_offset.ay;
	mpu6050_offset_reflect.az = mpu6050_raw.az - mpu6050_offset.az;
	mpu6050_offset_reflect.az += LSB_Sensitivity_ACC;//16384; //
	// for gyro
	mpu6050_offset_reflect.gx = mpu6050_raw.gx - mpu6050_offset.gx;
	mpu6050_offset_reflect.gy = mpu6050_raw.gy - mpu6050_offset.gy;
	mpu6050_offset_reflect.gz = mpu6050_raw.gz - mpu6050_offset.gz;

	// 4. convert
	mpu6050_converted.ax = (float)mpu6050_offset_reflect.ax/LSB_Sensitivity_ACC;
	mpu6050_converted.ay = (float)mpu6050_offset_reflect.ay/LSB_Sensitivity_ACC;
	mpu6050_converted.az = (float)mpu6050_offset_reflect.az/LSB_Sensitivity_ACC;

	mpu6050_converted.gx = (float)mpu6050_offset_reflect.gx/LSB_Sensitivity_GYRO;
	mpu6050_converted.gy = (float)mpu6050_offset_reflect.gy/LSB_Sensitivity_GYRO;
	mpu6050_converted.gz = (float)mpu6050_offset_reflect.gz/LSB_Sensitivity_GYRO;

	mpu6050_converted.temp = (float)((mpu6050_raw.temp/340)+36.53);


	// 5-1. angle using gyro
	mpu6050_gyro_angle.x += mpu6050_converted.gx * dt;
	mpu6050_gyro_angle.y += mpu6050_converted.gy * dt;
	mpu6050_gyro_angle.z += mpu6050_converted.gz * dt;

	// 5-2. angle using accelarator
	const float Radian_To_Degree = 180/3.14159;
	// for angle_y
	double denominator_yz = sqrt(pow(mpu6050_raw.ay, 2)+pow(mpu6050_raw.az, 2));
	mpu6050_accel_angle.y = atan(-mpu6050_offset_reflect.ax/denominator_yz)*Radian_To_Degree;
	// for angle_x
	double denominator_xz = sqrt(pow(mpu6050_raw.ax, 2)+pow(mpu6050_raw.az, 2));
	mpu6050_accel_angle.x = atan(-mpu6050_offset_reflect.ay/denominator_xz)*Radian_To_Degree;
	// for angle z
	mpu6050_accel_angle.z = 0.0;

#if 1 //unblock for test
	static uint64_t prev_systick64 = 0;
	static uint32_t measure_cnt = 0;

	if (get_lt_systick64() >= prev_systick64 + 1000)
	{
		++measure_cnt;
		printf("\r\n\n");
		printf("\r\n>> Angle by Gyroscope Sensor : %d <<", (int)measure_cnt);
		//printf("\r\ntime interval : %u[us]", (unsigned int)dt);
		printf("\r\ntime interval : %f[sec], %f[msec]", dt, dt*1000);
		//printf("\r\ntime interval : %f", dt);
#if 1//0//---
		printf("\r\ngyro_angle.x=%8.2f[D]	| gyro_angle.y=%8.2f[D]	| gyro_angle.z=%8.2f[D]"
				,mpu6050_gyro_angle.x, mpu6050_gyro_angle.y, mpu6050_gyro_angle.z);
		printf("\r\n>> Angle by Accelaration Sensor : %d <<", (int)measure_cnt);
					printf("accel_angle.x=%8.2f[D]	| accel_angle.y=%8.2f[D]	| accel_angle.z=%8.2f[D]"
							, mpu6050_accel_angle.x, mpu6050_accel_angle.y, mpu6050_accel_angle.z);

#endif//---

	prev_systick64 = get_lt_systick64();
	}
#endif
#if 1// for accruate time interval
	DWT_Init_Set_Cyccnt();
#endif
	}
}


//
mpu6050_converted_datatypes mpu6050_converted;
void mpu6050_sensor_converted_demo(void)
{
	if (mpu6050_offset_get_comleted == 1)
	{
	mpu6050_accel_datas_get();
	mpu6050_gyro_datas_get();
	mpu6050_temp_data_get();

	mpu6050_offset_reflect.ax = mpu6050_raw.ax - mpu6050_offset.ax;
	mpu6050_offset_reflect.ay = mpu6050_raw.ay - mpu6050_offset.ay;
	mpu6050_offset_reflect.az = mpu6050_raw.az - mpu6050_offset.az;
	mpu6050_offset_reflect.az += LSB_Sensitivity_ACC;//16384; //
	// for gyro
	mpu6050_offset_reflect.gx = mpu6050_raw.gx - mpu6050_offset.gx;
	mpu6050_offset_reflect.gy = mpu6050_raw.gy - mpu6050_offset.gy;
	mpu6050_offset_reflect.gz = mpu6050_raw.gz - mpu6050_offset.gz;


	mpu6050_converted.ax = (float)mpu6050_offset_reflect.ax/LSB_Sensitivity_ACC;
	mpu6050_converted.ay = (float)mpu6050_offset_reflect.ay/LSB_Sensitivity_ACC;
	mpu6050_converted.az = (float)mpu6050_offset_reflect.az/LSB_Sensitivity_ACC;

	mpu6050_converted.gx = (float)mpu6050_offset_reflect.gx/LSB_Sensitivity_GYRO;
	mpu6050_converted.gy = (float)mpu6050_offset_reflect.gy/LSB_Sensitivity_GYRO;
	mpu6050_converted.gz = (float)mpu6050_offset_reflect.gz/LSB_Sensitivity_GYRO;

	mpu6050_converted.temp = (float)((mpu6050_raw.temp/340)+36.53);


#if 1 //unblock for test
	static uint32_t prev_systick = 0;

	if (HAL_GetTick() >= prev_systick + 1000)
	{
	printf("\r\n\n");
	printf("\r\n>>---------- MPU6050 Offset Converted Data ----------");
	printf("\r\n-----------------------------------------------------");
	printf("\r\n	x	|	Y	|	 z	");
	printf("\r\n-----------------------------------------------------");
	printf("\r\nOffApp_Ax=%d | OffApp_Ay=%d | OffApp_Az=%d",
			mpu6050_offset_reflect.ax, mpu6050_offset_reflect.ay, mpu6050_offset_reflect.az);
	printf("\r\nConv_Ax=%1.2f[g] | Conv_Ay=%1.2f[g]  | Conv_Az=%1.2f[g]",
				mpu6050_converted.ax, mpu6050_converted.ay, mpu6050_converted.az);
	printf("\r\n-----------------------------------------------------");
	printf("\r\nOffApp_Gx=%d | OffApp_Gy=%d | OffApp_Gz=%d",
				mpu6050_offset_reflect.gx, mpu6050_offset_reflect.gy, mpu6050_offset_reflect.gz);
	printf("\r\nConv_Gx=%03.2f[D] | Conv_Gy=%03.2f[D]  | Conv_Gz=%03.2f[D]",
					mpu6050_converted.gx, mpu6050_converted.gy, mpu6050_converted.gz);
	printf("\r\n-----------------------------------------------------");
	printf("\r\n>> Temp Sensor <<");
	printf("\r\nTraw=%d, Tcel=%02.2f[C]",
			mpu6050_raw.temp, mpu6050_converted.temp);

	prev_systick = HAL_GetTick();
	}
#endif
	}
}

//
mpu6050_datatypes mpu6050_offset_reflect;
void mpu6050_offset_reflect_datas_get(void)
{
	if (mpu6050_offset_get_comleted == 1)

	mpu6050_accel_datas_get();
	mpu6050_gyro_datas_get();
	mpu6050_temp_data_get();

	// reflect sensor offset data
	// for acc
	mpu6050_offset_reflect.ax = mpu6050_raw.ax - mpu6050_offset.ax;
	mpu6050_offset_reflect.ay = mpu6050_raw.ay - mpu6050_offset.ay;
	mpu6050_offset_reflect.az = mpu6050_raw.az - mpu6050_offset.az;
	mpu6050_offset_reflect.az += LSB_Sensitivity_ACC;//16384; //
	// for gyro
	mpu6050_offset_reflect.gx = mpu6050_raw.gx - mpu6050_offset.gx;
	mpu6050_offset_reflect.gy = mpu6050_raw.gy - mpu6050_offset.gy;
	mpu6050_offset_reflect.gz = mpu6050_raw.gz - mpu6050_offset.gz;

	// no need

#if 1 //unblock for test
	static uint32_t prev_systick = 0;

	if (HAL_GetTick() >= prev_systick + 1000)
	{
	printf("\r\n");
	printf("\r\n>>---------- MPU6050 Offset Reflect Data ----------");
	printf("\r\nax:%d, ay:%d, az:%d",
			mpu6050_offset_reflect.ax, mpu6050_offset_reflect.ay, mpu6050_offset_reflect.az);

	printf("\r\ngx:%d, gy:%d, gz:%d",
				mpu6050_offset_reflect.gx, mpu6050_offset_reflect.gy, mpu6050_offset_reflect.gz);

	prev_systick = HAL_GetTick();
	}
#endif
}


//
mpu6050_datatypes mpu6050_offset={0,};
uint8_t mpu6050_offset_get_comleted = 0;
void mpu6050_offset_datas_get(void)
{
	static int32_t sum_ax=0, sum_ay=0, sum_az=0;
	static int32_t sum_gx=0, sum_gy=0, sum_gz=0;
	static int32_t sum_temp=0;
	static int32_t mpu6050_averaging_cnt = MPU6050_OFFSET_AVERAGING_CNT;
	static int32_t prev_systick = 0;


	if(mpu6050_offset_get_comleted == 1)
		return;

	do
	{
		if (HAL_GetTick() >= prev_systick + 1)
		{
#if 0
	mpu6050_raw_datas_get();
#else
	mpu6050_accel_datas_get();
	mpu6050_gyro_datas_get();
	mpu6050_temp_data_get();
#endif
	sum_ax += mpu6050_raw.ax;
	sum_ay += mpu6050_raw.ay;
	sum_az += mpu6050_raw.az;
	sum_gx += mpu6050_raw.gx;
	sum_gy += mpu6050_raw.gy;
	sum_gz += mpu6050_raw.gz;
	sum_temp += mpu6050_raw.temp;

	mpu6050_averaging_cnt--;

	prev_systick = HAL_GetTick();
		}
	} while(mpu6050_averaging_cnt>0);

	mpu6050_offset.ax = (int16_t)(sum_ax/MPU6050_OFFSET_AVERAGING_CNT);
	mpu6050_offset.ay = (int16_t)(sum_ay/MPU6050_OFFSET_AVERAGING_CNT);
	mpu6050_offset.az = (int16_t)(sum_az/MPU6050_OFFSET_AVERAGING_CNT);

	mpu6050_offset.gx = (int16_t)(sum_gx/MPU6050_OFFSET_AVERAGING_CNT);
	mpu6050_offset.gy = (int16_t)(sum_gy/MPU6050_OFFSET_AVERAGING_CNT);
	mpu6050_offset.gz = (int16_t)(sum_gz/MPU6050_OFFSET_AVERAGING_CNT);

	mpu6050_offset_get_comleted = 1;

#if 1//0//1 unblock for test
	printf("\r\n>>---------- MPU6050 Offset Data ----------");
	printf("\r\nax:%d, ay:%d, az:%d",
			mpu6050_offset.ax, mpu6050_offset.ay, mpu6050_offset.az);

	printf("\r\ngx:%d, gy:%d, gz:%d",
				mpu6050_offset.gx, mpu6050_offset.gy, mpu6050_offset.gz);

	//prev_systick = HAL_GetTick();
#endif
}
//


//
void mpu6050_raw_datas_get(void)
{

#if 0//1orig, very unstable

	uint8_t data[14]={0,};
	uint8_t i=0;

	if(MPU6050_DataReady() == 1)
	{
	#if 0//1//nok
	//MPU6050_Readbytes (MPU6050_ACCEL_XOUT_H, 14, &data[0]);
	#else//ok
		for (i=0; i<14; i++)
			MPU6050_Readbyte(MPU6050_ACCEL_XOUT_H, (uint8_t *)&data[i]);
	#endif
		mpu6050_raw.ax = (int16_t)((data[0] << 8) | data[1]);
		mpu6050_raw.ay = (int16_t)((data[2] << 8) | data[3]);
		mpu6050_raw.az = (int16_t)((data[4] << 8) | data[5]);

		mpu6050_raw.temp = (int16_t)((data[6] << 8) | data[7]);

		mpu6050_raw.gx = (int16_t)((data[8] << 8) | data[9]);
		mpu6050_raw.gy = (int16_t)((data[10] << 8) | data[11]);
		mpu6050_raw.gz = (int16_t)((data[12] << 8) | data[13]);
	}
#else
	mpu6050_accel_datas_get();
	mpu6050_gyro_datas_get();
	mpu6050_temp_data_get();
#endif


#if 1 //unblock for test
	static uint32_t prev_systick = 0;

	if (HAL_GetTick() >= prev_systick + 1000)
	{
	printf("\r\n>>---------- MPU6050 Raw Data ----------");
	printf("\r\nax:%d, ay:%d, az:%d",
			mpu6050_raw.ax, mpu6050_raw.ay, mpu6050_raw.az);

	printf("\r\ngx:%d, gy:%d, gz:%d",
				mpu6050_raw.gx, mpu6050_raw.gy, mpu6050_raw.gz);
	printf("\r\ntemp:%d", mpu6050_raw.temp);

	prev_systick = HAL_GetTick();
	}
#endif
}


void mpu6050_accel_datas_get(void)
{
	uint8_t Rec_Data[6]={0,};

	// Read 6 BYTES of data starting from ACCEL_XOUT_H (0x3B) register
	//HAL_I2C_Mem_Read (&hi2c2, MPU6050_ADDR, 0x3B, 1, Rec_Data, 6, 1000);
	MPU6050_Readbytes(MPU6050_ACCEL_XOUT_H, 6, &Rec_Data[0]);

	mpu6050_raw.ax = (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]);
	mpu6050_raw.ay = (int16_t)(Rec_Data[2] << 8 | Rec_Data [3]);
	mpu6050_raw.az = (int16_t)(Rec_Data[4] << 8 | Rec_Data [5]);
}

void mpu6050_gyro_datas_get(void)
{
	uint8_t Rec_Data[6]={0,};

	// Read 6 BYTES of data starting from GYRO_XOUT_H register
	//HAL_I2C_Mem_Read (&hi2c2, MPU6050_ADDR, 0x43, 1, Rec_Data, 6, 1000);
	MPU6050_Readbytes(MPU6050_GYRO_XOUT_H, 6, &Rec_Data[0]);

	mpu6050_raw.gx = (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]);
	mpu6050_raw.gy = (int16_t)(Rec_Data[2] << 8 | Rec_Data [3]);
	mpu6050_raw.gz = (int16_t)(Rec_Data[4] << 8 | Rec_Data [5]);
}

void mpu6050_temp_data_get(void)
{
	uint8_t Rec_Data[2]={0,};
	//int16_t Temp_RAW=0;

	// Read 6 BYTES of data starting from GYRO_XOUT_H register
	//HAL_I2C_Mem_Read (&hi2c2, MPU6050_ADDR, 0x41, 1, Rec_Data, 2, 1000);
	MPU6050_Readbytes(MPU6050_TEMP_XOUT_H, 2, &Rec_Data[0]);

	mpu6050_raw.temp = (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]);
}

//////////////////////////////////////////////////////////////////
void MPU6050_Initialization(void)
{

	uint8_t who_am_i = 0;

	// MPU6050_ADO_Setup
	MPU6050_ADO_Init();

	HAL_Delay(50);
	printf("\r\nChecking MPU6050...");

	MPU6050_Readbyte(MPU6050_WHO_AM_I, &who_am_i);
	if(who_am_i == 0x68)
	{
		printf("\r\nMPU6050 who_am_i = 0x%02x...OK", who_am_i);
	}
	else
	{
		printf("\r\nERROR!");
		printf("\r\nMPU6050 who_am_i : 0x%02x should be 0x68", who_am_i);
		while(1)
		{
			printf("\r\nwho am i error. Can not recognize mpu6050");
			HAL_Delay(100);
		}
	}

	//Reset the whole module before initialization
	MPU6050_Writebyte(MPU6050_PWR_MGMT_1, 0x1<<7);
	HAL_Delay(100);

	//Power Management setting
	/* Default is sleep mode
	 * necessary to wake up MPU6050*/
	MPU6050_Writebyte(MPU6050_PWR_MGMT_1, 0x00);
	HAL_Delay(50);

	//Sample rate divider
	/*Sample Rate = Gyroscope Output Rate / (1 + SMPRT_DIV) */
	//	MPU6050_Writebyte(MPU6050_SMPRT_DIV, 0x00); // ACC output rate is 1kHz, GYRO output rate is 8kHz
	MPU6050_Writebyte(MPU6050_SMPRT_DIV, 39); // Sample Rate = 200Hz
	HAL_Delay(50);

	//FSYNC and DLPF setting
	/*DLPF is set to 0*/
	MPU6050_Writebyte(MPU6050_CONFIG, 0x00);
	HAL_Delay(50);

	//GYRO FULL SCALE setting
	/*FS_SEL  Full Scale Range
	  0    	+-250 degree/s
	  1		+-500 degree/s
	  2		+-1000 degree/s
	  3		+-2000 degree/s	*/
	uint8_t FS_SCALE_GYRO = 0x0;
	MPU6050_Writebyte(MPU6050_GYRO_CONFIG, FS_SCALE_GYRO<<3);
	HAL_Delay(50);

	//ACCEL FULL SCALE setting
	/*FS_SEL  Full Scale Range
	  0    	+-2g
	  1		+-4g
	  2		+-8g
	  3		+-16g	*/
	uint8_t FS_SCALE_ACC = 0x0;
	MPU6050_Writebyte(MPU6050_ACCEL_CONFIG, FS_SCALE_ACC<<3);
	HAL_Delay(50);

	MPU6050_Get_LSB_Sensitivity(FS_SCALE_GYRO, FS_SCALE_ACC);
	//printf("LSB_Sensitivity_GYRO: %f, LSB_Sensitivity_ACC: %f\n",LSB_Sensitivity_GYRO, LSB_Sensitivity_ACC);

	//Interrupt PIN setting
	uint8_t INT_LEVEL = 0x0; //0 - active high, 1 - active low
	uint8_t LATCH_INT_EN = 0x0; //0 - INT 50us pulse, 1 - interrupt clear required
	uint8_t INT_RD_CLEAR = 0x1; //0 - INT flag cleared by reading INT_STATUS, 1 - INT flag cleared by any read operation
	MPU6050_Writebyte(MPU6050_INT_PIN_CFG, (INT_LEVEL<<7)|(LATCH_INT_EN<<5)|(INT_RD_CLEAR<<4)); //
	HAL_Delay(50);

	//Interrupt enable setting
	uint8_t DATA_RDY_EN = 0x1; // 1 - enable, 0 - disable
	MPU6050_Writebyte(MPU6050_INT_ENABLE, DATA_RDY_EN);
	HAL_Delay(50);

	printf("MPU6050 setting is finished\n");
}


void MPU6050_Writebyte(uint8_t reg_addr, uint8_t val)
{
	HAL_I2C_Mem_Write(&hi2c2, MPU6050_ADDR, reg_addr, I2C_MEMADD_SIZE_8BIT, &val, 1, 1);
}

void MPU6050_Writebytes(uint8_t reg_addr, uint8_t len, uint8_t* data)
{
	HAL_I2C_Mem_Write(&hi2c2, MPU6050_ADDR, reg_addr, I2C_MEMADD_SIZE_8BIT, data, len, 1);
}

void MPU6050_Readbyte(uint8_t reg_addr, uint8_t* data)
{
	HAL_I2C_Mem_Read(&hi2c2, MPU6050_ADDR, reg_addr, I2C_MEMADD_SIZE_8BIT, data, 1, 1);
}

void MPU6050_Readbytes(uint8_t reg_addr, uint8_t len, uint8_t* data)
{
	HAL_I2C_Mem_Read(&hi2c2, MPU6050_ADDR, reg_addr, I2C_MEMADD_SIZE_8BIT, data, len, 1);
}

void MPU6050_ADO_Init(void)
{
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_RESET);
}



void MPU6050_Get_LSB_Sensitivity(uint8_t FS_SCALE_GYRO, uint8_t FS_SCALE_ACC)
{

	switch(FS_SCALE_GYRO)
	{
	case 0:
		LSB_Sensitivity_GYRO = 131.f;
		break;
	case 1:
		LSB_Sensitivity_GYRO = 65.5f;
		break;
	case 2:
		LSB_Sensitivity_GYRO = 32.8f;
		break;
	case 3:
		LSB_Sensitivity_GYRO = 16.4f;
		break;
	}
	switch(FS_SCALE_ACC)
	{
	case 0:
		LSB_Sensitivity_ACC = 16384.f;
		break;
	case 1:
		LSB_Sensitivity_ACC = 8192.f;
		break;
	case 2:
		LSB_Sensitivity_ACC = 4096.f;
		break;
	case 3:
		LSB_Sensitivity_ACC = 2048.f;
		break;
	}

}

int MPU6050_DataReady(void)
{
	//old school way
	/*
	static uint8_t INT_STATE_FLAG = 0;
	static uint8_t DATA_RDY_INT_FLAG = 0;
	static uint8_t INT_PIN = 0;
	INT_PIN = LL_GPIO_IsInputPinSet(
	MPU6050_INT_PORT, MPU6050_INT_PIN);
	if(INT_PIN == 1)
	{
		MPU6050_Readbyte(MPU6050_INT_STATUS, &INT_STATE_FLAG); //flag cleared automatically within the sensor
		DATA_RDY_INT_FLAG = INT_STATE_FLAG & 0x01;
		if(DATA_RDY_INT_FLAG == 1)
		{
			INT_STATE_FLAG = 0; //flag clearing
			DATA_RDY_INT_FLAG = 0;
			INT_PIN = 0;
			return 1;
		}
	}
	return 0;
	 */
	return HAL_GPIO_ReadPin(MPU6050_INT_PORT, MPU6050_INT_PIN);
}
