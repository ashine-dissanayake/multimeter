/*
 *
 *  switches.c
 *
 *  Created on: 16 Oct 2021
 *  Author: ENGG3800 - Group 8
 *
 */

#include "switches.h"
#include "stm32l4xx_hal.h"

/**
 * Close relay switch dependent to the relay id inputed.
 *
 * return: (none)
 */
void closeSwitch(int switchNo) {

    switch (switchNo) {
    case SWITCH_1:
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);
        break;
    case SWITCH_2:
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET);
        break;
    case SWITCH_3:
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
        break;
    case SWITCH_4:
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
        break;
    }
}

/**
 * Open relay switch dependent to the relay id inputed.
 *
 * return: (none)
 */
void openSwitch(int switchNo) {

    switch (switchNo) {
    case SWITCH_1:
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET);
        break;
    case SWITCH_2:
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET);
        break;
    case SWITCH_3:
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
        break;
    case SWITCH_4:
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
        break;
    }
}
