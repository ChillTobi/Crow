/*
 * crow.h
 *
 * Created: 02.10.2021 12:02:30
 *  Author: Tobi
 */ 


#ifndef CROW_H_
#define CROW_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdbool.h>

/****************************************************************/

#define CROW_HEAD_DEFAULT_POS		0
#define CROW_HEAD_RIGHT_MAX_POS		254
#define CROW_HEAD_LEFT_MAX_POS		1

#define CROW_EYE_BRIGHTNESS_MAX		2
#define CROW_EYE_BRIGHTNESS_DEFAULT	2
#define CROW_EYE_OFF_TIME_10MS		30
#define CROW_EYE_TIME_OFFSET		200

#define CROW_HEAD_TIME_OFFSET		500


typedef enum _E_CROW_MODE
{
	CROW_INIT= 0,
	CROW_RUN= 1,
	CROW_ERROR
}E_CROW_MODE;
	
typedef enum _E_EYE_MODE
{
	EYE_FLICKER=	0,
	EYE_BLINK=		1
}E_EYE_MODE;

typedef enum _E_COLOR_THEME
{
	RED= 0,
	PURPLE= 1
}_E_COLOR_THEME;


typedef enum _E_MODE
{
	HEAD_MOVE_RANDOM= 0,
	HEAD_MOVE_BNF= 1,
	HEAD_STAY= 3
}E_MODE;


/****************************************************************/

void CROW_Init(void);
void CROW_Task(void);


/****************************************************************/



#endif /* CROW_H_ */