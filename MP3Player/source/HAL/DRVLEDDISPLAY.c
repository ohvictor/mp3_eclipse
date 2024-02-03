/*
 * DRVLEDDISPLAY.c
 *
 *      Author: Francisco Musich
 */

#include "DRVLEDDISPLAY.h"

/*	Global Static Variables declaration and definition	*/

static float display_brightness; //Should be less than one. Constant used to multiply by each RGB element

static RGB_t dot_color; //No use as of yet


static PPBuffer_t write_buffer; //Determines which Buffer is available for cpu to write on, flips after DMA transfer (display update)

static RGB_t game_matrix[LED_DISPLAY_HEIGHT][LED_DISPLAY_HEIGHT]; //contains RGB values which will be sent to the display

static uint16_t BufferA[LED_DISPLAY_HEIGHT][LED_DISPLAY_WIDTH][BITS_PER_CELL]; //PWM values which represent each bit of each RGB value, for every LED on the display
static uint16_t BufferB[LED_DISPLAY_HEIGHT][LED_DISPLAY_WIDTH][BITS_PER_CELL];


//static uint8_t resetBuffer[]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0}; //Cantidad de ticks que tengo que deshabilitar la salida del FTM para que se haga el reset.


/*	Local Functions declaration	*/

static void swapWriteBuffer();
static void RGB2PWMLevel(); //Translates bit to PWM value
void DMA0_IRQCallback();  // Swaps buffer and disables DMA and FTM channels
void applyBrightnessSetting();
void update_game_matrix(int modelMatrix[LED_DISPLAY_HEIGHT][LED_DISPLAY_WIDTH]);
void game_matrix_init(RGB_t initColor);

/*	GLOBAL FUNCTION DEFINITIONS	*/

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

	PORT_Configure2 (PORTD,0,UserPCR);		// PTD0 FTM Output

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
	FTM_DmaMode (FTM3,FTM_CH_0,FTM_DMA_ON); // DMA ON
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

	//Init values for game_matrix and both buffers set to 0
	RGB_t testColor ={.red =0, .blue =0, .green =0};
	game_matrix_init(testColor);
	LED_DISPLAY_setBrightness(0.5);
	RGB2PWMLevel();
	swapWriteBuffer();
	RGB2PWMLevel();
	//DMA_enableChannel(0);
}

/*LED_DISPLAY_updateMatrix(void* modelMatrix)*/
// Called upon every time the model updates
// Starts transmission to LED Display

void LED_DISPLAY_updateMatrix(MATRIX_BOARD_t modelMatrix)
{
	dot_color = modelMatrix.rgb_color;
	display_brightness = modelMatrix.brightness;

	update_game_matrix(modelMatrix.table); 	//Translate Model Matrix to RGB matrix the output is able to understand
	applyBrightnessSetting();			//Multiply each rgb cell by brightness setting.
	RGB2PWMLevel();						//Translates individual bits in rgb dot to Duty Cycle settings according to ws2812's needs
	FTM_StartClock(FTM3);				//Starts Clock and DMA module, transmitting bit information to the Led Matrix
	DMA_enableChannel(CHANNEL_0);

}

// Getter functions

uint8_t LED_DISPLAY_getBrightness()
{
	return display_brightness;
}

RGB_t LED_DISPLAY_getDotColor()
{
	return dot_color;
}

// Setter Function

void LED_DISPLAY_setBrightness(float newBri)
{
	display_brightness = newBri;
	//Multiply each cell by brightness
	applyBrightnessSetting();

}


void LED_DISPLAY_setDotColor(RGB_t newColor)
{
	dot_color = newColor;
}


/*	Static file scope function Definitions	*/

static void RGB2PWMLevel() // Translates logical level to timings required by the display
{	//Overcomplicated, tratar de arreglar
	int i,j,k;
	uint8_t aux_red = 0;
	uint8_t aux_green = 0;
	uint8_t aux_blue = 0;

	bool red_bit_chk = false;
	bool green_bit_chk = false;
	bool blue_bit_chk = false;
	for (i = 0; i < LED_DISPLAY_HEIGHT; ++i)
	{
		for (j = 0; j < LED_DISPLAY_WIDTH; ++j)
		{
			aux_red = game_matrix[i][j].red;
			aux_green = game_matrix[i][j].green;
			aux_blue = game_matrix[i][j].blue;
			for (k = 0; k <BITS_PER_COLOR ; k++)
			{
				red_bit_chk = CHECK_BIT(aux_red, BITS_PER_COLOR-1-k);
				green_bit_chk = CHECK_BIT(aux_green, BITS_PER_COLOR-1-k);
				blue_bit_chk = CHECK_BIT(aux_blue, BITS_PER_COLOR-1-k);

				if(green_bit_chk)
				{
					if(write_buffer == BUFFER_A)
						BufferA[i][j][k] = PWM_HIGH;
					else
						BufferB[i][j][k] = PWM_HIGH;
				}
				else
				{
					if(write_buffer == BUFFER_A)
						BufferA[i][j][k] = PWM_LOW;
					else
						BufferB[i][j][k] = PWM_LOW;
				}

				if(red_bit_chk)
				{
					if(write_buffer == BUFFER_A)
						BufferA[i][j][k+BITS_PER_COLOR] = PWM_HIGH;
					else
						BufferB[i][j][k+BITS_PER_COLOR] = PWM_HIGH;
				}
				else
				{
					if(write_buffer == BUFFER_A)
						BufferA[i][j][k+BITS_PER_COLOR] = PWM_LOW;
					else
						BufferB[i][j][k+BITS_PER_COLOR] = PWM_LOW;
				}

				if(blue_bit_chk)
				{
					if(write_buffer == BUFFER_A)
						BufferA[i][j][k+2*BITS_PER_COLOR] = PWM_HIGH;
					else
						BufferB[i][j][k+2*BITS_PER_COLOR] = PWM_HIGH;

				}
				else
				{
					if(write_buffer==BUFFER_A)
						BufferA[i][j][k+2*BITS_PER_COLOR] = PWM_LOW;
					else
						BufferB[i][j][k+2*BITS_PER_COLOR] = PWM_LOW;
				}
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

void applyBrightnessSetting()
{
	int i,j;
	for (i = 0; i < LED_DISPLAY_HEIGHT; ++i)
	{
		for (j = 0; j < LED_DISPLAY_WIDTH; ++j)
		{

			game_matrix[i][j].red =(uint8_t)((game_matrix[i][j].red)*display_brightness);
			game_matrix[i][j].green =(uint8_t)((game_matrix[i][j].green)*display_brightness);
			game_matrix[i][j].blue =(uint8_t)((game_matrix[i][j].blue)*display_brightness);

		}

	}
}


void game_matrix_init(RGB_t initColor)
{
	int i,j;
		for (i = 0; i < LED_DISPLAY_HEIGHT; ++i)
		{
			for (j = 0; j < LED_DISPLAY_WIDTH; ++j)
			{
				if((i==j)||(i==0)||(j==0)) //Solo para esta prueba va el condicional
				{
					game_matrix[i][j].red = initColor.red;
					game_matrix[i][j].green =initColor.green;
					game_matrix[i][j].blue = initColor.blue;
				}
				else
				{
					game_matrix[i][j].red = 0;
					game_matrix[i][j].green =0;
					game_matrix[i][j].blue = 0;
				}
			}

		}
}

void update_game_matrix(int modelMatrix[LED_DISPLAY_HEIGHT][LED_DISPLAY_WIDTH])
{
	int i,j;
	for (i = 0; i < LED_DISPLAY_HEIGHT; ++i)
	{
		for (j = 0; j < LED_DISPLAY_WIDTH; ++j)
		{
			if(modelMatrix[i][j]!=0)
			{
				game_matrix[i][j] = dot_color;
			}
			else
			{
				game_matrix[i][j].red = 0;
				game_matrix[i][j].green = 0;
				game_matrix[i][j].blue = 0;
			}
		}
	}

}

/*	Interrupt CallBack Definition */

void DMA0_IRQCallback() //Should swap pingpong buffer between screen refresh, allows cpu to update game output.
{
	swapWriteBuffer();
	FTM_StopClock(FTM3);
	DMA_disableChannel(CHANNEL_0);
}
