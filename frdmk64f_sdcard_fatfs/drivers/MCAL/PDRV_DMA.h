/***************************************************************************//**
  @file     PDRV_DMA.h
  @brief    DMA driver
  @author   Matias Bergerman, Pablo Gonzalez Leiro, Milagros Moutin, Pedro Carranza Velez
 ******************************************************************************/

#ifndef _DMA_H_
#define _DMA_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <Util.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef void (*dma_callback_t)(void);

typedef enum {	FTM0CH0 = 20, 	FTM0CH1 = 21, FTM0CH2 = 22, FTM0CH3 = 23,
			  	FTM0CH4 = 24, 	FTM0CH5 = 25, FTM0CH6 = 26, FTM0CH7 = 27,
			  	DMADAC0 = 45,	DMADAC1 = 46, DMAALWAYS63 = 63				} dma_request_t;

typedef struct dma_config_t
{
	void * source_buffer;
	void * destination_buffer;
	dma_request_t request_source;
	uint8_t source_offset;
	uint8_t destination_offset;
	uint8_t transfer_bytes;
	uint16_t major_cycles;
	uint32_t wrap_around;
    bool enable_major_int;  // Enable interrupt on major loop completion
    bool enable_half_int;   // Enable interrupt on half loop completion (used for ping-pong buffers)
}dma_config_t;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/



/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

void dma_init(uint8_t dma_channel, dma_config_t config);
dma_config_t dma_get_config(uint8_t dma_channel);
void dma_set_config(uint8_t dma_channel, dma_config_t config);
void dma_reset_major_count(uint8_t dma_channel);
void dma_set_major_callback(uint8_t dma_channel, dma_callback_t callback_fn);
void dma_set_half_callback(uint8_t dma_channel, dma_callback_t callback_fn);

/*******************************************************************************
 ******************************************************************************/

#endif // _DMA_H_
