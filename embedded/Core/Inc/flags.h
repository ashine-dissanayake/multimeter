/*
 *  flags.h
 * 	Flags Header File. Contains flags in the program.
 *  Created on: Sep 30, 2021
 *  Author: ENGG3800 - Group 8
 *
 */

#ifndef INC_FLAGS_H_
#define INC_FLAGS_H_

#include <stdbool.h>

/** Struct */
typedef enum { DC, AC, RESISTANCE, CONTINUITY } Mode;

/** External variable */
/** Current mode of multimeter. */
extern Mode currentMode;
/** Current brightness level. */
extern int brightnessLevel;
/** Current hold status. */
extern bool holdStatus;
/** Current reset status. */
extern bool resetStatus;
/** Current continuity status. */
extern bool continuityStatus;
/** Brightness flag when (+) or (-) button on LCD pressed. */
extern int brightnessFlag;
/** Hold flag when hold button on LCD pressed. */
extern int holdFlag;
/** Current zero status. */
extern bool zeroStatus;

#endif /* INC_FLAGS_H_ */
