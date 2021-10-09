/*
 * servo.c
 *
 * Created: 29.09.2021 19:46:10
 *  Author: Tobi
 */ 

#include "servo.h"


/****************************************************************/

static uint8_t u8Offset= 0;


/**
* SERVO_Init
*/
void SERVO_Init(void)
{
	/*****************************************/
	/* Time 0 - PWM Servo 
	* Output compare channel A, prescaler 64 */
	TCCR0A= (1<<COM0A1) | (1<<COM0A0) | (1<<WGM00) | (1<<WGM01);
	TCCR0B= (1<<CS02); 
	
	OCR0A= 0;
}


/**
* SERVO_SetPwm
* Set a 8Bit PWM value
*
* @u8Pwm	:	0..255
*/
void SERVO_SetPwm(uint8_t u8Pwm)
{
	OCR0A= u8Pwm;
}


/**
* SERVO_SetPwm
* Set a 7Bit offset value
*
* @u8Pwm	:	0..127
*/
void SERVO_SetOffset(uint8_t u8Value)
{
	/* Offset is saturated to avoid an overflow */
	u8Offset= u8Value & 127;
}