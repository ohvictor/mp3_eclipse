/*******************************************************************************
  @file     audioPlayer.h
  @brief
  @author
 ******************************************************************************/

#ifndef _AUDIOPLAYER_H
#define _AUDIOPLAYER_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum
{
    AUDIOPLAYER_NONE,
    AUDIOPLAYER_INIT,
    AUDIOPLAYER_FILE_LOADED,
    AUDIOPLAYER_PLAY,
    AUDIOPLAYER_STOP,
    AUDIOPLAYER_ERROR_FILE,
    AUDIOPLAYER_ERROR_FRAME,
} audio_states_t;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initializes the audio module.
 */
void audio_player_init(void);

/**
 * @brief Initializes the audio module.
 */
void audio_player_deinit(void);

/**
 * @brief           
 * @param filepath      Directory path for the audio files
 */
uint16_t audio_player_load_song(const char* filepath);

/**
 * @brief
 * @param
 */
uint16_t audio_player_play(void);

/**
 * @brief
 * @param
 */
bool audio_player_stop(void);

/**
 * @brief
 * @param
 */
bool audio_player_update_buffer(void);

/**
 * @brief
 * @param
 */
bool audio_player_stop(void);

/**
 * @brief
 * @param
 */
bool audio_player_inc_volume(void);

/**
 * @brief
 * @param
 */
bool audio_player_dec_volume(void);

/**
 * @brief
 * @param
 */
uint16_t audio_player_get_volume(void);

/**
 * @brief
 * @param
 */
bool audio_player_set_volume(uint16_t volume);



/*******************************************************************************
 ******************************************************************************/

#endif //_AUDIOPLAYER_H_
