/*
 *  measure.h
 *
 *  Created on: Oct 4, 2021
 *  Author: ENGG3800 - Group 8
 *
 */

#ifndef INC_MEASURE_H_
#define INC_MEASURE_H_

#include "stm32_DAC.h"
#include "stm32l4xx_hal.h"
#include "switches.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define RELAY_STATUS_PROTECTED 0
#define RELAY_STATUS_HIGH_R 1
#define RELAY_STATUS_MID_R 2
#define RELAY_STATUS_LOW_R 3

#define MAX_BUFFERSIZE 1000

struct __attribute__((__packed__)) ResistancePacket {
    char value[10];
    char unit;
};

extern struct ResistancePacket resistancePacket;

extern int bufferSize; // size of voltage and current buffers
extern float rZero;

float calcIDC(float *buf);
void initRMeas(
    DAC_HandleTypeDef *hdac); // call this when entering resistance or cont mode
float calcR(DAC_HandleTypeDef *hdac, float *vBuf,
             float *ibuf);                   // call this once every loop
void deinitRMeas(DAC_HandleTypeDef *hdac); // call this when switching away
                                             // from res or cont modes
void setRelayMode(unsigned int mode);
float calcVDC(float *buf);
float calcVRMS(float *buf);
float calcVMax(float *buf);
float calcVMin(float *buf);
void processResistance(void);

void setVZero(float newVZero);
void setRZero(float newRZero);

#endif /* INC_MEASURE_H_ */
