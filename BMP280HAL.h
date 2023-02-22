/*
 * BMP280HAL.h
 *
 *  Created on: Feb 15, 2023
 *      Author: OHYA Satoshi
 */

#ifndef BMP280_BMP280HAL_H_
#define BMP280_BMP280HAL_H_

#include "BMP280.h"
#include "i2c.h"
#include <array>

class BMP280_HAL : public BMP280{
public:
	BMP280_HAL(I2C_HandleTypeDef *hi2c,Address address)
	:BMP280(address),hi2c(hi2c){}
private:
	 I2C_HandleTypeDef *hi2c;


	void memWrite(uint8_t memAddress, uint8_t *pData);
	void memRead(uint8_t memAddress, uint8_t *pData);
	void memWrite(uint8_t memAddress, uint8_t *pData, uint8_t length);
	void memRead(uint8_t memAddress, uint8_t *pData, uint8_t length);
};

#endif /* BMP280_BMP280HAL_H_ */
