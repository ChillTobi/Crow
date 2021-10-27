/*
 * led.h
 *
 * Created: 29.09.2021 19:45:45
 *  Author: Tobi
 */ 


#ifndef LED_H_
#define LED_H_

/****************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>



/****************************************************************/

/* user defineable */
#define NUMBER_OF_LED			2

/****************************************************************/

/* APA102 specific */
#define LED_POS_START		0
#define LED_POS_END			(NUMBER_OF_LED+LED_POS_START)

#define LED_DEFAULT_LENGTH	2
#define LED_DATA_SIZE		4
#define LED_REG_GENERAL		0
#define LED_REG_BLUE		1
#define LED_REG_GREEN		2
#define LED_REG_RED			3

#define LED_START_PATTERN		0x00
#define LED_END_PATTERN			0x00
#define LED_BIT_GENERAL			0xE0

#define LED_MAX_BRIGHTNESS		0x1F
#define LED_DEFAULT_BRIGHTNESS	2

#define LED_STATE_IDLE			0
#define LED_STATE_START_UPDATE	1
#define LED_STATE_UPDATE		2
#define LED_STATE_FINISHED		3
#define LED_STATE_ERROR			255


#define COLOR_ORANGE    0x000028E6
#define COLOR_BLOODRED  0x00000020
#define COLOR_PURPLE    0x002C0046


/****************************************************************/
typedef union _T_LED
{
	struct 
	{
		uint8_t	u8Brightness:8;
		uint8_t u8Blue:8;
		uint8_t u8Green:8;
		uint8_t u8Red:8;
	}RGB;
		
	uint8_t		u8Data[4];
	uint32_t	u32Led;
}T_LED;

/****************************************************************/

void LED_Init(void);
void LED_Task(void);
void LED_OffAll(void);
void LED_SetBrightness(uint8_t u8Led, uint8_t u8Brightness);
void LED_SetBrightnessAll(uint8_t u8Brightness);
void LED_SetColor(uint8_t u8Led, uint32_t u32Color);
void LED_SetColorAll(uint32_t u32Color);
void LED_Update(void);


/****************************************************************/

#endif /* LED_H_ */