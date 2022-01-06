#ifndef LCD_H
#define	LCD_H

#include <xc.h>

#define _XTAL_FREQ 8000000 //define _XTAL_FREQ so delay routines work

#define LCD_RS PORTAbits.RA6
#define LCD_E PORTCbits.RC0
#define LCD_DB7 PORTDbits.RD1
#define LCD_DB6 PORTDbits.RD0
#define LCD_DB5 PORTCbits.RC2
#define LCD_DB4 PORTCbits.RC1


void E_TOG(void);
void LCDout(unsigned char number);
void SendLCD(unsigned char Byte, char type);
void LCD_String(char *string);
void LCD_Init(void);
void SetLine(char line);


#endif	/* LCD_H */

