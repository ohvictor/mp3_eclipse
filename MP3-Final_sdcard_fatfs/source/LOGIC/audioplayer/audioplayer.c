/*******************************************************************************
  @file     audio.c
  @brief    Audio UI and Controller Module
  @author   
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "audioPlayer.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "timer.h"
#include "ff.h"
#include "math.h"

#include "mp3decoder.h"
#include "DMA_DAC.h"
#include "fftvumeter.h"
#include "vumeter.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define AUDIO_MAX_FILENAME_LEN 256

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

void timerMP3callback();

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static tim_id_t         timerMP3;
static int16_t          MP3Buffers[2][OUTPUT_BUFFER_SIZE];
static float32_t        MP3FloatBuffers[2][OUTPUT_BUFFER_SIZE];
static int16_t*         pMP3Buffer;
static int16_t*         pPrevMP3Buffer;
static MP3FrameInfo     MP3frameInfo;
static uint32_t         MP3sampleRate;
static char*            filePath;
static audio_states_t   state = AUDIOPLAYER_NONE;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/**
 * @brief Initializes the audio player module.
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

    // Initialize DMA DAC
    DMA_DAC_init();

    // Initialize the MP3 timer
    timerMP3 = timerGetId();

    // Set the initialization state
    state = AUDIOPLAYER_INIT;

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
        state = AUDIOPLAYER_FILE_LOADED;
    }
    else
    {
        state = AUDIOPLAYER_ERROR_FILE;
    }
    return state;
}

/**
 * @brief Starts playing the loaded song.
 * @return State of the audio player after starting to play the song.
 */
uint16_t audioplayer_play(void)
{
    pMP3Buffer = MP3Buffers[0];

    uint16_t bufferRead = mp3_get_next_frame(pMP3Buffer);

    if (!bufferRead) {
        state = AUDIOPLAYER_ERROR_FRAME;
    }
    else {
        // Convert buffer to floating point
        for (int i = 0; i < bufferRead; i++) {
            MP3FloatBuffers[0][i] = (float32_t) pMP3Buffer[i];
        }

        // Float to 12-bit conversion
        for (int i = 0; i < OUTPUT_BUFFER_SIZE; i++) {
        	float32_t aux = MP3FloatBuffers[1][i] * 0x7FF / 0x7FFF;
            if (aux > (int16_t)0x7FF) {
                aux = 0x7FF;
            } else if (aux < (int16_t)0xF800) {
                aux = 0xF800;
            }
            ((uint16_t*)pMP3Buffer)[i] = (uint16_t)((int16_t)aux + (int16_t)0x800);
        }

        fftvumeter_init();
        if(fftvumeter_calculate_power(MP3FloatBuffers[1], 48000.0, 80, 10000))
        {
        	vu_set_power(fftvumeter_get_power());
        }

        // Start DMA DAC
        DMA_DAC_PingPong((uint16_t*)MP3Buffers[0], (uint16_t*)MP3Buffers[1], OUTPUT_BUFFER_SIZE);


        // Start the MP3 timer
        timerStart(timerMP3, TIMER_MS2TICKS(5U), TIM_MODE_PERIODIC, timerMP3callback);

        // Set the state to play
        state = AUDIOPLAYER_PLAY;
    }
    return state;
}

/**
 * @brief Stops the audio player.
 * @return True if successful, false otherwise.
 */
bool audioplayer_stop(void)
{
    // Stop the MP3 timer
    timerStop(timerMP3);

    // Pause DMA
    DMA_DAC_PingPong_pause();

    // Set the state to stop
    state = AUDIOPLAYER_STOP;
    return state;
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

/**
 * @brief Callback function for the MP3 timer.
 */
void timerMP3callback()
{
    //pMP3Buffer = (int16_t*) DMA_DAC_PingPong_table();

    if (pMP3Buffer != pPrevMP3Buffer) {

        pPrevMP3Buffer = pMP3Buffer;

        uint16_t buffer_read = mp3_get_next_frame(pMP3Buffer);

        if (buffer_read > 0) {
            for (int i = 0; i < buffer_read; i++) {
                MP3FloatBuffers[0][i] = (float32_t)pMP3Buffer[i];
            }

            // Float to 12-bit conversion
            for (int i = 0; i < OUTPUT_BUFFER_SIZE; i++) {
                float aux = MP3FloatBuffers[1][i] * 0x7FF / 0x7FFF;
                if (aux > (int16_t)0x7FF) {
                    aux = 0x7FF;
                } else if (aux < (int16_t)0xF800) {
                    aux = 0xF800;
                }
                ((uint16_t*)pMP3Buffer)[i] = (uint16_t)((int16_t)aux + (int16_t)0x800);
            }

            fftvumeter_init();
            if(fftvumeter_calculate_power(MP3FloatBuffers[1], 48000.0, 80, 10000))
            {
            	vu_set_power(fftvumeter_get_power());
            }
        }
        else {
            // Stop the audio player if no more frames
            audioplayer_stop();
        }
    }
}

/*
static void loadPlayingSong(void)
{
	MP3Decoder_LoadFile(playingSongFile.path);

	// First two buffers in 0V, no sound
	int i;
	for(i = 0; i < BUFFER_SIZE; i++)
	{
		processedAudioBuffer[i] = DAC_ZERO_VOLT_VALUE;
	}

	sampleRate = 44100;

	// Set a default sampleRate for first buffers
	AudioPlayer_LoadSong(processedAudioBuffer, sampleRate);

	mp3Handler_updateAudioPlayerBackBuffer();
}


void mp3Handler_updateAudioPlayerBackBuffer(void)
{
	uint32_t numOfSamples = 0;
	uint8_t numOfChannels = 1;
	float effects_in[BUFFER_SIZE];
	float effects_out[BUFFER_SIZE];

	// Update with the previous processed audio
	AudioPlayer_UpdateBackBuffer(processedAudioBuffer, sampleRate);

	// Clean buffers to rewrite
	memset(processedAudioBuffer, 0, sizeof(processedAudioBuffer));
	memset(decoder_buffer, 0, sizeof(decoder_buffer));

	// Fetch the new frame
	decoder_result_t res = MP3Decoder_DecodeFrame(decoder_buffer, 2*BUFFER_SIZE, &numOfSamples, &sampleRate);

	// Get the number of channels in the frame
	MP3Decoder_GetLastFrameNumOfChannels(&numOfChannels);

	// 1 - Scale from int16 to float[-1;1]
	float coef = 1.0/32768.0;
	uint32_t index;

	if(numOfChannels == 1)
	{
		for (index = 0; index < BUFFER_SIZE; index++)
		{
			effects_in[index] = decoder_buffer[index] * coef;
		}
	}
	else
	{
		// If stereo, sum L + R
		for (index = 0; index < BUFFER_SIZE; index++)
		{
			effects_in[index] = (decoder_buffer[index * 2] + decoder_buffer[index * 2 + 1]) * coef;
		}
	}

	// 2 - Apply audio effects
	EQ_Apply(effects_in, effects_out);


	// 3 - apply volume and
	// 4 - Scale to 12 bits, to fit in the DAC
	coef = (vol * 1.0) / MAX_VOLUME;

	for (index = 0; index < BUFFER_SIZE; index++)
	{
		processedAudioBuffer[index] = (effects_out[index] * coef + 1) * DAC_ZERO_VOLT_VALUE;
	}

	if (res == DECODER_END_OF_FILE)
	{
		// Complete the rest of the buffer with 0V

		for (uint32_t index = (numOfSamples / numOfChannels); index < BUFFER_SIZE ; index++)
		{
			processedAudioBuffer[index] = DAC_ZERO_VOLT_VALUE;
		}

		push_Queue_Element(NEXT_SONG_EV);

	}

	// Compute FFT and set the vumeter
	VU_FFT(effects_out, sampleRate, 80, 10000);
}
*/
