/***************************************************************************//**
  @file     DRV_WaveGen.c
  @brief    Waveform generator driver. Uses DMA to transfer data to DAC.
  @author   Matias Bergerman, Pablo Gonzalez Leiro, Milagros Moutin, Pedro Carranza Velez
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "DRV_WaveGen.h"
#include "PDRV_DAC.h"
#include "PDRV_DMA.h"
#include "PDRV_PIT.h"

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

static void start_dma_transfer();
static void wavegen_isr();

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static int16_t* wavegen_buffer;         // Ping Pong buffer
static uint16_t wavegen_buffer_size;
static uint16_t wavegen_buffer_half_size;
static uint8_t buffer_side = 0;         // 0: Ping, 1: Pong
static uint16_t buffer_index = 0;       // Index of the next sample to be loaded
static bool wavegen_running = false;
static void (*wavegen_callback)(void) = NULL;
static dma_config_t dma_config; 

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void wavegen_init(int16_t* buffer_pointer, uint32_t buffer_size, uint32_t freq){
	initDAC();

    wavegen_buffer = buffer_pointer;        // The buffer should already be allocated
    wavegen_buffer_size = buffer_size;
    wavegen_buffer_half_size = buffer_size/2;

    dma_config.source_buffer        = wavegen_buffer;
    dma_config.destination_buffer   = DAC_DMA_DEST;
    dma_config.request_source       = DMAALWAYS63;
    dma_config.source_offset        = 2;
    dma_config.destination_offset   = 0;
    dma_config.transfer_bytes       = 2;
    dma_config.major_cycles         = buffer_size;
    dma_config.wrap_around          = buffer_size*sizeof(wavegen_buffer[0]);
    dma_config.enable_major_int     = true;
    dma_config.enable_half_int      = true;

	dma_init(DAC_DMA_CHNNL, dma_config);
    dma_set_major_callback(DAC_DMA_CHNNL, wavegen_isr);
    dma_set_half_callback(DAC_DMA_CHNNL, wavegen_isr);

	pit_init(0x02FAF07F);	// 1/50Mhz = 20ns   (1s/20ns)-1= 49,999,999 cycles or 0x02FAF07F);
	pit_set_callback(start_dma_transfer);
	wavegen_set_samplerate(freq);
}

void wavegen_set_samplerate(uint32_t samplerate){
	pit_set_frequency(samplerate);
}

int16_t* wavegen_get_buffer_addr(){
    return wavegen_buffer + buffer_index;
}

int16_t wavegen_get_buffer_max(){
    return (buffer_side)? wavegen_buffer_size - buffer_index : wavegen_buffer_half_size - buffer_index;
}

void wavegen_increase_buffer_index(uint16_t amount){
    buffer_index += amount;
}

void wavegen_set_callback(void (*callback_fn)(void)){
    wavegen_callback = callback_fn;
}

void wavegen_start(){
    wavegen_running = true;
    buffer_index = 0;
    buffer_side = 0;

    dma_set_config(DAC_DMA_CHNNL, dma_config);
    pit_enable();
}

void wavegen_resume(){
    wavegen_running = true;
    pit_enable();
}

void wavegen_stop(){
    wavegen_running = false;
    pit_disable();
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
 
static void start_dma_transfer(){
	pit_start_dma_transfer(DAC_DMA_CHNNL);
}

static void wavegen_isr(){
    if(wavegen_running){
        buffer_side = (buffer_side + 1) % 2;                        // Switch sides
        buffer_index = (buffer_side)? 0 : wavegen_buffer_half_size; // Reset index

        if(wavegen_callback){
            wavegen_callback();
        }
    }
}
