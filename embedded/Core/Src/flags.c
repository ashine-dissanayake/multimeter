/*
 *  flags.c
 * 	Flags Source File. Contains flags in the program.
 *  Created on: 4 Oct 2021
 *  Author: ENGG3800 - Group 8
 *
 */

#include "flags.h"

/** Default values for external variables */
int brightnessLevel = 3; // default is level 3.
bool holdStatus = false; // default is set to false.
Mode currentMode = DC; // default is set to DC mode.
bool resetStatus = false; //
int brightnessFlag = 0; // default is set to off.
int holdFlag = 0; // default is set to off.
bool zeroStatus = false; // default is set to false.
