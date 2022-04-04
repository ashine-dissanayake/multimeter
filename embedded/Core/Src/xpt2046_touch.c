/* vim: set ai et ts=4 sw=4: */

#include "xpt2046_touch.h"
#include "stm32l4xx_hal.h"

#define READ_X 0xD0
#define READ_Y 0x90

void XPT2046_init() {
    HAL_GPIO_WritePin(XPT2046_TOUCH_CS_GPIO_Port, XPT2046_TOUCH_CS_Pin,
                      GPIO_PIN_SET);
}

void XPT2046_TouchSelect() {
    HAL_GPIO_WritePin(XPT2046_TOUCH_CS_GPIO_Port, XPT2046_TOUCH_CS_Pin,
                      GPIO_PIN_RESET);
}

void XPT2046_TouchUnselect() {
    HAL_GPIO_WritePin(XPT2046_TOUCH_CS_GPIO_Port, XPT2046_TOUCH_CS_Pin,
                      GPIO_PIN_SET);
}

bool XPT2046_TouchPressed() {
    return HAL_GPIO_ReadPin(XPT2046_TOUCH_IRQ_GPIO_Port,
                            XPT2046_TOUCH_IRQ_Pin) == GPIO_PIN_RESET;
}

bool XPT2046_TouchGetCoordinates(uint16_t *x_coord, uint16_t *y_coord) {
    static const uint8_t cmd_read_x[] = {READ_X};
    static const uint8_t cmd_read_y[] = {READ_Y};
    static const uint8_t zeroes_tx[] = {0x00, 0x00};

    XPT2046_TouchSelect();

    uint32_t avg_x = 0;
    uint32_t avg_y = 0;
    uint8_t nsamples = 0;
    for (uint8_t i = 0; i < 16; i++) {
        //        if(!XPT2046_TouchPressed())
        //            break;

        nsamples++;

        HAL_SPI_Transmit(&XPT2046_TOUCH_SPI_PORT, (uint8_t *)cmd_read_y,
                         sizeof(cmd_read_y), HAL_MAX_DELAY);
        while (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY)
            ;
        uint8_t y_raw[2];
        HAL_SPI_TransmitReceive(&XPT2046_TOUCH_SPI_PORT, (uint8_t *)zeroes_tx,
                                y_raw, sizeof(y_raw), HAL_MAX_DELAY);
        while (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY)
            ;

        HAL_SPI_Transmit(&XPT2046_TOUCH_SPI_PORT, (uint8_t *)cmd_read_x,
                         sizeof(cmd_read_x), HAL_MAX_DELAY);
        while (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY)
            ;
        uint8_t x_raw[2];
        HAL_SPI_TransmitReceive(&XPT2046_TOUCH_SPI_PORT, (uint8_t *)zeroes_tx,
                                x_raw, sizeof(x_raw), HAL_MAX_DELAY);
        while (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY)
            ;

        avg_x += (((uint16_t)x_raw[0]) << 8) | ((uint16_t)x_raw[1]);
        avg_y += (((uint16_t)y_raw[0]) << 8) | ((uint16_t)y_raw[1]);
    }

    XPT2046_TouchUnselect();

    if (nsamples < 16)
        return false;

    uint32_t raw_x = (avg_x / 16);
    if (raw_x < XPT2046_TOUCH_MIN_RAW_X)
        raw_x = XPT2046_TOUCH_MIN_RAW_X;
    if (raw_x > XPT2046_TOUCH_MAX_RAW_X)
        raw_x = XPT2046_TOUCH_MAX_RAW_X;

    uint32_t raw_y = (avg_y / 16);
    if (raw_y < XPT2046_TOUCH_MIN_RAW_X)
        raw_y = XPT2046_TOUCH_MIN_RAW_Y;
    if (raw_y > XPT2046_TOUCH_MAX_RAW_Y)
        raw_y = XPT2046_TOUCH_MAX_RAW_Y;

    // Uncomment this line to calibrate touchscreen:
    // UART_Printf("raw_x = %d, raw_y = %d\r\n", x_coord, y_coord);

    *x_coord = (raw_x - XPT2046_TOUCH_MIN_RAW_X) * XPT2046_TOUCH_SCALE_X /
               (XPT2046_TOUCH_MAX_RAW_X - XPT2046_TOUCH_MIN_RAW_X);
    *y_coord = (raw_y - XPT2046_TOUCH_MIN_RAW_Y) * XPT2046_TOUCH_SCALE_Y /
               (XPT2046_TOUCH_MAX_RAW_Y - XPT2046_TOUCH_MIN_RAW_Y);

    return true;
}