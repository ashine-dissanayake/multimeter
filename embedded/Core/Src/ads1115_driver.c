/*
 *  ads1115_driver.c
 * 	ADC1115 Driver Source File.
 *  Created on: Aug 30, 2021
 *  Author: ENGG3800 - Group 8
 *
 */

#include "ads1115_driver.h"

/** Initialise all external variables. */
float adcMin = 0.0;
float adcMax = 0.0;
float adcCurr = 0.0;
float currentCT = 0.0;
float dcOffset = 0.0;
int currentCTStatus = 0;
int firstADCFlag = 1;
float resistance = 0.0;

/**
 * ADC reads raw ADC values depending on the inputed mode.
 *
 * hi2c: i2c handler.
 * mode: dependent on the current mode.
 *
 * return: raw adc value.
 */
extern int16_t ads1115ReadRawADC(I2C_HandleTypeDef *hi2c, int mode) {
    uint8_t ads1115Config[3] = {0x00, 0x00, 0X00}; // init to zero

    switch (mode) {

    case DC_VOLTAGE_CIRCUIT:
    case AC_VOLTAGE_CIRCUIT:
        ads1115Config[0] = 0x01;
        ads1115Config[1] = 0xC1; // 11000000 AINP = AIN0 and AINN = GND
        ads1115Config[2] = 0xE3; // 11100011 // 11100101
        break;
    case RESISTANCE_CIRCUIT:
    case CONTINUITY_CIRCUIT:
        ads1115Config[0] = 0x01;
        ads1115Config[1] = 0xE1; // 11100001 AINP = AIN2 and AINN = GND
        ads1115Config[2] = 0xE3; // 11100011
        break;
    }

    HAL_I2C_Master_Transmit(hi2c, ADS1115_ADDR << 1, ads1115Config, 3, 100);

    ads1115Config[0] = 0x00;
    HAL_I2C_Master_Transmit(hi2c, ADS1115_ADDR << 1, 0X00, 1, 100);
    HAL_I2C_Master_Receive(hi2c, ADS1115_ADDR << 1, ads1115Config, 2, 100);

    // full 16 bit value.
    return (ads1115Config[0] << 8 | ads1115Config[1]);
}

/**
 * Convert ADC value to a voltage value.
 *
 * rawValue: raw adc value.
 *
 * return: voltage equivalent of raw value.
 */
extern float adc1115VoltageValue(int16_t rawValue) {
	// setting all negative values to zero.
    if (rawValue < 0) {
        rawValue = 0;
    }
    return rawValue * VOLTAGE_CONV; // raw_value * voltageConv;
}
