/***************************************************************************//**
  @file     PDRV_DMA.c
  @brief    DMA driver
  @author   Matias Bergerman, Pablo Gonzalez Leiro, Milagros Moutin, Pedro Carranza Velez
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "PDRV_DMA.h"
#include "MK64F12.h"
#include "hardware.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define DMA_ERQ_MASK(x) (0x1 << x)

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void dma_irq_handler(uint8_t dma_channel);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static dma_config_t dma_channel_config[16];
static dma_callback_t dma_major_callbacks[16];
static dma_callback_t dma_half_callbacks[16];

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void dma_init(uint8_t dma_channel, dma_config_t config)
{
	// Remember channel config
	dma_channel_config[dma_channel] = config;

	// Clock gating
	SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
	SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;


	if(config.request_source >= 58){
		DMAMUX->CHCFG[dma_channel] |= DMAMUX_CHCFG_ENBL_MASK | DMAMUX_CHCFG_TRIG_MASK | DMAMUX_CHCFG_SOURCE((uint8_t)config.request_source);
	}else{
		DMAMUX->CHCFG[dma_channel] |= DMAMUX_CHCFG_ENBL_MASK | DMAMUX_CHCFG_SOURCE((uint8_t)config.request_source);
	}

	// Clear all the pending events
	NVIC_ClearPendingIRQ(DMA0_IRQn + dma_channel);
	// Enable the DMA interrupts
	NVIC_EnableIRQ(DMA0_IRQn + dma_channel);

	dma_set_config(dma_channel, config);
}

dma_config_t dma_get_config(uint8_t dma_channel){
    return dma_channel_config[dma_channel];
}

void dma_set_config(uint8_t dma_channel, dma_config_t config){
	// Set source
	DMA0->TCD[dma_channel].SADDR = (uint32_t)(config.source_buffer);

    // Set destination
	DMA0->TCD[dma_channel].DADDR = (uint32_t)(config.destination_buffer);


	// Set an offset for source and destination address
	DMA0->TCD[dma_channel].SOFF = config.source_offset;
	DMA0->TCD[dma_channel].DOFF = config.destination_offset;

	// Set source and destination data transfer size
	switch(config.transfer_bytes)
	{
		case(1):
			DMA0->TCD[dma_channel].ATTR = DMA_ATTR_SSIZE(0) | DMA_ATTR_DSIZE(0);
		break;
		case(2):
			DMA0->TCD[dma_channel].ATTR = DMA_ATTR_SSIZE(1) | DMA_ATTR_DSIZE(1);
		break;
		case(4):
			DMA0->TCD[dma_channel].ATTR = DMA_ATTR_SSIZE(2) | DMA_ATTR_DSIZE(2);
		break;

	}

	// Number of bytes to be transfered in each service request of the channel
	DMA0->TCD[dma_channel].NBYTES_MLNO = config.transfer_bytes;

	// Number of major transfer cycles
	DMA0->TCD[dma_channel].CITER_ELINKNO = DMA_CITER_ELINKNO_CITER(config.major_cycles);
	DMA0->TCD[dma_channel].BITER_ELINKNO = DMA_BITER_ELINKNO_BITER(config.major_cycles);

	// Set Wrap Around
	DMA0->TCD[dma_channel].SLAST = -config.wrap_around;


    // DLASTSGA DLAST Scatter and Gather
    DMA0->TCD[dma_channel].DLAST_SGA = 0x00;

	// Setup control and status register.
    if(config.enable_major_int){
	    DMA0->TCD[dma_channel].CSR |= DMA_CSR_INTMAJOR_MASK;	//Enable Major Interrupt.
    }else{
        DMA0->TCD[dma_channel].CSR &= ~DMA_CSR_INTMAJOR_MASK;	//Disable Major Interrupt.
    }
    if(config.enable_half_int){
        DMA0->TCD[dma_channel].CSR |= DMA_CSR_INTHALF_MASK;		//Enable Halfway Interrupt.
    }else{
        DMA0->TCD[dma_channel].CSR &= ~DMA_CSR_INTHALF_MASK;	//Disable Halfway Interrupt.
    }

	// Enable request signal
	DMA0->ERQ = DMA_ERQ_MASK(dma_channel);
}

void dma_reset_major_count(uint8_t dma_channel){
    DMA0->CR |= DMA_CR_CX(dma_channel);         // Stop the executing channel and force the minor loop to finish
    DMA0->TCD[dma_channel].CITER_ELINKNO = DMA_CITER_ELINKNO_CITER(dma_channel_config[dma_channel].major_cycles);
}

void dma_set_major_callback(uint8_t dma_channel, dma_callback_t callback_fn){
	dma_major_callbacks[dma_channel] = callback_fn;
}

void dma_set_half_callback(uint8_t dma_channel, dma_callback_t callback_fn){
	dma_half_callbacks[dma_channel] = callback_fn;
}

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void dma_irq_handler(uint8_t dma_channel){
	/* Clear the interrupt flag. */
	DMA0->CINT |= DMA_CINT_CINT(dma_channel);

    if(DMA0->TCD[dma_channel].CSR & DMA_CSR_DONE_MASK){ 
        // Major loop is DONE
        if(dma_major_callbacks[dma_channel]){
            dma_major_callbacks[dma_channel]();
            
        }

        
	}else if(DMA0->TCD[dma_channel].CITER_ELINKNO == (DMA0->TCD[dma_channel].BITER_ELINKNO>>1)){
        // Halfway interrupt
        if(dma_half_callbacks[dma_channel]){
            dma_half_callbacks[dma_channel]();

		}
    }
}

void DMA0_IRQHandler(void){ dma_irq_handler(0); }
void DMA1_IRQHandler(void){ dma_irq_handler(1); }
void DMA2_IRQHandler(void){ dma_irq_handler(2); }
void DMA3_IRQHandler(void){ dma_irq_handler(3); }
void DMA4_IRQHandler(void){ dma_irq_handler(4); }
void DMA5_IRQHandler(void){ dma_irq_handler(5); }
void DMA6_IRQHandler(void){ dma_irq_handler(6); }
void DMA7_IRQHandler(void){ dma_irq_handler(7); }
void DMA8_IRQHandler(void){ dma_irq_handler(8); }
void DMA9_IRQHandler(void){ dma_irq_handler(9); }
void DMA10_IRQHandler(void){ dma_irq_handler(10); }
void DMA11_IRQHandler(void){ dma_irq_handler(11); }
void DMA12_IRQHandler(void){ dma_irq_handler(12); }
void DMA13_IRQHandler(void){ dma_irq_handler(13); }
void DMA14_IRQHandler(void){ dma_irq_handler(14); }
void DMA15_IRQHandler(void){ dma_irq_handler(15); }

void DMA_Error_IRQHandler(void){
	/* Clear all error indicators.*/
	DMA0->CERR = DMA_CERR_CAEI(1);
}

/******************************************************************************/
