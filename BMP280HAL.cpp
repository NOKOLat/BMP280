/*
 * BMP280HAL.cpp
 *
 *  Created on: Feb 15, 2023
 *      Author: OHYA Satoshi
 */

#include "BMP280HAL.h"

void BMP280_HAL::memWrite(uint8_t memAddress, uint8_t *pData){
	HAL_I2C_Mem_Write(hi2c, (uint16_t)address, memAddress, 1, pData, 1, 100);
}

void BMP280_HAL::memRead(uint8_t memAddress, uint8_t *pData){
	HAL_I2C_Mem_Read(hi2c, (uint16_t)address, memAddress, 1, pData, 1, 100);
}

void BMP280_HAL::memWrite(uint8_t memAddress, uint8_t *pData, uint8_t length){
	HAL_I2C_Mem_Write(hi2c, (uint16_t)address, memAddress, 1, pData, length, 100);
}

void BMP280_HAL::memRead(uint8_t memAddress, uint8_t *pData, uint8_t length){
	HAL_I2C_Mem_Read(hi2c, (uint16_t)address, memAddress, 1, pData, length, 100);
}
