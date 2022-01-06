#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <string.h>
#include "LCD.h"
#include "dc_motor.h"
#include "IRsensors.h"

#define _XTAL_FREQ 8000000
#pragma config OSC = IRCIO, WDTEN = OFF //internal oscillator, WDT off

void IR_Init() {
    /*IR REGISTERS FOR BOTH IR RECIEVERS*/


    T5CON = 0b00010001; //BIT 6:  Special Event Trigger Reset is enabled.
    //BIT 5:  Continuous Count mode is enabled.
    //BIT 4-3: Prescaler set to 1:8.
    //BIT 0: Timer 5 enabled.

    CAP1CON = 0b01000110; //BIT 6: Enable timing mode. (CAP2 used for pin RA3).
    CAP2CON = 0b01000110; //BIT 6: Enable timing mode. (CAP2 used for pin RA3).
    //BIT 3-0: Pulse-Width Measurement mode, every falling to rising edge.
    DFLTCON = 0b01011111; //Noise filter for T5 and CAP2.

    ANSEL0bits.ANS2 = 0; //Turn off analog read for pin RA2   
    ANSEL0bits.ANS3 = 0; //Turn off analog read for pin RA3

    TRISAbits.RA2 = 1; //RA3 is an input
    TRISAbits.RA3 = 1; //RA3 is an input

}

int IR_Track() {


    //CAP1CON = 0b01000110; //BIT 6: Enable timing mode. (CAP2 used for pin RA3).
    //CAP2CON = 0b01000110; //BIT 6: Enable timing mode. (CAP2 used for pin RA3).
    //********************* Local Variables *********************//

    // Signal Acquisition 
    unsigned int IRSignalR; //Define Right IR sensor signal
    unsigned int IRSignalL; //Define Left IR sensor signal
    int IRDiff; //Signal difference
    unsigned int IRSum; //Signal sum
    int Direction = 0;
    char buf[16];

    //Signal Filtering
    int IRstabilise = 0; //Use for error elimination
    int IRcheck = 0; //RENAME
    char DirArray[20]; //Array to store direction values for error minimisation



    SendLCD(0b00000001, 0); //Clear LCD
    __delay_ms(5); //delay 5ms
    SetLine(1);


    while (IRstabilise != 1) {

        for (int i = 0; i <= 20; i++) {

            IRSignalR = CAP2BUFL; //8 LSB of right signal
            IRSignalR += ((unsigned int) CAP2BUFH << 8); //Shift and ad CAP2BUFH
            // to right IR signal to obtain the 16-bit signal

            IRSignalL = CAP1BUFL; //Same process for left signal
            IRSignalL += ((unsigned int) CAP1BUFH << 8);

            IRDiff = IRSignalR - IRSignalL; //Define difference 
            IRSum = IRSignalR + IRSignalL; //Define sum
            SetLine(2);
            sprintf(buf, "%d", IRDiff);
            LCD_String(buf); //Print out difference

          
            if (IRSum > 2000) { //Check if a general signal is being recorded by sensors
                //if it is, locate the direction the robot should move

                if ((IRDiff) > 50) { //for positive (above 500) difference, turn right
                    Direction = 1;

                } else if ((IRDiff)<-35) { //for negative (below -500) difference, turn left
                    Direction = 2;

                } else { //If signal is between [-500,500], IR source is straight ahead
                    Direction = 3;

                }
            } else { //If no signal is registered, keep searching

                Direction = 4;
            }
            DirArray[i] = Direction;
        } 
        
        for (int i = 0; i <= 19; i++) {
            IRcheck = DirArray[i] & DirArray[i + 1]; //Check if all elements in DirArray ar the same
        }
        if (IRcheck == 1) { //If all elements are the same, exit while loop and move accordingly
            IRstabilise = 1; //BETTER COMMENTS HERE
        }

        return Direction;

    }//While loop ends as signal is stable

}

/*
int IR_Run(void) {
    char buf[16];
    int IRSignalR;
    int IRSignalL;
    int IRDiff;
    int motorcommand;

    IRSignalR = CAP2BUFL;
    IRSignalR += ((unsigned int) CAP2BUFH << 8);

    IRSignalL = CAP1BUFL;
    IRSignalL += ((unsigned int) CAP1BUFH << 8);

    IRDiff = IRSignalR - IRSignalL;

    if ((IRDiff) > 500) {
        motorcommand = 1;
        sprintf(buf, "Turn Right");
    } else if ((IRDiff)<-500) {
        motorcommand = 2;
        sprintf(buf, "Turn Left");
    } else {
        motorcommand = 3;
        sprintf(buf, "Full Speed Ahead");
    }

    return motorcommand;
}
*/
