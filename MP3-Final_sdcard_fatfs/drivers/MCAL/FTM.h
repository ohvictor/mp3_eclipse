/***************************************************************************//**
  @file     FTM.h
  @brief    
  @author   
 ******************************************************************************/

#ifndef _FTM_H_
#define _FTM_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "hardware.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define FTM_CH_0 0
#define FTM_CH_1 1
#define FTM_CH_2 2
#define FTM_CH_3 3
#define FTM_CH_4 4
#define FTM_CH_5 5
#define FTM_CH_6 6
#define FTM_CH_7 7

#define FTM_DMA_ON  1
#define FTM_DMA_OFF 0

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum
{
	FTM_mInputCapture,
	FTM_mOutputCompare,
	FTM_mPulseWidthModulation,
} FTMmode_t;

typedef enum
{
	FTM_eRising 		= 0x01,
	FTM_eFalling 		= 0x02,
	FTM_eEither 		= 0x03,
} FTMedge_t;

typedef enum
{
	FTM_eToggle 		= 0x01,
	FTM_eClear 			= 0x02,
	FTM_eSet 			= 0x03,
} FTMeffect_t;

typedef enum
{
	FTM_lAssertedHigh	= 0x02,
	FTM_lAssertedLow 	= 0x03,
} FTMlogic_t;

typedef enum
{
	FTM_PSC_x1		= 0x00,
	FTM_PSC_x2		= 0x01,
	FTM_PSC_x4		= 0x02,
	FTM_PSC_x8		= 0x03,
	FTM_PSC_x16		= 0x04,
	FTM_PSC_x32		= 0x05,
	FTM_PSC_x64		= 0x06,
	FTM_PSC_x128	= 0x07,

} FTM_prescale_t;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

typedef 	FTM_Type 	*FTM_t;
typedef 	uint16_t 	FTMdata_t;
typedef 	uint32_t 	FTMchannel_t; 


/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

void 		FTM_init (uint16_t initialDuty);

void 		FTM_set_prescaler (FTM_t, FTM_prescale_t);

void 		FTM_set_modulus (FTM_t, FTMdata_t);
FTMdata_t 	FTM_get_modulus (FTM_t);

void 		FTM_start_clock (FTM_t);
void 		FTM_stop_clock (FTM_t);

void 		FTM_set_overflow_mode (FTM_t, bool);
bool 		FTM_is_overflow_pending (FTM_t);
void 		FTM_clear_overflow_flag (FTM_t);

void 		FTM_set_working_mode (FTM_t, FTMchannel_t, FTMmode_t);
FTMmode_t 	FTM_get_working_mode (FTM_t, FTMchannel_t);

void 		FTM_set_input_capture_edge (FTM_t ftm, FTMchannel_t channel, FTMedge_t edge);
FTMedge_t 	FTM_get_input_capture_edge (FTM_t, FTMchannel_t);

void 		FTM_set_output_compare_effect (FTM_t, FTMchannel_t, FTMeffect_t);
FTMeffect_t FTM_get_output_compare_effect (FTM_t, FTMchannel_t);

void 		FTM_set_pulse_width_modulation_logic (FTM_t, FTMchannel_t, FTMlogic_t);
FTMlogic_t 	FTM_get_pulse_width_modulation_logic (FTM_t, FTMchannel_t);

void 		FTM_set_counter (FTM_t, FTMchannel_t, FTMdata_t);
FTMdata_t 	FTM_get_counter (FTM_t, FTMchannel_t);

void 		FTM_set_interrupt_mode (FTM_t, FTMchannel_t, bool);
bool 		FTM_is_interrupt_pending (FTM_t, FTMchannel_t);
void 		FTM_clear_interrupt_flag (FTM_t, FTMchannel_t);

void 		FTM_dma_mode (FTM_t ftm, FTMchannel_t channel, bool dma_mode);







#endif //_FTM_H_
