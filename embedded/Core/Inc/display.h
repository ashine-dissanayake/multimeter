/*
 *  display.h
 * 	LCD Display Driver Header File.
 *  Created on: 23 Oct 2021
 *  Author: ENGG3800 - Group 8
 *
 */

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_

#include <stdbool.h>
#include "flags.h"

/** Function */
void updateConnection(void);
void updateVerticalBar(int brightnessLevel);
void updateModesControl(Mode currentMode);
void updateButtonsText(Mode currentMode);
void updateDCDisplay(void);
void updateACDisplay(void);
void updateResistanceDisplay(void);
void updateMinMax(void);
void updateResetButton(void);
void updateZeroButton(void);
void updateHoldButton(bool holdStatus);
void updateContinuityDisplay(void);
void updateTouch(void);
void updateDisplay(void);

#endif /* INC_DISPLAY_H_ */
