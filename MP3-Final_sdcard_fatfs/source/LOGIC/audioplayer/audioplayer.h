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
void audioplayer_init(void);

/**
 * @brief Initializes the audio module.
 */
void audioplayer_deinit(void);

/**
 * @brief           
 * @param filepath      Directory path for the audio files
 */
uint16_t audioplayer_load_song(const char* filepath);

/**
 * @brief
 * @param
 */
uint16_t audioplayer_play(void);

/**
 * @brief
 * @param
 */
bool audioplayer_stop(void);

/**
 * @brief
 * @param
 */
bool audioplayer_update_buffer(void);


/**
 * @brief
 */
bool audioplayer_inc_volume(void);

/**
 * @brief
 */
bool audioplayer_dec_volume(void);
/**
 * @brief
 */
uint16_t audioplayer_get_volume(void);

/**
 * @brief
 */
bool audioplayer_set_volume(uint16_t volume);


/**
 * @brief
 */
char* audioplayer_get_songname(void);

/**
 * @brief
 */
char* audioplayer_get_artist(void);

/**
 * @brief
 */
char* audioplayer_get_album(void);

/**
 * @brief
 */
char* audioplayer_get_year(void);



/*******************************************************************************
 ******************************************************************************/

#endif //_AUDIOPLAYER_H_
