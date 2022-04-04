/*
 *  usart.c
 *
 *  Created on: Sep 11, 2021
 *  Author: ENGG3800 - Group 8
 *
 */

#include "usart.h"
#include "measure.h"
#include <string.h>

/** External variable */
int isPacket = 0;
int packetIndex = 0;
int packetReady = 0;
int isJoin = 0;
int joinTime = 0;
uint8_t packetRX[26] = {0}; // Store the received packet.
uint8_t message[17] = {0}; // Store message sent.
static uint8_t UART2_rxBufferUart[1] = {0};

/**
 * Initialise usart.
 *
 * huart: usart handler.
 *
 * return: (none)
 */
void usartInit(UART_HandleTypeDef *huart) {
    HAL_UART_Receive_IT(huart, UART2_rxBufferUart, 1);
}

/**
 * Usart callback function.
 *
 * huart: usart handler.
 *
 * return: (none)
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (UART2_rxBufferUart[0] == 0xBB) {
        isJoin = 1;
    }
    if (UART2_rxBufferUart[0] == 0xAA) { // Check preamble

        isPacket = 1;
    } else if (isPacket == 1) { // Check if packet flag is received.

        if (UART2_rxBufferUart[0] != 194) { // Check for the random variable.

            packetRX[packetIndex++] = UART2_rxBufferUart[0];
        }
    }

    // Check if end of packet it received.
    if (packetIndex == 25) { // When the entire packet recevied.

        packetIndex = 0;
        isPacket = 0;
        packetReady = 1;
    }
    HAL_UART_Receive_IT(huart, UART2_rxBufferUart, 1); // trigger = 1 byte.
}

/**
 * Connect to the GUI by sending the Join preamble to GUI.
 *
 * huart: usart handler.
 *
 * return: (none)
 */
void connectGui(UART_HandleTypeDef *huart) {
    if (HAL_GetTick() - joinTime >= 1000) {
        joinTime = HAL_GetTick();
        uint8_t joinBuffer[1] = {0XBB};
        isJoin = 0; // reset isJoin flag.
        HAL_UART_Transmit(huart, joinBuffer, sizeof(joinBuffer), HAL_MAX_DELAY);
    }
}

/**
 * Send the packet to the GUI.
 * 	1 - MODE. Sends all relevant MODE data to the GUI.
 * 	2 - HOLD. Send the hold status of the GUI.
 * 	3 - BLVL. Brightness level.
 *
 * 	return: (none)
 */
void sendPacket(UART_HandleTypeDef *huart, int type) {
    struct MultimeterPacket multimeterPacket = {0};
    char txBuffer[32] = {0};
    int temp = 0;

    switch (type) {
    case 0: // MODE
        multimeterPacket.preamble = PREAMBLE;
        multimeterPacket.type = MODE;

        if (currentMode == DC || currentMode == AC) {
            sprintf((char *)multimeterPacket.payload,
                    "MODE%d%c%06.2f%06.2f%06.2f%06.2f", currentMode, '0',
                    adcCurr, adcMin, adcMax, dcOffset);
        } else if (currentMode == RESISTANCE) {
            sprintf((char *)multimeterPacket.payload,
                    "MODE%d%c%s", currentMode, resistancePacket.unit,
                    resistancePacket.value);
        } else if (currentMode == CONTINUITY) {
            sprintf((char *)multimeterPacket.payload, "MODE%d%c%d%06.2f",
                    currentMode, '0', currentCTStatus, currentCT);
        }
        // Copy the struct into the array to send to GUI.
        memcpy(txBuffer, &multimeterPacket, sizeof(multimeterPacket));
        HAL_UART_Transmit(huart, (uint8_t *)txBuffer, sizeof(txBuffer),
                          HAL_MAX_DELAY);
        break;
    case 1:
        multimeterPacket.preamble = PREAMBLE;
        multimeterPacket.type = HOLD;
        // Determine if the hold status is 1 or 0 depending on if its True or False.
        if (holdStatus) {
            temp = 1;
        } else {
            temp = 0;
        }
        sprintf((char *)multimeterPacket.payload, "HOLD%d\r", temp);
        memcpy(txBuffer, &multimeterPacket, sizeof(multimeterPacket));
        HAL_UART_Transmit(huart, (uint8_t *)txBuffer, sizeof(txBuffer),
                          HAL_MAX_DELAY);
        break;
    case 2:
        multimeterPacket.preamble = PREAMBLE;
        multimeterPacket.type = BLVL;
        sprintf((char *)multimeterPacket.payload, "BLVL%d\r", brightnessLevel);
        memcpy(txBuffer, &multimeterPacket, sizeof(multimeterPacket));
        HAL_UART_Transmit(huart, (uint8_t *)txBuffer, sizeof(txBuffer),
                          HAL_MAX_DELAY);
        break;
    }
}
