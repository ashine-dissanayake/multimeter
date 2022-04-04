/*
 *  lcd.c
 * 	LCD backlight Driver Source File.
 *  Created on: Sep 18, 2021
 *  Author: ENGG3800 - Group 8
 *
 */

#include "lcd.h"

/**
 * Initialise the PWM to start the PWM to control the backlight.
 *
 * return: (none)
 */
void pwmInit(TIM_HandleTypeDef *htim) {
    HAL_TIM_PWM_Start(htim, TIM_CHANNEL_1);
}

/**
 * Dependent of the brightness level, the pwm is changed accordingly.
 *
 * return: (none)
 */
void adjustBrightness(void) {
    switch (brightnessLevel) {
    case 1: // BLVL 1
        TIM2->CCR1 = 0;
        break;
    case 2: // BLVL 2
        TIM2->CCR1 = 10;
        break;
    case 3: // BLVL 3
        TIM2->CCR1 = 20;
        break;
    case 4: // BLVL 4
        TIM2->CCR1 = 30;
        break;
    case 5: // BLVL 5
        TIM2->CCR1 = 40;
        break;
    }
}
