/***************************************************************************//**
  @file     DRV_LED_Matrix_alt.c
  @brief    LED marix driver.
  @author   Grupo 1
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <string.h>

#include "PDRV_DMA.h"
#include "PDRV_FTM.h"

#include "DRV_Timers.h"
#include "DRV_LEDMatrix.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define LED_BITS 24

#define GREEN_CHECK 0
#define RED_CHECK	1
#define BLUE_CHECK	2

#define COLOR_RESOLUTION 8

#define FTM_MODULO 75

#define MATRIX0	(uint32_t) 25
#define MATRIX1 (uint32_t) 50
#define MATRIXR (uint32_t) 1

#define MATRIX_BIT_SYMBOL(brightness, i)	(((brightness >> (COLOR_RESOLUTION-1 - i%COLOR_RESOLUTION) & 0x01) == 0) ? MATRIX0 : MATRIX1)

#define PMW_ARRAY_LEN	HEIGHT*WIDTH*LED_BITS

#define MAX_BRIGHTNESS			100

#define LED_MATRIX_DMA_CHNNL	0
#define LED_MATRIX_FTM_CHNNL	0

#define RESET_TICKS		TIMER_MS2TICKS(1)

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef uint32_t PWM_Array_t[PMW_ARRAY_LEN];

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/
LEDMatrix_t rgb_led_matrix;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void LEDMatrix_RGB2PWM();
static void LEDMatrix_reset();
static void LEDMatrix_restart();
static void copyMatrix(LEDMatrix_t source, LEDMatrix_t destination);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static FTM_Config_t ftm_config;
static dma_config_t dma_config;

static PWM_Array_t pwm_led_matrix;

static tim_id_t reset_tim_id;

static uint8_t matrix_brightness = 20.0;

static const uint32_t led_green[COLOR_RESOLUTION] = {MATRIX0, MATRIX0, MATRIX1, MATRIX1, MATRIX0, MATRIX0, MATRIX1, MATRIX1};
static const uint32_t led_red[COLOR_RESOLUTION] = {MATRIX0, MATRIX0, MATRIX1, MATRIX1, MATRIX0, MATRIX0, MATRIX1, MATRIX1};
static const uint32_t led_blue[COLOR_RESOLUTION] = {MATRIX0, MATRIX0, MATRIX1, MATRIX1, MATRIX0, MATRIX0, MATRIX1, MATRIX1};
static const uint32_t led_off[COLOR_RESOLUTION] = {MATRIX0, MATRIX0, MATRIX0, MATRIX0, MATRIX0, MATRIX0, MATRIX0, MATRIX0};

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


void LEDMatrix_init()
{
	initTimers();

	ftm_config.channel = FTM_Channel_0;
	ftm_config.mode = FTM_PWM;
	ftm_config.prescale = FTM_Prescale_1;
	ftm_config.CLK_source = FTM_SysCLK;
	ftm_config.PWM_logic = FTM_High;
	ftm_config.modulo = FTM_MODULO;
	ftm_config.PWM_DC = 20;
	ftm_config.active_low = false;
	ftm_config.DMA_on = true;
	ftm_config.interrupt_on = true;

	dma_config.source_buffer = pwm_led_matrix;
	dma_config.destination_buffer = FTM_getCounterPointer(0, FTM_Channel_0);
	dma_config.request_source = FTM0CH0;
	dma_config.source_offset = sizeof(uint32_t);
	dma_config.destination_offset = 0;
	dma_config.transfer_bytes = sizeof(uint16_t);
	dma_config.major_cycles = PMW_ARRAY_LEN;
	dma_config.wrap_around = sizeof(pwm_led_matrix);
	dma_config.enable_major_int = true;

	reset_tim_id = timerGetId();

	LEDMatrix_RGB2PWM();

	FTM_init(LED_MATRIX_FTM_CHNNL, ftm_config);
	dma_init(LED_MATRIX_DMA_CHNNL, dma_config);
	dma_set_major_callback(LED_MATRIX_DMA_CHNNL, LEDMatrix_reset);

	FTM_start(LED_MATRIX_FTM_CHNNL);
}


void LEDMatrix_updateLED(color_t led, uint8_t height, uint8_t width){
	rgb_led_matrix[height][width] = led;
	LEDMatrix_RGB2PWM();
}

void LEDMatrix_updateMatrix(){
	LEDMatrix_RGB2PWM();
}

void LEDMatrix_setBrightness(float brightness)
{
	matrix_brightness = (brightness > MAX_BRIGHTNESS) ? MAX_BRIGHTNESS : ((brightness < 0) ? 0 : brightness);
}

float LEDMatrix_getBrightness()
{
	return matrix_brightness;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void LEDMatrix_RGB2PWM()
{
	uint8_t LED_counter = 0;

	for(uint8_t x = 0; x < HEIGHT; x++){
		for(uint8_t y = 0; y < WIDTH; y++){

            if(rgb_led_matrix[x][y].g > 0){
                memcpy(&pwm_led_matrix[(LED_counter * LED_BITS) + GREEN_CHECK*COLOR_RESOLUTION], led_green, sizeof(uint32_t)*COLOR_RESOLUTION);
            }
            else{
                memcpy(&pwm_led_matrix[(LED_counter * LED_BITS) + GREEN_CHECK*COLOR_RESOLUTION], led_off, sizeof(uint32_t)*COLOR_RESOLUTION);
            }
            
            if(rgb_led_matrix[x][y].r > 0){
                memcpy(&pwm_led_matrix[(LED_counter * LED_BITS) + RED_CHECK*COLOR_RESOLUTION], led_red, sizeof(uint32_t)*COLOR_RESOLUTION);
            }
            else{
                memcpy(&pwm_led_matrix[(LED_counter * LED_BITS) + RED_CHECK*COLOR_RESOLUTION], led_off, sizeof(uint32_t)*COLOR_RESOLUTION);
            }
            
            if(rgb_led_matrix[x][y].b > 0){
                memcpy(&pwm_led_matrix[(LED_counter * LED_BITS) + BLUE_CHECK*COLOR_RESOLUTION], led_blue, sizeof(uint32_t)*COLOR_RESOLUTION);
            }
            else{
                memcpy(&pwm_led_matrix[(LED_counter * LED_BITS) + BLUE_CHECK*COLOR_RESOLUTION], led_off, sizeof(uint32_t)*COLOR_RESOLUTION);
            }

			LED_counter++;
		}
	}
}

static void LEDMatrix_reset(){
	FTM_stop(LED_MATRIX_FTM_CHNNL);
	timerStart(reset_tim_id, RESET_TICKS, TIM_MODE_SINGLESHOT, LEDMatrix_restart);
}

static void LEDMatrix_restart(){
	LEDMatrix_RGB2PWM();
	FTM_start(LED_MATRIX_FTM_CHNNL);
}

// Function to copy elements from one 8x8 matrix to another
static void copyMatrix(LEDMatrix_t source, LEDMatrix_t destination) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            destination[i][j] = source[i][j];
        }
    }
}
