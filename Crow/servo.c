/*
 * servo.c
 *
 * Created: 29.09.2021 19:46:10
 *  Author: Tobi
 */ 

#include "servo.h"

/****************************************************************/

static uint8_t u8Offset= 0;
volatile static uint8_t u8PwmServo= 0;


/**
* SERVO_Init
*/
void SERVO_Init(void)
{
	/*****************************************/
	/* Time 0 - PWM Servo 
	* PWM, phase correct, Channel A, prescaler 256 => 244Hz */
	TCCR0A= (1<<COM0A1) | (1<<WGM00) | (1<<WGM01);
	TCCR0B= (1<<CS02); 
	
	OCR0A= 93;
}


/**
* SERVO_SetPwm
* Set a 8Bit PWM value
*
* @u8Pwm	:	0..255
*/
void SERVO_SetPwm(uint8_t u8Pwm)
{
	u8PwmServo= u8Pwm;
	OCR0A= u8PwmServo;
}


/**
* SERVO_SetAngle
* Set an angle value 0..180
*
* @u8Pwm	:	0..180
*/
void SERVO_SetAngle(uint8_t u8Angle)
{
	uint16_t u16Temp;
	
	if(u8Angle > 180)
	{
		u8Angle= 180;
	}
	
	u16Temp= u8Angle * SERVO_ANGLE_MULT;
	u16Temp= u16Temp / 100;
	
	u8PwmServo= (uint8_t)u16Temp + SERVO_MIN_ANGLE_VALUE;
	
	OCR0A= u8PwmServo;
}


/**
* SERVO_SetOffset
* Set a 7Bit offset value
*
* @u8Pwm	:	0..127
*/
void SERVO_SetOffset(uint8_t u8Value)
{
	/* Offset is saturated to avoid an overflow */
	u8Offset= u8Value & 127;
}