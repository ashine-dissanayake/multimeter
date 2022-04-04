/* vim: set ai et ts=4 sw=4: */
#ifndef __XPT2046_TOUCH_H__
#define __XPT2046_TOUCH_H__

#include <stdbool.h>
#include <stdio.h>
#include "stm32l4xx_hal.h"

/*** Redefine if necessary ***/

// Warning! Use SPI bus with < 1.3 Mbit speed, better ~650 Kbit to be save.
#define XPT2046_TOUCH_SPI_PORT hspi1
extern SPI_HandleTypeDef XPT2046_TOUCH_SPI_PORT;

#define XPT2046_TOUCH_IRQ_Pin GPIO_PIN_0 // Arduino D5
#define XPT2046_TOUCH_IRQ_GPIO_Port GPIOB
#define XPT2046_TOUCH_CS_Pin GPIO_PIN_12 // Arduino D2
#define XPT2046_TOUCH_CS_GPIO_Port GPIOA

// change depending on screen orientation
#define XPT2046_TOUCH_SCALE_X 128
#define XPT2046_TOUCH_SCALE_Y 160

// to calibrate uncomment UART_Printf line in XPT2046_touch.c
#define XPT2046_TOUCH_MIN_RAW_X 3400
#define XPT2046_TOUCH_MAX_RAW_X 29000
#define XPT2046_TOUCH_MIN_RAW_Y 3300
#define XPT2046_TOUCH_MAX_RAW_Y 30000

// call before initializing any SPI devices
void XPT2046_TouchUnselect();
void XPT2046_init();
bool XPT2046_TouchPressed();
bool XPT2046_TouchGetCoordinates(uint16_t *x_coord, uint16_t *y_coord);

#endif // __ILI9341_TOUCH_H__
