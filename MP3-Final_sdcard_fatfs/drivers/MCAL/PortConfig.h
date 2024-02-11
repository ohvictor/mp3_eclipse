/***************************************************************************//**
  @file     PortConfig.h
  @brief    Port Configuration File
  @author   uLab 2021 G3
 ******************************************************************************/

#ifndef __PTCONFIG_
#define __PTCONFIG_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "gpio.h"
#include "hardware.h"
#include "MK64F12.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define PIN_SEG_A	PORTNUM2PIN(PC,3)	//PTC3
#define PIN_SEG_B	PORTNUM2PIN(PC,2)	//PTC2
#define PIN_SEG_C	PORTNUM2PIN(PA,2)	//PTA2
#define PIN_SEG_D	PORTNUM2PIN(PB,23)	//PTB23
#define PIN_SEG_E	PORTNUM2PIN(PA,1)	//PTA1
#define PIN_SEG_F	PORTNUM2PIN(PB,9)	//PTB9
#define PIN_SEG_G	PORTNUM2PIN(PC,17)	//PTC17
#define PIN_SEG_DP	PORTNUM2PIN(PC,16)	//PTC16

#define PIN_SEL_0	PORTNUM2PIN(PC,5)	//PTC5
#define PIN_SEL_1	PORTNUM2PIN(PC,7)	//PTC7

#define PIN_STA_0	PORTNUM2PIN(PB,19)	//PTB19
#define PIN_STA_1	PORTNUM2PIN(PB,18)	//PTB18

#define PIN_RCHA	PORTNUM2PIN(PC,0)	//PTC0
#define PIN_RCHB	PORTNUM2PIN(PC,9)	//PTC9
#define PIN_RSW		PORTNUM2PIN(PC,8)	//PTC8

#define PIN_PLAY	PORTNUM2PIN(PB,2)	//PTB2
#define PIN_PAUSE	PORTNUM2PIN(PB,3)	//PTB3
#define	PIN_STOP	PORTNUM2PIN(PB,10)	//PTB10
#define PIN_NEXT	PORTNUM2PIN(PC,11)	//PTC11
#define PIN_PREV	PORTNUM2PIN(PB,11)	//PTC10

// Test point
#define TP				PORTNUM2PIN(PC,5) // PTC4
#define TP2				PORTNUM2PIN(PC,7)


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct{

	 uint32_t PS		    :1;
	 uint32_t PE		    :1;
	 uint32_t SRE		    :1;
	 uint32_t RESERV0	  :1;
	 uint32_t PFE		    :1;
	 uint32_t ODE		    :1;
	 uint32_t DSE		    :1;
	 uint32_t RESERV1	  :1;
	 uint32_t MUX		    :3;
	 uint32_t RESERV2	  :4;
	 uint32_t LK		    :1;
	 uint32_t IRQC		  :4;
	 uint32_t RESERV3	  :4;
	 uint32_t ISF		    :1;
	 uint32_t RESERV4	  :7;

} FIELD_T;  //PCR struct


typedef union
{
	FIELD_T    FIELD;
	uint32_t   PCR;
} PCRstr;   

typedef enum
{
	PORT_mAnalog,
	PORT_mGPIO,
	PORT_mAlt2,
	PORT_mAlt3,
	PORT_mAlt4,
	PORT_mAlt5,
	PORT_mAlt6,
	PORT_mAlt7,
} PORTMux_t;

typedef enum
{
	PORT_eDisabled				= 0x00,
	PORT_eDMARising				= 0x01,
	PORT_eDMAFalling			= 0x02,
	PORT_eDMAEither				= 0x03,
	PORT_eInterruptDisasserted	= 0x08,
	PORT_eInterruptRising		= 0x09,
	PORT_eInterruptFalling		= 0x0A,
	PORT_eInterruptEither		= 0x0B,
	PORT_eInterruptAsserted		= 0x0C,
} PORTEvent_t;

typedef PORT_Type*  PORT_t; /* PORTA, PORTB, PORTC, PORTD, PORTE */
typedef uint32_t    PORTBit_t;
typedef uint32_t    PORTFlags_t;

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

__ISR__ 	PORTA_IRQHandler			  (void);
__ISR__ 	PORTB_IRQHandler			  (void);
__ISR__ 	PORTC_IRQHandler			  (void);
__ISR__ 	PORTD_IRQHandler			  (void);
__ISR__ 	PORTE_IRQHandler			  (void);

void    port_config_init(void);

void    port_config_set_pull_resistor_mode (PORT_t port, PORTBit_t bit, bool mode);

void    port_config_set_pull_up_mode (PORT_t port, PORTBit_t bit, bool mode);

void    port_config_set_open_drain_mode (PORT_t port, PORTBit_t bit, bool mode);

void    port_config_SetMuxMode (PORT_t port, PORTBit_t bit, PORTMux_t mux);

void    port_config_set_interrupt_mode (PORT_t port, PORTBit_t bit, PORTEvent_t event);

bool    port_config_is_interrupt_pending (PORT_t port, PORTBit_t bit);

void    port_config_clear_interrup_flag (PORT_t port, PORTBit_t bit);

void    port_config_configure (PORT_t port, PORTBit_t bit, bool pr, bool pu, bool od, bool pf, bool ssr, bool hds, PORTMux_t mux, PORTEvent_t event);

void    port_config_pcr(PORT_t port, PORTBit_t bit, PCRstr UserPCR);

void 	port_config_clear_port_flags(PORT_t port);

#endif // _PTCONFIG_
