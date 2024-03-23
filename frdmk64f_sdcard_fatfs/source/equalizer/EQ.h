/**
 * @file EQ.c
 *
 * @brief Initialization and usage of an equalizer library.
 */

#include "arm_math.h"
#include "math_helper.h"

/**
 * @brief Initializes the equalizer with the given frame size.
 *
 * This function initializes the equalizer with the specified frame size.
 *
 * @param frameSize The size of the frame for the equalizer.
 */
void initEQ(uint32_t frameSize);

/**
 * @brief Applies the equalizer filter to the input frame and produces the output frame.
 *
 * This function applies the equalizer filter to the input frame and produces the output frame.
 *
 * @param inputF32 Pointer to the input frame (float32_t array).
 * @param outputF32 Pointer to the output frame (float32_t array).
 */
void eqFilterFrame(float32_t * inputF32, float32_t * outputF32);