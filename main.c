/*Using high interrupt for RFID and Low interrupt for IR Readings*/
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <string.h>
#include "LCD.h"
#include "dc_motor.h"
#include "IRsensors.h"
#include "rfid_module.h"

#define _XTAL_FREQ 8000000
#pragma config OSC = IRCIO, WDTEN = OFF //internal oscillator, WDT off

//********************* Global Variables *********************//

volatile char g_RFIDmatrix[16]; //Array for storing characters of RFID 

void delay_s(char seconds) // function to delay in seconds
{
    int i;
    for (i = 0; i <= (1000 * seconds); i++) {
        __delay_ms(1);
    }
}

void InterruptSetup(void) {
    //The following commands are used to set up the high order interrupt

    // Enable interrupt priority
    INTCONbits.GIEH = 1; // Enable all high priority interrupts
    INTCONbits.GIEL = 1; // Enable all low priority peripheral interrupts 

    PIE1bits.RCIE = 1; //Enable USART receive interrupt

    RCONbits.IPEN = 1; //Enable priority levels

    INTCONbits.TMR0IE = 1; // Timer overflow interrupt   IS THIS NEEDED HERE (CHECK PAGE 95 DATASHEET)  
    INTCON2bits.TMR0IP = 0; //TMR0 low priority
}

// High priority interrupt routine. This is triggered when RFID card is read

void interrupt InterruptHandlerHigh() {

    if (PIR1bits.RCIF == 1) { //If RFID reader receives data

        volatile char incRFID = 0;
        volatile char currentRFID;
        currentRFID = getCharSerial(); //receive serial from RFID

        //TRY THIS
        //while (RCREG != 0x02); //Wait until beginning of signal has been received

        for (incRFID = 0; incRFID < 16; incRFID++) {
            //Create a counter that equals g_RFIDmatrix element number

            currentRFID = getCharSerial();
            g_RFIDmatrix[incRFID] = currentRFID; //Store scanned value on array

            if (currentRFID == 0x03) {

                incRFID = 16; // If 0x03 is read it is ends the for loop

            } //If 0x03 was read by RFID
        } //End interrupt for-loop

        RCSTAbits.SPEN = 0; //Disable serial port 
        RCSTAbits.CREN = 0; //Disable continuous receive mode 

    }

}

void main(void) {

    OSCCON = 0x72; //8MHz
    while (!OSCCONbits.IOFS); //Wait for OSC to become stable 

    /********** LAT & TRIST PORTS *************/

    LATA = 0;
    LATC = 0;
    LATD = 0;
    TRISA = 0;
    TRISC = 0;
    TRISD = 0;

    //********************* Motor Setup *********************//

    int PWMcycle = 199; //Input PWMcycle as calculated by hand
    struct DC_motor motorL, motorR; //declare two DC_motor structures

    //For LEFT motor
    motorL.power = 0; //zero power to start
    motorL.direction = 1; //set default motor direction
    motorL.dutyLowByte = (unsigned char *) (&PDC0L); //store address of PWM duty low byte
    motorL.dutyHighByte = (unsigned char *) (&PDC0H); //store address of PWM duty high byte
    motorL.dir_pin = 0; //pin RB0/PWM0 controls direction (USE LB0)
    motorL.PWMperiod = PWMcycle; //store PWMperiod for motor

    //For RIGHT motor
    motorR.power = 0; //zero power to start
    motorR.direction = 1; //set default motor direction
    motorR.dutyLowByte = (unsigned char *) (&PDC1L); //store address of PWM duty low byte
    motorR.dutyHighByte = (unsigned char *) (&PDC1H); //store address of PWM duty high byte
    motorR.dir_pin = 2; //pin RB2/PWM2 controls direction (USE LB2)
    motorR.PWMperiod = PWMcycle; //store PWMperiod for motor

    //********************* Initialisations *********************/ /

    //Timer0Setup();
    InterruptSetup(); //Set up interrupt commands
    LCD_Init(); //Initialise LCD screen
    IR_Init(); //Initiate IR sensors
    initPWM(); //Setup PWM registers
    MotorSetup(); //Initiate motor associated pins
    SetupEUSART(); //Initialise EUSART 

    //********************* Local Variables *********************//

    //IR Variables

    int Direction;

    //Misc. Counters

    char buf[16];
    int j = 0; //Counter for ReturnArray
    char ReturnArray[100]; //Used to store movements made by robot
    
    //********************* Routine Preparation *********************//

    stop(&motorL, &motorR);

    TRISCbits.RC3 = 1; //Enable button

    SetLine(1);
    sprintf(buf, "Press button"); //Request user to press button
    LCD_String(buf);

    while (PORTCbits.RC3 == 0); //Loop breaks once button is pressed

    SendLCD(0b00000001, 0); //Clear screen
    __delay_ms(5); //delay 5ms
    SetLine(1);

    //********************* Initiate Tracking *********************//

    while (g_RFIDmatrix[5] == 0) { //Run while RFID has not received any data

        Direction = IR_Track(); //Determine directions using IR Sensors

        if (Direction == 1) {

            turnRight(&motorL, &motorR); //Turns robot to the right
            __delay_ms(80);
            __delay_ms(80);

        } else if (Direction == 2) {

            turnLeft(&motorL, &motorR); //Turns robot to the left
            __delay_ms(80);
            __delay_ms(80);

        } else if (Direction == 3) {

            fullSpeedAhead(&motorL, &motorR); //Moves robot forward
            __delay_ms(80);
            __delay_ms(80);
            
        } else if (Direction == 4) { //Search routine

            turnRight(&motorL, &motorR); //Searching method FIND SOMETHING BETTER??
            __delay_ms(80);
            __delay_ms(80);
            stop(&motorL, &motorR);
        }
        ReturnArray[j] = Direction;
        j++;

    } //End of tracking routine because RFID received data

    stop(&motorL, &motorR); //Once RFID has received data, the robot stops 

    delay_s(1);

    Check_Sum(); //Function located in "IRfunctions"

    while (PORTCbits.RC3 == 0) {
        if (j >= 0) {
            if (ReturnArray[j] == 1) {
                turnLeft(&motorL, &motorR);
                __delay_ms(80);
                __delay_ms(80);
            } else if (ReturnArray[j] == 2) {
                turnRight(&motorL, &motorR);
                __delay_ms(80);
                __delay_ms(80);
            } else if (ReturnArray[j] == 3) {
                fullSpeedReturn(&motorL, &motorR);
                __delay_ms(80);
                __delay_ms(80);
            } else if (ReturnArray[j] == 4) {
                turnLeft(&motorL, &motorR);
                __delay_ms(80);
                __delay_ms(80);
                ;
            }
        } else {
            stop(&motorL, &motorR);
        }
        j = j - 1;
    }
}
