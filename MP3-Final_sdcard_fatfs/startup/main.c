/***************************************************************************//**
  @file     main.c
  @brief    FW main
  @author   Grupo 5
 ******************************************************************************/

#include "hardware.h"

#include "fsm_states.h"
#include "evQueue.h"
#include "fsm_motor.h"
#include "gpio.h"
#include "board.h"
#include "systick.h"
#include "time.h"
#include "controls.h"
#include "filesystem.h"
#include "mp3decoder.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

void App_Init (void);
void App_Run (void);


int main (void)
{
    hw_Init();
    hw_DisableInterrupts();
    App_Init(); /* Program-specific setup */
    hw_EnableInterrupts();

    int result = filesystem_init();

    result = filesystem_map_files();

    int files = filesystem_get_files_count();

    char * folder1 = filesystem_open_folder();

    folder1 = filesystem_open_folder();

    folder1 = filesystem_open_folder();

    char * path = filesystem_get_path();

    audio_player_init();

    int num = audio_player_load_song(path);

    ID3Tag_t tag;

    mp3_get_tag_data(tag);

    num = audio_player_play();

    //__FOREVER__
     //   App_Run(); /* Program-specific loop  */
}
