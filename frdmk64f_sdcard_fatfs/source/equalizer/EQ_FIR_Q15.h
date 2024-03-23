#include "Util.h"
#include "arm_math.h"
#include "math_helper.h"

#define NUM_FILTERS   3

static const float32_t noGains[NUM_FILTERS] =
{
	1.0f/3.0f, 1.0f/3.0f, 1.0f/3.0f
};

static const float32_t JazzGains[NUM_FILTERS] =
{
    2.0f/3.5f, 0.5f/3.5f, 1.0f/3.5f
};

static const float32_t rockGains[NUM_FILTERS] =
{
    1.2f/3.2f, 0.8f/3.2f, 1.2f/3.2f
};

static const float32_t popGains[NUM_FILTERS] =
{
    0.5f/3.5f, 1.5f/3.5f, 1.5f/3.5f
};

static const float32_t classicGains[NUM_FILTERS] =
{
    1.2f/4.0f, 0.8f/4.0f, 2.0f/4.0f
};

static const float32_t * EQGains[] = {noGains, JazzGains, rockGains, popGains, classicGains};

enum eq_types{NO_EQ, JAZZ_EQ, ROCK_EQ, POP_EQ, CLASSIC_EQ};

void eq_init(uint32_t frameSize);
void eq_filter_frame(q15_t * inputF32, int16_t * outputI32, uint32_t frameSize);
void eq_filter_sample(q15_t inputI16, int16_t * outputI16);
void eq_set_frame_size(uint32_t frameSize);
void eq_set_eq(uint8_t eq);
