#include "vumeter.h"
#include "DRVLEDDISPLAY.h"
#include "FTM.h"
#include "eDMA.h"
#include "PORT.h"

#define LEVEL_ON	(2U)
#define	LEVEL_OFF	(0U)

typedef struct
{
	RGB_t	color;
	uint8_t	state;
}cell_t;


static cell_t SW_matrix[LED_ROWS][LED_COLS];
static MATRIX_BOARD_t DRV_matrix;

void screen_update(void);

void vu_init()
{
	// Initialize LED matrix
	PORT_Init();
	DMA_init();
	FTM_Init();
	LED_DISPLAY_init();

	int i,j;
    for(j=0; j<LED_COLS; j++)
	{
    	SW_matrix[0][j].color.r = LEVEL_ON;
    	SW_matrix[0][j].color.g = LEVEL_OFF;
    	SW_matrix[0][j].color.b = LEVEL_OFF;

    	SW_matrix[1][j].color.r = LEVEL_ON;
    	SW_matrix[1][j].color.g = LEVEL_OFF;
    	SW_matrix[1][j].color.b = LEVEL_OFF;

    	SW_matrix[2][j].color.r = LEVEL_ON;
    	SW_matrix[2][j].color.g = LEVEL_ON;
    	SW_matrix[2][j].color.b = LEVEL_OFF;

    	SW_matrix[3][j].color.r = LEVEL_ON;
    	SW_matrix[3][j].color.g = LEVEL_ON;
    	SW_matrix[3][j].color.b = LEVEL_OFF;

    	SW_matrix[4][j].color.r = LEVEL_OFF;
    	SW_matrix[4][j].color.g = LEVEL_ON;
    	SW_matrix[4][j].color.b = LEVEL_OFF;

    	SW_matrix[5][j].color.r = LEVEL_OFF;
    	SW_matrix[5][j].color.g = LEVEL_ON;
    	SW_matrix[5][j].color.b = LEVEL_OFF;

    	SW_matrix[6][j].color.r = LEVEL_OFF;
    	SW_matrix[6][j].color.g = LEVEL_ON;
    	SW_matrix[6][j].color.b = LEVEL_OFF;

    	SW_matrix[7][j].color.r = LEVEL_OFF;
    	SW_matrix[7][j].color.g = LEVEL_ON;
    	SW_matrix[7][j].color.b = LEVEL_OFF;

    	for(i=0; i<LED_ROWS; i++)
    	{
    		SW_matrix[i][j].state = 1;
    	}
	}
    screen_update();
}

void vu_update(uint8_t bands[F_BAND_N])
{
	uint8_t i,j;
	for(j=0; j<F_BAND_N; j++)
	{
		uint8_t power = bands[j]/32; // Solo me interesan los 3 bits mas altos de resolucion

		for(i=0; i<LED_ROWS; i++)
		{
			if(i<power)
			{
				SW_matrix[i][j].state = 0;
			}
			else
			{
				SW_matrix[i][j].state = 1;
			}
		}
	}
	screen_update();
}

void screen_update(void)
{
	uint8_t i,j;
	cell_t data;
	for(i=0; i<LED_ROWS; i++)
	{
		for(j=0; j<LED_COLS; j++)
		{
			data = SW_matrix[i][j];
			DRV_matrix.table[i][j].r = data.color.r * data.state;
			DRV_matrix.table[i][j].g = data.color.g * data.state;
			DRV_matrix.table[i][j].b = data.color.b * data.state;
		}
	}
	LED_DISPLAY_updateMatrix(DRV_matrix);
}
