/*******************************************************************************
  @file     DRVLEDDISPLAY.C
  @brief    LED Matrix driver
  @author   Victor Oh
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "DRVLEDDISPLAY.h"
#include "eDMA.h"
#include "FTM.h"
#include "PORTConfig.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

typedef enum {
	BUFFER_A,
	BUFFER_B
}PPBuffer_t;

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

/**
 * 75
 * 21
 * 54
 */

#define PWM_MOD		75 //Con prescaler setteado en 1->resolucion 20ns. 63*20ns = 1,26us es el periodo que necesita el ws2812
#define PWM_LOW		21 // 19/63 = 0.3 ->Duty cycle que indica un bit in =0
#define PWM_HIGH	54  // 37/63 = 0.58 -> Duty cycle que indica un bit in =1

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

uint32_t rgb_2_grb(RGB_t color);

static void swapWriteBuffer();
static void RGB2PWMLevel(); //Translates bit to PWM value
void game_matrix_init(void);


void DMA0_IRQCallback();  // Swaps buffer and disables DMA and FTM channels


/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/**
 * Ping Pong buffers
 */

static PPBuffer_t write_buffer; //Determines which Buffer is available for cpu to write on, flips after DMA transfer (display update)
static uint16_t BufferA[LED_ROWS][LED_COLS][LED_BITS];
static uint16_t BufferB[LED_ROWS][LED_COLS][LED_BITS];

static RGB_t matrix_data[LED_ROWS][LED_COLS];

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/*	LED_DISPLAY_init()*/
//Sets up FTM3 channel 0
//Sets up DMA channel 0
//Initializes game_matrix to 0 and defaults brightness
void LED_DISPLAY_init()
{
	//Set up FTM03 PWM mode  period = 1,25uS Allowing  DMA Transfers
	// PTD 0 as PWM

	PCRstr UserPCR;

	UserPCR.PCR=false;						// Default All false, Set only those needed

	UserPCR.FIELD.DSE=true;
	UserPCR.FIELD.MUX=PORT_mAlt4;			// FTM3 CH0
	UserPCR.FIELD.IRQC=PORT_eDisabled;

	port_config_pcr(PORTD,0,UserPCR);		// PTD0 FTM Output

	//  Set FTM configuration
	FTM_SetPrescaler(FTM3, FTM_PSC_x1); //20ns resolution
	FTM_SetInterruptMode (FTM3,FTM_CH_0, true); 					// Enable interrupts

	//	Set FTM as PWM mode
	FTM_SetWorkingMode(FTM3, 0, FTM_mPulseWidthModulation);			// MSA  / B
	FTM_SetPulseWidthModulationLogic(FTM3, 0, FTM_lAssertedHigh);   // ELSA / B

	//  Set PWWM Modulus and initial Duty
	FTM_SetModulus(FTM3, PWM_MOD); //Valores iniciales raros
	FTM_SetCounter(FTM3, 0, 30);

	//  Enable FTM3-CH0 DMA Request
	FTM_DmaMode(FTM3,FTM_CH_0,FTM_DMA_ON); // DMA ON
	//FTM_StartClock(FTM0);


	//Configuro Fuente de request para DMA Cambiar a FTM3
	DMA_setChannelRequestSource(0,mDmaRequestMux0FTM3Channel0);

	//Cuentas para tamanios de transferencias

	int totalBytes = sizeof(BufferA);
	int totalElements = (sizeof(BufferA)/sizeof(BufferA[0][0][0]));

	/*int totalBytes2 = sizeof(resetBuffer);
	int totalElements2 = (sizeof(resetBuffer)/sizeof(resetBuffer[0]));
	*/

	int totalBytes3 = sizeof(BufferB);
	int totalElements3 = (sizeof(BufferB)/sizeof(BufferB[0][0][0]));


	//TCD0 Fuente FTM3 source BufferA dest PWMcontrol SOFF 2bytes DOFF 0 SSize Dsize 2bytes SLAST=-64 SGA TCD1 IRQ Cambio buffer de escritura y lectura

	DMA_setChannelDataSource(CHANNEL_0,0,BufferA,2);
	DMA_setChannelDataDestination(CHANNEL_0,0,(&(FTM3->CONTROLS[0].CnV)),0);
	DMA_setMinLoopTransferSize(CHANNEL_0,0,TWO_BYTE,TWO_BYTE,2);
	DMA_setMajorLoopTransfer(CHANNEL_0,0,totalElements,totalElements,-totalBytes);

	/*
	//TCD1 Fuente FTM3 source 0  des PWMControl Soff 0 Doff 0 Citer 1.25*1/refrate SLAST 0 SGA TCD2 Deshabilitar IRQ(?)
	DMA_setChannelDataSource(0,1,resetBuffer,1);
	DMA_setChannelDataDestination(0,1,(&(FTM0->OUTMASK)),0);
	DMA_setMinLoopTransferSize(0,1,ONE_BYTE,ONE_BYTE,1);
	DMA_setMajorLoopTransfer(0,1,totalElements2,totalElements2,0);
	*/

	//TCD2 Fuente FTM3 source BufferB dest PWMControl Soff 2 bytes, Doff 0 bytes Dsize/SSize 2 bytes SLAST -64 SGA TCD0 IRQ Cambio buffer de escritura
	DMA_setChannelDataSource(CHANNEL_0,1,BufferB,2);
	DMA_setChannelDataDestination(CHANNEL_0,1,(&(FTM3->CONTROLS[0].CnV)),0);
	DMA_setMinLoopTransferSize(CHANNEL_0,1,TWO_BYTE,TWO_BYTE,2);
	DMA_setMajorLoopTransfer(CHANNEL_0,1,totalElements3,totalElements3,-totalBytes3);

	/*
	//TCD3 idem al resetBuffer

	DMA_setChannelDataSource(0,3,resetBuffer,1);
	DMA_setChannelDataDestination(0,3,(&(FTM0->OUTMASK)),0);
	DMA_setMinLoopTransferSize(0,3,ONE_BYTE,ONE_BYTE,1);
	DMA_setMajorLoopTransfer(0,3,totalElements2,totalElements2,0);
	*/

	// Habilito interrupciones al final de cada major loop
	DMA_enableMajorInterrupt(CHANNEL_0,0);
	DMA_enableMajorInterrupt(CHANNEL_0,1);
	/*DMA_enableMajorInterrupt(0,2);
	DMA_enableMajorInterrupt(0,3);
	*/
	//Habilito Scatter and Gather en todos los TCDs

	DMA_enableScatterandGather(CHANNEL_0,0);
	DMA_enableScatterandGather(CHANNEL_0,1);
	/*DMA_enableScatterandGather(0,2);
	DMA_enableScatterandGather(0,3);
	*/

	//Armo el camino de scatter and gather para cada tcd

	DMA_setScatterAndGather(CHANNEL_0,0,1);
	DMA_setScatterAndGather(CHANNEL_0,1,0);
	/*DMA_setScatterAndGather(0,2,3);
	DMA_setScatterAndGather(0,3,0);
	*/
	//Paso callback de interrupcion
	DMA_setChannelIRQ(0,DMA0_IRQCallback);

	//Settro TCD0 como DMA->TCD[0]
	DMA_setChannelTCD(CHANNEL_0,0);

	//setColor(255,0,0);
	write_buffer = BUFFER_A;

	// TODO: CHANGE
	//Init values for game_matrix and both buffers set to 0

	game_matrix_init();

	RGB2PWMLevel();

	swapWriteBuffer();

	RGB2PWMLevel();
	DMA_enableChannel(CHANNEL_0);
}

void LED_DISPLAY_updateMatrix(MATRIX_BOARD_t modelMatrix)
{
	int i,j;
	for(i=0; i<LED_ROWS; i++)
	{
		for(j=0; j<LED_COLS; j++)
		{
			matrix_data[i][j] = modelMatrix.table[i][j];
		}
	}

	RGB2PWMLevel();						//Translates individual bits in rgb dot to Duty Cycle settings according to ws2812's needs

	FTM_StartClock(FTM3);				//Starts Clock and DMA module, transmitting bit information to the Led Matrix
	DMA_enableChannel(CHANNEL_0);

}
/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


/*LED_DISPLAY_updateMatrix(void* modelMatrix)*/
// Called upon every time the model updates
// Starts transmission to LED Display



/*	Static file scope function Definitions	*/

static void RGB2PWMLevel() // Translates logical level to timings required by the display
{
	int i,j,k;

	uint16_t* write_addr;
	if(write_buffer == BUFFER_A)
		write_addr = &BufferA[0][0][0];
	else if(write_buffer == BUFFER_B)
		write_addr = &BufferB[0][0][0];

	for (i=0; i<LED_ROWS; i++)
	{
		for (j=0; j<LED_COLS; j++)
		{
			uint32_t color = rgb_2_grb(matrix_data[i][j]);
			uint16_t* led_addr = write_addr + i*LED_COLS*LED_BITS + j*LED_BITS;
			for (k=0; k<LED_BITS; k++)
			{
				if(color & (0x00800000 >> k))
					*(led_addr + k) = PWM_HIGH;
				else
					*(led_addr + k) = PWM_LOW;
			}
		}
	}
}

static void swapWriteBuffer() //Switches between active write buffer
{
	if(write_buffer == BUFFER_A)
	{
		write_buffer = BUFFER_B;
	}
	else
	{
		write_buffer = BUFFER_A;
	}
	return;
}

void game_matrix_init(void)
{
	int i,j;
	for (i = 0; i < LED_ROWS; ++i)
	{
		for (j = 0; j < LED_COLS; ++j)
		{
			if(i<8)
			{
				matrix_data[i][j].g = (i%2)*128 + j*16;
				matrix_data[i][j].r = (i%2)*128 + j*16;
				matrix_data[i][j].b = (i%2)*128 + j*16;
			}

			if(i<6)
			{
				matrix_data[i][j].g = 0;
				matrix_data[i][j].r = 0;
				matrix_data[i][j].b = (i%2)*128 + j*16;
			}

			if(i<4)
			{
				matrix_data[i][j].g = 0;
				matrix_data[i][j].r = (i%2)*128 + j*16;
				matrix_data[i][j].b = 0;
			}

			if(i<2)
			{
				matrix_data[i][j].g = (i%2)*128 + j*16;
				matrix_data[i][j].r = 0;
				matrix_data[i][j].b = 0;
			}
		}

	}
}

uint32_t rgb_2_grb(RGB_t color)
{
	uint32_t retval = 0;
	retval = retval | (color.g<<(8*2)) | (color.r << (8*1)) | (color.b << (8*0));
	return retval;
}

/*	Interrupt CallBack Definition */

void DMA0_IRQCallback() //Should swap pingpong buffer between screen refresh, allows cpu to update game output.
{
	swapWriteBuffer();
	FTM_StopClock(FTM3);
	DMA_disableChannel(CHANNEL_0);
}
