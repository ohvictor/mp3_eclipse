/*******************************************************************************
  @file     audiobuffer.c
  @brief
  @author
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "fsl_dac.h"
#include "fsl_edma.h"
#include "fsl_pdb.h"
#include "fsl_dmamux.h"
#include "fsl_common.h"
#include "audiobuffer.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define PDB_BASEADDR           PDB0
#define PDB_MODULUS_VALUE      0xFFFU
#define PDB_DELAY_VALUE        0U
#define PDB_DAC_CHANNEL        kPDB_DACTriggerChannel0
#define PDB_DAC_INTERVAL_VALUE 0xFFFU
#define DAC_BASEADDR           DAC0
#define DMAMUX_BASEADDR        DMAMUX
#define DMA_CHANNEL            1U
#define DMA_DAC_SOURCE         45U
#define DMA_BASEADDR           DMA0
#define DAC_DATA_REG_ADDR      0x400cc000U
#define DMA_IRQ_ID             DMA0_IRQn

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*
 * @brief
 */
static void eDMA_configuration(void);
/*
 * @brief
 */
static void DMAMUX_configuration(void);
/*
 * @brief
 */
static void PDB_configuration(void);
/*
 * @brief
 */
static void DAC_configuration(void);
/*
 * @brief
 */
static void eDMA_Callback(edma_handle_t *handle, void *userdata, bool transferdone, uint32_t tcds);


/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static edma_handle_t g_edma_handler;
static edma_transfer_config_t g_transferconfig;
static volatile uint32_t g_index = 0U;

static int16_t 		buffers[2][BUFFER_SIZE];
static int16_t * 	activebuffer = buffers[0];
static int16_t * 	backbuffer= buffers[1];
static bool 		backbufferstate = false;
static bool 		pause = false;
static bool			stop = false;

static int16_t mute[DAC_DATL_COUNT] = {2048U};

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/*
 * @brief
 */
void audiobuffer_init(void)
{
	for(uint8_t i = 0; i < DAC_DATL_COUNT; i++)
	{
		mute[i] = 0;
	}
	eDMA_configuration();

	DMAMUX_configuration();

	PDB_configuration();

	DAC_configuration();
}


/*
 *@brief
 */
void audiobuffer_load_song(uint16_t *song_firstFrame, uint16_t sampleRate)
{
	memcpy(activebuffer, song_firstFrame, BUFFER_SIZE * sizeof(uint16_t));
	backbufferstate = true;
	g_index = 0U;

	audiobuffer_update_samplerate(sampleRate);
}


/*
 * @brief
*/
void audiobuffer_play(void)
{
	g_index = 0U;
	stop = false;

	if(pause)
	{
		pause = false;
		return;
	}
	// DMAMUX:
    DMAMUX_EnableChannel(DMAMUX_BASEADDR, DMA_CHANNEL);

	// EDMA:
	EDMA_PrepareTransfer(&g_transferconfig, (void *)(activebuffer + g_index), sizeof(uint16_t),
	                         (void *)DAC_DATA_REG_ADDR, sizeof(uint16_t), DAC_DATL_COUNT * sizeof(uint16_t),
	                         DAC_DATL_COUNT * sizeof(uint16_t), kEDMA_MemoryToMemory);
	EDMA_SubmitTransfer(&g_edma_handler, &g_transferconfig);

	// Enable interrupt when transfer is done
	EDMA_EnableChannelInterrupts(DMA_BASEADDR, DMA_CHANNEL, kEDMA_MajorInterruptEnable);

	// Enable transfer.
	EDMA_StartTransfer(&g_edma_handler);

	DAC_EnableBufferInterrupts(DAC_BASEADDR, kDAC_BufferReadPointerTopInterruptEnable);

	DAC_EnableBufferDMA(DAC_BASEADDR, true);

	PDB_DoSoftwareTrigger(PDB_BASEADDR);
}

/*
 * @brief Pauses the audio reproduction.
*/
void audiobuffer_pause(void)
{
	pause = true;
}

/*
 * @brief
*/
void audiobuffer_stop(void)
{
	stop = true;
}

/*
 *@brief
*/
void audiobuffer_update_samplerate(uint16_t samplerate)
{
    pdb_config_t pdbConfigStruct;
    pdb_dac_trigger_config_t pdbDacTriggerConfigStruct;
    PDB_GetDefaultConfig(&pdbConfigStruct);

    // PARA 60MHz
	if(samplerate == 8000)
	{
		pdbConfigStruct.dividerMultiplicationFactor = kPDB_DividerMultiplicationFactor1;
		pdbConfigStruct.prescalerDivider = kPDB_PrescalerDivider1;
		PDB_SetModulusValue(PDB_BASEADDR, 7500);
		PDB_SetDACTriggerIntervalValue(PDB_BASEADDR, PDB_DAC_CHANNEL, 7500);
	}
	else if(samplerate == 11025)
	{
		pdbConfigStruct.dividerMultiplicationFactor = kPDB_DividerMultiplicationFactor1;
		pdbConfigStruct.prescalerDivider = kPDB_PrescalerDivider1;
		PDB_SetModulusValue(PDB_BASEADDR, 5442);
		PDB_SetDACTriggerIntervalValue(PDB_BASEADDR, PDB_DAC_CHANNEL, 5442);
	}
	else if(samplerate == 12000)
	{
		pdbConfigStruct.dividerMultiplicationFactor = kPDB_DividerMultiplicationFactor1;
		pdbConfigStruct.prescalerDivider = kPDB_PrescalerDivider1;
		PDB_SetModulusValue(PDB_BASEADDR, 5000);
		PDB_SetDACTriggerIntervalValue(PDB_BASEADDR, PDB_DAC_CHANNEL, 5000);
	}
	else if(samplerate == 16000)
	{
		pdbConfigStruct.dividerMultiplicationFactor = kPDB_DividerMultiplicationFactor1;
		pdbConfigStruct.prescalerDivider = kPDB_PrescalerDivider1;
		PDB_SetModulusValue(PDB_BASEADDR, 3750);
		PDB_SetDACTriggerIntervalValue(PDB_BASEADDR, PDB_DAC_CHANNEL, 3750);
	}
	else if(samplerate == 22050)
	{
		pdbConfigStruct.dividerMultiplicationFactor = kPDB_DividerMultiplicationFactor1;
		pdbConfigStruct.prescalerDivider = kPDB_PrescalerDivider1;
		PDB_SetModulusValue(PDB_BASEADDR, 2721);
		PDB_SetDACTriggerIntervalValue(PDB_BASEADDR, PDB_DAC_CHANNEL, 2721);
	}
	else if(samplerate == 24000)
	{
		pdbConfigStruct.dividerMultiplicationFactor = kPDB_DividerMultiplicationFactor1;
		pdbConfigStruct.prescalerDivider = kPDB_PrescalerDivider1;
		PDB_SetModulusValue(PDB_BASEADDR, 2500);
		PDB_SetDACTriggerIntervalValue(PDB_BASEADDR, PDB_DAC_CHANNEL, 2500);
	}
	else if(samplerate == 32000)
	{
		pdbConfigStruct.dividerMultiplicationFactor = kPDB_DividerMultiplicationFactor1;
		pdbConfigStruct.prescalerDivider = kPDB_PrescalerDivider1;
		PDB_SetModulusValue(PDB_BASEADDR, 1875);
		PDB_SetDACTriggerIntervalValue(PDB_BASEADDR, PDB_DAC_CHANNEL, 1875);
	}
	else if(samplerate == 44100)
	{
		pdbConfigStruct.dividerMultiplicationFactor = kPDB_DividerMultiplicationFactor1;
		pdbConfigStruct.prescalerDivider = kPDB_PrescalerDivider1;
		PDB_SetModulusValue(PDB_BASEADDR, 1361);
		PDB_SetDACTriggerIntervalValue(PDB_BASEADDR, PDB_DAC_CHANNEL, 1361);
	}
	else if(samplerate == 48000)
	{
		pdbConfigStruct.dividerMultiplicationFactor = kPDB_DividerMultiplicationFactor1;
		pdbConfigStruct.prescalerDivider = kPDB_PrescalerDivider1;
		PDB_SetModulusValue(PDB_BASEADDR, 1250);
		PDB_SetDACTriggerIntervalValue(PDB_BASEADDR, PDB_DAC_CHANNEL, 1250);
	}

	else //For testing with DEMO case
	{
		pdbConfigStruct.dividerMultiplicationFactor = kPDB_DividerMultiplicationFactor40;
		pdbConfigStruct.prescalerDivider = kPDB_PrescalerDivider1;
		PDB_SetModulusValue(PDB_BASEADDR, PDB_MODULUS_VALUE);
	}

	// Pongo estas configuraciones de PDB aca porque necesitan info del sample rate
	pdbConfigStruct.enableContinuousMode        = true;
	PDB_Init(PDB_BASEADDR, &pdbConfigStruct);
	PDB_EnableInterrupts(PDB_BASEADDR, kPDB_DelayInterruptEnable);
	PDB_SetCounterDelayValue(PDB_BASEADDR, PDB_DELAY_VALUE);

    /* Set DAC trigger. */
    pdbDacTriggerConfigStruct.enableExternalTriggerInput = false;
    pdbDacTriggerConfigStruct.enableIntervalTrigger      = true;
    PDB_SetDACTriggerConfig(PDB_BASEADDR, PDB_DAC_CHANNEL, &pdbDacTriggerConfigStruct);

    /* Load PDB values. */
    PDB_DoLoadValues(PDB_BASEADDR);
}

/*
 * @brief
*/
audiobuffer_states_t audiobuffer_update_backbuffer(uint16_t *newbackbuffer)
{
	if(backbufferstate)
	{
		memcpy(backbuffer, newbackbuffer, BUFFER_SIZE * sizeof(uint16_t));
		backbufferstate = false;
		return NO_ERROR;
	}
	else
		return BACKBUFFER_FULL;
}

/*
 *@brief
*/
bool audiobuffer_backbuffer_state(void)
{
	return backbufferstate;
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


/*
 * @brief
 */
static void eDMA_configuration(void)
{
	EDMA_CreateHandle(&g_edma_handler, DMA_BASEADDR, DMA_CHANNEL);
    EDMA_SetCallback(&g_edma_handler, eDMA_Callback, NULL);
}
/*
 * @brief
 */
static void DMAMUX_configuration(void)
{
    DMAMUX_SetSource(DMAMUX_BASEADDR, DMA_CHANNEL, DMA_DAC_SOURCE);
}
/*
 * @brief
 */
static void PDB_configuration(void)
{
    pdb_config_t pdbConfigStruct;
    pdb_dac_trigger_config_t pdbDacTriggerConfigStruct;

    PDB_GetDefaultConfig(&pdbConfigStruct);

    pdbConfigStruct.dividerMultiplicationFactor = kPDB_DividerMultiplicationFactor40;
    pdbConfigStruct.enableContinuousMode        = true;
    PDB_Init(PDB_BASEADDR, &pdbConfigStruct);
    PDB_EnableInterrupts(PDB_BASEADDR, kPDB_DelayInterruptEnable);
    PDB_SetModulusValue(PDB_BASEADDR, PDB_MODULUS_VALUE);
    PDB_SetCounterDelayValue(PDB_BASEADDR, PDB_DELAY_VALUE);

    /* Set DAC trigger. */
    pdbDacTriggerConfigStruct.enableExternalTriggerInput = false;
    pdbDacTriggerConfigStruct.enableIntervalTrigger      = true;
    PDB_SetDACTriggerConfig(PDB_BASEADDR, PDB_DAC_CHANNEL, &pdbDacTriggerConfigStruct);
    PDB_SetDACTriggerIntervalValue(PDB_BASEADDR, PDB_DAC_CHANNEL, PDB_DAC_INTERVAL_VALUE);

    /* Load PDB values. */
    PDB_DoLoadValues(PDB_BASEADDR);
}
/*
 * @brief
 */
static void DAC_configuration(void)
{
    dac_config_t dacConfigStruct;
    dac_buffer_config_t dacBufferConfigStruct;

    DAC_GetDefaultConfig(&dacConfigStruct);
    DAC_Init(DAC_BASEADDR, &dacConfigStruct);
    DAC_Enable(DAC_BASEADDR, true); /* Enable output. */

    /* Configure the DAC buffer. */
    DAC_EnableBuffer(DAC_BASEADDR, true);
    DAC_GetDefaultBufferConfig(&dacBufferConfigStruct);
    dacBufferConfigStruct.triggerMode = kDAC_BufferTriggerByHardwareMode;
    DAC_SetBufferConfig(DAC_BASEADDR, &dacBufferConfigStruct);
    DAC_SetBufferReadPointer(DAC_BASEADDR, 0U); /* Make sure the read pointer to the start. */
}
/*
 * @brief
 */
static void eDMA_Callback(edma_handle_t *handle, void *userData, bool transferdone, uint32_t tcds)
{
    /* Clear Edma interrupt flag. */
    EDMA_ClearChannelStatusFlags(DMA_BASEADDR, DMA_CHANNEL, kEDMA_InterruptFlag);
    /* Setup transfer */

    void * srcAdd = NULL;
    if(pause || stop)
	{
		srcAdd = mute;
	}
    else
    {
    	g_index += DAC_DATL_COUNT;
		if (g_index == BUFFER_SIZE)
		{
			g_index = 0U;
			void * temp = activebuffer;
			activebuffer = backbuffer;
			backbuffer = temp;
			backbufferstate = true;
		}
		srcAdd = (activebuffer + g_index);
    }

    EDMA_PrepareTransfer(&g_transferconfig, (void *)(srcAdd), sizeof(uint16_t),
                        (void *)DAC_DATA_REG_ADDR, sizeof(uint16_t), DAC_DATL_COUNT * sizeof(uint16_t),
                        DAC_DATL_COUNT * sizeof(uint16_t), kEDMA_MemoryToMemory);


    EDMA_SetTransferConfig(DMA_BASEADDR, DMA_CHANNEL, &g_transferconfig, NULL);
    /* Enable transfer. */
    EDMA_StartTransfer(&g_edma_handler);
}


