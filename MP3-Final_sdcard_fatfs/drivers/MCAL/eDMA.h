/*
 * eDMA.h
 *
 *      Author: Francisco Musich
 */

#ifndef EDMA_H_
#define EDMA_H_

#include "stdbool.h"
#include "stdint.h"

#define CHANNELS 16
#define TCDs_AMMOUNT 4



typedef struct
{
	uint32_t SADDR;
	uint16_t SOFF;
	uint16_t ATTR;
	union
	{
		uint32_t NBYTES_MLNO;
		uint32_t NBYTES_MLOFFNO;
		uint32_t NBYTES_MLOFFYES;
	};
		uint32_t SLAST;
		uint32_t DADDR;
		uint16_t DOFF;
	union
	{
		uint16_t CITER_ELINKNO;
		uint16_t CITER_ELINKYES;
	};
		uint32_t DLASTSGA;
		uint16_t CSR;
	union
	{
		uint16_t BITER_ELINKNO;
		uint16_t BITER_ELINKYES;
	};
}TCD_t;

typedef enum {
	ONE_BYTE = 0,
	TWO_BYTE = 1,
	FOUR_BYTE = 2,
	SIXTEEN_BYTE = 4
 } MINLOOP_SIZE_TRANSFER_t;

 typedef enum{
	 CHANNEL_0,
	 CHANNEL_1,
	 CHANNEL_2,
	 CHANNEL_3,
	 CHANNEL_4,
	 CHANNEL_5,
	 CHANNEL_6,
	 CHANNEL_7,
	 CHANNEL_8,
	 CHANNEL_9,
	 CHANNEL_10,
	 CHANNEL_11,
	 CHANNEL_12,
	 CHANNEL_13,
	 CHANNEL_14,
	 CHANNEL_15
 }DMAChannel_t;

 typedef enum
 {
     mDmaRequestMux0Disable          = 0|0x100U,    /**< DMAMUX TriggerDisabled. */
     mDmaRequestMux0Reserved1        = 1|0x100U,    /**< Reserved1 */
     mDmaRequestMux0UART0Rx          = 2|0x100U,    /**< UART0 Receive. */
     mDmaRequestMux0UART0Tx          = 3|0x100U,    /**< UART0 Transmit. */
     mDmaRequestMux0UART1Rx          = 4|0x100U,    /**< UART1 Receive. */
     mDmaRequestMux0UART1Tx          = 5|0x100U,    /**< UART1 Transmit. */
     mDmaRequestMux0UART2Rx          = 6|0x100U,    /**< UART2 Receive. */
     mDmaRequestMux0UART2Tx          = 7|0x100U,    /**< UART2 Transmit. */
     mDmaRequestMux0UART3Rx          = 8|0x100U,    /**< UART3 Receive. */
     mDmaRequestMux0UART3Tx          = 9|0x100U,    /**< UART3 Transmit. */
     mDmaRequestMux0UART4            = 10|0x100U,   /**< UART4 Transmit or Receive. */
     mDmaRequestMux0UART5            = 11|0x100U,   /**< UART5 Transmit or Receive. */
     mDmaRequestMux0I2S0Rx           = 12|0x100U,   /**< I2S0 Receive. */
     mDmaRequestMux0I2S0Tx           = 13|0x100U,   /**< I2S0 Transmit. */
     mDmaRequestMux0SPI0Rx           = 14|0x100U,   /**< SPI0 Receive. */
     mDmaRequestMux0SPI0Tx           = 15|0x100U,   /**< SPI0 Transmit. */
     mDmaRequestMux0SPI1             = 16|0x100U,   /**< SPI1 Transmit or Receive. */
     mDmaRequestMux0SPI2             = 17|0x100U,   /**< SPI2 Transmit or Receive. */
     mDmaRequestMux0I2C0             = 18|0x100U,   /**< I2C0. */
     mDmaRequestMux0I2C1I2C2         = 19|0x100U,   /**< I2C1 and I2C2. */
     mDmaRequestMux0I2C1             = 19|0x100U,   /**< I2C1 and I2C2. */
     mDmaRequestMux0I2C2             = 19|0x100U,   /**< I2C1 and I2C2. */
     mDmaRequestMux0FTM0Channel0     = 20|0x100U,   /**< FTM0 C0V. */
     mDmaRequestMux0FTM0Channel1     = 21|0x100U,   /**< FTM0 C1V. */
     mDmaRequestMux0FTM0Channel2     = 22|0x100U,   /**< FTM0 C2V. */
     mDmaRequestMux0FTM0Channel3     = 23|0x100U,   /**< FTM0 C3V. */
     mDmaRequestMux0FTM0Channel4     = 24|0x100U,   /**< FTM0 C4V. */
     mDmaRequestMux0FTM0Channel5     = 25|0x100U,   /**< FTM0 C5V. */
     mDmaRequestMux0FTM0Channel6     = 26|0x100U,   /**< FTM0 C6V. */
     mDmaRequestMux0FTM0Channel7     = 27|0x100U,   /**< FTM0 C7V. */
     mDmaRequestMux0FTM1Channel0     = 28|0x100U,   /**< FTM1 C0V. */
     mDmaRequestMux0FTM1Channel1     = 29|0x100U,   /**< FTM1 C1V. */
     mDmaRequestMux0FTM2Channel0     = 30|0x100U,   /**< FTM2 C0V. */
     mDmaRequestMux0FTM2Channel1     = 31|0x100U,   /**< FTM2 C1V. */
     mDmaRequestMux0FTM3Channel0     = 32|0x100U,   /**< FTM3 C0V. */
     mDmaRequestMux0FTM3Channel1     = 33|0x100U,   /**< FTM3 C1V. */
     mDmaRequestMux0FTM3Channel2     = 34|0x100U,   /**< FTM3 C2V. */
     mDmaRequestMux0FTM3Channel3     = 35|0x100U,   /**< FTM3 C3V. */
     mDmaRequestMux0FTM3Channel4     = 36|0x100U,   /**< FTM3 C4V. */
     mDmaRequestMux0FTM3Channel5     = 37|0x100U,   /**< FTM3 C5V. */
     mDmaRequestMux0FTM3Channel6     = 38|0x100U,   /**< FTM3 C6V. */
     mDmaRequestMux0FTM3Channel7     = 39|0x100U,   /**< FTM3 C7V. */
     mDmaRequestMux0ADC0             = 40|0x100U,   /**< ADC0. */
     mDmaRequestMux0ADC1             = 41|0x100U,   /**< ADC1. */
     mDmaRequestMux0CMP0             = 42|0x100U,   /**< CMP0. */
     mDmaRequestMux0CMP1             = 43|0x100U,   /**< CMP1. */
     mDmaRequestMux0CMP2             = 44|0x100U,   /**< CMP2. */
     mDmaRequestMux0DAC0             = 45|0x100U,   /**< DAC0. */
     mDmaRequestMux0DAC1             = 46|0x100U,   /**< DAC1. */
     mDmaRequestMux0CMT              = 47|0x100U,   /**< CMT. */
     mDmaRequestMux0PDB              = 48|0x100U,   /**< PDB0. */
     mDmaRequestMux0PortA            = 49|0x100U,   /**< PTA. */
     mDmaRequestMux0PortB            = 50|0x100U,   /**< PTB. */
     mDmaRequestMux0PortC            = 51|0x100U,   /**< PTC. */
     mDmaRequestMux0PortD            = 52|0x100U,   /**< PTD. */
     mDmaRequestMux0PortE            = 53|0x100U,   /**< PTE. */
     mDmaRequestMux0IEEE1588Timer0   = 54|0x100U,   /**< ENET IEEE 1588 timer 0. */
     mDmaRequestMux0IEEE1588Timer1   = 55|0x100U,   /**< ENET IEEE 1588 timer 1. */
     mDmaRequestMux0IEEE1588Timer2   = 56|0x100U,   /**< ENET IEEE 1588 timer 2. */
     mDmaRequestMux0IEEE1588Timer3   = 57|0x100U,   /**< ENET IEEE 1588 timer 3. */
     mDmaRequestMux0AlwaysOn58       = 58|0x100U,   /**< DMAMUX Always Enabled slot. */
     mDmaRequestMux0AlwaysOn59       = 59|0x100U,   /**< DMAMUX Always Enabled slot. */
     mDmaRequestMux0AlwaysOn60       = 60|0x100U,   /**< DMAMUX Always Enabled slot. */
     mDmaRequestMux0AlwaysOn61       = 61|0x100U,   /**< DMAMUX Always Enabled slot. */
     mDmaRequestMux0AlwaysOn62       = 62|0x100U,   /**< DMAMUX Always Enabled slot. */
     mDmaRequestMux0AlwaysOn63       = 63|0x100U,   /**< DMAMUX Always Enabled slot. */
 } DMA_REQ_SOURCE_t;


 typedef void (*channelIrqFun_t)(void);


void DMA_init();

void DMA_enableChannel(DMAChannel_t channel);

void DMA_disableChannel(DMAChannel_t channel);

void DMA_setChannelRequestSource(DMAChannel_t channel,DMA_REQ_SOURCE_t triggerSource);

void DMA_setChannelDataSource(DMAChannel_t channel,uint8_t TCDnum,void* sourcePointer,uint16_t sourceOffset);

void DMA_setChannelDataDestination(DMAChannel_t channel,uint8_t TCDnum, void* destPointer,uint16_t destinationOffset);

void DMA_setMinLoopTransferSize(DMAChannel_t channel,uint8_t TCDnum,MINLOOP_SIZE_TRANSFER_t my_SSIZE, MINLOOP_SIZE_TRANSFER_t my_DSIZE,uint32_t my_NBYTES);

void DMA_setMajorLoopTransfer(DMAChannel_t channel,uint8_t TCDnum, uint32_t CITER, uint32_t BITER, int SLAST);

void DMA_setScatterAndGather(DMAChannel_t channel,uint8_t TCDnumBase, uint8_t TCDnumNext);

void DMA_setChannelTCD(DMAChannel_t channel,uint8_t TCDnum);

void DMA_enableScatterandGather(DMAChannel_t channel, uint8_t TCDnum);

void DMA_disableScatterandGather(DMAChannel_t channel, uint8_t TCDnum);

void DMA_setChannelIRQ(DMAChannel_t channel, channelIrqFun_t callbackFunction);

void DMA_enableMajorChannelLink(DMAChannel_t channel, uint8_t TCDnum,uint8_t channel2Link); //TO DO

void DMA_enableMinorChannelLink(DMAChannel_t channel, uint8_t TCDnum,uint8_t channel2Link);	//TO DO

void DMA_enableMajorInterrupt(DMAChannel_t channel,uint8_t TCDnum);

void DMA_enableHalfMajorInterrupt(DMAChannel_t channel,uint8_t TCDnum);

// Disable Interrupts

void DMA_ISRhandler(DMAChannel_t channel);



#endif /* EDMA_H_ */
