/*******************************************************************************
  @file     audioplayer.c
  @brief
  @author
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "ff.h"
#include "math.h"

#include "audioplayer.h"
#include "audiobuffer.h"
#include "mp3decoder.h"
#include "fftvumeter.h"
#include "vumeter.h"



/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define AUDIO_MAX_FILENAME_LEN 	256
#define MAX_VOLUME				100U


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
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static audio_states_t   state = AUDIOPLAYER_NONE;

static char*            filePath;

static uint8_t 			vol = 15;


static uint16_t 	g_bufferRead[BUFFER_SIZE];
static short 		decoder_buffer[2*BUFFER_SIZE];

static uint16_t          MP3Buffers[2][OUTPUT_BUFFER_SIZE];

static float32_t         MP3FloatBuffers[2][BUFFER_SIZE];

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/**
 * @brief
 */
void audioplayer_init(void)
{
    // Check if already initialized
    if (state == AUDIOPLAYER_INIT) {
        return;
    }

    // Initialize MP3 Decoder
    mp3_decoder_init();

    fftvumeter_init();

    audiobuffer_init();

    // Initialize DMA DAC
    //DMA_DAC_init();

    // Initialize the MP3 timer
    //timerMP3 = timerGetId();

    // Set the initialization state
    state = AUDIOPLAYER_INIT;
}

/**
 * @brief
 */
void audioplayer_deinit(void)
{

}


/**
 * @brief Loads a song into the audio player.
 * @param filepath Path to the song file.
 * @return State of the audio player after loading the song.
 */
uint16_t audioplayer_load_song(const char* filepath)
{
    if(mp3_load_song(filepath))
    {
        filePath = filepath;

    	memset(g_bufferRead, 0x08, sizeof(g_bufferRead));

    	audiobuffer_load_song(g_bufferRead, 48000);

    	audioplayer_update_buffer();

        state = AUDIOPLAYER_FILE_LOADED;
    }
    else
    {
        state = AUDIOPLAYER_ERROR_FILE;
    }
    return state;
}

/**
 * @brief
 */
uint16_t audioplayer_play(void)
{
	audiobuffer_play();
	state = AUDIOPLAYER_PLAY;
}

/**
 * @brief
 */
bool audioplayer_stop(void)
{
	mp3_load_song(filePath);
	audiobuffer_pause();
	state = AUDIOPLAYER_STOP;
}

/**
 * @brief
 */
bool audioplayer_update_buffer(void)
{
	uint16_t sampleCount = 0;
	uint8_t channelCount = 1;

	audiobuffer_update_backbuffer(g_bufferRead);

	/* Clean buffers to rewrite */
	memset(g_bufferRead, 0, sizeof(g_bufferRead));
	memset(decoder_buffer, 0, sizeof(decoder_buffer));

	/* Fetch the new frame */
	mp3_decoder_states_t check = mp3_decoded_frame(decoder_buffer, 2*BUFFER_SIZE, &sampleCount);

	/* Get the amount of channels in the frame */
	mp3_get_last_frame_channel_count(&channelCount);

	/* Scale from int16 to float[-1;1] */
	float coef = 1.0/32768.0;

	for (uint32_t index = 0; index < BUFFER_SIZE; index++)
	{
		MP3FloatBuffers[0][index] = decoder_buffer[channelCount * index]*coef;
	}

	/* Scale to 12 bits, to fit in the DAC */
	coef = (vol*1.0)/MAX_VOLUME;
	for (uint32_t index = 0; index < BUFFER_SIZE; index++)
	{
		g_bufferRead[index] = (MP3FloatBuffers[0][index]*coef+1)*2048;
	}

	if (check == MP3DECODER_END_OF_FILE)
	{
		/* Por las dudas completo la salida para tener 0V */
		for (uint32_t index = (sampleCount / channelCount); index < BUFFER_SIZE ; index++)
		{
			g_bufferRead[index] = 2048;
		}

		//emitEvent(NEXT_SONG_EV);

	}

    fftvumeter_init();
    if(fftvumeter_calculate_power(MP3FloatBuffers[0], 48000.0, 80, 10000))
    {
    	vu_set_power(fftvumeter_get_power());
    }
}

/**
 * @brief
 */
bool audioplayer_inc_volume(void)
{

}

/**
 * @brief
 */
bool audioplayer_dec_volume(void)
{

}

/**
 * @brief
 */
uint16_t audioplayer_get_volume(void)
{

}

/**
 * @brief
 */
bool audioplayer_set_volume(uint16_t volume)
{

}

/**
 * @brief
 */
char* audioplayer_get_songname(void)
{

}

/**
 * @brief
 */
char* audioplayer_get_artist(void)
{

}

/**
 * @brief
 */
char* audioplayer_get_album(void)
{

}

/**
 * @brief
 */
char* audioplayer_get_year(void)
{

}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/




