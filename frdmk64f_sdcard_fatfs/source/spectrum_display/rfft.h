#ifndef _RFFT_H
#define _RFFT_H

#include <stdio.h>
#include <stdint.h>
#include "arm_math.h"

/**
 * @file rfft.h
 * @brief Header file for a Real Fast Fourier Transform (RFFT) module.
 */

// Constant for the size of the FFT
#define FFT_SIZE    4096

/**
 * @brief Initializes the RFFT module.
 *
 * This function initializes the necessary resources for the RFFT module.
 */
void rfft_init();

/**
 * @brief Computes the Real Fast Fourier Transform (RFFT).
 *
 * @param inputSignal The input signal for the RFFT.
 * @param outputRFFT The output RFFT data.
 * @return 0 if successful, -1 on error.
 *
 * This function computes the RFFT of the input signal and stores the result in the outputRFFT array.
 */
int rfft(float32_t *inputSignal, float32_t *outputRFFT);

/**
 * @brief Computes the magnitude of a given RFFT output.
 *
 * @param inputRFFT The input RFFT data.
 * @param outputMAG The output magnitude data.
 * @return 0 if successful, -1 on error.
 *
 * This function computes the magnitude of the RFFT output and stores the result in the outputMAG array.
 */
int rfft_mag(float32_t *inputRFFT, float32_t *outputMAG);

#endif