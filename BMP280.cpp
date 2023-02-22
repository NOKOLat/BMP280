/*
 * BMP280.cpp
 *
 *  Created on: Feb 15, 2023
 *      Author: OHYA Satoshi
 */

#include "BMP280.h"

bool BMP280::isMatchChipID(){
	uint8_t value=0;
	memRead((uint8_t)Register::chip_id, &value);
	if(value == 0x58){
		return true;
	}
	return false;
}

void BMP280::reset(){
	uint8_t value = 0xB6;
	memWrite((uint8_t)Register::reset, &value);
}

uint8_t BMP280::getStatus(){
	uint8_t value=0;
	memRead((uint8_t)Register::status, &value);
	return value;
}

void BMP280::setMode(Mode mode){
	uint8_t value=0;
	memRead((uint8_t)Register::ctrl_meas, &value);
	value = (value &(0xff<<2))|(uint8_t)mode;
	memWrite((uint8_t)Register::ctrl_meas, &value);
}

void BMP280::setOversamplingPress(Oversampling oversampling){
	uint8_t value;
	memRead((uint8_t)Register::ctrl_meas, &value);
	value = (value &(0b11100011))|((uint8_t)oversampling<<2);
	memWrite((uint8_t)Register::ctrl_meas, &value);

}
void BMP280::setOversamplingTemp(Oversampling oversampling){
	uint8_t value;
	memRead((uint8_t)Register::ctrl_meas, &value);
	value = (value &(0b00011111))|((uint8_t)oversampling<<5);
	memWrite((uint8_t)Register::ctrl_meas, &value);
}

void BMP280::setConfig_standbyTime(StdbyTime tsb){
	uint8_t value;
	memRead((uint8_t)Register::config, &value);
	value = (value &(0b00011111))|((uint8_t)tsb<<5);
	memWrite((uint8_t)Register::config, &value);
}

void BMP280::setConfig_filter(FilterConfig filterConfig){
	uint8_t value;
	memRead((uint8_t)Register::config, &value);
	value = (value &(0b11100011))|((uint8_t)filterConfig<<2);
	memWrite((uint8_t)Register::config, &value);
}


void BMP280::setConfig_setIsEnableSPI(bool isEnableSPI){

}

void BMP280::updateCalibration(){
	uint8_t buffer[24]={};
	memRead((uint8_t)Register::calibration, buffer, 24);

	calibration.dig_T1 = ((uint16_t)buffer[1]<<8)|buffer[0];
	calibration.dig_T2 = ((int16_t)buffer[3]<<8)|buffer[2];
	calibration.dig_T3 = ((int16_t)buffer[5]<<8)|buffer[4];
	calibration.dig_P1 = ((uint16_t)buffer[7]<<8)|buffer[6];
	calibration.dig_P2 = ((int16_t)buffer[9]<<8)|buffer[8];
	calibration.dig_P3 = ((int16_t)buffer[11]<<8)|buffer[10];
	calibration.dig_P4 = ((int16_t)buffer[13]<<8)|buffer[12];
	calibration.dig_P5 = ((int16_t)buffer[15]<<8)|buffer[14];
	calibration.dig_P6 = ((int16_t)buffer[17]<<8)|buffer[16];
	calibration.dig_P7 = ((int16_t)buffer[19]<<8)|buffer[18];
	calibration.dig_P8 = ((int16_t)buffer[21]<<8)|buffer[20];
	calibration.dig_P9 = ((int16_t)buffer[23]<<8)|buffer[22];
}

void BMP280::updatePressure(){
	uint8_t buffer[6]={};
	memRead((uint8_t)Register::press_msb, buffer, 6);

	rawPressure = ((uint32_t)buffer[0]<<12)|((uint32_t)buffer[1]<<4)|((uint32_t)buffer[2]>>4);
	rawTemperature = ((uint32_t)buffer[3]<<12)|((uint32_t)buffer[4]<<4)|((uint32_t)buffer[5]>>4);

	compensateTemperature(rawTemperature);
	compensatePressure(rawPressure);
}

double BMP280::getPressure(){
	return pressure;
}

/*
 * Code under this comment are copied from datasheet BMP280 and edited.
 *
 */
// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// temperatureFine carries fine temperature as global value
void BMP280::compensateTemperature(int32_t rawTemperature){
	double var1, var2;
	var1 = (((double)rawTemperature)/16384.0 - ((double)calibration.dig_T1)/1024.0) * ((double)calibration.dig_T2);
	var2 = ((((double)rawTemperature)/131072.0 - ((double)calibration.dig_T1)/8192.0) *
	(((double)rawTemperature)/131072.0 - ((double)calibration.dig_T1)/8192.0)) * ((double)calibration.dig_T3);
	temperatureFine = (int32_t)(var1 + var2);
	temperature = (var1 + var2) / 5120.0;
}
// Returns pressure in Pa as unsigned 32 bit integer. Output value of “96386” equals 96386 Pa = 963.86 hPa
void BMP280::compensatePressure(int32_t rawPressure){
	double var1, var2, p;
	var1 = ((double)temperatureFine/2.0) - 64000.0;
	var2 = var1 * var1 * ((double)calibration.dig_P6) / 32768.0;
	var2 = var2 + var1 * ((double)calibration.dig_P5) * 2.0;
	var2 = (var2/4.0)+(((double)calibration.dig_P4) * 65536.0);
	var1 = (((double)calibration.dig_P3) * var1 * var1 / 524288.0 + ((double)calibration.dig_P2) * var1) / 524288.0;
	var1 = (1.0 + var1 / 32768.0)*((double)calibration.dig_P1);
	p = 1048576.0 - (double)rawPressure;
	p = (p - (var2 / 4096.0)) * 6250.0 / var1;
	var1 = ((double)calibration.dig_P9) * p * p / 2147483648.0;
	var2 = p * ((double)calibration.dig_P8) / 32768.0;
	pressure = p + (var1 + var2 + ((double)calibration.dig_P7)) / 16.0;
}
