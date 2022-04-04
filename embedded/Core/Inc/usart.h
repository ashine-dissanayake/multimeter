/*
 *  usart.h
 *
 *  Created on: Sep 11, 2021
 *  Author: ENGG3800 - Group 8
 *
 */

#ifndef INC_USART_H_
#define INC_USART_H_

#include "ads1115_driver.h"
#include "flags.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_it.h"
#include <stdlib.h>

/** Define */
/** Preamble of packet */
#define PREAMBLE 0xAA
/** Join type */
#define JOIN 0x10
/** Mode id */
#define MODE 0x12
/** Hold id */
#define HOLD 0x13
/** Brightness lvl id */
#define BLVL 0x14
/** Join preamble */
#define JOIN_PREAMBLE 0xBB

/** External variables. */
/* When the preamble is received, isPacket is 1. */
extern int isPacket;
/** Received packet index. */
extern int packetIndex;
/** When the entire packet is received, packetReady is 1. */
extern int packetReady;
/** 1 when joined; otherwise 0. */
extern int isJoin;
/** Previous join time. */
extern int joinTime;
/** Store receive bytes. */
extern uint8_t packetRX[26];
/** Stores message received. */
extern uint8_t message[17];

/* Structs */
struct __attribute__((__packed__)) MultimeterPacket {
    uint8_t preamble;
    uint8_t type;
    uint8_t payload[31];
};

/** Functions */
void usartInit(UART_HandleTypeDef *huart);
void connectGui(UART_HandleTypeDef *huart);
void sendPacket(UART_HandleTypeDef *huart, int type);

#endif /* INC_USART_H_ */
