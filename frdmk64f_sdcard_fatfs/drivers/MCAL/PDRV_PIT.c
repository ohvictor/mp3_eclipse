/***************************************************************************//**
  @file     DRV_PIT.c
  @brief    Periodic Interrupt Timer driver
  @author   Matias Bergerman, Pablo Gonzalez Leiro, Milagros Moutin, Pedro Carranza Velez
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <PDRV_PIT.h>
#include "MK64F12.h"
#include "hardware.h"
#include "fsl_clock.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

//#define CLK_FREQ 60000000.0F

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

static uint32_t clk_freq = 0;

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void (*pit_callback)(void) = NULL;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void pit_init(uint32_t pit_time){
	static bool alreadyInit = false;
	if (alreadyInit) return;

	clk_freq = CLOCK_GetFreq(kCLOCK_CoreSysClk);

	// Enable PIT clock
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;

	// Turn on PIT
	PIT->MCR = 0;

	// Configure PIT to produce an interrupt
	PIT->CHANNEL[0].LDVAL = pit_time;
	
	// Enable interrupt registers ISER and ICPR
	NVIC_EnableIRQ(PIT0_IRQn);
}

void pit_enable(){
    PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK | PIT_TCTRL_TEN_MASK; // Enable interrupt and enable timer
}

void pit_disable(){
    PIT->CHANNEL[0].TCTRL &= ~(PIT_TCTRL_TIE_MASK | PIT_TCTRL_TEN_MASK); // Disable interrupt and disable timer
}

void pit_set_frequency(uint32_t freq){

	PIT->CHANNEL[0].LDVAL = ((clk_freq) / freq - 1);//((__CORE_CLOCK__) / (2*freq) - 1);
}

void pit_set_callback(void (*callback_fn)()){
	pit_callback = callback_fn;
}

void pit_start_dma_transfer(uint32_t channel){
	//Start DMA channel 0 transfer
	DMA0->TCD[channel].CSR |= DMA_CSR_START_MASK;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

//ISR of the PIT timer. Clears the PIT interrupt flag and starts DMA channel 0 transfer
void PIT0_IRQHandler(void)
{
	// Clear interrupt
	PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF_MASK;

	if(pit_callback != NULL){
		pit_callback();
	}
}
