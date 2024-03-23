/***************************************************************************//**
  @file     DRV_WaveGen.h
  @brief    Waveform generator driver. Uses DMA to transfer data to DAC.
  @author   Matias Bergerman, Pablo Gonzalez Leiro, Milagros Moutin, Pedro Carranza Velez
 ******************************************************************************/

#ifndef DRV_WAVEGEN_H_
#define DRV_WAVEGEN_H_

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

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

void wavegen_init(int16_t* buffer_pointer, uint32_t buffer_size, uint32_t freq);
void wavegen_set_samplerate(uint32_t);
void wavegen_set_amp(float);
float wavegen_get_amp();
int16_t* wavegen_get_buffer_addr();
int16_t wavegen_get_buffer_max();
void wavegen_increase_buffer_index(uint16_t amount);
void wavegen_set_callback(void (*callback_fn)(void));
void wavegen_start();
void wavegen_resume();
void wavegen_stop();

/*******************************************************************************
 ******************************************************************************/

#endif /* DRV_WAVEGEN_H_ */
