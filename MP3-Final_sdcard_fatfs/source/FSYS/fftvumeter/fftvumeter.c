/***************************************************************************/ /**
   @file		fftvumeter.c
   @brief
   @author
   @date
  ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "arm_math.h"
#include "math.h"
#include <stdbool.h>
#include "fsl_device_registers.h"
#include "fftvumeter.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define SAMPLE_LENGTH FFT_SIZE
#define NUMBER_OF_BANDS 8
#define VUMETER_HEIGHT 8
#define NOISE 5
#define MAX_AMPLITUDE 50
#define AVERAGE 2

static arm_rfft_fast_instance_f32 rfft_fast_instance;
static float32_t output[SAMPLE_LENGTH];
static float32_t outputfft[SAMPLE_LENGTH / 2];
static int vumeterMatrix[NUMBER_OF_BANDS];

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void fftvumeter_init()
{
    arm_rfft_fast_init_f32(&rfft_fast_instance, SAMPLE_LENGTH);
}

bool fftvumeter_calculate_power(float32_t *signal, float32_t sampleRate, int lowerFreqBand, int higherFreqBand)
{
    //Clean up matrix
    for (int j = 0; j < NUMBER_OF_BANDS; j++)
    {
        vumeterMatrix[j] = 0;
    }


    arm_rfft_fast_f32(&rfft_fast_instance, signal, output, 0);


    static volatile char average = 0;
    volatile unsigned int usableBins = (SAMPLE_LENGTH / 2 - 1);
    volatile float32_t bin_width = SAMPLE_LENGTH / sampleRate;

    volatile float32_t base = higherFreqBand / lowerFreqBand;
    volatile float32_t exp = 1.0f / (NUMBER_OF_BANDS - 1.0f);
    volatile float freq_step = powf(base, exp);


    for (uint16_t j = 0; j < SAMPLE_LENGTH / 2; j++)
    {
        volatile float32_t temp = output[2 * j] * output[2 * j] + output[2 * j + 1] * output[2 * j + 1];
        arm_sqrt_f32(temp, &outputfft[j]);
    }

    outputfft[0] = 0;

    volatile float curr_bin_freq = lowerFreqBand;
    volatile float next_bin_freq = lowerFreqBand * freq_step;
    volatile float curr_center_bin;
    volatile float next_bin_center;
    volatile float bin_lower_limit = 0;
    volatile float bin_upper_limit;
    volatile float temp;

    for (size_t i = 0; i < NUMBER_OF_BANDS; i++)
    {
        curr_center_bin = next_bin_freq * bin_upper_limit;
        next_bin_center = next_bin_freq * bin_upper_limit;
        bin_lower_limit = i ? bin_upper_limit : 0;
        bin_upper_limit = (curr_center_bin + next_bin_center) * 0.5;

        temp = 0;
        for (size_t j = (int)(bin_lower_limit); ((j < (int)(bin_upper_limit)) && (j < usableBins)); j++)
        {
            temp += (outputfft[j] > NOISE) ? outputfft[j] : 0;
        }
        int roundedHeight = (int)(temp / MAX_AMPLITUDE);

        vumeterMatrix[i] += (roundedHeight > VUMETER_HEIGHT ? VUMETER_HEIGHT : roundedHeight) / AVERAGE;

        curr_bin_freq = next_bin_freq;
        next_bin_freq *= freq_step;
    }

    average = (average + 1) % AVERAGE;

    return true;
}


uint8_t* fftvumeter_get_power()
{
    return vumeterMatrix;
}
