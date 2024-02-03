/***************************************************************************//**
  @file     gpio.c
  @brief    Simple GPIO Pin services, similar to Arduino
  @author   Facundo Molina
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "gpio.h"
#include "MK64F12.h"
#include "hardware.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static pinIrqFun_t pinCallbacks[5][32];


/*Punteros:
 *GPIO Peripheral Access Layer (donde cada palabra de 32 bits permite habilitar o deshabilitar funcionalidades de cada pin del puerto X). Chapter 55.
 *Port Peripheral Access Layer: The Port Control and Interrupt (PORT) module provides support for port control, digital filtering, and external interrupt functions. Contiene PCR, Global Pin Control Registers (Low and High), Digital Enables, Interruptions, etc.
 *SIM System Integration Module: Clock Gating. */
static GPIO_Type* gpioPtr[] = GPIO_BASE_PTRS;	//{ GPIOA, GPIOB, GPIOC, GPIOD, GPIOE }
static PORT_Type* portPtr[] = PORT_BASE_PTRS;	//{ PORTA, PORTB, PORTC, PORTD, PORTE }
static SIM_Type* simPtr = SIM;		//Puntero a SIM_Type a la posicion (0x40047000u).

//Arreglo con posiciones de memoria para la habilitacion del clock de los puertos.
static uint32_t simMasks[] = {SIM_SCGC5_PORTA_MASK, SIM_SCGC5_PORTB_MASK, SIM_SCGC5_PORTC_MASK, SIM_SCGC5_PORTD_MASK, SIM_SCGC5_PORTE_MASK };

/**
 * @brief Configures the specified pin to behave either as an input or an output
 * @param pin the pin whose mode you wish to set (according PORTNUM2PIN)
 * @param mode INPUT, OUTPUT, INPUT_PULLUP or INPUT_PULLDOWN.
 */
void gpioMode (pin_t pin, uint8_t mode)
{
	uint32_t portX = PIN2PORT(pin); //Puerto A, B, C, D, E
	uint32_t pinNum = PIN2NUM(pin); //Numero del pin

	//Activo clock y selecciono puerto y gpio correspondiente.
	simPtr->SCGC5 |= simMasks[portX]; 	//Enable clocking for port X. OR bitwise equivale a un SET.
	PORT_Type *port = portPtr[portX];	//
	GPIO_Type *gpio = gpioPtr[portX];

	//Configuracion de PCR: MUX y pin.
	port->PCR[pinNum] = 0;	//Reset.
	port->PCR[pinNum] |= PORT_PCR_MUX(1);	//Setea en 001 los bits MUX, es decir, en configuracion GPIO.

	switch (mode)
	{
		case INPUT:
			gpio->PDDR &= ~(1 << pinNum);
			break;
		case OUTPUT:
			gpio->PDDR |= (1 << pinNum);
			break;
		case INPUT_PULLUP:
			gpio->PDDR &= ~(1 << pinNum);
			port->PCR[pinNum] |= PORT_PCR_PE(1); //Habilita resistencia interna de pull up o pull down.
			port->PCR[pinNum] |= PORT_PCR_PS(1); //Habilita resistencia interna de pull up.
			break;
		case INPUT_PULLDOWN:
			gpio->PDDR &= ~(1 << pinNum);
			port->PCR[pinNum] |= PORT_PCR_PE(1); //Habilita resistencia interna de pull up o pull down.
			port->PCR[pinNum] &= PORT_PCR_PS(0); //Habilita resistencia interna de pull down.
			break;
	}
}

/**
 * @brief Write a HIGH or a LOW value to a digital pin
 * @param pin the pin to write (according PORTNUM2PIN)
 * @param val Desired value (HIGH or LOW)
 */
void gpioWrite (pin_t pin, bool value)
{
	uint32_t portX = PIN2PORT(pin); //Puerto A, B, C, D, E
	uint32_t pinNum = PIN2NUM(pin); //Numero de pin

	GPIO_Type *gpio = gpioPtr[portX];

	//if((gpio->PDDR & (1 << pinNum)) == (1 << pinNum)) //Compruebo que este seteado como output
	//{
		if(value == HIGH)
			gpio->PDOR |= (1 << pinNum);
		else
			gpio->PDOR &= ~(1 << pinNum);
	//}
}

/**
 * @brief Toggle the value of a digital pin (HIGH<->LOW)
 * @param pin the pin to toggle (according PORTNUM2PIN)
 */
void gpioToggle (pin_t pin)
{
	uint32_t portX = PIN2PORT(pin); //Puerto A, B, C, D, E
	uint32_t pinNum = PIN2NUM(pin); //Numero de pin

	GPIO_Type *gpio = gpioPtr[portX];

	gpio->PTOR |= (1 << pinNum);
}

/**
 * @brief Reads the value from a specified digital pin, either HIGH or LOW.
 * @param pin the pin to read (according PORTNUM2PIN)
 * @return HIGH or LOW
 */
bool gpioRead (pin_t pin)
{
	uint32_t portX = PIN2PORT(pin); //Puerto A, B, C, D, E
	uint32_t pinNum = PIN2NUM(pin); //Numero de pin

	GPIO_Type *gpio = gpioPtr[portX];
	bool value = (gpio->PDIR & (1 << pinNum)) == (1 << pinNum);
	return value;
}

/**
 * @brief Configures how the pin reacts when an IRQ event ocurrs
 * @param pin the pin whose IRQ mode you wish to set (according PORTNUM2PIN)
 * @param irqMode disable, risingEdge, fallingEdge or bothEdges
 * @param irqFun function to call on pin event
 * @return Registration succeed
 */
bool gpioIRQ(pin_t pin, uint8_t irqMode, pinIrqFun_t irqFun)
{
	uint32_t portX = PIN2PORT(pin); //Puerto A, B, C, D, E
	uint32_t pinNum = PIN2NUM(pin); //Numero de pin
	PORT_Type *port = portPtr[portX];

	port->PCR[pinNum] |= PORT_PCR_IRQC(irqMode);
	NVIC_EnableIRQ(PORTA_IRQn + portX);

	pinCallbacks[portX][pinNum] = irqFun;
	bool result = NVIC_GetEnableIRQ(PORTA_IRQn + portX); // not implemented yet
	return result;
}

void portsCallbacksHandler(uint8_t portX)
{
	PORT_Type *port = portPtr[portX];
	uint32_t isfr = port->ISFR;
	for (int i = 0; i < 32; i++)
	{
		if (pinCallbacks[portX][i] && (isfr & 0x1))
		{
			pinCallbacks[portX][i]();
			port->PCR[i] |= PORT_PCR_ISF_MASK;
			break;
		}
		isfr >>= 1;
	}
}

__ISR__ PORTA_IRQHandler(void)
{
	portsCallbacksHandler(PA);
}

__ISR__ PORTB_IRQHandler(void)
{
	portsCallbacksHandler(PB);
}

__ISR__ PORTC_IRQHandler(void)
{
	portsCallbacksHandler(PC);
}

__ISR__ PORTD_IRQHandler(void)
{
	portsCallbacksHandler(PD);
}

__ISR__ PORTE_IRQHandler(void)
{
	portsCallbacksHandler(PE);
}



