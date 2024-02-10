/***************************************************************************//**
  @file     PIT.c
  @brief    
  @author   
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "PIT.h"
#include "MK64F12.h"
#include "hardware.h"
#include "stddef.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static PIT_callback_t PIT_callbacks[PIT_COUNT];


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void PIT_init(PIT_MODE pit, PIT_callback_t callback) {
    // Clock gating for PIT peripheral
    SIM->SCGC6 |= SIM_SCGC6_PIT(1);

    // Enable module
    PIT->MCR = PIT_MCR_MDIS(0);

    if (callback != NULL) {
		PIT_callbacks[pit] = callback;
		PIT->CHANNEL[pit].TCTRL = PIT_TCTRL_TIE_MASK;
		
            // Enable NVIC for PIT
        switch (pit) {
            case PIT_0:
                NVIC_EnableIRQ(PIT0_IRQn);
                break;
            case PIT_1:
                NVIC_EnableIRQ(PIT1_IRQn);
                break;
            case PIT_2:
                NVIC_EnableIRQ(PIT2_IRQn);
                break;
            case PIT_3:
                NVIC_EnableIRQ(PIT3_IRQn);
                break;
            default:
                break;
        }
	}
	else {
		PIT->CHANNEL[pit].TCTRL = 0x00;
	}

}

void PIT_set_interval(PIT_MODE pit, uint32_t ticks) {
    PIT->CHANNEL[pit].LDVAL = ticks; // load count value
}

void PIT_start(PIT_MODE pit) {
    PIT->CHANNEL[pit].TCTRL |= PIT_TCTRL_TEN_MASK; // start PIT (enable timer)
}

void PIT_stop(PIT_MODE pit) {
    PIT->CHANNEL[pit].TCTRL &= ~PIT_TCTRL_TEN_MASK; // stop PIT (disable timer)
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/*******************************************************************************
 *******************************************************************************
						            INTERRUPT SERVICE ROUTINES
 *******************************************************************************
 ******************************************************************************/

// Adjust these ISR functions based on your PIT channel usage
__ISR__ PIT0_IRQHandler(void) {
	PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF_MASK;		// Clear flag
	PIT_callbacks[0]();									// Callback()
}

__ISR__ PIT1_IRQHandler(void) {
    	PIT->CHANNEL[1].TFLG = PIT_TFLG_TIF_MASK;		// Clear flag
	PIT_callbacks[1]();									// Callback()

}

__ISR__ PIT2_IRQHandler(void) {
    	PIT->CHANNEL[2].TFLG = PIT_TFLG_TIF_MASK;		// Clear flag
	PIT_callbacks[2]();									// Callback()

}

__ISR__ PIT3_IRQHandler(void) {
    PIT->CHANNEL[3].TFLG = PIT_TFLG_TIF_MASK;		// Clear flag
	PIT_callbacks[3]();									// Callback()

}

/******************************************************************************/

