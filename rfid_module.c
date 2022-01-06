//LOOK AT CHAPTER 19 OF DATASHEET FOR EUSART GUIDANCE
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include "rfid_module.h"
#include "LCD.h"
#include <string.h>


volatile char g_RFIDmatrix[16]; //array for storing characters of RFID

char getCharSerial(void) {
    while (!PIR1bits.RCIF); //wait for the data to arrive
    return RCREG; //return byte in RCREG
}

void SetupEUSART(void) { //CHANGE THIS NAME, WE ARE NOT USING EUSART NOW
    TRISCbits.RC7 = 1; //RC7 on the PIC18F should be an input since it receives data from RX2 (pin 9)
    SPBRG = 204; //set baud rate to 9600
    SPBRGH = 0;
    BAUDCONbits.BRG16 = 1; //set baud rate scaling to 16 bit mode
    TXSTAbits.BRGH = 1; //high baud rate select bit
    RCSTAbits.CREN = 1; //continuous receive mode
    RCSTAbits.SPEN = 1; //enable serial port, other settings default


}


/*
void RFIDread(void) {

    //int state;
    char currentRFID; // reads serial values

    currentRFID = getCharSerial(); //Reads the RFID value for the first time
    SetLine(1);
    while (currentRFID != 0x02); //The RFID value will be simply read 
    //until 0x02, i.e. the start bit have been scanned
    //Once scanned, the following for-loop will start storing the read values

    for (int incRFID = 0; incRFID < 16; incRFID++) {
        currentRFID = getCharSerial(); //receive serial from RFID on each iteration
        if (currentRFID != 0x03) {
            g_RFIDmatrix[incRFID] = currentRFID; //Store scanned value on array element
            SendLCD(currentRFID, 1);
            // __delay_ms(5); //delay 5?s
        }
        //
        if (currentRFID == 0x03) { //This if statement is located above the for-loop so that
            // if 0x03 is read it is not stored in RFIDmatrix
            RCSTAbits.SPEN = 0; //disable serial port once 0x03 has been read
            //RCSTAbits.CREN = 0; //continuous receive mode is now off
            //state = 1;

        }
        //
        //return state;
    }
}
 * */

/********************************************************************/

/* Function: Check_Sum
 Purpose: This function is applied on the data obtained by reading the RFID
 card. The high interrupt in the main creates an array g_RFIDmatrix where the 
 obtained values are stored. Check_Sum converts these values to decimal and then 
 sums up these decimal values in pairs, for a total of 5 pairs. These 5 pairs
 and XORed with the comparison sum, and if true, the LCD prints out that 
 Check Sum is correct. The conversion method is outlined in a later block
 Parameters: 
 *      RFID_decimal -- Stores RFID values in decimal form
        RFIDelement -- Used as a counter for a loop
        Sum_Pairs -- Stores the sum of pairs (total of 5 elements)
        Comparison_Sum -- Last pair, used for comparison
 Returns: Nothing   */

void Check_Sum() { //This function uses RFIDmatrix to check sum 

    char RFID_decimal[12]; //Array with decimal values read
    char Sum_Pairs [5]; //Sums of pairs of read elements


    SendLCD(0b00000001, 0);
    __delay_ms(5); //delay 5ms

    SetLine(1);
    LCD_String(g_RFIDmatrix);



    for (int RFIDelement = 0; RFIDelement < 12; RFIDelement++) { //Run the for 
        // loop as many times as there are stored elements in RFIDmatrix

        /*ASCII range [0,9] --> decimal range [48,57] 
        For example, 0 (ASCII) -> 48 (decimal). Therefore decimal = ASCII - 48
        check if examined element is within the range [0,9] */

        if (g_RFIDmatrix [RFIDelement] >= 48 && g_RFIDmatrix [RFIDelement] <= 57) {
            RFID_decimal[RFIDelement] = g_RFIDmatrix[RFIDelement] - 48;
            //Apply above eqt and store decimal result in RFID_decimal element
        }

        /*ASCII range [A,F], decimal [65,70], hexa [10,15]
        i.e. A (ASCII) --> 65 (decimal) but also A --> 10 (Hexa)
        Therefore, decimal = ASCII - 55
        check if examined element is within the range [65,70] */

        if (g_RFIDmatrix [RFIDelement] >= 65 && g_RFIDmatrix [RFIDelement] <= 70) {
            RFID_decimal[RFIDelement] = g_RFIDmatrix[RFIDelement] - 55;
        }
    }

    //The following sections sums up the pairs of inputs
    //the first 10 elements of RFID_decimal need to be manipulated. 

    for (int i = 0; i <= 4; i++) {
        Sum_Pairs[i] = (RFID_decimal[2 * i] << 4)+(RFID_decimal[(2 * i) + 1]);
    }

    char Comparison_Sum; //The sum of last two elements of RFID_decimal array 
    Comparison_Sum = (RFID_decimal[10] << 4) + RFID_decimal[11]; 

    char RFIDbuf[16];

    //If the XOR statement is true, the RFID_decimal array is printed on the LCD
    if (Sum_Pairs[0]^Sum_Pairs[1]^Sum_Pairs[2]^Sum_Pairs[3]^Sum_Pairs[4] == Comparison_Sum) {
        SetLine(2);
        strcpy(RFIDbuf, "CheckSum worked");
        LCD_String(RFIDbuf);
    }

}



