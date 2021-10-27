/*
 * led.c
 *
 * Created: 29.09.2021 19:45:54
 *  Author: Tobi
 */ 


#include "led.h"


/****************************************************************/

static void SPI_Transmit(uint8_t u8Data);
static void SPI_TransmitIT(uint8_t u8Data);

/****************************************************************/

volatile static uint8_t	u8NumOfLeds= LED_DEFAULT_LENGTH;
volatile static uint8_t	u8LedState= LED_STATE_IDLE;
volatile static uint8_t	u8LedIndex= 0;
volatile static uint8_t u8LedRegisterIndex= 0;

//volatile static T_LED	*stLed;
volatile static T_LED	stLed[NUMBER_OF_LED + LED_DEFAULT_LENGTH];
volatile uint8_t u8Debug;

/****************************************************************/

/**
* LED_Init
* Initializes the LED interface and allocates memory for up to 253 LEDs
*
* @param u8Num	:	Number of LEDs in a row 1..253
*/
void LED_Init(void)
{
	uint8_t u8Idx= 0;
	
	u8NumOfLeds= NUMBER_OF_LED + LED_DEFAULT_LENGTH;
		
	for(;u8Idx < u8NumOfLeds; ++u8Idx)
	{
		if((u8Idx > 0) && (u8Idx < (u8NumOfLeds-1)))
		{
			stLed[u8Idx].RGB.u8Brightness= LED_BIT_GENERAL;
		}
		else
		{
			stLed[u8Idx].RGB.u8Brightness= 0;
		}
		
		stLed[u8Idx].RGB.u8Blue= 0;
		stLed[u8Idx].RGB.u8Green= 0;
		stLed[u8Idx].RGB.u8Red= 0;
	}
	

	/*******************************************************
	Enable SPI in Master mode,
	CPOL High, CPHS High,
	Prescaler= 128 => 125kHz */
	SPCR= (1<<SPE) | (1<<DORD) | (1<<MSTR) |(1<<CPOL) |(1<<CPHA) | (1<<SPR1) | (1<<SPR0);
}


/**
* LED_Task
*/
void LED_Task(void)
{
	switch(u8LedState)
	{		
		case LED_STATE_IDLE:
		
		break;
		
		/*********************************************/
		case LED_STATE_START_UPDATE:
			u8LedIndex= 0;
			u8LedRegisterIndex= 0;
			SPI_TransmitIT(stLed[u8LedIndex].u8Data[u8LedRegisterIndex]);
		
			u8LedState= LED_STATE_UPDATE;
		break;
	
		/*********************************************/
		case LED_STATE_UPDATE:
		
		break;
		
		/*********************************************/
		case LED_STATE_FINISHED:
			
			u8LedState= LED_STATE_IDLE;
		break;
		
		/*********************************************/
		default:
			u8LedState= LED_STATE_IDLE;
		break;
		
	}	
}


/**
* LED_Update
* Start update of the LED data.
*/
void LED_Update(void)
{
	if(u8LedState == LED_STATE_IDLE)
	{
		u8LedState= LED_STATE_START_UPDATE;	
	}
}


/**
* LED_SetColor
* Set the color of specific LED
*
* @param	u8Led		:	Index of the LED
* @param	u32Color	:	24Bit BGR value
*/
void LED_SetColor(uint8_t u8Led, uint32_t u32Color)
{
	if(u8Led >= NUMBER_OF_LED)
	{
		u8Led= 	NUMBER_OF_LED-1;
	}
	
	stLed[u8Led+1].RGB.u8Blue= u32Color >> 16;
	stLed[u8Led+1].RGB.u8Green= (u32Color >> 8) & 0x00FF;
	stLed[u8Led+1].RGB.u8Red= u32Color & 0x000000FF;
}


/**
* LED_SetColorAll
* Set the color of all LEDs to same value
*
* @param	u32Color	:	24Bit BGR value
*/
void LED_SetColorAll(uint32_t u32Color)
{
	uint8_t u8Idx= 0;
	
	for(;u8Idx < NUMBER_OF_LED; ++u8Idx)
	{
		stLed[u8Idx+1].RGB.u8Blue= u32Color >> 16;
		stLed[u8Idx+1].RGB.u8Green= (u32Color >> 8) & 0x00FF;
		stLed[u8Idx+1].RGB.u8Red= u32Color & 0x000000FF;
	}
}


/**
* LED_SetBrightness
* Set the brightness of a specific LED
*
* @param u8Led			:	Index of the LED
* @param u8Brightness	:	0..31
*/
void LED_SetBrightness(uint8_t u8Led, uint8_t u8Brightness)
{
	if(u8Led >= NUMBER_OF_LED)
	{
		u8Led= 	NUMBER_OF_LED-1;
	}
		
	u8Brightness&= LED_MAX_BRIGHTNESS;
	stLed[u8Led+1].RGB.u8Brightness= u8Brightness | LED_BIT_GENERAL;
}


/**
* LED_SetBrightnessAll
* Set the brightness for all LEDs to the same value
*
* @param u8Brightness	:	0..31
*/
void LED_SetBrightnessAll(uint8_t u8Brightness)
{
	uint8_t u8Idx= 0;
	
	u8Brightness&= LED_MAX_BRIGHTNESS;
	
	for(;u8Idx < NUMBER_OF_LED; ++u8Idx)
	{
		/* offset for the start pattern, which is always 0 */
		stLed[u8Idx+1].RGB.u8Brightness= u8Brightness | LED_BIT_GENERAL;
	}
}


/**
* LED_OffAll
* Turns all LEDs off
*/
void LED_OffAll(void)
{
	uint8_t u8Idx= 0;
	
	for(;u8Idx < NUMBER_OF_LED; ++u8Idx)
	{
		stLed[u8Idx+1].RGB.u8Brightness= LED_BIT_GENERAL;
		stLed[u8Idx+1].RGB.u8Blue= 0;
		stLed[u8Idx+1].RGB.u8Green= 0;
		stLed[u8Idx+1].RGB.u8Red= 0;
	}
}



/**
* SPI_Transmit
* Transmit one byte of data.
*/
inline static void SPI_Transmit(uint8_t u8Data)
{
	SPDR= u8Data;
}


/**
* SPI_TransmitIT
* Transmit one byte of data with interrupt.
*/
inline static void SPI_TransmitIT(uint8_t u8Data)
{
	SPCR|= (1<<SPIE);
	SPDR= u8Data;
}


/**
* Interrupt service routine for SPI
*/
ISR(SPI_STC_vect)
{
	//volatile uint8_t *u8PtrTemp;
	volatile uint8_t u8Temp;
	
	++u8LedRegisterIndex;
	if(u8LedRegisterIndex > (LED_DATA_SIZE-1))
	{
		u8LedRegisterIndex= 0;
		++u8LedIndex;
	}
	
	if(u8LedIndex < u8NumOfLeds)
	{
		u8Temp= stLed[u8LedIndex].u8Data[u8LedRegisterIndex];
		SPI_TransmitIT(u8Temp);
	}
	else
	{
		u8LedState= LED_STATE_FINISHED;
	}
	
}