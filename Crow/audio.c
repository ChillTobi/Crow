/*
 * audio.c
 *
 * Created: 29.09.2021 21:11:21
 *  Author: Tobi
 */ 


#include "audio.h"
#include <avr/pgmspace.h>


/****************************************************************/
#include "crow_audio.h"

/****************************************************************/
static E_AUDIO_MODE	eAudioMode= MODE_TONE;
static E_AUDIO_STATE eAudioState= STOP;

/* pointer to the audio data file in none DDS mode */
static uint8_t*	ptr8AudioFile;
/* buffer sample pointer */
static uint8_t u8AudioPtr= 0;

/* overall size of the audio file */
volatile static uint16_t u16AudioFileSize= 0;
/* current time step in n/f_sample */
volatile static uint16_t u16AudioTimeStep= 0;

volatile static uint16_t u16AudioTimeStepBuffer= 0;
/* current buffer state */
volatile static E_BUFFER eBufferState= EMPTY;
/* temporary sample memory */
volatile static uint8_t u8Buffer[AUDIO_BUFFER_SIZE]= {0};
	

/****************************************************************/
/* DDS related for simple frequency generation 
* The resolution is 10Bit 
* 
* f_tone= n/N * f_sample = n/1024 * 7.8125kHz 
*/
volatile static uint16_t u16Phase= 0;
volatile static uint16_t u16PhaseAccu= 0;
PROGMEM const uint8_t u8SineWave[256]= {
	128,131,134,137,140,143,146,149,153,156,159,162,165,168,171,174,177,180,182,185,188,191,194,196,199,201,204,207,209,211,214,216,218,
	220,223,225,227,229,231,232,234,236,238,239,241,242,243,245,246,247,248,249,250,251,252,253,253,254,254,255,255,255,255,255,255,255,
	255,255,255,254,254,253,253,252,251,251,250,249,248,247,245,244,243,241,240,238,237,235,233,232,230,228,226,224,222,219,217,215,213,
	210,208,205,203,200,198,195,192,189,187,184,181,178,175,172,169,166,163,160,157,154,151,148,145,142,139,135,132,129,126,123,120,116,
	113,110,107,104,101,98,95,92,89,86,83,80,77,74,71,68,66,63,60,57,55,52,50,47,45,42,40,38,36,33,31,29,27,25,23,22,20,18,17,15,14,12,
	11,10,8,7,6,5,4,4,3,2,2,1,1,0,0,0,0,0,0,0,0,0,0,1,1,2,2,3,4,5,6,7,8,9,10,12,13,14,16,17,19,21,23,24,26,28,30,32,35,37,39,41,44,46,
	48,51,54,56,59,61,64,67,70,73,75,78,81,84,87,90,93,96,99,102,106,109,112,115,118,121,124,128};

/****************************************************************/
static uint8_t AUDIO_DDS(uint16_t u16TempPhase);
static void ADUIO_ClearBuffer(void);
/****************************************************************/

/**
* AUDIO_Init
*/
void AUDIO_Init(void)
{
	uint16_t u16Idx= 0;
	
	ptr8AudioFile= &u8CrowAudio[0];
	u16AudioFileSize= sizeof(u8CrowAudio);
	
	for(;u16Idx < AUDIO_BUFFER_SIZE; ++u16Idx)
	{
		u8Buffer[u16Idx]= 128;
	}
	
	/*****************************************/
	/* Timer 1 - Frequency 7.8125kHz  */
	TCCR1A= (1<<WGM10) | (1<<COM1A0) | (1<<COM1A1);
	TCCR1B= (1<<WGM12) | (1<<CS11);
	TCCR1C= 0;
	
	OCR1A= 0;
	
	/* Interrupt on overflow*/
	//TIMSK1= (1<<TOIE1);
}



/**
* AUDIO_Task
*/
void AUDIO_Task(void)
{
	volatile uint16_t u8Idx= 0;
	uint16_t u16Temp= 0;
	
	/*********************************************/
	switch (eAudioState)
	{
		case STOP:
		
			if(eBufferState != EMPTY)
			{
				AUDIO_Stop();
			}

		break;
		
		/*************************************************************************/
		case PLAY:
		
			switch(eAudioMode)
			{
				/*************************************************************************/	
				case MODE_TONE:
		
					/* write lower half of the buffer */
					if(eBufferState == FILL_LOWER)
					{
						for(u8Idx= 0; u8Idx < ((AUDIO_BUFFER_SIZE/2)); ++u8Idx)
						{
							u8Buffer[u8Idx]= AUDIO_DDS(u16Phase);
						}
						
						eBufferState= FILLED;
					}
					/* write upper half of the buffer */
					else if(eBufferState == FILL_UPPER)
					{
						for(u8Idx= AUDIO_BUFFER_SIZE/2; u8Idx < AUDIO_BUFFER_SIZE; ++u8Idx)
						{
							u8Buffer[u8Idx]= AUDIO_DDS(u16Phase);
						}
						
						eBufferState= FILLED;
					}
					else
					{
						;
					}
		
				break;
				
				/*************************************************************************/				
				case MODE_FILE:
	
					/* write lower half of the buffer */
					if(eBufferState == FILL_LOWER)
					{
						u16Temp= u16AudioTimeStepBuffer + (AUDIO_BUFFER_SIZE/2);
						
						for(u8Idx= 0; u8Idx < (AUDIO_BUFFER_SIZE/2); ++u8Idx)
						{
							u8Buffer[u8Idx]= pgm_read_byte(&ptr8AudioFile[u16Temp+u8Idx]);
						}
						
						eBufferState= FILLED;
					}
					/* write upper half of the buffer */
					else if(eBufferState == FILL_UPPER)
					{
						u16Temp= u16AudioTimeStepBuffer + (AUDIO_BUFFER_SIZE/2);
						
						for(u8Idx= AUDIO_BUFFER_SIZE/2; u8Idx < AUDIO_BUFFER_SIZE; ++u8Idx)
						{
							u8Buffer[u8Idx]= pgm_read_byte(&ptr8AudioFile[u16Temp+u8Idx]);
						}
						
						eBufferState= FILLED;
					}
					else
					{
						;
					}
		
				break;
		

				/*************************************************************************/
				default:
		
				break;
			}
	
		/*************************************************************************/
		default:
		
		break;		
			
	}
}


/**
* AUDIO_SetMode
* Set the current audio mode to either play a file or just to generate a tone.
*
* @param	eMode	:	MODE_FILE,
*						MODE_TONE
*/
void AUDIO_SetMode(E_AUDIO_MODE eMode)
{
	eAudioMode= eMode;
}


/**
* AUDIO_SetState
* Set the player state
*
* @param	eState	:	STOP,
*						PLAY,
*						MUTE
*/
void AUDIO_SetState(E_AUDIO_STATE eState)
{
	eAudioState= eState;
}


/**
* AUDIO_Play
*/
void AUDIO_Play(void)
{
	u8AudioPtr= 0;
	eAudioState= PLAY;
	eBufferState= FILL_UPPER;
	
	OCR1A= 0;
	TCNT1= 0;
	TCCR1B|= (1<<CS11);
	TIMSK1|= (1<<TOIE1);
}


/**
* AUDIO_Stop
*/
void AUDIO_Stop(void)
{
	eAudioState= STOP;
	TCCR1B&= ~(1<<CS11);
	TIMSK1&= ~(1<<TOIE1);
	
	ADUIO_ClearBuffer();
}


/**
* ADUIO_SetToneFrequency
*/
void ADUIO_SetDdsFrequency(uint16_t u16Freq)
{
	volatile uint32_t u32Temp= 0;
	u32Temp= ((uint32_t)u16Freq * (uint32_t)10 * (uint32_t)AUDIO_DDS_SIZE);
	u16Phase= (uint16_t)(u32Temp / (uint32_t)AUDIO_SAMPLE_FREQ);
}



/**
* AUDIO_DDS
* Generate a sine tone with DDS.
*
* @param u16Phase	:	Phase width 0..511
* @retval Sine value
*/
static uint8_t AUDIO_DDS(uint16_t u16TempPhase)
{
	volatile uint8_t u8Temp;
	uint8_t u8Table;
	
	u16TempPhase&= (AUDIO_DDS_SIZE/2)-1;
	
	u16PhaseAccu+= u16TempPhase;
	u16PhaseAccu&= (AUDIO_DDS_SIZE-1);
	u8Temp= u16PhaseAccu >> 2;
	
	u8Table= pgm_read_byte(&u8SineWave[u8Temp]);
	return u8Table;
}


/**
* AUDIO_SetFile
* Set another audio file, the Buffer will be preloaded with the new audio file.
*
* @param ptrFile	:	Address of the new file
*/
void AUDIO_SetFile(uint8_t *ptrFile)
{
	uint16_t u16Idx= 0;
	
	ptr8AudioFile= ptrFile;
	u16AudioFileSize= sizeof(ptr8AudioFile) + AUDIO_BUFFER_SIZE;
	
	u8AudioPtr= 0;
	u16AudioTimeStep= 0;
	
	/* play nothing at first, this is  */
	for(;u16Idx < (AUDIO_BUFFER_SIZE/2); ++u16Idx)
	{
		u8Buffer[u16Idx]= AUDIO_RESOLUTION/2;
	}
	
	for(;u16Idx < AUDIO_BUFFER_SIZE; ++u16Idx)
	{
		u8Buffer[u16Idx]= pgm_read_byte(ptr8AudioFile);
		++ptr8AudioFile;
	}
}


/**
* ADUIO_ClearBuffer
*/
static void ADUIO_ClearBuffer(void)
{
	uint16_t u16Idx= 0;
	
	u8AudioPtr= 0;
	u16AudioTimeStep= 0;
	
	for(;u16Idx < AUDIO_BUFFER_SIZE; ++u16Idx)
	{
		u8Buffer[u16Idx]= 0;
	}
	
	eBufferState= EMPTY;
}


/**
* Interrupt service routine for Timer2
*/
ISR(TIMER1_OVF_vect)
{
	OCR1A= u8Buffer[u8AudioPtr];
	
	if(eAudioMode == MODE_FILE)
	{
		/* Audio file sample counter */
		++u16AudioTimeStep;
		if(u16AudioTimeStep < u16AudioFileSize)
		{
			++u8AudioPtr;	
		}
		else
		{
			eAudioState= STOP;
		}
	}

	++u8AudioPtr;
		
	/* Fill half of the buffer, depending if reading from the upper or lower half,
		the respective other half will be filled */
	if(u8AudioPtr == 0)
	{
		eBufferState= FILL_UPPER;
		u16AudioTimeStepBuffer= u16AudioTimeStep;
	}
	else if(u8AudioPtr == (AUDIO_BUFFER_SIZE/2))
	{
		eBufferState= FILL_LOWER;
		u16AudioTimeStepBuffer= u16AudioTimeStep;
	}
	else
	{
		eBufferState= FILLED;
	}
	
}