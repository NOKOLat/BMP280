/*
 * BMP280.h
 *
 *  Created on: Feb 15, 2023
 *      Author: OHYA Satoshi
 */

#ifndef BMP280_BMP280_H_
#define BMP280_BMP280_H_

#include <stdint.h>
#include <array>

class BMP280 {
public:
	enum class Address: uint8_t{
		LOW  = 0b1110110<<1,
		HIGH = 0b1110111<<1
	};

	enum class Register:uint8_t{
		chip_id 	= 0xD0,
		reset		= 0xE0,
		status		= 0xF3,
		ctrl_meas 	= 0xF4,
		config		= 0xF5,
		press_msb	= 0xF7,
		press_lsb	= 0xF8,
		press_xlsb	= 0xF9,
		temp_msb	= 0xFA,
		temp_lsb	= 0xFB,
		temp_xlsb	= 0xFC,
		calibration	= 0x88,
	};

	BMP280(Address address)
		:address(address)
		{};

	bool isMatchChipID();
	void reset();
	uint8_t getStatus();

	enum class Mode{
		sleep = 0,
		forced = 1,
		normal = 0b11,
	};
	void setMode(Mode mode);

	enum class Oversampling{
		skip = 0,
		_1,
		_2,
		_4,
		_8,
		_16,

	};
	void setOversamplingPress(Oversampling oversampling);
	void setOversamplingTemp(Oversampling oversampling);

	enum class StdbyTime{
		MS_5,
		MS62_5,
		MS125,
		MS250,
		MS500,
		MS1000,
		MS2000,
		MS4000,
	};
	void setConfig_standbyTime(StdbyTime tsb);
	enum class FilterConfig{
		off,
		_2,
		_4,
		_8,
		_16,
	};
	void setConfig_filter(FilterConfig filterConfig);
	void setConfig_setIsEnableSPI(bool isEnableSPI=false);

	void updateCalibration();

	double getPressure();
	double getTemperature(){
		return temperature;
	}
	uint32_t getRawPressure(){
		return rawPressure;
	}
	uint32_t getRawTemperature(){
		return rawTemperature;
	}

	void updatePressure();
	void updatePressure_callback(){}

protected:
	Address address;

private:

	struct Calibration{
		uint16_t dig_T1=0;
		int16_t dig_T2=0;
		int16_t dig_T3=0;
		uint16_t dig_P1=0;
		int16_t dig_P2=0;
		int16_t dig_P3=0;
		int16_t dig_P4=0;
		int16_t dig_P5=0;
		int16_t dig_P6=0;
		int16_t dig_P7=0;
		int16_t dig_P8=0;
		int16_t dig_P9=0;
	}calibration;

	int32_t temperatureFine = 0;
	double temperature = 0;
	double pressure = 0;
	uint32_t rawPressure;
	uint32_t rawTemperature;
	void compensateTemperature(int32_t rawTemperature);
	void compensatePressure(int32_t rawPressure);

	virtual void memWrite(uint8_t memAddress, uint8_t *pData){}
	virtual void memRead(uint8_t memAddress, uint8_t *pData){}
	virtual void memWrite(uint8_t memAddress, uint8_t *pData, uint8_t length){}
	virtual void memRead(uint8_t memAddress, uint8_t *pData, uint8_t length){}
};

#endif /* BMP280_BMP280_H_ */
