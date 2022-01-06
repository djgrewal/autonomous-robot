/* 
 * File:   IRsensors.h
 * Author: Spyros
 *
 * Created on 05 December 2017, 17:19
 */

#ifndef IRSENSORS_H
#define	IRSENSORS_H

#define _XTAL_FREQ 8000000

#pragma config OSC = IRCIO, WDTEN = OFF //internal oscillator, WDT off
//near the start of your main function

void IR_Init();
int IR_Track();


#endif	/* IRSENSORS_H */

