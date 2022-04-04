/*
 *  stm32_DAC.h
 * 	DAC Header File.
 *  Created on: Aug 31, 2021
 *  Author: ashdi
 *
 */

#ifndef INC_STM32_DAC_H_
#define INC_STM32_DAC_H_

#include "stm32l4xx_hal.h"

/** Define */
/** Reference voltage. */
#define V_REF (float)(3.3)
/** DAC resolution. */
#define DAC_RES (int)(4096)

/** External function */
extern void stm32DACInit(DAC_HandleTypeDef *hdac, uint32_t channel);
extern float stm32DACToVoltage(int DAC_value);
extern uint32_t stm32VoltageToDAC(float voltage);
extern void setDACValue(DAC_HandleTypeDef *hdac, uint32_t channel,
                          uint32_t value);
extern void stm32DACClear(DAC_HandleTypeDef *hdac, uint32_t channel);

#endif /* INC_STM32_DAC_H_ */
