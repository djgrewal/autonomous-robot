#ifndef _RFID_MODULE_H
#define _RFID_MODULE_H
#define _XTAL_FREQ 8000000
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include "rfid_module.h"
#include "LCD.h"
#include <string.h>

char getCharSerial(void);
void SetupEUSART(void);
//void RFIDread (void);
void Check_Sum (void);


#endif