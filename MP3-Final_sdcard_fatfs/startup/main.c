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
#include "LCD_GDM1602A.h"

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

    LCD_Init();

    LCD_writeStrInPos("DEFAULT             ", 16, 0, 0);

	int result = filesystem_init();

	filesystem_map_files();

	file_t file = filesystem_get_first_file();

	bool isMP3 = filesystem_is_MP3file("/DIR_1/coldplay.mp3");

	audio_player_init();
//ColdplayASkyFullOfStars
	audio_player_load_song("/DIR_1/coldplay1.mp3");

	uint16_t song = audio_player_play();

	FSM_start();

    __FOREVER__
        App_Run(); /* Program-specific loop  */
}
