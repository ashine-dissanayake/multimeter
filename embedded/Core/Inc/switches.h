/*
 *  switches.h
 * 	Switches (Relays) Driver Header File.
 *  Created on: 16 Oct 2021
 *  Author: ENGG3800 - Group 8
 *
 */

#ifndef INC_SWITCHES_H_
#define INC_SWITCHES_H_

/** Define */
#define SWITCH_1 0
#define SWITCH_2 1
#define SWITCH_3 2
#define SWITCH_4 3

/** Functions. */
void closeSwitch(int switchNo);
void openSwitch(int switchNo);

#endif /* INC_SWITCHES_H_ */
