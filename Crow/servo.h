/*
 * servo.h
 *
 * Created: 29.09.2021 19:46:02
 *  Author: Tobi
 */ 


#ifndef SERVO_H_
#define SERVO_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdbool.h>

/****************************************************************/

#define SERVO_MIN_ANGLE_VALUE	63
#define SERVO_MAX_ANGLE_VALUE	125
#define SERVO_MID_ANGLE_VALUE	93

/* (125-62)/180 * 100 = 35 */
#define SERVO_ANGLE_MULT		68
/****************************************************************/

void SERVO_Init(void);
void SERVO_SetPwm(uint8_t u8Pwm);
void SERVO_SetOffset(uint8_t u8Value);
void SERVO_SetAngle(uint8_t u8Angle);

/****************************************************************/


#endif /* SERVO_H_ */