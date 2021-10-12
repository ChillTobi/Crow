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
	SYS_Init();

	LED_Init();
	SERVO_Init();
	AUDIO_Init();
	
	CROW_Init();
	
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
	/* 
	PB5:	SCK
	PB3:	MOSI
	PB2:	SS (must be activated for SPI Master mode)
	PB1:	OC1A (Audio-DAC)
	PB0:	Debug-I/O
	*/
	DDRB= 0x2F;
	PORTB= 0;
	
	/*
	PD6:	PWM
	*/
	DDRD= 0x40;
	PORTD= 0;
	/*****************************************/
	
	
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








