
#include "EQ_FIR_Q15.h"
#include "arm_math.h"
#include "math_helper.h"

#include "fsl_debug_console.h"

#define TAPS    64
#define BLOCK_SIZE  64

static uint8_t selected_eq = NO_EQ;

//static q15_t filter_state[NUM_FILTERS][TAPS+BLOCK_SIZE-1];
static q15_t filter_state_0[TAPS+BLOCK_SIZE-1];
static q15_t filter_state_1[TAPS+BLOCK_SIZE-1];
static q15_t filter_state_2[TAPS+BLOCK_SIZE-1];

//static arm_fir_instance_q15 filter_instance[NUM_FILTERS];
static arm_fir_instance_q15 filter_instance_0;
static arm_fir_instance_q15 filter_instance_1;
static arm_fir_instance_q15 filter_instance_2;

static int16_t mid_buffer[BLOCK_SIZE];

static uint32_t eqFrameSize;
static q15_t output[TAPS];

static const q15_t bass_filter[TAPS] __attribute__((aligned(16))) =  {-47, -46, -47, -48, -50, -50, -49, -43, -33, -16, 9, 42,
																		86, 140, 205, 281, 368, 464, 569, 682, 799, 920, 1041, 1160,
																		1275, 1382, 1480, 1566, 1637, 1692, 1729, 1748, 1748, 1729, 1692,
																		1637, 1566, 1480, 1382, 1275, 1160, 1041, 920, 799, 682, 569,
																		464, 368, 281, 205, 140, 86, 42, 9, -16, -33, -43, -49, -50,
																		-50, -48, -47, -46, -47};

static const q15_t mid_filter[TAPS] __attribute__((aligned(16))) = {-35, -39, -1, -3, -68, -85, -13, -18, -162, -198, -40, -49,
																	-340, -399, -75, -82, -611, -695, -82, -71, -988, -1104, 25, 91,
																	-1543, -1742, 511, 800, -2839, -3716, 4084, 13944, 13944, 4084,
																	-3716, -2839, 800, 511, -1742, -1543, 91, 25, -1104, -988, -71,
																	-82, -695, -611, -82, -75, -399, -340, -49, -40, -198, -162, -18,
																	-13, -85, -68, -3, -1, -39, -35};

static const q15_t treble_filter[TAPS] __attribute__((aligned(16))) = {20, -26, -29, 28, 17, 50, -121, 31, -37, 217, -130, -64,
																		-153, 217, 254, -245, -142, -398, 919, -220, 254, -1416, 822,
																		397, 935, -1347, -1637, 1696, 1111, 3807, -12805, 7993, 7993,
																		-12805, 3807, 1111, 1696, -1637, -1347, 935, 397, 822, -1416,
																		254, -220, 919, -398, -142, -245, 254, 217, -153, -64, -130,
																		217, -37, 31, -121, 50, 17, 28, -29, -26, 20};


static const q15_t * eqFIRCoef[NUM_FILTERS] =
{
	bass_filter,
    mid_filter,
    treble_filter
};

void eq_init(uint32_t frameSize)
{	
	//for (uint32_t i=0; i < NUM_FILTERS; i++)
	//{
		if (arm_fir_init_q15(&filter_instance_0, TAPS, bass_filter, filter_state_0, BLOCK_SIZE) == ARM_MATH_ARGUMENT_ERROR){
			PRINTF("filter 0 init error");
		}
		if (arm_fir_init_q15(&filter_instance_1, TAPS, mid_filter, filter_state_1, BLOCK_SIZE) == ARM_MATH_ARGUMENT_ERROR){
			PRINTF("filter 1 init error");
		}
		if (arm_fir_init_q15(&filter_instance_2, TAPS, treble_filter, filter_state_2, BLOCK_SIZE) == ARM_MATH_ARGUMENT_ERROR){
			PRINTF("filter 2 init error");
		}
	//}

  eqFrameSize = frameSize;
}

void eq_filter_frame(q15_t * inputF32, int16_t * outputI32, uint32_t frameSize)
{ 
	for (uint32_t i=0; i < frameSize/(BLOCK_SIZE); i++)
	{
	  //for (uint32_t j=0; j < NUM_FILTERS; j++)
	  //{
			arm_fir_fast_q15(&(filter_instance_0), inputF32 + (i * BLOCK_SIZE), output, BLOCK_SIZE);
			for (uint32_t k=0; k < BLOCK_SIZE; k++)
			{
				mid_buffer[k] += (int16_t)((EQGains[selected_eq][0] * output[k])); //(int16_t) (((uint32_t) ((EQGains[selected_eq][j] * output[k]) + 32768)) >> 4);
			}
			arm_fir_fast_q15(&(filter_instance_1), inputF32 + (i * BLOCK_SIZE), output, BLOCK_SIZE);
			for (uint32_t k=0; k < BLOCK_SIZE; k++)
			{
				mid_buffer[k] += (int16_t)((EQGains[selected_eq][1] * output[k])); //(int16_t) (((uint32_t) ((EQGains[selected_eq][j] * output[k]) + 32768)) >> 4);
			}
			arm_fir_fast_q15(&(filter_instance_2), inputF32 + (i * BLOCK_SIZE), output, BLOCK_SIZE);
			for (uint32_t k=0; k < BLOCK_SIZE; k++)
			{
				mid_buffer[k] += (int16_t)((EQGains[selected_eq][2] * output[k])); //(int16_t) (((uint32_t) ((EQGains[selected_eq][j] * output[k]) + 32768)) >> 4);
				//PRINTF("%d ", outputI32[k + (i * BLOCK_SIZE)]);
				//if(j==NUM_FILTERS-1){
					outputI32[k + (i * BLOCK_SIZE)] = (int16_t) (((uint32_t)(mid_buffer[k] + 32768)) >> 4);
					//PRINTF("%d ", outputI32[k + (i * BLOCK_SIZE)]);
					mid_buffer[k] = 0;
				//}
			}
	  //}
	}
}

void eq_filter_sample(q15_t inputI16, int16_t * outputI16)
{
	  q15_t output_buffer;
	  float32_t gain;
	  //for (uint32_t j=0; j < NUM_FILTERS; j++)
	  //{
		//  arm_fir_fast_q15(&(filter_instance[j]), &inputI16, &output_buffer, 1);
		//  gain = EQGains[selected_eq][j];
		  //*outputI16 += (int16_t)((gain * output_buffer));
		//  *outputI16 = inputI16;
		  //if(j == NUM_FILTERS-1){
		  //	  outputI16 = (int16_t) (((uint32_t)(series_buffer + 32768)) >> 4);
		  //	  series_buffer = 0;
		  //}
	  //}
}

void eq_set_frame_size(uint32_t frameSize)
{
	eqFrameSize = frameSize;
}

void eq_set_eq(uint8_t eq){
	selected_eq = eq;
}
