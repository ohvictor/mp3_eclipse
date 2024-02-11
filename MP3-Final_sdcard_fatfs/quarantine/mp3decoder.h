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

#define ID3_MAX_NUM_CHARS           50

#define DEFAULT_ID3TAG               "Unknown"


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum
{
	MP3DECODER_WORKED,
	MP3DECODER_ERROR,
	MP3DECODER_NO_FILE,
	MP3DECODER_END_OF_FILE,
	MP3DECODER_OVERFLOW
} mp3_decoder_states_t;

typedef struct {
  char tag[3];
  char title[ID3_MAX_NUM_CHARS];
  char artist[ID3_MAX_NUM_CHARS];
  char album[ID3_MAX_NUM_CHARS];
  char year[4];
  char comment[ID3_MAX_NUM_CHARS];
} ID3Tag_t;


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
 * @param @outBuffer      to be filled with the decoded data.
 * @return Decoder state
*/
mp3_decoder_states_t mp3_decoded_frame(short* outputBuffer, uint16_t bufferSize, uint16_t* decodedSamples);

/**
 * @brief  
 * @return
*/
bool mp3_get_last_frame_channel_count(uint8_t* channelCount);

/**
 * @brief Get MP3 file tag data.
 */
ID3Tag_t* mp3_get_tag_data();

/**
 * @brief Check if file has ID3 tag.
 */
bool mp3_has_tag(void);

/**
 * @brief: Get the current file title.
 * @return: true if hasID3 returns true.
 */
char* mp3_get_tag_title(void);


/**
 * @brief: Get the current file's album.
 * @param album_: is a pointer to the char pointer (array of chars) that forms the album word.
 * @return: true if hasID3 returns true.
 */
char* mp3_get_tag_album(void);


/**
 * @brief: Get the current file's artist.
 * @param artist_: is a pointer to the char pointer (array of chars) that forms the artist word.
 * @return: true if hasID3 returns true.
 */
char* mp3_get_tag_artist(void);


/**
 * @brief: Get the current file's year.
 * @param year_: is a pointer to the char pointer (array of chars) that forms the year word.
 * @return: true if hasID3 returns true.
 */
char* mp3_get_tag_year(void);



/*******************************************************************************
 ******************************************************************************/

#endif // _MP3DECODEDER_H_
