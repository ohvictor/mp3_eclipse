/*******************************************************************************
  @file     uart.c
  @brief    Hardware implementation of UART protocol in K64
  @author   Victor Oh
 ******************************************************************************/


/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include	"uart.h"

#include	"fifo.h"
#include	"port.h"

#include	"MK64F12.h"
#include	"hardware.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define UART_HAL_DEFAULT_BAUDRATE 9600

#define UART0_PIN_RX	PORTNUM2PIN(PB,16)	// PTB16
#define UART0_PIN_TX	PORTNUM2PIN(PB,17)	// PTB16

#define UART3_PIN_RX	PORTNUM2PIN(PC,16)	// PTB16
#define UART3_PIN_TX	PORTNUM2PIN(PC,17)	// PTB16

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void UART_SetBaudRate ( UART_Type* uart, uint32_t baudrate);

void UART_Init_0(uart_cfg_t config);
void UART_Init_1(uart_cfg_t config);
void UART_Init_2(uart_cfg_t config);
void UART_Init_3(uart_cfg_t config);
void UART_Init_4(uart_cfg_t config);
void UART_Init_5(uart_cfg_t config);

uint8_t uart_fifo_size(uint8_t code);
uint8_t uart_rx_fifo_space(UART_Type* uart);
uint8_t uart_tx_fifo_space(UART_Type* uart);


/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static UART_Type* uarts[] = UART_BASE_PTRS;

static bool uart_tx_flag[UART_CANT_IDS];	// Transmit Complete flags
static bool uart_rx_flag[UART_CANT_IDS];	// Received Message flags

static FIFO_t uart_rx_fifos[UART_CANT_IDS];
static FIFO_t uart_tx_fifos[UART_CANT_IDS];

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void uart_Init (uint8_t id, uart_cfg_t config)
{
	UART_Type * uart = uarts[id];
	switch(id)
	{
	case 0: UART_Init_0(config); break;
//	case 1: UART_Init_1(config); break;
//	case 2: UART_Init_2(config); break;
	case 3: UART_Init_3(config); break;
//	case 4: UART_Init_4(config); break;
//	case 5: UART_Init_5(config); break;
	}

	uart->BDH |= UART_BDH_LBKDIE(0) | UART_BDH_RXEDGIE(1) | UART_BDH_SBNS(0);
	UART_SetBaudRate(uart, config.baudrate);

	// I'm interested in Reception Interrupts
	// Enable Transmission Interrupts only when I'm transmitting
	uart->C2 |= UART_C2_RIE(1);

	// I want to use the integrated TX and RX FIFOs
	uart->PFIFO |= UART_PFIFO_RXFE(1) | UART_PFIFO_TXFE(1);
	uart->CFIFO |= UART_CFIFO_RXFLUSH(1) | UART_CFIFO_TXFLUSH(1);

	uart_rx_flag[id] = false;
	uart_tx_flag[id] = true;

	FIFO_Init(&uart_tx_fifos[id]);
	FIFO_Init(&uart_rx_fifos[id]);

	// Enable RX only at the end of the setup
	uart->C2 |= UART_C2_RE(1) | UART_C2_TE(1);
}

uint8_t uart_is_rx_msg(uint8_t id)
{
	return uart_rx_flag[id];
}

uint8_t uart_get_rx_msg_length(uint8_t id)
{
	FIFO_t * fifo_rx = &uart_rx_fifos[id];



	return FIFO_Msg_Len(fifo_rx);
}

uint32_t uart_read_msg(uint8_t id, char* msg, uint8_t cant)
{
	uint32_t n_read;
	FIFO_t * fifo_rx = &uart_rx_fifos[id];

	n_read = FIFO_Pop_n(fifo_rx, msg, cant);
	uart_rx_flag[id] = false;

	return n_read;
}

uint32_t uart_write_msg(uint8_t id, const char* msg, uint8_t cant)
{
	UART_Type *uart = uarts[id];
	uint32_t n_written;
	FIFO_t * fifo_tx = &uart_tx_fifos[id];

	n_written = FIFO_Push_n(fifo_tx, msg, cant);
	uart_tx_flag[id] = false;

	uart->C2 |= UART_C2_TIE(1);
	uart->C2 |= UART_C2_TE(1);


	return n_written;
}

uint8_t uart_is_tx_msg_complete(uint8_t id)
{
	return uart_tx_flag[id];
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void UART_SetBaudRate ( UART_Type* uart, uint32_t baudrate)
{
	uint16_t sbr, brfa;
	uint32_t clock;

	clock = ((uart == UART0) || (uart == UART1)) ? (__CORE_CLOCK__):(__CORE_CLOCK__ >> 1);

	baudrate = ((baudrate == 0) ? (UART_HAL_DEFAULT_BAUDRATE):
			((baudrate > 0x1FFF)?(UART_HAL_DEFAULT_BAUDRATE):(baudrate)));

	sbr = clock / (baudrate<<4);	// sbr = clock / (baudrate x 16)
	brfa = (clock << 1) / baudrate - (sbr << 5); // brfa = 2*clock/baudrate - 32*sbr

	uart->BDH = UART_BDH_SBR(sbr >> 8);
	uart->BDL = UART_BDL_SBR(sbr);
	uart->C4 = (uart->C4 & ~UART_C4_BRFA_MASK) | UART_C4_BRFA(brfa);
}

void UART_Init_0(uart_cfg_t config)
{
	SIM->SCGC4 |= SIM_SCGC4_UART0(1);

	PORT_Init(UART0_PIN_RX);
	PORT_SetMuxMode(UART0_PIN_RX, PORT_mAlt3);
	PORT_SetInterruptMode(UART0_PIN_RX, PORT_eDisabled);

	PORT_Init(UART0_PIN_TX);
	PORT_SetMuxMode(UART0_PIN_TX, PORT_mAlt3);
	PORT_SetInterruptMode(UART0_PIN_TX, PORT_eDisabled);

	NVIC_EnableIRQ(UART0_RX_TX_IRQn);
}

void UART_Init_3(uart_cfg_t config)
{
	SIM->SCGC4 |= SIM_SCGC4_UART3(1);

	PORT_Init(UART3_PIN_RX);
	PORT_SetMuxMode(UART3_PIN_RX, PORT_mAlt3);
	PORT_SetInterruptMode(UART3_PIN_RX, PORT_eDisabled);

	PORT_Init(UART3_PIN_TX);
	PORT_SetMuxMode(UART3_PIN_TX, PORT_mAlt3);
	PORT_SetInterruptMode(UART3_PIN_TX, PORT_eDisabled);

	NVIC_EnableIRQ(UART3_RX_TX_IRQn);
}

void UART_Handler(uint8_t id)
{
	UART_Type * uart = uarts[id];

	uint8_t status;
	status = uart->S1;

	if (status & UART_S1_RDRF_MASK)
	{
		FIFO_t * fifo_rx = &uart_rx_fifos[id];
		uint8_t n_rx;
		n_rx = uart->RCFIFO;

		for(int i=0; i<n_rx; i++)
		{
			FIFO_Push(fifo_rx, uart->D);
		}
		uart_rx_flag[id] = true;
	}
	if (status & UART_S1_TDRE_MASK)
	{
		FIFO_t * fifo_tx = &uart_tx_fifos[id];
		uint8_t tx_space;
		uint8_t temp;
		tx_space = uart_tx_fifo_space(uart);

		for(int i=0; i<tx_space; i++)
		{
			temp = FIFO_Peek(fifo_tx);
			if(temp == '\0')
			{
				uart->C2 &= ~UART_C2_TIE(1);
				break;
			}
			uart->D = FIFO_Pop(fifo_tx);
		}
		uart_tx_flag[id] = true;
	}
	if (uart->S1 & UART_S1_TC(1))
	{

		uart->C2 &= ~UART_C2_TE(1);
	}
}

/******************************************************************************/

__ISR__ UART0_RX_TX_IRQHandler (void)
{
	UART_Handler(0);
}

__ISR__ UART3_RX_TX_IRQHandler (void)
{
	UART_Handler(3);
}

uint8_t uart_fifo_size(uint8_t code)
{
	uint8_t retval = 0;
	switch(code)
	{
	case 0: retval = 1; break;
	case 1: retval = 4; break;
	case 2: retval = 8; break;
	case 3: retval = 16; break;
	case 4: retval = 32; break;
	case 5: retval = 64; break;
	case 6: retval = 128; break;
	case 7: retval = 0; break;
	}

	return retval;
}

uint8_t uart_rx_fifo_space(UART_Type* uart)
{
	uint8_t size;
	uint8_t used;

	size = (uart->PFIFO & UART_PFIFO_RXFIFOSIZE_MASK)>>UART_PFIFO_RXFIFOSIZE_SHIFT;
	size = uart_fifo_size(size);

	used = uart->RCFIFO;

	return (size-used);

}

uint8_t uart_tx_fifo_space(UART_Type* uart)
{
	if(uart->PFIFO & UART_PFIFO_TXFE(1))
	{
		uint8_t size;
		uint8_t used;

		size = (uart->PFIFO & UART_PFIFO_TXFIFOSIZE_MASK)>>UART_PFIFO_TXFIFOSIZE_SHIFT;
		size = uart_fifo_size(size);

		used = uart->TCFIFO;

		return (size-used);
	}
	else
	{
		return 1;
	}
}
