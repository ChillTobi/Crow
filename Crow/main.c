/*
 * Crow.c
 *
 * Created: 26.09.2021 18:20:34
 * Author : Tobi
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdbool.h>


#include "dio.h"
#include "servo.h"
#include "led.h"
#include "audio.h"
#include "crow.h"

/****************************************************************/
/* Number of LEDs in a row */


/****************************************************************/

void SYS_Init(void);


/****************************************************************/

volatile static uint8_t	u8Tick10ms= 0;
volatile static uint8_t	u16Tick1s= 0;
volatile static uint16_t u16WaitCounter= 0;


/****************************************************************/


/**
* MAIN
*/
int main(void)
{
	
	/*********************************************/
	DIO_Init();
	LED_Init();
	SERVO_Init();
	AUDIO_Init();
	
	CROW_Init();
	
	SYS_Init();
	
	/*********************************************/
    /* Replace with your application code */
    while (1) 
    {
		/* 10ms Timer */
		if(u8Tick10ms >= 1)
		{
			u8Tick10ms= 0;
			++u16Tick1s;
			
			CROW_Task();
			LED_Task();
		}
		
	
		/* 1s Timer */
		if(u16Tick1s >= 100)
		{
			u16Tick1s= 0;	
		}
		
		AUDIO_Task();	
    }
}



/**
* SYS_Init
* General system initialization
*/
void SYS_Init(void)
{
	cli();
	
	/*****************************************/
	/* Timer 1 - Timebase 1ms */
	TCCR2A= (1<<WGM21);
	TCCR2B= (1<<CS22);
	
	/* 100Hz */
	OCR2A= 249;
	
	/* Interrupt on compare match */
	TIMSK2= (1<<OCIE2A);
		
	/*****************************************/
	
	sei();
}



/**
* Timer2 interrupt service routine for system base time
*/
ISR(TIMER2_COMPA_vect)
{
	static uint8_t u8Tick1ms= 0;
	
	++u8Tick1ms;	
	if(u8Tick1ms >= 10)
	{
		u8Tick1ms= 0;
		++u8Tick10ms;
	}
}








