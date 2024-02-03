/***************************************************************************//**
  @file     mp3decoder.h
  @brief    MP3 file decoder
  @author   Grupo 3
 ******************************************************************************/

#ifndef _MP3DECODER_H_
#define _MP3DECODER_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include "helix/pub/mp3dec.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

// The frame size is the number of samples contained in a frame. It is constant and always 1152 16-bit samples for Layer 3. Therefore,
// the maximum size required by the output buffer for stereo audio data is 2304 (1152 x 2) 16-bit samples.
#define OUTBUFFER_SIZE	(1152*2)U

#define TAG_SIZE 128

#define ID3_MAX_FIELD_SIZE      50

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct
{
    uint16_t    bitRate;
    uint8_t     channelCount;
    uint16_t    sampleCount;
    uint16_t	sampleRate;
    uint16_t    binitsPerSample;
} mp3_frame_data_t;

typedef struct
{
    uint8_t title[ID3_MAX_FIELD_SIZE];
    uint8_t artist[ID3_MAX_FIELD_SIZE];
    uint8_t album[ID3_MAX_FIELD_SIZE];
    uint8_t trackNum[ID3_MAX_FIELD_SIZE];
    uint8_t year[ID3_MAX_FIELD_SIZE];

} mp3_id3_tag_data_t;

typedef enum
{
	MP3DECODER_OK,
	MP3DECODER_ERROR,
	MP3DECODER_NO_FILE,
	MP3DECODER_END_OF_FILE,
	MP3DECODER_OVERFLOW
} decoder_state_t;


/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/



/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief  Initialize the MP3 decoder using the Helix MP3 decoder library and resets the context data.
 * @return false if error.
*/
bool  MP3DecoderInit(void);

/**
 * @brief  Selects and loads the MP3 file.
 * @param  filepath: a pointer to the MP3 file path.
 * @return returns true if it successfully opened the file, false otherwise.
*/
bool  MP3LoadSong(const char* filepath);

/**
 * @brief  Gets the last frame data and stores it in the MP3FrameInfo from the Helix Library.
 * @param  Pointer to the MP3FrameInfo structure for the frame.
 * @return true if it returns the last frame info successfully, false otherwise.
*/
bool MP3LastFrame(MP3FrameInfo* data);

/*
* @brief 	Gets the information of the following frame to be decoded in case it exists.
* @param 	Pointer to the MP3FrameInfo structure for the frame.
* @returns 	true if successful, false otherwise.
*/
bool MP3NextFrame(MP3FrameInfo* data);


/**
 * @brief  Reads the current file, finds the sync word, stores and decodes the next frame. If successful, updates the pointers for the next frame.
 * @param
 * 			@outBuffer to be filled with the decoded data.
 * 			@bufferSize outBuffer size to store the decoded data.
 * 			@decodedSamples pointer to a counter of the decoded samples.
 * @return mp3_decoder_result_t
*/
decoder_state_t MP3DecodedFrame(short* outBuffer, uint16_t bufferSize, uint16_t* decodedSamples);

/*
* @brief Gets the MP3 file tag data: name, artist, album, year, trackNum.
* @param Pointer to the mp3_id3_tag_data_t structure.
* @returns true if successful, false otherwise
*/
bool MP3GetTagData(mp3_id3_tag_data_t* data);

/*******************************************************************************
 ******************************************************************************/

#endif // _MP3DECODER_H_
