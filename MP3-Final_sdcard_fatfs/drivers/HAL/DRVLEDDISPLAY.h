/*******************************************************************************
  @file     DRVLEDDISPLAY.h
  @brief    LED Matrix driver
  @author   Victor Oh
 ******************************************************************************/

#ifndef DRVLEDDISPLAY_H_
#define DRVLEDDISPLAY_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "MK64F12.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define LED_ROWS	8
#define	LED_COLS	8
#define	LED_N		(LED_ROWS*LED_COLS)

#define COLOR_BITS	8
#define LED_BITS	(COLOR_BITS*3)


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef struct{
	uint8_t r;
	uint8_t g;
	uint8_t b;
}RGB_t;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

typedef struct {
	RGB_t table[LED_ROWS][LED_COLS];
}MATRIX_BOARD_t;

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

void LED_DISPLAY_init(); //Sets up FTM and DMA operation modes and their outputs.

void LED_DISPLAY_updateMatrix(MATRIX_BOARD_t modelMatrix);  //Updates display

/*******************************************************************************
 ******************************************************************************/


#endif /* DRVLEDDISPLAY_H_ */
