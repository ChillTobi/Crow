/*
 * crow.c
 *
 * Created: 02.10.2021 12:02:38
 *  Author: Tobi
 */ 


#include "crow.h"
#include "dio.h"
#include "led.h"
#include "servo.h"
#include "audio.h"
#include "crow_audio.h"


/****************************************************************/

static E_CROW_MODE	eCrowMode= CROW_INIT;
static E_MODE		eHead= HEAD_STAY;
static uint8_t		u8HeadPosition= 0;
static uint16_t		u16HeadTimer= 0;

static _E_COLOR_THEME eEyeColor= RED;
static E_EYE_MODE	eEye= EYE_BLINK;
static uint8_t		u8NumEyes= 2;
static uint16_t		u16EyeTimer= 0;
static uint16_t		u16EyeBlinkTimer= 0;
static uint32_t		u32EyeColor= COLOR_BLOODRED;

static uint16_t		u16FreqSweep= 1;
static bool			bFlagUp= true;

/****************************************************************/


/**
* CROW_Init
*/
void CROW_Init(void)
{
	u16EyeTimer= 10;
	eCrowMode= CROW_INIT;
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
	
	static uint16_t u16Temp= 0;

	/*********************************************/
	
	u8Temp= DIO_GetMode();
	
	eHead= u8Temp & 0x03;
	eEye= (u8Temp & 0x04) >> 2;
	eEyeColor= (u8Temp & 0x08) >> 3;
	
	switch(eCrowMode)
	{
		case CROW_INIT:
		if (u16EyeTimer > 0)
		{
			--u16EyeTimer;
			LED_SetBrightnessAll(0);
			LED_SetColorAll(0);
			LED_Update();
			
			SERVO_SetAngle(45);
		}
		else
		{
			LED_SetBrightnessAll(CROW_EYE_BRIGHTNESS_DEFAULT);
			LED_SetColorAll(u32EyeColor);
			
			AUDIO_SetMode(MODE_FILE);
			//AUDIO_SetFile(&u8CrowAudio[0]);
			//ADUIO_SetDdsFrequency(u16FreqSweep);
			//AUDIO_Play();
			
			eCrowMode= CROW_RUN;
		}
		
		break;
		
		/*********************************************/
		case CROW_RUN:
		
			/*ADUIO_SetDdsFrequency(u16FreqSweep);
		
			if(bFlagUp == true)
			{
				++u16FreqSweep;
				if(u16FreqSweep > 3500)
				{
					bFlagUp= false;
				}
			}
			else
			{
				--u16FreqSweep;
				if (u16FreqSweep < 1)
				{
					bFlagUp= true;
				}
			}*/
			
			
			/************************************************************************/
			/* Head movement and eye blink and color  */
			
			if(eEyeColor == RED)
			{
				u32EyeColor= COLOR_BLOODRED;
			}
			else
			{
				u32EyeColor= COLOR_PURPLE;
			}
			
			LED_SetColorAll(u32EyeColor);
			

			switch(eEye)
			{
				/*********************************************/
				case EYE_FLICKER:
		
				/* generate flickering LEDs */
				for(u8Idx= 0; u8Idx < u8NumEyes; ++u8Idx)
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
						LED_SetBrightnessAll(CROW_EYE_BRIGHTNESS_DEFAULT);
					}
				
					if(u16EyeBlinkTimer > 0)
					{
						--u16EyeBlinkTimer;
					}
					else
					{
						/* generate a random number between 0 and 511, offset is 2sec thus the time between blink is
						between 2sec and 7.1s */
						u16EyeBlinkTimer= rand() % 0x1FF + CROW_EYE_TIME_OFFSET;
						u16EyeTimer= CROW_EYE_OFF_TIME_10MS;
					}

				break;
					
				/*********************************************/
				default:
				break;
			}
	
			LED_Update();
		
			/******************************************************************************************************/
			/* random head movement or put in a defined position for mechanical adjustment */
			switch(eHead)
			{
				/********************************************************/
				case HEAD_MOVE_RANDOM:
			
					if(u16HeadTimer > 0)
					{
						--u16HeadTimer;
					}
					else
					{
						/* generate a random number between 0 and 1023, offset is 5sec thus the time between head movement is
						between 5sec and 15.2s */
						u16HeadTimer= rand() % 0x03FF + CROW_HEAD_TIME_OFFSET;
		
						/* the head default position is 45° equals locking in forward direction 
						the first generated random is for the coarse head position (left, middle, right) while the second one 
						is for the absolute position */
						u8HeadPosition= ((int8_t)(rand() % 3) - 1) * (rand() % 45) + 45;
					}
			
				break;
			
				/********************************************************/
				case HEAD_MOVE_BNF:
				
					/* Turn head from one maximum to other maximum and vice versa */
					u8HeadPosition= u16Temp / 10;
					if(bFlagUp == true)
					{
						++u16Temp;
					
						if(u8HeadPosition >= 90)
						{
							bFlagUp= false;
						}
					}
					else
					{
						--u16Temp;
						if(u8HeadPosition == 0)
						{
							bFlagUp= true;
						}
					}

				break;
			
				/********************************************************/
				default:
				case HEAD_STAY:
					u8HeadPosition= 45;
				break;
	
			}

			SERVO_SetAngle(u8HeadPosition);
			
		break;
	
		/******************************************************/
		default:
		break;
		
	}
	
	
}