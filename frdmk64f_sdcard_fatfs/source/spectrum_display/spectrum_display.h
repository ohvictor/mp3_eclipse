#ifndef _SPECTRUM_DISPLAY_H
#define _SPECTRUM_DISPLAY_H

#include <stdio.h>
#include <stdint.h>
#include "arm_math.h"

/**
 * @file spectrum_display.h
 * @brief Header file for a spectrum display module.
 */

// Constants for FFT size, minimum frequency, and maximum frequency
#define FFT_SIZE    4096
#define MIN_FREQ    20
#define MAX_FREQ    20000

/**
 * @brief Initializes the spectrum display.
 */
void spectrum_display_init();

/**
 * @brief Computes the Fast Fourier Transform (FFT) for the display.
 *
 * @param inputSignal The input signal for the FFT.
 * @param sampleRate The sample rate of the input signal.
 * @return 0 if successful, -1 on error.
 */
int spectrum_display_fft(float32_t *inputSignal, float32_t sampleRate);

/**
 * @brief Writes a given point matrix to the display.
 *
 * @param matrix The point matrix to be displayed.
 */
void spectrum_display_write(int *matrix);

/**
 * @brief Updates the display.
 */
void spectrum_display_update();

/**
 * @brief Clears the display.
 */
void spectrum_display_clear();

#endif
