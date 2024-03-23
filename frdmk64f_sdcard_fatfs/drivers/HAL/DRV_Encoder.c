/***************************************************************************//**
  @file     DRV_Encoder.c
  @brief    Rotary encoder driver
  @author   Grupo 1
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "DRV_Board.h"
#include "DRV_Encoder.h"
#include "PDRV_GPIO.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

// Definiciones de pines del encoder
#define PIN_A 		PORTNUM2PIN(PC,3)
#define PIN_B 		PORTNUM2PIN(PC,2)

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void encoder_callback(void);

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

// Data de encoder
static bool status;
static encoderResult_t encoder_event;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void initEncoder() {
    // Inicializo Data
    encoder_event = ENC_NONE;
    status = false;

    // Seteo los Pines
	gpioMode(PIN_A, INPUT);
    gpioIRQ(PIN_A, GPIO_IRQ_MODE_FALLING_EDGE, encoder_callback);
	gpioMode(PIN_B, INPUT);
}

bool encoderGetStatus(){
	if( status ) {
		status = false;
		return true;
	}
    else {
		return false;
	}
}

encoderResult_t encoderGetEvent(){
	return encoder_event;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void encoder_callback(void){
    if( gpioRead(PIN_B) == true ) {
        encoder_event = ENC_LEFT;
    }
    else {
        encoder_event = ENC_RIGHT;
    }
    status = true;
}
