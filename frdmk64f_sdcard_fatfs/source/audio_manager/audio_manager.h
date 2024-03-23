/***************************************************************************//**
  @file     audio_manager.h
  @brief    Audio Manager
  @author   Matias Bergerman, Pablo Gonzalez Leiro, Milagros Moutin, Pedro Carranza Velez
 ******************************************************************************/

#ifndef _AUDIO_MANAGER_H_
#define _AUDIO_MANAGER_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "mp3decoder.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define MAX_VOLUME          (30)
#define MIN_VOLUME          (0)

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

void init_audio_manager(void);
bool audio_load_song(char* song_path);
void audio_get_tag_data(mp3decoder_tag_data_t* ID3Data);
void audio_print_tag_data();
void audio_start_playing(void);
bool audio_decode_loop(void);
void audio_stop_playing(void);
bool audio_is_playing(void);

void audio_set_eq(uint8_t eq);

uint8_t increase_volume(void);
uint8_t decrease_volume(void);
bool audio_play_pause(void);

/*******************************************************************************
 ******************************************************************************/

#endif // _AUDIO_MANAGER_H_
