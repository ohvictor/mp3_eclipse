/*******************************************************************************
  @file     audiobuffer.h
  @brief
  @author
 ******************************************************************************/

#ifndef _AUDIOBUFFER_H
#define _AUDIOBUFFER_H_

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
    NO_ERROR,
    BACKBUFFER_FULL
} audiobuffer_states_t;

#define BUFFER_SIZE 1152U

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/


/*
 * @brief
 */
void audiobuffer_init(void);


/*
 *@brief
 */
void audiobuffer_load_song(uint16_t *song_firstFrame, uint16_t sampleRate);


/*
 * @brief
*/
void audiobuffer_play(void);

/*
 * @brief Pauses the audio reproduction.
*/
void audiobuffer_pause(void);

/*
 * @brief
*/
void audiobuffer_stop(void);

/*
 *@brief
*/
void audiobuffer_update_samplerate(uint16_t samplerate);

/*
 * @brief
*/
audiobuffer_states_t audiobuffer_update_backbuffer(uint16_t *newbackbuffer);

/*
 *@brief
*/
bool audiobuffer_backbuffer_state(void);


/*******************************************************************************
 ******************************************************************************/

#endif //_AUDIOBUFFER_H
