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

#include "DRV_FILEEXPLORER.h"
#include "DRV_AudioControl.h"
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
#include <arm_math.h>

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

    sdcardInit();
    init_audio_manager();
    initTimers();

    audioControllerInit();

    //gpioMode(PORTNUM2PIN(PB, 20), OUTPUT);
    //gpioMode(PORTNUM2PIN(PB, 18), OUTPUT);
    //gpioMode(PORTNUM2PIN(PD, 2), OUTPUT);
    //gpioWrite(PORTNUM2PIN(PD, 2), 1);
}


/*!
 * @brief Main function
 */
int App_Run(void)
{

	while(!sdcardIsInserted()){}
	sdcardMount();
    
    fileExplorerInit(NULL);
    //gpioWrite(PORTNUM2PIN(PD, 2), 0);
    audioControllerLCDConfigOn();

	while(true){
        loopAudioControl();
		audio_decode_loop();
		lcd_display_loop();
	}

    //test();
    while(true){
        __asm("nop");
    }
}

