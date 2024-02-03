/*******************************************************************************
  @file     MP3AudioPlayer.h
  @brief    Audio UI and Controller Module
  @author   
 ******************************************************************************/

#ifndef _MP3_AUDIO_PLAYER_H
#define _MP3_AUDIO_PLAYER_H

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "events/events.h"

#include <stdint.h>

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

/**
 * @brief Initializes the audio module.
 */
void audioInit(void);

/**
 * @brief Cycles the audio module with the next user event.
 * @param event     Next event
 */
void audioRun(event_t event);

/**
 * @brief Filename and path of current song, starts playing the audio.
 * @param path      Directory path for the audio files
 * @param file      Filename of the starting audio
 * @param index     Index of the filename in the directory
 */
void audioSetFolder(const char* path, const char* file, uint8_t index);

/*******************************************************************************
 ******************************************************************************/

#endif