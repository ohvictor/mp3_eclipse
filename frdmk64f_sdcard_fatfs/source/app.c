/***************************************************************************//**
  @file     app.c
  @brief    Application 
  @author   Matias Bergerman, Pablo Gonzalez Leiro, Milagros Moutin, Pedro Carranza Velez
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_sysmpu.h"
#include "fsl_debug_console.h"

#include "DRV_SD.h"
#include "audio_manager.h"
#include "PDRV_GPIO.h"
#include "DRV_Timers.h"
#include "mp3decoder.h"
#include "lcd_display/lcd_display.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define MAIN_DEBUG

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/


int App_Init(){
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    SYSMPU_Enable(SYSMPU, false);

    //sdcardInit();
    //init_audio_manager();
    //initTimers();
    
    lcd_display_init();

}



/*!
 * @brief Main function
 */
int App_Run(void)
{
	lcd_display_config();
	timerDelay(100);
	lcd_display_clear();
	timerDelay(100);
	lcd_display_write_string("holi como andas todo bien :3");

	/*while(!sdcardIsInserted()){}
	sdcardMount();

	while(true){
		if(audio_load_song("reality_24000.mp3")){
			audio_print_tag_data();
			audio_start_playing();

			while(audio_decode_loop()){ }

			audio_stop_playing();
		}
	}

    //test();
     *
     * */
    while(true){
        __asm("nop");
    }
}

