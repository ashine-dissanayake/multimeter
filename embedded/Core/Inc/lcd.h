/*
 *  lcd.h
 * 	LCD backlight Source File. Contains pwm driver to control
 * 	the backlight.
 *  Created on: Sep 4, 2021
 *  Author: ENGG3800 - Group 8
 *
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_

#include "flags.h"
#include "stm32l4xx_hal.h"

/** Functions */
void adjustBrightness(void);
void pwmInit(TIM_HandleTypeDef *htim);

#endif /* INC_LCD_H_ */
