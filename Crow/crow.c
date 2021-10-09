/*
 * crow.c
 *
 * Created: 02.10.2021 12:02:38
 *  Author: Tobi
 */ 


#include "crow.h"
#include "led.h"
#include "servo.h"
#include "audio.h"


/****************************************************************/
static uint16_t u16EyeTimer= 0;
static uint16_t u16HeadTimer= 0;
static uint16_t u16BreakTimer= 0;

static E_CROW_MODE eCrowMode= 0;

volatile static uint8_t u8HeadPosition= 0;

static uint8_t		u8NumEyes= 2;
static E_EYE_MODE	eEye= EYE_BLINK;
static uint8_t		u8EyeBrightness= 0;

static uint32_t		u32EyeColor= COLOR_BLOODRED;

/****************************************************************/


/**
* CROW_Init
*/
void CROW_Init(void)
{
	u16BreakTimer= 10;
	eCrowMode= CROW_INIT;
	
	SERVO_SetPwm(127);
}




/**
* CROW_Task
* Head movement and eye blink control
* Call time base is 10ms
*/
void CROW_Task(void)
{
	/*********************************************/
	uint8_t u8Temp= 0;
	uint8_t u8Idx= 0;

	/*********************************************/
	switch(eCrowMode)
	{
		case CROW_INIT:
		if (u16BreakTimer > 0)
		{
			--u16BreakTimer;
			LED_SetBrightnessAll(0);
			LED_SetColorAll(0);
			LED_Update();
			
		}
		else
		{
			LED_SetBrightnessAll(CROW_EYE_BRIGHTNESS_DEFAULT);
			LED_SetColorAll(u32EyeColor);
			
			AUDIO_SetMode(MODE_TONE);
			ADUIO_SetDdsFrequency(100);
			AUDIO_Play();
			
			eCrowMode= CROW_RUN;
		}
		
		break;
		
		/*********************************************/
		case CROW_RUN:
			switch(eEye)
			{
				/*********************************************/
				case EYE_FLICKER:
		
				/* generate flickering LEDs */
				for(;u8Idx < u8NumEyes; ++u8Idx)
				{
					u8Temp= (rand() % CROW_EYE_BRIGHTNESS_MAX) + CROW_EYE_BRIGHTNESS_DEFAULT;
					LED_SetBrightness(u8Idx, u8Temp);
				}
		
		
				break;
					
				/*********************************************/
				case EYE_BLINK:
		
				if(u16EyeTimer > 0)
				{
					LED_OffAll();
					--u16EyeTimer;
				}
				else
				{
					LED_SetColorAll(u32EyeColor);
					LED_SetBrightnessAll(CROW_EYE_BRIGHTNESS_DEFAULT);
				}

				LED_Update();
			
				break;
					
				/*********************************************/
				default:
				break;
			}
	
	
			/******************************************************/
			/* Head movement and eye blink */

			if(u16BreakTimer > 0)
			{
				--u16BreakTimer;
			}
			else
			{
				/* generate a random number between 0 and 511, offset is 2sec thus the time between blink is
				between 2sec and 7.1s */
				u16BreakTimer= rand() % 0x1FF + CROW_EYE_TIME_OFFSET;
				u16EyeTimer= CROW_EYE_OFF_TIME_10MS;
			}
	
	
			if(u16HeadTimer > 0)
			{
				--u16HeadTimer;
			}
			else
			{
				/* generate a random number between 0 and 1023, offset is 3sec thus the time between blink is
				between 3sec and 13.2s */
				u16HeadTimer= rand() % 0x02FF + CROW_HEAD_TIME_OFFSET;
		
				/* the head default position is 127 equals locking in forward direction 
				the first generated random is for the coarse head position (left, middle, right) while the second one 
				is for the absolute position */
				u8HeadPosition= ((rand() % 3) - 1) * (rand() % 100) + 127;
				SERVO_SetPwm(u8HeadPosition);
			}
			
		break;
	
		/******************************************************/
		default:
		break;
		
	}
	
	
}