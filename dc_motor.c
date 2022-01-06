#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include "dc_motor.h"

void initPWM(void) { // function to setup PWM


    // Set up the PWM duty cycles
    PDC0L = 0; //duty cycle for PWM1 low bits
    PDC0H = 0; // Same for high bits
    PDC1L = 0; //duty cycle for PWM3 low bits
    PDC1H = 0; // Same for high bits
   

    PTCON0 = 0b00000000; // free running mode, 1:1 
    PTCON1 = 0b10000000; // enable PWM timer
    PWMCON0 = 0b01101111; // PWM pins 0-3 are set as outputs, all independent mode
    PWMCON1 = 0x00; // special features, all 0 (default)
    /* To calculate PTPER and then split it in PTPERH and PTPERL, we need to
     * first take into account the Tpwm equation. Therefore, use the following:
     * Fosc = 8 MHz, PTMRPS = 1 (given in PTCON0) and hence the result is
     * PTPER = 199 = 1001110000. Split this as follows */

    PTPERL = 0b11000111; // base PWM period low byte
    PTPERH = 0b00000000; // base PWM period high byte
    //note: the two LSBs in PDC0L have a different function;
    //this mean to you need to shift your on period to the left <<
    //by 2 bits


    /* To get 50% duty cycle, Ton should be exactly half of the value of Tpwm
     * So if for Tpwm (PTPER+1)=200, then (PTMR+1) =100. This means PTMR is 99
     * 99 = 0b01100011. This is bitshifted to the left by two for PDC0 and PDC1*/

}

void setMotorPWM(struct DC_motor *m) {

    int PWMduty; //tmp variable to store PWM duty cycle
    PWMduty = ((int) (m->power)*(m->PWMperiod)) / 100;

    if (m->direction) { //if forward
        // low time increases with power
        PWMduty = m->PWMperiod - PWMduty;
        LATB = LATB | (1 << (m->dir_pin)); // set dir_pin bit in LATB to high
    } else { //if reverse
        LATB = LATB & (~(1 << (m->dir_pin))); // set dir_pin bit in LATB to low
        // high time increases with power

    }
    PWMduty = (PWMduty << 2); // two LSBs are reserved for other things
    *(m->dutyLowByte) = PWMduty & 0xFF; //set low duty cycle byte
    *(m->dutyHighByte) = (PWMduty >> 8) & 0x3F; //set high duty cycle byte
}

void stop(struct DC_motor *m_L, struct DC_motor *m_R) {
    //Command to stop both motors on the spot
    
    m_L->power = 0;
    m_R->power = 0;

    setMotorPWM(m_R);
    setMotorPWM(m_L);
    __delay_ms(50);
    
}

//function to make the robot turn left 

int turnLeft(struct DC_motor *m_L, struct DC_motor *m_R) {
    //Command to turn the robot left on the spot
    
    int QEIPOSR = 0;
    int angle = 0;
    //QEICON = 0b01111011;
    
    m_L->direction = 0; // 0 = left track reverse
    m_R->direction = 1; //1 = right track forward
    /*
        m_L->power = 70;
        m_R->power = 70;
        setMotorPWM(m_R);
        setMotorPWM(m_L);
     */
    while ((m_L->power) < 73 || (m_R->power) < 70) { //This while loop ensures the changes in motor speeds
        //are smooth and incremental instead of instant
        (m_L->power)++;
        (m_R->power)++;
        setMotorPWM(m_L);
        setMotorPWM(m_R);
    }
    setMotorPWM(m_L);
    setMotorPWM(m_R);
    
    QEIPOSR = POSCNTL; //8 LSB of signal
    QEIPOSR += ((unsigned int) POSCNTH << 8); //Shift 8 POSCNTH bits and add them
    //angle = QEIPOSR/4;
    //return angle;
  //  QEICON = 0b01100011;

}

//function to make the robot turn right 

int turnRight(struct DC_motor *m_L, struct DC_motor *m_R) {
    //Command to turn the robot right on the spot
    
    int QEIPOSR = 0;
    int angle=0;
    //QEICON = 0b01111011;
    
    m_L->direction = 1; // 1 = left track forward
    m_R->direction = 0; //0 = right track reverse
    /*
        m_L->power = 70;
        m_R->power = 70;
        setMotorPWM(m_R);
        setMotorPWM(m_L);
     */
    while ((m_L->power) < 73 || (m_R->power) < 70) { //This while loop ensures the changes in motor speeds
        //are smooth and incremental instead of instant
        (m_L->power)++;
        (m_R->power)++;
        setMotorPWM(m_L);
        setMotorPWM(m_R);
    }
    setMotorPWM(m_L);
    setMotorPWM(m_R);
    
    QEIPOSR = POSCNTL; //8 LSB of signal
    QEIPOSR += ((unsigned int) POSCNTH << 8); //Shift 8 POSCNTH bits and add them
    //angle = 314*QEIPOSR/4;
    //return QEIPOSR;
    //QEICON = 0b01100011;

}

//function to make the robot go straight

int fullSpeedAhead(struct DC_motor *m_L, struct DC_motor *m_R) {
    
    int QEIPOSR = 0;
   // QEICON = 0b01111011;
    //This command makes the robot move forward
    //Both motors spin in the same direction, forward (direction 0)
    
    m_L->direction = 1; // 1= left track forward
    m_R->direction = 1; //1 = right track forward
   
    while ((m_L->power) < 74 || (m_R->power) < 70) { //This while loop ensures the changes in motor speeds
        //are smooth and incremental instead of instant
        
        (m_L->power)++;
        (m_R->power)++;
        setMotorPWM(m_L);
        setMotorPWM(m_R);

    }
    setMotorPWM(m_L);
    setMotorPWM(m_R);
    
    QEIPOSR = POSCNTL; //8 LSB of signal
    QEIPOSR += ((unsigned int) POSCNTH << 8); //Shift 8 POSCNTH bits and add them
    return QEIPOSR;
    //QEICON = 0b01100011;
}

//function to make the robot go straight

int fullSpeedReturn(struct DC_motor *m_L, struct DC_motor *m_R) {
    
    int QEIPOSR = 0;
   // QEICON = 0b01111011;
    //This command makes the robot move forward
    //Both motors spin in the same direction, forward (direction 0)
    
    m_L->direction = 0; // -1= left track backward
    m_R->direction = 0; //-1 = right track backward
   
    while ((m_L->power) < 78 || (m_R->power) < 76) { //This while loop ensures the changes in motor speeds
        //are smooth and incremental instead of instant
        
        (m_L->power)++;
        (m_R->power)++;
        setMotorPWM(m_L);
        setMotorPWM(m_R);

    }
    setMotorPWM(m_L);
    setMotorPWM(m_R);
    
    QEIPOSR = POSCNTL; //8 LSB of signal
    QEIPOSR += ((unsigned int) POSCNTH << 8); //Shift 8 POSCNTH bits and add them
    return QEIPOSR;
    //QEICON = 0b01100011;
}

void MotorSetup(void) { //This function sets up the motor pins


    TRISBbits.TRISB0 = 0; //Set pins to output
    TRISBbits.TRISB2 = 0;
    LATBbits.LB0 = 1; //Set pins to high
    LATBbits.LB2 = 1;


}