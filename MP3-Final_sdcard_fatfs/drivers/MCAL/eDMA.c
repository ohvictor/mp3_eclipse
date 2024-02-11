/*
 * eDMA.c
 *
 *      Author: Francisco Musich
 */

#include "eDMA.h"
#include "MK64F12.h"
#include "hardware.h"

static TCD_t tcd[CHANNELS][TCDs_AMMOUNT] __attribute__ ((aligned (32))); // TCDs_AMOUNT is the amount

static DMA_Type* DMAptr = DMA0;

static DMAMUX_Type* DMAMUXptr = DMAMUX;

static channelIrqFun_t DMAcallbackArray[CHANNELS];


void DMA_init()
{
	/* Enable the clock for the eDMA and the DMAMUX. */
		SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
		SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
}

void DMA_enableChannel(uint8_t channel)
{
	uint32_t ERQ_ENAB = 1<<channel;
	DMAptr->ERQ |= ERQ_ENAB;
}

void DMA_disableChannel(DMAChannel_t channel)
{
	uint32_t ERQ_ENAB = 1<<channel;
	DMAptr->ERQ &= ~(ERQ_ENAB);
}

void DMA_setChannelRequestSource(DMAChannel_t channel, DMA_REQ_SOURCE_t triggerSource)
{
	DMAMUXptr->CHCFG[channel] |= DMAMUX_CHCFG_ENBL_MASK | DMAMUX_CHCFG_SOURCE(triggerSource);
}

void DMA_setChannelDataSource(DMAChannel_t channel,uint8_t TCDnum ,void* sourcePointer,uint16_t sourceOffset)
{
	tcd[channel][TCDnum].SADDR = (uint32_t)(sourcePointer);
	tcd[channel][TCDnum].SOFF = sourceOffset;
}

void DMA_setChannelDataDestination(DMAChannel_t channel,uint8_t TCDnum, void* destPointer,uint16_t destinationOffset)
{
	tcd[channel][TCDnum].DADDR = (uint32_t)(destPointer);
	tcd[channel][TCDnum].DOFF = destinationOffset;

}

//void DMA_setminorLoop(uint8_t channel,uint8_t TCDnum, uint8_t )

void DMA_setMinLoopTransferSize(DMAChannel_t channel,uint8_t TCDnum,MINLOOP_SIZE_TRANSFER_t my_SSIZE, MINLOOP_SIZE_TRANSFER_t my_DSIZE,uint32_t my_NBYTES)
{
	tcd[channel][TCDnum].ATTR = DMA_ATTR_SSIZE(my_SSIZE) | DMA_ATTR_DSIZE(my_DSIZE);
	tcd[channel][TCDnum].NBYTES_MLNO = my_NBYTES;

}

void DMA_setMajorLoopTransfer(DMAChannel_t channel,uint8_t TCDnum, uint32_t CITER, uint32_t BITER, int SLAST)
{
	tcd[channel][TCDnum].CITER_ELINKNO =DMA_CITER_ELINKNO_CITER(CITER);
	tcd[channel][TCDnum].BITER_ELINKNO = DMA_BITER_ELINKNO_BITER(BITER);
	tcd[channel][TCDnum].SLAST = SLAST;

}

void DMA_enableScatterandGather(DMAChannel_t channel, uint8_t TCDnum)
{
	tcd[channel][TCDnum].CSR |= DMA_CSR_ESG_MASK;
}

void DMA_disableScatterandGather(DMAChannel_t channel, uint8_t TCDnum)
{
	tcd[channel][TCDnum].CSR &= ~(DMA_CSR_ESG(0)); // HACHE
}

void DMA_setScatterAndGather(DMAChannel_t channel,uint8_t TCDnumBase, uint8_t TCDnumNext)
{
	tcd[channel][TCDnumBase].DLASTSGA = (uint32_t)(&(tcd[channel][TCDnumNext]));
}

void DMA_setChannelTCD(DMAChannel_t channel,uint8_t TCDnum)
{
	DMAptr->TCD[channel].SADDR = tcd[channel][TCDnum].SADDR;
	DMAptr->TCD[channel].SOFF = tcd[channel][TCDnum].SOFF;
	DMAptr->TCD[channel].ATTR = tcd[channel][TCDnum].ATTR;
	DMAptr->TCD[channel].NBYTES_MLNO = tcd[channel][TCDnum].NBYTES_MLNO;
	DMAptr->TCD[channel].SLAST = tcd[channel][TCDnum].SLAST;
	DMAptr->TCD[channel].DADDR = tcd[channel][TCDnum].DADDR;
	DMAptr->TCD[channel].DOFF = tcd[channel][TCDnum].DOFF;
	DMAptr->TCD[channel].CITER_ELINKNO = tcd[channel][TCDnum].CITER_ELINKNO;
	DMAptr->TCD[channel].DLAST_SGA = tcd[channel][TCDnum].DLASTSGA;
	DMAptr->TCD[channel].BITER_ELINKNO = tcd[channel][TCDnum].BITER_ELINKNO;
	DMAptr->TCD[channel].CSR = tcd[channel][TCDnum].CSR;
}

void DMA_enableMajorInterrupt(DMAChannel_t channel,uint8_t TCDnum)
{
	tcd[channel][TCDnum].CSR |= DMA_CSR_INTMAJOR_MASK;
	NVIC_ClearPendingIRQ(DMA0_IRQn+channel);
	NVIC_EnableIRQ(DMA0_IRQn+channel); //En el vector de fuentes de interrupcion DMA channels son los primeros 15
}

void DMA_enableHalfMajorInterrupt(DMAChannel_t channel,uint8_t TCDnum)
{
	tcd[channel][TCDnum].CSR |= DMA_CSR_INTHALF_MASK;
	NVIC_ClearPendingIRQ(DMA0_IRQn+channel);
	NVIC_EnableIRQ(DMA0_IRQn+channel); //En el vector de fuentes de interrupcion DMA channels son los primeros 15
}

void DMA_enableMajorChannelLink(DMAChannel_t channel, uint8_t TCDnum,uint8_t channel2Link)
{

}

void DMA_setChannelIRQ(DMAChannel_t channel, channelIrqFun_t callbackFunction)
{
	DMAcallbackArray[channel] = callbackFunction;
}

void DMA_ISRhandler(DMAChannel_t channel)
{
	DMAcallbackArray[channel]();
}

/*	Interruption Handlers	*/

__ISR__ DMA0_IRQHandler()
{
	/* Clear the interrupt flag. */
	DMAptr->CINT |= DMA_CINT_CINT(0); // Creo que es asi
	DMA_ISRhandler(0);
}

__ISR__ DMA1_IRQHandler()
{
	/* Clear the interrupt flag. */
	DMAptr->CINT |= DMA_CINT_CINT(1); // Creo que es asi
	DMA_ISRhandler(1);
}

__ISR__ DMA2_IRQHandler()
{
	/* Clear the interrupt flag. */
	DMAptr->CINT |= DMA_CINT_CINT(2); // Creo que es asi
	DMA_ISRhandler(2);
}

__ISR__ DMA3_IRQHandler()
{
	/* Clear the interrupt flag. */
	DMAptr->CINT |= DMA_CINT_CINT(3); // Creo que es asi
	DMA_ISRhandler(3);
}

__ISR__ DMA4_IRQHandler()
{
	/* Clear the interrupt flag. */
	DMAptr->CINT |= DMA_CINT_CINT(4); // Creo que es asi
	DMA_ISRhandler(4);
}

