/*
 * MPU6050.h
 *
 *  Created on: Jun 3, 2021
 *      Author: jwan
 */
#include "MPU6050.h"
MPU6050::MPU6050(){
	 GoffsetX = 0 ;
	 GoffsetY = 0 ;
	 GoffsetZ = 0 ;

}

bool MPU6050::init(){
	uint8_t id = 0;
	uint8_t Data = 0;
	HAL_StatusTypeDef test = HAL_I2C_Mem_Read(i2c, MPU_ADDR, MPU6050_RA_WHO_AM_I, 1, &id, 1, 1000);
	if(test == HAL_OK){
		Data = 0x0B;
		test = HAL_I2C_Mem_Write(i2c, MPU_ADDR, MPU6050_RA_PWR_MGMT_1  , 1, &Data, 1, 1000);
		Data = 0x00;
		test = HAL_I2C_Mem_Write(i2c, MPU_ADDR, MPU6050_RA_SMPLRT_DIV  , 1, &Data, 1, 1000);
		Data = 0x00;
		test = HAL_I2C_Mem_Write(i2c, MPU_ADDR, MPU6050_RA_CONFIG      , 1, &Data, 1, 1000);
		Data = 0x08;
		test = HAL_I2C_Mem_Write(i2c, MPU_ADDR, MPU6050_RA_GYRO_CONFIG , 1, &Data, 1, 1000);
		Data = 0x00;
		test = HAL_I2C_Mem_Write(i2c, MPU_ADDR, MPU6050_RA_ACCEL_CONFIG, 1, &Data, 1, 1000);

		set_GoffsetZ();
		return true ;
	}else{
		return false ;
	}
}

void MPU6050::reset(){
	uint8_t Data = 0x80;
	HAL_I2C_Mem_Write(i2c, MPU_ADDR, MPU6050_RA_PWR_MGMT_1  , 1, &Data, 1, 1000);
	GoffsetX = 0 ;
	GoffsetY = 0 ;
	GoffsetZ = 0 ;

}

bool MPU6050::MPu_sleep(bool flag){
	uint8_t Data = 0x40;
	if(!flag){
		Data = 0x00;
	}
	HAL_I2C_Mem_Write(i2c, MPU_ADDR, MPU6050_RA_PWR_MGMT_1  , 1, &Data, 1, 1000);
	return true ;
}

void MPU6050::set_i2c(I2C_HandleTypeDef* i2c){
	this->i2c = i2c ;
}


void MPU6050::set_GoffsetZ(){
	float sum = 0 ;
	float GZ;
	for(int i = 0 ; i < LOOPS ; i++){
		MPU_getGyroZ( &GZ);
		sum += GZ;
		HAL_Delay(1);
	}
	GoffsetZ = (float)sum/LOOPS;
}
bool MPU6050::MPU_getGyroX(float* gyroX){
	uint8_t temp[2];
	HAL_StatusTypeDef test = HAL_I2C_Mem_Read(i2c, MPU_ADDR, MPU6050_RA_GYRO_XOUT_H , 1, temp, 2, 1000);
	if(test == HAL_OK){
		int16_t temp2 = (int16_t)(temp[0] << 8 | temp[1]);;
		*gyroX = temp2/gyro_lsb_to_degsec - GoffsetX;
		return true ;
	}
	return false ;
}
bool MPU6050::MPU_getGyroY(float* gyroY){
	uint8_t temp[2];
	HAL_StatusTypeDef test = HAL_I2C_Mem_Read(i2c, MPU_ADDR, MPU6050_RA_GYRO_YOUT_H , 1, temp, 2, 1000);
	if(test == HAL_OK){
		int16_t temp2 = (int16_t)(temp[0] << 8 | temp[1]);;
		*gyroY = temp2/gyro_lsb_to_degsec - GoffsetY;
		return true ;
	}
	return false ;
}
bool MPU6050::MPU_getGyroZ(float* gyroZ){
	uint8_t temp[2];
	HAL_StatusTypeDef test = HAL_I2C_Mem_Read(i2c, MPU_ADDR, MPU6050_RA_GYRO_ZOUT_H , 1, temp, 2, 1000);
	if(test == HAL_OK){
		int16_t temp2 = (int16_t)(temp[0] << 8 | temp[1]);;
		*gyroZ = temp2/gyro_lsb_to_degsec - GoffsetZ;
		return true ;
	}
	return false ;
}

