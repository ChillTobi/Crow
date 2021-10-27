/*
 * dio.c
 *
 * Created: 24.10.2021 11:21:37
 *  Author: Tobi
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>

#include "dio.h"

/****************************************************************/



/****************************************************************/

/**
* DIO_Init
*/
void DIO_Init(void)
{
	/*****************************************/
	
	/* 
	PC0:	
	PC1:	
	PC2:	
	PC3:	
	*/
	DDRC= 0xF0;
	PORTC= 0x0F;
	
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
}



/**
* DIO_GetMode
* Returns the state of mode switches
*/
uint8_t DIO_GetMode(void)
{
	return ((~PINC) & 0x0F);
}