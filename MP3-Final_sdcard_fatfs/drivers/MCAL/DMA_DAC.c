/*******************************************************************************
  @file     DMA_DAC.c
  @brief    
  @author   
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "DMA_DAC.h"
#include "MK64F12.h"
#include "PortConfig.h"
#include "PIT.h"
#include "DAC.h"
#include "FTM.h"
#include "gpio.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define TABLE_LENGHT_BYTES (1584*2)

#define TS 22680 // ns

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct
{
    uint32_t SADDR;
    uint16_t SOFF;
    uint16_t ATTR;
    union
    {
        uint32_t NBYTES_MLNO;
        uint32_t NBYTES_MLOFFNO;
        uint32_t NBYTES_MLOFFYES;
    };
    uint32_t SLAST;
    uint32_t DADDR;
    uint16_t DOFF;
    union
    {
        uint16_t CITER_ELINKNO;
        uint16_t CITER_ELINKYES;
    };
    uint32_t DLAST_SGA;
    uint16_t CSR;
    union
    {
        uint16_t BITER_ELINKNO;
        uint16_t BITER_ELINKYES;
    };
} TCD_t;

typedef enum {FREE, BUSY} tableStates_t;

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

static uint32_t global_memDirTable;
static TCD_t tcdA  __attribute__ ((aligned (32)));
static TCD_t tcdB  __attribute__ ((aligned (32)));
static tableStates_t table1 = FREE;
static tableStates_t table2 = FREE;
static uint16_t* table1Internal;
static uint16_t* table2Internal;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

void DMA_DAC_init() {

    PIT_init(PIT_0, NULL);
    PIT_set_interval(PIT_0, PIT_NS2TICK(TS));

    DAC_init(DAC_0);

    DAC0->DAT[0].DATH = 0x8U; 
    DAC0->DAT[0].DATL = 0x00U;

    // Clock and PCR PORTC
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;

    PORTC->PCR[6]=0x00;
    PORTC->PCR[6]|=PORT_PCR_MUX(PORT_mGPIO); 		      
    PORTC->PCR[6]|=PORT_PCR_PE(1);          		       
    PORTC->PCR[6]|=PORT_PCR_PS(1);          		       
    PORTC->PCR[6]|=PORT_PCR_IRQC(PORT_eDMAFalling);

    // Clock for eDMA and DMAMUX
    SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
    SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;

    DMAMUX->CHCFG[0] |= DMAMUX_CHCFG_ENBL_MASK | DMAMUX_CHCFG_SOURCE(20); // FTM0 CH0

    NVIC_ClearPendingIRQ(DMA0_IRQn);
    NVIC_EnableIRQ(DMA0_IRQn);
}

void DMA_DAC_PingPong(uint16_t* dirTable1, uint16_t* dirTable2, uint16_t tableSize) {
    
    PIT_stop(PIT_0);

    table1Internal = dirTable1;
    table2Internal = dirTable2;

    TCD_t *currentTcd = &tcdA;
    TCD_t *nextTcd = &tcdB;

    for (int i = 0; i < 2; ++i) 
    {
        currentTcd->SADDR = (uint32_t)(i == 0 ? dirTable1 : dirTable2);
        currentTcd->DADDR = (uint32_t)&DAC0->DAT[0].DATL;

        currentTcd->SOFF = 0x02;
        currentTcd->DOFF = 0x00;
        currentTcd->ATTR = DMA_ATTR_SSIZE(1) | DMA_ATTR_DSIZE(1);
        currentTcd->NBYTES_MLNO = 0x02;
        currentTcd->CITER_ELINKNO = DMA_CITER_ELINKNO_CITER(tableSize);
        currentTcd->BITER_ELINKNO = DMA_BITER_ELINKNO_BITER(tableSize);
        currentTcd->SLAST = 0;
        currentTcd->DLAST_SGA = (uint32_t)nextTcd;
        currentTcd->CSR = DMA_CSR_ESG_MASK | DMA_CSR_INTMAJOR_MASK;

        TCD_t *temp = currentTcd;
        currentTcd = nextTcd;
        nextTcd = temp;
    }

    DMA0->TCD[0].SADDR = tcdA.SADDR;
    DMA0->TCD[0].DADDR = tcdA.DADDR;

    DMA0->TCD[0].SOFF =tcdA.SOFF; 
    DMA0->TCD[0].DOFF =tcdA.DOFF; 

    DMA0->TCD[0].ATTR = tcdA.ATTR;

    DMA0->TCD[0].NBYTES_MLNO= tcdA.NBYTES_MLNO;

    DMA0->TCD[0].CITER_ELINKNO = tcdA.CITER_ELINKNO;
    DMA0->TCD[0].BITER_ELINKNO = tcdA.BITER_ELINKNO;

    DMA0->TCD[0].SLAST = tcdA.SLAST;
    DMA0->TCD[0].DLAST_SGA = tcdA.DLAST_SGA;

    DMA0->TCD[0].CSR = tcdA.CSR; 

    DMA0->ERQ |= DMA_ERQ_ERQ0_MASK;

    PIT_start(PIT_0);
    table1 = BUSY;
    table2 = FREE;
}

uint16_t* DMA_DAC_PingPong_table()
{
	hw_DisableInterrupts();

	if(table1 == FREE)
    {
		hw_EnableInterrupts();
		return table1Internal;
	}
	else if(table2 == FREE)
    {
		hw_EnableInterrupts();
		return table2Internal;
	}
	return NULL;
}

void DMA_DAC_PingPong_pause()
{
	PIT_stop(PIT_0);
	DAC0->DAT[0].DATH = 0x8U;	
	DAC0->DAT[0].DATL = 0x00U;
}

void DMA_DAC_PingPong_continue()
{
	PIT_start(PIT_0);
}

/**
 * @brief Initializes DMA for displaying a table.
 *
 * @param memDirTable Memory address of the table.
 */

void DMA_DAC_init_display_table(uint32_t memDirTable) {
    // Initialize ports and FTM.
    port_config_init();
    FTM_Init();
    /**
     * Terminar de configurar aca el FTMx
     */

    // Disable interrupts temporarily for configuration.
    hw_DisableInterrupts();

    // Store the memory address of the table globally.
    global_memDirTable = memDirTable;

    // Configure SW2 (PTC6) as the DMA request source.
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;  // Enable the clock for PORT C
    PORTC->PCR[6] = 0x00;
    PORTC->PCR[6] |= PORT_PCR_MUX(PORT_mGPIO);              // Set MUX to GPIO
    PORTC->PCR[6] |= PORT_PCR_PE(1);                        // Pull UP/Down Enable
    PORTC->PCR[6] |= PORT_PCR_PS(1);                        // Pull UP
    PORTC->PCR[6] |= PORT_PCR_IRQC(PORT_eDMAFalling);       // Configure for falling edge
    DMAMUX->CHCFG[1] |= DMAMUX_CHCFG_ENBL_MASK | DMAMUX_CHCFG_SOURCE(20);  // Enable DMA channel 1 for FTM0 CH0

    // Enable DMA and DMAMUX clocks.
    SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
    SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;

    // Clear pending DMA events and enable interrupts for channel 1.
    NVIC_ClearPendingIRQ(DMA1_IRQn);
    NVIC_EnableIRQ(DMA1_IRQn);

    // Re-enable interrupts.
    hw_EnableInterrupts();

    return;
}

/**
 * @brief Performs a DMA transfer to display a table.
 */
void DMA_DAC_display_table(void) {
    // Initialize TCD0 for DMA transfer.
    DMA0->TCD[1].SADDR = (uint32_t)((uint16_t*)global_memDirTable + 1);
    DMA0->TCD[1].DADDR = (uint32_t)(&(FTM0->CONTROLS[0].CnV));
    DMA0->TCD[1].SOFF = 0x02;  // Source address offset of 2 bytes per transaction.
    DMA0->TCD[1].DOFF = 0x00;  // Destination address offset of 0 bytes per transaction.
    DMA0->TCD[1].ATTR = DMA_ATTR_SSIZE(1) | DMA_ATTR_DSIZE(1);  // Source and destination data transfer size is 2 bytes.
    DMA0->TCD[1].NBYTES_MLNO = 0x02;  // Number of bytes to be transferred in each service request of the channel.
    DMA0->TCD[1].CITER_ELINKNO = DMA_CITER_ELINKNO_CITER(TABLE_LENGHT_BYTES / 2);  // Current major iteration count.
    DMA0->TCD[1].BITER_ELINKNO = DMA_BITER_ELINKNO_BITER(TABLE_LENGHT_BYTES / 2);  // Starting major iteration count.
    DMA0->TCD[1].SLAST = -TABLE_LENGHT_BYTES / 2;  // Last source Address adjustment.
    DMA0->TCD[1].DLAST_SGA = 0x00;  // Last destination address adjustment.
    DMA0->TCD[1].CSR = DMA_CSR_INTMAJOR_MASK;  // Enable Major Interrupt.

    // Enable DMA request for channel 1.
    DMA0->ERQ |= DMA_ERQ_ERQ1_MASK;

    // Reinitialize FTM and start the clock.
    FTM_Init(); //Aca tambien
    FTM_StartClock(FTM0);

    return;
}
/*
__ISR__ DMA0_IRQHandler(void)
{
	DMA0->INT = DMA_INT_INT0(1);

	if(table1 == BUSY)
		table1 = FREE;
	else
		table1 = BUSY;

	if(table2 == BUSY)
		table2 = FREE;
	else
		table2 = BUSY;
}
__ISR__ DMA1_IRQHandler(void)
{
    DMA0->INT = DMA_INT_INT1(1);	
    FTM_StopClock(FTM0);
	FTM1->CNT = 0X00;
}
*/
