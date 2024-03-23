/***************************************************************************/ /**
   @file     spectrum_app_test.c
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

#include "spectrum_display.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

// #define MAIN_DEBUG
#define SAMPLE_RATE 24000                            // Sample rate in Hz
#define DURATION 0.75                                 // Duration of the wave in seconds
#define SAMPLES ((uint32_t)(SAMPLE_RATE * DURATION)) // Num of samples

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
static float32_t waveform[SAMPLES];

/*******************************************************************************
 * Code
 ******************************************************************************/

int App_Init()
{
  BOARD_InitPins();
  BOARD_BootClockRUN();
  BOARD_InitDebugConsole();
  SYSMPU_Enable(SYSMPU, false);

  // Frequency of the square wave
  float32_t frequency = 1000.0;

  // Generate the square wave
  int i;
  for (i = 0; i < SAMPLES; i++)
  {
    float32_t t = (float32_t)i / SAMPLE_RATE;
    waveform[i] = (0.75*sin(2 * PI * 100 * t)) + (0.5*sin(2 * PI * 440 * t)) + (0.1*sin(2 * PI * 1000 * t)); //>= 0) ? 1.0 : -1.0;
  }

  initTimers();

  // debug
  //gpioMode(PORTNUM2PIN(PB,19), OUTPUT);
  //gpioWrite (PORTNUM2PIN(PB,19), 0);
  //gpioMode(PORTNUM2PIN(PB,18), OUTPUT);
  //gpioWrite (PORTNUM2PIN(PB,18), 0);
  //


  spectrum_display_init(); // Last init
  spectrum_display_clear();
}

/*!
 * @brief Main function
 */
int App_Run(void)
{

  for (int i = 0; (i + FFT_SIZE) < SAMPLES; i += FFT_SIZE)
  {

	// gpio = 1
	//gpioWrite(PORTNUM2PIN(PB,19), 1);
    if (spectrum_display_fft((waveform + i), SAMPLE_RATE) == 0)
    {
      spectrum_display_update();
    }
    else
    {
      //printf("Sample FFT failed!");
      spectrum_display_clear();
    }
    //gpioWrite (PORTNUM2PIN(PB,19), 0);
    timerDelay(100);
  }


  while (1);
}
