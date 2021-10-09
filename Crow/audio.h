/*
 * audio.h
 *
 * Created: 29.09.2021 21:11:32
 *  Author: Tobi
 */ 


#ifndef AUDIO_H_
#define AUDIO_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdbool.h>
#include <avr/pgmspace.h>

/****************************************************************/

#define AUDIO_SAMPLE_FREQ	(uint32_t)78125
#define AUDIO_BUFFER_SIZE	256

#define AUDIO_DDS_SIZE		1024

/****************************************************************/

typedef enum _E_AUDIO_MODE
{
	MODE_TONE=	0,
	MODE_FILE=	1
	
}E_AUDIO_MODE;


typedef enum _E_AUDIO_STATE
{
	STOP=	0,
	PLAY=	1,
	MUTE=	2
}E_AUDIO_STATE;


typedef enum _E_BUFFER
{
	EMPTY=	0,
	FILL_LOWER=	1,
	FILL_UPPER= 2,
	FILLED= 3		
}E_BUFFER;

/****************************************************************/

void AUDIO_Init(void);
void AUDIO_Task(void);

void AUDIO_Play(void);
void AUDIO_Stop(void);
void AUDIO_SetFile(uint8_t *ptrFile);
void AUDIO_SetState(E_AUDIO_STATE eState);
void AUDIO_SetMode(E_AUDIO_MODE eMode);
void ADUIO_SetDdsFrequency(uint16_t u16Freq);


/****************************************************************/







#endif /* AUDIO_H_ */