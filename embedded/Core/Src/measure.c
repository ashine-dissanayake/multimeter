/*
 *  measure.c
 *
 *  Created on: Oct 4, 2021
 *  Author: ENGG3800 - Group 8
 *
 */

#include "measure.h"
#include "ads1115_driver.h"
#include "switches.h"
#include <math.h>

// Measure and set these values to calibrate
// voltage drop characteristics due to zener current leakage
#define VD_EXP 8.205079 // voltage line
#define VD_SCALE (0.001 * 0.02098864 * 1.12)
#define ID_EXP 8.212609
#define ID_SCALE (0.001 * 0.0274328) // current line

// measure and set these values to calibrate
#define V_MID 1.6816 // adc input when V at probes is 0
#define V_15 2.7984  // adc input when V is 15

#define V_SCALE (15.0 / (V_15 - V_MID))
#define V_OFFSET (-V_MID)

#define I_SCALE_100                                                            \
    (0.0000267 /                                                               \
     (2.0 * 1.282 * 0.85978 * 1.5537 * 0.8829 * 0.8545 * 0.92 *                \
      0.9259)) // Amps per volt.
#define I_SCALE_1                                                              \
    ((0.0000267 / (2.0 * 1.282 * 0.85978)) * 100 * 5.4725 * 0.884 * 1.24)
#define I_OFFSET (-0.0815)

// Using a min DAC voltage of 0.1V:
// 100 Ohm shunt can measure 1.1M - 3k
// 1 Ohm shunt can measure down to 30 Ohm
// 100m shunt can measure down to 3 Ohm
// can reduce DAC below 0.1V to measure lower than that

// The range of voltages that are reliably output by the DAC
#define DAC_MIN_V 0.05
#define MAX_DAC_INCREMENT 0.05
#define DAC_MAX_V 3.2

// The range of pin voltages that are reliably measured by the ADC
#define ADC_MAX_V 3.0
#define ADC_MIN_V 0.1

// The range of safe measured voltages for resistance and continuity mode
#define V_SAFE_MAX 3.3
#define V_SAFE_MIN -0.1

#define V_TARGET 1.7

struct ResistancePacket resistancePacket = {0};

unsigned int relayStatus = 0;
#define RELAY_STATUS_PROTECTED 0
#define RELAY_STATUS_HIGH_R 1
#define RELAY_STATUS_MID_R 2
#define RELAY_STATUS_LOW_R 3

#define MAX_RELAY_STATUS                                                       \
    RELAY_STATUS_MID_R // the lowest allowable resistance state

float vDac = 0;
float rShunt = 100;
float vZero = 0;
float dacIncrement = MAX_DAC_INCREMENT;

void setVZero(float newVZero) { vZero = newVZero; }

void growBuffer(float *buff, int newSz) {
    // get average in buffer
    float tot = 0;
    for (int i = 0; i < bufferSize; i++) {
        if (buff[i] < 4 && buff[i] > -0.2) {
            tot += buff[i];
        } else {
            tot += tot / (i + 1);
        }
    }
    float ave = tot / bufferSize;
    // set new spaces to match average
    for (int i = 0; i < newSz; i++) {
        buff[i] = ave;
    }
}

/**
 * Configures relays and relayStatus top match the provided mode
 */
void setRelayMode(unsigned int mode) {
    switch (mode) {
    case RELAY_STATUS_PROTECTED:
        openSwitch(SWITCH_1);
        openSwitch(SWITCH_2);
        openSwitch(SWITCH_4);
        openSwitch(SWITCH_3);
        break;
    case RELAY_STATUS_HIGH_R:
        closeSwitch(SWITCH_1);
        closeSwitch(SWITCH_2);
        openSwitch(SWITCH_4);
        openSwitch(SWITCH_3);
        break;
    case RELAY_STATUS_MID_R:
        closeSwitch(SWITCH_1);
        closeSwitch(SWITCH_2);
        closeSwitch(SWITCH_4);
        openSwitch(SWITCH_3);
        break;
    case RELAY_STATUS_LOW_R:
        closeSwitch(SWITCH_1);
        closeSwitch(SWITCH_2);
        closeSwitch(SWITCH_4);
        closeSwitch(SWITCH_3);
        break;
    }
}

/**
 * calculates the correction term for zener diode current leak on the voltage
 *  ADC channel using a power series
 */
float vADCCorrection(float vMeas) {
    float correction = VD_SCALE * powf(vMeas, VD_EXP);
    // cap at 100mv
    correction = correction > 0.1 ? 0.1 : correction;
    return correction;
}

/**
 * calculates the correction term for zener diode current leak on the
 * current ADC channel using a power series
 */
float iADCCorrection(float vMeas) {
    float correction = ID_SCALE * powf(vMeas, ID_EXP);
    // cap at 100mv
    correction = correction > 0.1 ? 0.1 : correction;
    return correction;
}

/**
 * Returns actual average DC voltage at the probes
 */
float calcVDC(float *buf) {
    float vSum = 0.0;
    // average the voltage buffer
    for (int i = 0; i < bufferSize; i++) {
        float vAdd = V_SCALE * ((buf[i]) + vADCCorrection((vSum / bufferSize)) +
                                V_OFFSET);
        if (vAdd > 17 || vAdd < -17) {
            vSum += vSum / (i + 1);
        } else {
            vSum += buf[i];
        }
    }
    return (V_SCALE * ((vSum / bufferSize) +
                       vADCCorrection((vSum / bufferSize)) + V_OFFSET)) -
           vZero;
}

/**
 * Returns actual average DC voltage at the probes
 */
float calcIDC(float *buf) {
    float iSum = 0.0;
    // average the current buffer
    for (int i = 0; i < bufferSize; i++) {
        iSum += buf[i];
    }
    return (I_SCALE_100 * ((iSum / bufferSize) +
                           iADCCorrection(iSum / bufferSize) + I_OFFSET));
}

/**
 * Returns actual RMS voltage at the probes
 */
float calcVRMS(float *buf) {
    float vSum = 0.0;
    for (int i = 0; i < bufferSize; i++) {
        vSum += ((V_SCALE * ((buf[i] + vADCCorrection(buf[i])) + V_OFFSET)) -
                 vZero) *
                ((V_SCALE * ((buf[i] + vADCCorrection(buf[i])) + V_OFFSET)) -
                 vZero);
    }
    return sqrt(vSum / bufferSize);
}

/**
 * Returns actual max voltage at the probes
 */
float calcVMax(float *buf) {
    float vmax = 0.0;
    for (int i = 0; i < bufferSize; i++) {
        if (buf[i] > vmax) {
            vmax = buf[i];
        }
    }
    return V_SCALE * (vmax + vADCCorrection(vmax) + V_OFFSET);
}

/**
 * Returns actual min voltage at the probes
 */
float calcVMin(float *buf) {
    float vMin = 1000.0;
    for (int i = 0; i < bufferSize; i++) {
        if (buf[i] < vMin) {
            vMin = buf[i];
        }
    }
    return (V_SCALE * (vMin + vADCCorrection(vMin) + V_OFFSET));
}

/**
 * Returns resistance at the probes, or -999.0 if the protection circuit was
 * activated call this once every loop
 */
float calcR(DAC_HandleTypeDef *hdac, float *vBuf, float *iBuf) {

    // set calculation values based on shunt mode
    float iScale = 0;
    switch (relayStatus) {
    case RELAY_STATUS_HIGH_R:
        iScale = I_SCALE_100;
        break;
    case RELAY_STATUS_MID_R:
        iScale = I_SCALE_1;
        break;
    }

    // average dac reading for current channel
    float iSum = 0.0;
    // average the current buffer
    for (int i = 0; i < bufferSize; i++) {
        float vi = (iScale * (iBuf[i] + I_OFFSET));
        if (vi < -15 || vi > 15) {
            iSum += i > 0 ? iBuf[i - 1] : iBuf[i + 1];
        } else {
            iSum += iBuf[i];
        }
    }

    float v_i_ave =
        (iSum / bufferSize) +
        iADCCorrection(iSum / bufferSize); // actual voltage at the ADC input
    float current = (iScale * (v_i_ave + I_OFFSET));

    float voltage = calcVDC(vBuf);

    // trigger or turn off protected mode for high voltage
    if (voltage > V_SAFE_MAX || voltage < V_SAFE_MIN) {
        setRelayMode(RELAY_STATUS_PROTECTED);
        relayStatus = RELAY_STATUS_PROTECTED;
        return -999;
    } else if (relayStatus == RELAY_STATUS_PROTECTED) {
        relayStatus++;
    }

    /* configure shunt resistor relays and adjust DAC output*/
    if (v_i_ave >= ADC_MAX_V) { // I too high
        vDac -= dacIncrement;   // lower V -> lower I.
        if (vDac <= DAC_MIN_V) {
            // close a relay
            if (!(relayStatus > MAX_RELAY_STATUS)) {
                relayStatus++;
                vDac = DAC_MAX_V - MAX_DAC_INCREMENT;
            }
        }
    } else if (v_i_ave + I_OFFSET <= ADC_MIN_V) { // I too low
        vDac += dacIncrement;                     // higher V -> higher I
        if (vDac >= DAC_MAX_V) {
            // open a relay
            if (!(relayStatus <= RELAY_STATUS_HIGH_R)) {
                relayStatus--;
                vDac = DAC_MIN_V + MAX_DAC_INCREMENT;
            }
        }
    } else if (voltage < V_TARGET) {
        vDac += dacIncrement;
    }

    // apply new DAC voltage if needed
    if (vDac < DAC_MIN_V) {
        vDac = DAC_MIN_V;
    } else if (vDac >= DAC_MAX_V) {
        vDac = DAC_MAX_V;
    }
    setDACValue(hdac, DAC_CHANNEL_1, stm32VoltageToDAC(vDac));
    // apply relay change
    relayStatus =
        relayStatus >= MAX_RELAY_STATUS ? MAX_RELAY_STATUS : relayStatus;
    setRelayMode(relayStatus);

    /* calculate resistance */
    float resistance = voltage / current;

    //	return current;
    return resistance;
}

/*
 * call this when entering resistance or cont mode
 */
void initRMeas(DAC_HandleTypeDef *hdac) {
    // go to highest resistance and open isolation relays
    setRelayMode(RELAY_STATUS_HIGH_R);

    // start DAC and set to min output
    HAL_DAC_Start(hdac, DAC_CHANNEL_1);
    setDACValue(hdac, DAC_CHANNEL_1, stm32VoltageToDAC(DAC_MIN_V));
}

/*
 * call this when switching away from res or cont modes
 */
void deinitRMeas(DAC_HandleTypeDef *hdac) {
    // turn off DAC
    setDACValue(hdac, DAC_CHANNEL_1, stm32VoltageToDAC(DAC_MIN_V));
    HAL_DAC_Stop(hdac, DAC_CHANNEL_1);
    // go to protected mode
    setRelayMode(RELAY_STATUS_PROTECTED);
}

void processResistance(void) {

    char temp[12] = {0};
    int decPoint;
    float filteredValue = 0.0;

    if (resistance > 999999) {
        resistancePacket.unit = 'M';
        filteredValue = resistance / 1000000;
    } else if (resistance > 999) {
        resistancePacket.unit = 'k';
        filteredValue = resistance / 1000;
    } else {
        filteredValue = resistance;
        resistancePacket.unit = '0';
    }

    if (filteredValue < 0) {
        filteredValue = 0;
    }
    sprintf(temp, "%f", filteredValue);

    for (int i = 0; i < strlen(temp); i++) {
        if (temp[i] == '.') {
            decPoint = i;
            break;
        }
    }

    if (temp[0] == '0') {
        sprintf(resistancePacket.value, "%.3f", filteredValue);
    } else if (decPoint == 1) {
        sprintf(resistancePacket.value, "%.2f", filteredValue);
    } else if (decPoint == 2) {
        sprintf(resistancePacket.value, "%.1f", filteredValue);
    } else if (decPoint == 3) {
        sprintf(resistancePacket.value, "%d", (int)filteredValue);
    } else {
        sprintf(resistancePacket.value, "%d",
                (int)((round(filteredValue / pow(10, (decPoint - 3)))) *
                      pow(10, (decPoint - 3))));
    }
}
