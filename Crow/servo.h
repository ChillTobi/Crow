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

void SERVO_Init(void);
void SERVO_SetPwm(uint8_t u8Pwm);
void SERVO_SetOffset(uint8_t u8Value);


/****************************************************************/


#endif /* SERVO_H_ */