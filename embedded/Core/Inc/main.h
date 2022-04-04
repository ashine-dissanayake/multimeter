/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32_DAC.h"
#include "usart.h"
#include "ads1115_driver.h"
#include "colors.h"
#include "display.h"
#include "ee.h"
#include "eeConfig.h"
#include "flags.h"
#include "ili9163.h"
#include "lcd.h"
#include "measure.h"
#include "switches.h"
#include "xpt2046_touch.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define RELAY_1_PIN GPIO_PIN_10
#define RELAY_1_GPIO_PORT GPIOB
#define RELAY_2_PIN GPIO_PIN_11
#define RELAY_2_GPIO_PORT GPIOB
#define LED_PIN GPIO_PIN_12
#define LED_GPIO_PORT GPIOB
#define LCD_RST_PIN GPIO_PIN_13
#define LCD_RST_GPIO_PORT GPIOB
#define LCD_CS_PIN GPIO_PIN_8
#define LCD_CS_GPIO_PORT GPIOA
#define TOUCH_CS_PIN GPIO_PIN_12
#define TOUCH_CS_GPIO_PORT GPIOA
#define LCD_DATA_CS_PIN GPIO_PIN_5
#define LCD_DATA_CS_GPIO_PORT GPIOB
#define RELAY_3_PIN GPIO_PIN_6
#define RELAY_3_GPIO_PORT GPIOB
#define RELAY_4_PIN GPIO_PIN_7
#define RELAY_4_GPIO_PORT GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
