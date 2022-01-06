#ifndef _DC_MOTOR_H
#define _DC_MOTOR_H

#define _XTAL_FREQ 8000000

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>

struct DC_motor {
    char power; //motor power, out of 100
    char direction; //motor direction, forward(1), reverse(0)
    unsigned char *dutyLowByte; //PWM duty low byte address
    unsigned char *dutyHighByte; //PWM duty high byte address
    char dir_pin; // pin that controls direction on PORTB
    int PWMperiod; //base period of PWM cycle
};

void initPWM(void);
void setMotorPWM(struct DC_motor *m);
void stop(struct DC_motor *m_L, struct DC_motor *m_R);
int turnLeft(struct DC_motor *m_L, struct DC_motor *m_R);
int turnRight(struct DC_motor *m_L, struct DC_motor *m_R);
int fullSpeedAhead(struct DC_motor *m_L, struct DC_motor *m_R);
int fullSpeedReturn (struct DC_motor *m_L, struct DC_motor *m_R);
void MotorSetup();

#endif