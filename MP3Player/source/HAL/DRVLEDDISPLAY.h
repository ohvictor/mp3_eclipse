/*
 * DRVLEDDISPLAY.h
 *
 *      Author: Francisco Musich
 */

#ifndef DRVLEDDISPLAY_H_
#define DRVLEDDISPLAY_H_

#include "eDMA.h"
#include "FTM.h"
#include "PortConfig.h"


#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

/*	Definitions for Display dimentions	*/

#define LED_DISPLAY_HEIGHT 8  //Test
#define LED_DISPLAY_WIDTH 8

#define LED_QUANTITY LED_DISPLAY_HEIGHT*LED_DISPLAY_WIDTH

#define BITS_PER_CELL 24
#define BITS_PER_COLOR 8

/*	Defintitions for PWM output signal	*/


#define PWM_MOD 62  //Con prescaler setteado en 1->resolucion 20ns. 63*20ns = 1,26us es el periodo que necesita el ws2812

#define PWM_LOW 19 // 19/63 = 0.3 ->Duty cycle que indica un bit in =0
#define PWM_HIGH 37  // 37/63 = 0.58 -> Duty cycle que indica un bit in =1


/* Enumarations and data type definitions */

typedef struct RGB RGB_t;

struct RGB
{
	uint8_t red;
	uint8_t green;
	uint8_t blue;

};


typedef struct CELL CELL_t;

struct CELL
{
	RGB_t color;
	uint8_t brightness;
};

typedef enum {
	BUFFER_A,
	BUFFER_B
}PPBuffer_t;

typedef struct {
	int table[LED_DISPLAY_HEIGHT][LED_DISPLAY_WIDTH];
	float brightness;
	RGB_t rgb_color;
}MATRIX_BOARD_t;

/*	Global functions declaration	*/
void LED_DISPLAY_init(); //Sets up FTM and DMA operation modes and their outputs.

void LED_DISPLAY_updateMatrix(MATRIX_BOARD_t modelMatrix);  //Updates display

//Setters
void LED_DISPLAY_setDotColor(RGB_t newColor);

void LED_DISPLAY_setBrightness(float newBri);


uint8_t LED_DISPLAY_getBrightness();


RGB_t LED_DISPLAY_getDotColor();


#endif /* DRVLEDDISPLAY_H_ */
