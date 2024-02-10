/***************************************************************************//**
  @file     mp3decoder.h
  @brief
  @author
 ******************************************************************************/

#ifndef _MP3DECODEDER_H_
#define _MP3DECODEDER_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include "mp3dec.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

// The frame size is the number of samples contained in a frame. It is constant and always 1152 16-bit samples for Layer 3. Therefore,
// the maximum size required by the output buffer for stereo audio data is 2304 (1152 x 2) 16-bit samples.

#define OUTPUT_BUFFER_SIZE	        1152U

#define INPUT_BUFFER_SIZE			MAINBUF_SIZE

#define MP3_SAMPLE_RATE				44100


/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief  Initialize the MP3 decoder using the Helix MP3 decoder library and resets the context data.
 * @return false if error.
*/
bool  mp3_decoder_init(void);

/**
 * @brief  Selects and loads the MP3 file.
 * @param  filepath: a pointer to the MP3 file path.
 * @return returns true if it successfully opened the file, false otherwise.
*/
bool  mp3_load_song(const char* filepath);

/**
 * @brief Decodes current loaded song
 * @param
 * 			@outBuffer      to be filled with the decoded data.
 * @return MP3FrameInfo struct with outputSamps bytes written in output buffer.
*/
uint16_t mp3_get_next_frame(int16_t* outputBuffer);

/*******************************************************************************
 ******************************************************************************/

#endif // _MP3DECODEDER_H_
