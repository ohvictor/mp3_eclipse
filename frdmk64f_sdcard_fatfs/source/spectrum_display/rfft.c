#include "arm_math.h"
#include "math.h"
#include <stdbool.h>
#include "rfft.h"

static arm_rfft_fast_instance_f32 rfft_fast_instance;
static float32_t output[FFT_SIZE];


void rfft_init()
{
    arm_rfft_fast_init_f32(&rfft_fast_instance, FFT_SIZE);
}


int rfft(float32_t * inputSignal, float32_t * outputRFFT)
{ 
    arm_rfft_fast_f32(&rfft_fast_instance, inputSignal, outputRFFT, 0);
    return 0;
}

int rfft_mag(float32_t * inputRFFT, float32_t * outputMAG)
{
    for(uint16_t j = 0; j < FFT_SIZE/2; j++)
    {
        volatile float32_t temp = inputRFFT[2*j]*inputRFFT[2*j] + inputRFFT[2*j+1]*inputRFFT[2*j+1];
        arm_sqrt_f32(temp, &outputMAG[j]);
    }
    outputMAG[0] = 0; //Removing DC
    return 0;
}
