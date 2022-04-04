/*
 *  ads1115_driver.h
 * 	ADC1115 Driver Header File.
 *  Created on: Aug 30, 2021
 *  Author: ENGG3800 - Group 8
 *
 */

#ifndef INC_ADS1115_DRIVER_H_
#define INC_ADS1115_DRIVER_H_

#include "stm32l4xx_hal.h"
#include <math.h>
#include <stdio.h>

/** Default ADC address */
#define ADS1115_ADDR 0x48 // 1001000
/** Conversion value */
#define VOLTAGE_CONV (float)(6.144 / 32768.0);

/** Each mode has a unique measurement key */
#define DC_VOLTAGE_CIRCUIT 0
#define AC_VOLTAGE_CIRCUIT 1
#define RESISTANCE_CIRCUIT 2
#define CONTINUITY_CIRCUIT 3

/** NOTE THE I2C Handler in MAIN.C */

/** External Variables */
/** ADC Minimum */
extern float adcMin;
/** ADC Maximum */
extern float adcMax;
/** ADC Current Reading */
extern float adcCurr;
/** Use when the reset status is true, to set
 * the minimum and maximum
 */
extern int firstADCFlag;
/** Current continuity threshold */
extern float currentCT;
/** DC Offset */
extern float dcOffset;
/** */
extern int currentCTStatus;

extern float resistance;	// the last measured resistance. display this	// ESSENTIAL


/** External Functions */
extern float adc1115VoltageValue(int16_t rawValue);
extern int16_t ads1115ReadRawADC(I2C_HandleTypeDef *hi2c, int mode);

#endif /* INC_ADS1115_DRIVER_H_ */
