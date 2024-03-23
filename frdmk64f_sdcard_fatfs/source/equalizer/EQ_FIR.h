#include "Util.h"
#include "arm_math.h"
#include "math_helper.h"

#define NUM_FILTERS   5

static const float32_t noGains[NUM_FILTERS] =
{
	1.0f, 1.0f, 1.0f, 1.0f, 1.0f
};

static const float32_t JazzGains[NUM_FILTERS] =
{
    2.0f, 0.5f, 1.0f, 1.0f, 1.5f
};

static const float32_t rockGains[NUM_FILTERS] =
{
    0.2f, 0.5f, 1.0f, 1.0f, 0.5f
};

static const float32_t popGains[NUM_FILTERS] =
{
    0.2f, 1.5f, 1.0f, 0.5f, 1.5f
};

static const float32_t classicGains[NUM_FILTERS] =
{
    1.5f, 1.0f, 1.0f, 0.5f, 1.5f
};

static const float32_t * EQGains[] = {noGains, JazzGains, rockGains, popGains, classicGains};

enum eq_types{NO_EQ, JAZZ_EQ, ROCK_EQ, POP_EQ, CLASSIC_EQ};

void eq_init(uint32_t frameSize);
void eq_filter_frame(float32_t * inputF32, int16_t * outputF32, uint32_t frameSize);
void eq_filter_sample(int16_t inputI16, float32_t * outputF32);
void eq_set_frame_size(uint32_t frameSize);
void eq_set_eq(uint8_t eq);
