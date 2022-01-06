#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <string.h>
#include "LCD.h"
//compiler directive to set clock to internal oscillator
#pragma config OSC = IRCIO

//DO WE NEED BUF HERE?
volatile char buf[16]; //buffer for characters for LCD

void E_TOG(void) {
    //don?t forget to put a delay between the on and off
    LCD_E = 1;
    __delay_us(5); // 5us delay ? remember to define _XTAL_FREQ
    LCD_E = 0;
    //commands! 5us will be plenty.

}
//function to send four bits to the LCD

void LCDout(unsigned char number) {
    //set data pins using the four bits from number
    
    LCD_DB4 = (number & 0b00000001); //Mask the input number depending on the DB of the LCD
    LCD_DB5 = (number & 0b00000010) >> 1;
    LCD_DB6 = (number & 0b00000100) >> 2;
    LCD_DB7 = (number & 0b00001000) >> 3;


    //toggle the enable bit to send data
    E_TOG();
    __delay_us(5); // 5us delay
    
}
//function to send data/commands over a 4bit interface

void SendLCD(unsigned char Byte, char type) {
    // set RS pin whether it is a Command (0) or Data/Char (1)
    LCD_RS = type;
    // using type as the argument
    // send high bits of Byte using LCDout function
    //first split byte into two nibbles
    unsigned char lowN = Byte & 0b00001111;
    unsigned char hiN = (Byte >> 4) & 0b00001111;

    // send high bits of Byte using LCDout function
    LCDout(hiN);

    __delay_us(10); // 10us delay

    // send low bits of Byte using LCDout function
    LCDout(lowN);
    __delay_us(10); // 10us delay


}

void LCD_String(char *string) {

    while (*string != 0) {
        //Send out the current byte pointed to
        // and increment the pointer
        SendLCD(*string++, 1);
        __delay_us(50); //so we can see each character
        //being printed in turn (remove delay if you want
        //your message to appear almost instantly)
    }
}

void LCD_Init(void) {
    //TRISCbits.RC3 = 1; //Set RC3 as an input (port of the button)


    // Initialisation sequence code - see the data sheet

    __delay_ms(15); //delay 15mS
    LCDout(0b0011); //send 0b0011 using LCDout
    __delay_ms(5); //delay 5ms
    LCDout(0b0011); //send 0b0011 using LCDout
    __delay_us(200); //delay 200us
    LCDout(0b0011); //send 0b0011 using LCDout
    __delay_us(50); //delay 50us
    LCDout(0b0010); //send 0b0010 using LCDout set to four bit mode
    __delay_us(50); //delay 50us

    // now use SendLCD to send whole bytes 

    // Set function 2 line display
    SendLCD(0b00101000, 0);
    __delay_us(50); //delay 50us

    // Display off
    SendLCD(0b00001000, 0);
    __delay_us(50); //delay 50us

    // Display clear
    SendLCD(0b00000001, 0);
    __delay_ms(5); //delay 5?s

    // Entry mode set with increase cursor
    SendLCD(0b00000110, 0);
    __delay_us(50); //delay 50us

    //Display on
    SendLCD(0b00001100, 0);
    __delay_us(100);


    
}

void SetLine(char line) { //function to put cursor to start of line
    //Send 0x80 to set line to 1 (0x00 ddram address)
    if (line == 1) {
        SendLCD(0x80, 0);
    } //Send 1000 0000 to set line to 1 (0000 0000 is ddram address)
    if (line == 2) {
        SendLCD(0xC0, 0);
    } //Send 1100 0000 to set line to 1 (0100 0000 is ddram address)   
    __delay_us(50); // 50us delay
}