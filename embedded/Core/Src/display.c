/*
 *  display.c
 * 	LCD Display Driver Header File.
 *  Created on: 23 Oct 2021
 *  Author: ENGG3800 - Group 8
 *
 */

#include "display.h"
#include "ili9163.h"
#include "lcd.h"
#include "xpt2046_touch.h"
#include <stdbool.h>

/**
 * Updates whether the PC connection has been established.
 *
 * return: (none)
 */
void updateConnection(void) {
    ILI9163_drawString(124, 1, Font_16x26, BLACK, "~");

    if (isJoin == 1) {
        ILI9163_drawRect(126, 4, 138, 14, 5, YELLOW);
    } else {
        ILI9163_drawRect(126, 4, 138, 14, 5, RED);
    }
}

/**
 * Updates whether the the brightness level status on the LCD.
 *
 * return: (none)
 */
void updateVerticalBar(int brightnessLevel) {
    switch (brightnessLevel) {
    case 1: // level 1
        ILI9163_fillRect(147, 25, 153, 39, WHITE);  // rect bar 1
        ILI9163_fillRect(147, 41, 153, 55, WHITE);  // rect bar 2
        ILI9163_fillRect(147, 57, 153, 71, WHITE);  // rect bar 3
        ILI9163_fillRect(147, 73, 153, 87, WHITE);  // rect bar 4
        ILI9163_fillRect(147, 89, 153, 103, GREEN); // rect bar 5
        break;
    case 2: // level 2
        ILI9163_fillRect(147, 25, 153, 39, WHITE);
        ILI9163_fillRect(147, 41, 153, 55, WHITE);
        ILI9163_fillRect(147, 57, 153, 71, WHITE);
        ILI9163_fillRect(147, 73, 153, 87, GREEN);
        ILI9163_fillRect(147, 89, 153, 103, GREEN);
        break;
    case 3: // level 3
        ILI9163_fillRect(147, 25, 153, 39, WHITE);
        ILI9163_fillRect(147, 41, 153, 55, WHITE);
        ILI9163_fillRect(147, 57, 153, 71, GREEN);
        ILI9163_fillRect(147, 73, 153, 87, GREEN);
        ILI9163_fillRect(147, 89, 153, 103, GREEN);
        break;
    case 4: // level 4
        ILI9163_fillRect(147, 25, 153, 39, WHITE);
        ILI9163_fillRect(147, 41, 153, 55, GREEN);
        ILI9163_fillRect(147, 57, 153, 71, GREEN);
        ILI9163_fillRect(147, 73, 153, 87, GREEN);
        ILI9163_fillRect(147, 89, 153, 103, GREEN);
        break;
    case 5: // level 5
        ILI9163_fillRect(147, 25, 153, 39, GREEN);
        ILI9163_fillRect(147, 41, 153, 55, GREEN);
        ILI9163_fillRect(147, 57, 153, 71, GREEN);
        ILI9163_fillRect(147, 73, 153, 87, GREEN);
        ILI9163_fillRect(147, 89, 153, 103, GREEN);
        break;
    }
}

/**
 * Updates the current multimeter mode on the LCD.
 * Make the active mode tab bigger.
 *
 * return: (none)
 */
void updateModesControl(Mode currentMode) {
    switch (currentMode) {
    case DC:
        ILI9163_fillRect(0, 20, 20 + 5, 40, BLACK);
        ILI9163_fillRect(0, 45, 20, 65, BLACK);
        ILI9163_fillRect(0, 70, 20, 90, BLACK);
        ILI9163_fillRect(0, 95, 20, 115, BLACK);
        break;
    case AC:
        ILI9163_fillRect(0, 20, 20, 40, BLACK);
        ILI9163_fillRect(0, 45, 20 + 5, 65, BLACK);
        ILI9163_fillRect(0, 70, 20, 90, BLACK);
        ILI9163_fillRect(0, 95, 20, 115, BLACK);
        break;
    case RESISTANCE:
        ILI9163_fillRect(0, 20, 20, 40, BLACK);
        ILI9163_fillRect(0, 45, 20, 65, BLACK);
        ILI9163_fillRect(0, 70, 20 + 5, 90, BLACK);
        ILI9163_fillRect(0, 95, 20, 115, BLACK);
        break;
    case CONTINUITY:
        ILI9163_fillRect(0, 20, 20, 40, BLACK);
        ILI9163_fillRect(0, 45, 20, 65, BLACK);
        ILI9163_fillRect(0, 70, 20, 90, BLACK);
        ILI9163_fillRect(0, 95, 20 + 5, 115, BLACK);
        break;
    }
}

/**
 * Updates the current multimeter mode on the LCD.
 * move the active mode text slightly over.
 *
 * return: (none)
 */
void updateButtonsText(Mode currentMode) {
    switch (currentMode) {
    case DC:
        ILI9163_drawString(2 + 3, 26, Font_7x10, WHITE, "DC");
        ILI9163_drawString(2, 50, Font_7x10, WHITE, "AC");
        ILI9163_drawString(2, 75, Font_7x10, WHITE, "RE");
        ILI9163_drawString(2, 100, Font_7x10, WHITE, "CN");
        break;
    case AC:
        ILI9163_drawString(2, 26, Font_7x10, WHITE, "DC");
        ILI9163_drawString(2 + 3, 50, Font_7x10, WHITE, "AC");
        ILI9163_drawString(2, 75, Font_7x10, WHITE, "RE");
        ILI9163_drawString(2, 100, Font_7x10, WHITE, "CN");
        break;
    case RESISTANCE:
        ILI9163_drawString(2, 26, Font_7x10, WHITE, "DC");
        ILI9163_drawString(2, 50, Font_7x10, WHITE, "AC");
        ILI9163_drawString(2 + 3, 75, Font_7x10, WHITE, "RE");
        ILI9163_drawString(2, 100, Font_7x10, WHITE, "CN");
        break;
    case CONTINUITY:
        ILI9163_drawString(2, 26, Font_7x10, WHITE, "DC");
        ILI9163_drawString(2, 50, Font_7x10, WHITE, "AC");
        ILI9163_drawString(2, 75, Font_7x10, WHITE, "RE");
        ILI9163_drawString(2 + 3, 100, Font_7x10, WHITE, "CN");
        break;
    }
}

/**
 * Current mode is DC. The corresponding DC voltage value.
 *
 * return: (none)
 */
void updateDCDisplay(void) {

	// Store the DC voltage.
    char current[10] = {0};
    snprintf(current, 10, "V = %.2fV", adcCurr);

    ILI9163_drawString(30, 25, Font_7x10, BLUE, current);
}

/**
 * Current mode is AC. The corresponding Vrms and DC offset
 * voltage value.
 *
 * return: (none)
 */
void updateACDisplay(void) {

	// Store the AC voltage.
    char current[20] = {0};
    snprintf(current, 15, "Vrms = %.2fV", adcCurr);
    ILI9163_drawString(30, 25, Font_7x10, BLUE, current);
    sprintf(current, "DC OFF. = %.2fV", dcOffset);
    ILI9163_drawString(30, 40, Font_7x10, BLUE, current);
}

/**
 * Current mode is Resistance. The corresponding resistance value.
 *
 * return: (none)
 */
void updateResistanceDisplay(void) {
    int offsetX = -10;
    int offsetY = -3;

    ILI9163_drawString(30, 25, Font_7x10, BLACK, "RESISTANCE:");

    ILI9163_drawStringF(60 + offsetX, 52 + offsetY, Font_11x18, BLUE, "%s%c~", resistancePacket.value, resistancePacket.unit);
}

/**
 * Displays/ writes the min and max values in DC and AC voltage mode.
 *
 * return: (none)
 */
void updateMinMax(void) {
    int offsetX = 0;
    int offsetY = 2;

    char maxValue[10] = {0};
    char minValue[10] = {0};
    sprintf(maxValue, "MAX:%.2fV", adcMax);
    sprintf(minValue, "MIN:%.2fV", adcMin);

    ILI9163_drawString(30 + offsetX, 55 + offsetY, Font_7x10, BLACK, minValue);
    ILI9163_drawString(30 + offsetX, 70 + offsetY, Font_7x10, BLACK, maxValue);

    // If the reset button was pressed, the min and max are set to zero.
    if (resetStatus) {
        resetStatus = false;
        firstADCFlag = 1;
        adcMax = 0.0;
        adcMin = 0.0;
    }
}

/**
 * Draw the reset button on LCD screen.
 *
 * return: (none)
 */
void updateResetButton(void) {
    ILI9163_fillCircle(65, 8, 9, ORANGE);
    ILI9163_drawString(62, 5, Font_7x10, WHITE, "R");
}

/**
 * Draw the zero button on LCD screen.
 *
 * return: (none)
 */
void updateZeroButton(void) {
    ILI9163_fillCircle(100, 8, 9, GREEN);
    ILI9163_drawString(97, 5, Font_7x10, WHITE, "Z");
}

/**
 * Draws the hold button on the LCD screen. If the holdStatus is
 * true, the button is green. Otherwise button is red.
 *
 * return: (none)
 */
void updateHoldButton(bool holdStatus) {
    int offsetX = 15;
    int offsetY = 8;

    ILI9163_drawString(40 + offsetX, 100 + offsetY, Font_7x10, BLACK, "HOLD");
    if (!holdStatus) { // hold off
        ILI9163_fillCircle(85 + offsetX, 104 + offsetY, 12, RED);
        ILI9163_drawString(75 + offsetX, 100 + offsetY, Font_7x10, WHITE,
                           "OFF");
    } else { // hold on
        ILI9163_fillCircle(85 + offsetX, 104 + offsetY, 12, GREEN);
        ILI9163_drawString(75 + offsetX, 100 + offsetY, Font_7x10, WHITE, "ON");
    }
}

/**
 * Current mode is continuity. Write corresponding continuity threshold
 * value and whether continuity is OPEN or CLOSED.
 *
 * return: (none)
 */
void updateContinuityDisplay(void) {
	// current values
    char current[5] = {0};
    sprintf(current, "%.2f", currentCT);

    ILI9163_drawString(30, 25, Font_7x10, BLACK, "CONTINUITY:");

    if (currentCTStatus == 1) {
        ILI9163_drawString(70, 40, Font_7x10, ORANGE, "CLOSED");
    } else {
        ILI9163_drawString(70, 40, Font_7x10, ORANGE, "OPEN");
    }
    ILI9163_drawString(30, 55, Font_7x10, BLACK, "THRESHOLD (~):");

    ILI9163_drawString(70, 70, Font_7x10, BLACK, current);
}

/**
 * Update the display by calling all methods to draw LCD display.
 * Once all items are drawn on the LCD, the LCD should be rendered.
 *
 * return: (none)
 */
void updateDisplay(void) {
    ILI9163_newFrame(); // Call this to clear the frame buffer out
    // Create brightness vertical bar:
    ILI9163_drawRect(145, 23, 155, 41, 2, BLACK);
    ILI9163_drawRect(145, 39, 155, 57, 2, BLACK);
    ILI9163_drawRect(145, 55, 155, 73, 2, BLACK);
    ILI9163_drawRect(145, 71, 155, 89, 2, BLACK);
    ILI9163_drawRect(145, 87, 155, 105, 2, BLACK);
    ILI9163_drawString(145, 5, Font_11x18, BLACK, "+");
    ILI9163_drawString(145, 104, Font_11x18, BLACK, "-");
    updateVerticalBar(brightnessLevel);

    // Create "DMulti" label:
    ILI9163_fillRect(0, 0, 45, 15, RED);
    ILI9163_drawString(2, 2, Font_7x10, WHITE, "DMULTI");
    ILI9163_drawString(30, 87, Font_7x10, BLACK, (char *)message);

    // Create "Mode" buttons:
    updateModesControl(currentMode);
    updateButtonsText(currentMode);

    // Create "HOLD" button:
    updateHoldButton(holdStatus);

    // Create display for different modes:
    if (currentMode == DC) {
        updateDCDisplay();
        updateMinMax();
    } else if (currentMode == AC) {
        updateACDisplay();
        updateMinMax();
    } else if (currentMode == RESISTANCE) {
        updateResistanceDisplay();
    } else if (currentMode == CONTINUITY) {
        updateContinuityDisplay();
    }

    // Create connection icon.
    updateConnection();

    // Create RESET button:
    updateResetButton();

    // Create zero probing button:
    updateZeroButton();

    ILI9163_render(); // Draws the whole frame buffer via DMA.
}

/**
 * Check if the LCD registers a touch. Checking if any
 * buttons were pressed.
 *
 * return: (none)
 */
void updateTouch(void) {
    uint16_t x = 0;
    uint16_t y = 0;

    if (XPT2046_TouchPressed()) { // checking press.
        XPT2046_TouchGetCoordinates(&y, &x);

        // Brightness level
        if ((x >= 140 && x <= 160) && (y >= 110 && y <= 130)) {
            if (brightnessLevel < 5) {
                brightnessFlag = 1;
                brightnessLevel++;
                adjustBrightness();
            }
        } else if ((x >= 140 && x <= 160) && (y >= 0 && y <= 15)) {
            if (brightnessLevel > 1) {
                brightnessFlag = 1;
                brightnessLevel--;
                adjustBrightness();
            }
        }
        // Check for modes:
        if ((x >= 0 && x <= 20) && (y >= 100 && y <= 120)) {
        	if (currentMode != DC) {
        		holdStatus = false;
        	}
            currentMode = DC;
        } else if ((x >= 0 && x <= 20) && (y >= 31 && y <= 90)) {
        	if (currentMode != AC) {
        		holdStatus = false;
        	}
            currentMode = AC;
        } else if ((x >= 0 && x <= 20) && (y >= 16 && y <= 30)) {
        	if (currentMode != RESISTANCE) {
        		holdStatus = false;
        	}
            currentMode = RESISTANCE;
        } else if ((x >= 0 && x <= 20) && (y >= 0 && y <= 15)) {
        	if (currentMode != CONTINUITY) {
        		holdStatus = false;
        	}
            currentMode = CONTINUITY;
        }
        // Check for "HOLD" button:
        if ((x >= 50 && x <= 130) && (y >= 0 && y <= 20)) {
            holdStatus = !holdStatus;
            holdFlag = 1;
        }
        // Check if the reset button has been pressed:
        if ((x >= 20 && x <= 30) && (y >= 120 && y <= 128)) {
            resetStatus = true;
        }
        // Check if the zero button has been pressed.
        if ((x >= 120 && x <= 128) && (y >= 120 && y <= 128)) {
            zeroStatus = true;
        }
    }
}
