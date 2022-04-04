/*
 *  stm32_DAC.c
 * 	DAC Header File.
 *  Created on: Aug 31, 2021
 *  Author: ENGG3800 - Group 8
 *
 */

#include "stm32_DAC.h"

/**
 * Initialise DAC.
 *
 * return: (none)
 */
extern void stm32DACInit(DAC_HandleTypeDef *hdac, uint32_t channel) {
    HAL_DAC_Start(hdac, channel);
}

/**
 * Converting DAC value to voltage values.
 *
 * return: (none)
 */
extern float stm32DACToVoltage(int dacValue) {
    return dacValue * (V_REF / DAC_RES);
}

/**
 * Converting voltage value to DAC values.
 *
 * return: (none)
 */
extern uint32_t stm32VoltageToDAC(float voltage) {
    return (voltage * DAC_RES) * (1 / V_REF);
}

/**
 * Set DAC value.
 *
 * return: (none)
 */
extern void setDACValue(DAC_HandleTypeDef *hdac, uint32_t channel,
                          uint32_t value) {
    HAL_DAC_SetValue(hdac, channel, DAC_ALIGN_12B_R, value);
}

/**
 * Stop DAC.
 */
extern void stm32DACClear(DAC_HandleTypeDef *hdac, uint32_t channel) {
    HAL_DAC_Stop(hdac, channel);
}
