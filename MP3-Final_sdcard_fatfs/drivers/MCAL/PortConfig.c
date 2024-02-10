#include "PortConfig.h"

void port_config_init (void)
{
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;


	PORT_ClearPortFlags (PORTA);
	PORT_ClearPortFlags (PORTB);
	PORT_ClearPortFlags (PORTC);
	PORT_ClearPortFlags (PORTD);
	PORT_ClearPortFlags (PORTE);

	NVIC_EnableIRQ(PORTA_IRQn);
	NVIC_EnableIRQ(PORTB_IRQn);
	NVIC_EnableIRQ(PORTC_IRQn);
	NVIC_EnableIRQ(PORTD_IRQn);
	NVIC_EnableIRQ(PORTE_IRQn);
}


void port_config_set_pull_resistor_mode (PORT_t port, PORTBit_t bit, bool mode)
{
	port->PCR[bit] &= ~PORT_PCR_PE_MASK;
	port->PCR[bit] |= PORT_PCR_PE(mode);
}

void port_config_set_pull_up_mode (PORT_t port, PORTBit_t bit, bool mode)
{
	port->PCR[bit] &= ~PORT_PCR_PS_MASK;
	port->PCR[bit] |= PORT_PCR_PS(mode);
}

void port_config_set_open_drain_mode (PORT_t port, PORTBit_t bit, bool mode)
{
	port->PCR[bit] &= ~PORT_PCR_ODE_MASK;
	port->PCR[bit] |= PORT_PCR_ODE(mode);
}

void port_config_SetMuxMode (PORT_t port, PORTBit_t bit, PORTMux_t mux)
{
	port->PCR[bit] &= ~PORT_PCR_MUX_MASK;
	port->PCR[bit] |= PORT_PCR_MUX(mux);
}

void port_config_set_interrupt_mode (PORT_t port, PORTBit_t bit, PORTEvent_t event)
{
	port->PCR[bit] &= ~PORT_PCR_IRQC_MASK;
	port->PCR[bit] |= PORT_PCR_IRQC(event);
}

bool port_config_is_interrupt_pending (PORT_t port, PORTBit_t bit)
{
	return port->PCR[bit] & PORT_PCR_ISF_MASK;
}

void port_config_clear_interrup_flag (PORT_t port, PORTBit_t bit)
{
	port->PCR[bit] |= PORT_PCR_ISF_MASK;
}

void port_config_configure (PORT_t port, PORTBit_t bit, bool pr, bool pu, bool od, bool pf, bool ssr, bool hds, PORTMux_t mux, PORTEvent_t event)
{
	port->PCR[bit] = PORT_PCR_PE(pr) |
			         PORT_PCR_PS(pu) |
					 PORT_PCR_ODE(od) |
					 PORT_PCR_PFE(pf) |
					 PORT_PCR_SRE(ssr) |
					 PORT_PCR_DSE(hds) |
					 PORT_PCR_MUX(mux) |
					 PORT_PCR_IRQC(event) |
					 PORT_PCR_ISF_MASK;
}

void port_config_pcr(PORT_t port, PORTBit_t bit, PCRstr UserPCR)
{
	port->PCR[bit]=UserPCR.PCR ;
}

PORTFlags_t port_config_get_port_flags(PORT_t port)
{
	return port->ISFR & PORT_ISFR_ISF_MASK;
}

void port_config_clear_port_flags(PORT_t port)
{
	port->ISFR = PORT_ISFR_ISF_MASK;
}