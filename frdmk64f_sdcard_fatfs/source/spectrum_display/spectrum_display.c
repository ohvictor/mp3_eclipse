#include "math.h"
#include <stdbool.h>
#include "fsl_device_registers.h"
#include "spectrum_display.h"
#include "rfft.h"
#include "DRV_LEDMatrix.h"
#include "fsl_debug_console.h"
#include "PDRV_GPIO.h"

#define SAMPLE_LENGTH       FFT_SIZE
#define NUMBER_OF_BANDS     8  
#define DISPLAY_HEIGHT      8
#define NOISE               5
#define MAX_AMPLITUDE       200
#define AVERAGE             2

extern LEDMatrix_t rgb_led_matrix;

static arm_rfft_fast_instance_f32 rfft_fast_instance;
static float32_t outputRFFT[SAMPLE_LENGTH];
static float32_t outputMAG[SAMPLE_LENGTH / 2];

// Bins for freqs: 0-100-500-1500-3000-5000-8000-12000-22050
static const int bin[NUMBER_OF_BANDS+1] = {0, 10, 47, 140, 279, 465, 743, 1115, 2048};

static int displayMagValues[NUMBER_OF_BANDS];

void spectrum_display_init()
{
    rfft_init();
    LEDMatrix_init();
}

int spectrum_display_fft(float32_t * inputSignal, float32_t sampleRate)
{ 
    static volatile char average = 0;
    volatile unsigned int usableBins = (SAMPLE_LENGTH / 2 - 1);

    //gpioWrite (PORTNUM2PIN(PB,18), 1);

    rfft(inputSignal, outputRFFT);
    rfft_mag(outputRFFT, outputMAG);

    volatile float32_t temp;
    for (size_t i = 0; i < NUMBER_OF_BANDS; i++)
    {
        temp = 0;
        for (size_t j = (int)(bin[i]); ((j < (int)(bin[i+1]))&&(j < usableBins)); j++)
        {
        	temp += (outputMAG[j] > NOISE) ? outputMAG[j] : 0;
        }
        int roundedHeight = (int)(temp/MAX_AMPLITUDE);
        displayMagValues[i] += (roundedHeight > DISPLAY_HEIGHT ? DISPLAY_HEIGHT : roundedHeight)/AVERAGE;
    } 
    average = (average+1) % AVERAGE;
    if(average == 0)
    {

    	spectrum_display_write(displayMagValues);
    	for(int j = 0; j <  NUMBER_OF_BANDS; j++)
    		 displayMagValues[j] = 0;
    }

    //gpioWrite (PORTNUM2PIN(PB,18), 0);

    return 0;
}

void spectrum_display_write(int * Matrix)
{
    for(int i = 0; i < NUMBER_OF_BANDS; i++)
    {
    	for(int j = 0; j<DISPLAY_HEIGHT; j++)
    	{
    		if(Matrix[i] > j)
			{
				if(j >= 7)
					rgb_led_matrix[i][j] = (color_t){.r=255, .g=0, .b=0}; // red
				else if(j >= 4)
					rgb_led_matrix[i][j] = (color_t){.r=255, .g=255, .b=0}; // yellow
				else
					rgb_led_matrix[i][j] = (color_t){.r=0, .g=255, .b=0}; // green
			}
			else
				rgb_led_matrix[i][j] = (color_t){.r=0, .g=0, .b=0}; // clear
    	}
    }
}

void spectrum_display_clear()
{
    for(int i = 0; i < NUMBER_OF_BANDS; i++)
    {
    	for(int j = 0; j < DISPLAY_HEIGHT; j++)
    	{
    		rgb_led_matrix[i][j] = (color_t){.r=0, .g=0, .b=0};
    	}
    	displayMagValues[i] = 0;
    }
    spectrum_display_update();
}

void spectrum_display_update()
{
    LEDMatrix_updateMatrix();
}
