/*******************************************************************************
  @file     i2c.c
  @brief    I2C driver - Based on App Note 4803 I2C Non-Blocking Communication from NXP.
  @author   Grupo 3
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "i2c.h"
#include "MK64F12.h"
#include "hardware.h"
#include "board.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


#define I2C_START_SIGNAL         (i2cPtr[id]->C1 |= I2C_C1_MST_MASK)
#define I2C_STOP_SIGNAL          (i2cPtr[id]->C1 &= ~I2C_C1_MST_MASK)
#define I2C_REPEAT_START_SIGNAL  (i2cPtr[id]->C1 |= I2C_C1_RSTA_MASK)
#define I2C_WRITE_BYTE(data)     (i2cPtr[id]->D = data)
#define I2C_READ_BYTE            (i2cPtr[id]->D)
#define I2C_GET_IRQ_FLAG         (i2cPtr[id]->S & I2C_S_IICIF_MASK)
#define I2C_CLEAR_IRQ_FLAG       (i2cPtr[id]->S |= I2C_S_IICIF_MASK)
#define I2C_GET_RX_ACK			 (i2cPtr[id]->S & I2C_S_RXAK_MASK)
#define I2C_SET_RX_MODE          (i2cPtr[id]->C1 = (i2cPtr[id]->C1 & ~I2C_C1_TX_MASK) | I2C_C1_TX(0))
#define I2C_SET_TX_MODE          (i2cPtr[id]->C1 |= I2C_C1_TX_MASK)
#define I2C_GET_TX				 (i2cPtr[id]->C1 & I2C_C1_TX_MASK)
#define I2C_SET_NACK	         (i2cPtr[id]->C1 |= I2C_C1_TXAK_MASK)
#define I2C_CLEAR_NACK      	 (i2cPtr[id]->C1 &= ~I2C_C1_TXAK_MASK)
#define I2C_CHECK_BUS		  	 (i2cPtr[id]->S & I2C_S_BUSY_MASK)
#define I2C_GET_TCF				 (i2cPtr[id]->S & I2C_S_TCF_MASK)

#define BUS_BUSY 				 1
#define TIMEOUT					 500



/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct
{
	I2C_MODE mode;
	I2C_TRM_STAGE stage;
	uint8_t device_address_w;
	uint8_t device_address_r;
	uint8_t data_index;
	I2C_FAULT fault;
	uint8_t register_address;
	uint8_t * data;
	uint8_t data_size;
	i2c_callback callback;
	bool isInit;
	bool isBlocking;
} I2C_COM_CTR;

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

__ISR__  	I2C0_IRQHandler(void);
__ISR__   	I2C1_IRQHandler(void);
__ISR__   	I2C2_IRQHandler(void);
void 		I2C_IRQDispatcher(I2C_INSTANCES instance);


/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static I2C_Type* i2cPtr[] = I2C_BASE_PTRS;
static IRQn_Type I2C_IRQs[] = I2C_IRQS;

static I2C_COM_CTR i2cCtr [3];

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/**
 * @brief Init I2C peripheral driver in Master Mode.
 * @param id: I2C Module number.
 */
void I2C_Init(uint8_t id)
{
	if(i2cCtr[id].isInit)
	{
		return;
	}

	/* 1-CLOCK GATING */

	// Clock gating PORTE
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

	// Clock gating I2C peripheral
	switch(id)
	{
		case(2):
			SIM->SCGC1 |= SIM_SCGC1_I2C2_MASK;
			break;
		case(1):
			SIM->SCGC4 |= SIM_SCGC4_I2C1_MASK;
			break;
		case(0):
			SIM->SCGC4 |= SIM_SCGC4_I2C0_MASK;
			break;
	}

    /* 2-CONFIGURE PINS */

	PORT_Type* portPtr[] = PORT_BASE_PTRS;

    //Set pins for I2C mode and Open Drain output
	portPtr[PIN2PORT(I2C_SDA)]->PCR[PIN2NUM(I2C_SDA)] = PORT_PCR_MUX(5) | PORT_PCR_ODE_MASK;
	portPtr[PIN2PORT(I2C_SCL)]->PCR[PIN2NUM(I2C_SCL)] = PORT_PCR_MUX(5) | PORT_PCR_ODE_MASK;

	/* 3-CONFIGURE MODULE */

    //Master mode
	i2cPtr[id]->A1 = 0;

	// Configure baud rate
	i2cPtr[id]->F = I2C_F_MULT(2) | I2C_F_ICR(5);

	//Enable I2C module and interrupts
	i2cPtr[id]->S = I2C_S_TCF_MASK | I2C_S_IICIF_MASK;   //Status register
	i2cPtr[id]->C1 = I2C_C1_IICEN(1) | I2C_C1_IICIE(1);  // Enables I2C module operation and interrupt requests.

	i2cCtr[id].stage = I2C_TRM_STAGE_NONE;

	//Enable the NVIC for the I2C peripheral
	NVIC_EnableIRQ(I2C_IRQs[id]);
}


/**
 * @brief Set Communication Control Structure for the I2C transmission.
 * @param
 */
void I2C_ConfigCom(I2C_INSTANCES id, uint8_t * data, uint8_t slave_address, i2c_callback callback)
{
	if(i2cCtr[id].stage == I2C_TRM_STAGE_NONE)
	{
		i2cCtr[id].device_address_r = (slave_address << 1) | 0x1;
		i2cCtr[id].device_address_w = (slave_address << 1) & 0xFE;
		i2cCtr[id].data = data;
		i2cCtr[id].callback = callback;
	}
}

/**
 * @brief Starts a transaction on the I2C bus. Non blocking.
 * @param
 */
void I2C_TXRX_NonBlocking(I2C_INSTANCES id, I2C_MODE mode, uint8_t data_size, uint8_t register_address)
{
	if(i2cCtr[id].stage == I2C_TRM_STAGE_NONE)
	{
		i2cCtr[id].fault = I2C_NO_FAULT;
		i2cCtr[id].mode = mode;
		i2cCtr[id].register_address = register_address;
		i2cCtr[id].data_size = data_size;
		i2cCtr[id].data_index = 0;
		i2cCtr[id].stage =  I2C_TRM_STAGE_WRITE_REG_ADDRESS;
		i2cCtr[id].isBlocking = false;

		i2cPtr[id]->C1 = (i2cPtr[id]->C1 & ~I2C_C1_TXAK_MASK) | I2C_C1_TXAK(0);
		i2cPtr[id]->C1 = (i2cPtr[id]->C1 & ~I2C_C1_TX_MASK) | I2C_C1_TX(1);
		i2cPtr[id]->C1 = (i2cPtr[id]->C1 & ~I2C_C1_MST_MASK) | I2C_C1_MST(1);

		I2C_WRITE_BYTE(i2cCtr[id].device_address_w);
	}
}


/**
 * @brief Getter: stage of transmission.
 * @param
 */
I2C_TRM_STAGE I2C_ComState(I2C_INSTANCES id)
{
	return i2cCtr[id].stage;
}


/**
 * @brief Getter: return error state.
 * @param
 */
I2C_FAULT I2C_ErrorState(I2C_INSTANCES id)
{
	return i2cCtr[id].fault;
}


/**
 * @brief Blocking reading transaction service.
 * @param
 */
I2C_FAULT I2C_RXBlocking(I2C_INSTANCES id, uint8_t data_size, uint8_t register_address)
{
	//NVIC_DisableIRQ(I2C_IRQs[id]);

	i2cCtr[id].register_address = register_address;
	i2cCtr[id].data_size = data_size;
	i2cCtr[id].data_index = 0;
	i2cCtr[id].fault = I2C_NO_FAULT;


	i2cPtr[id]->C1 = (i2cPtr[id]->C1 & ~I2C_C1_TXAK_MASK) | I2C_C1_TXAK(0);
	i2cPtr[id]->C1 = (i2cPtr[id]->C1 & ~I2C_C1_TX_MASK) | I2C_C1_TX(1);
	i2cPtr[id]->C1 = (i2cPtr[id]->C1 & ~I2C_C1_MST_MASK) | I2C_C1_MST(1);
	I2C_WRITE_BYTE(i2cCtr[id].device_address_r);

	while(!(I2C_GET_IRQ_FLAG)){}
	if(I2C_GET_TCF != 0){ return I2C_SLAVE_ERROR;}

	if(I2C_GET_RX_ACK == 0)
	{

		I2C_WRITE_BYTE(i2cCtr[id].register_address);

		while(!(I2C_GET_IRQ_FLAG)){}
		if(I2C_GET_TCF != 0){ return I2C_SLAVE_ERROR;}

		if((I2C_GET_RX_ACK) == 0)
		{

			I2C_REPEAT_START_SIGNAL;
			I2C_WRITE_BYTE(i2cCtr[id].device_address_r);

			while(!(I2C_GET_IRQ_FLAG)){}
			if(I2C_GET_TCF != 0){ return I2C_SLAVE_ERROR;}

			if(I2C_GET_RX_ACK == 0)
			{
				for (int i = 100; i>0; i--){};

				I2C_SET_RX_MODE;

				if(i2cCtr[id].data_index == i2cCtr[id].data_size-1)
				{
					I2C_SET_NACK;
				}

				uint8_t dummy = I2C_READ_BYTE;

				while(!(I2C_GET_IRQ_FLAG)){}
				if(I2C_GET_TCF != 0){ return I2C_SLAVE_ERROR;}

				while(i2cCtr[id].data_index <= i2cCtr[id].data_size)
				{
					if(i2cCtr[id].data_index == i2cCtr[id].data_size)
					{
						I2C_STOP_SIGNAL;
						I2C_CLEAR_NACK;
					}
					else
					{
						if(i2cCtr[id].data_index == i2cCtr[id].data_size-1)
						{
							I2C_SET_NACK;
						}
						i2cCtr[id].data[i2cCtr[id].data_index] = I2C_READ_BYTE;

						while(!(I2C_GET_IRQ_FLAG)){}
						if(I2C_GET_TCF != 0){ return I2C_SLAVE_ERROR;}
					}
					i2cCtr[id].data_index++;
				}
			}
			else
			{
				I2C_STOP_SIGNAL;
				i2cCtr[id].fault = I2C_SLAVE_ERROR;
				return i2cCtr[id].fault;
			}
		}

		else
		{
			I2C_STOP_SIGNAL;
			i2cCtr[id].fault = I2C_SLAVE_ERROR;
			return i2cCtr[id].fault;
		}
	}
	else
	{
		I2C_STOP_SIGNAL;
		i2cCtr[id].fault = I2C_SLAVE_ERROR;
		return i2cCtr[id].fault;
	}

	while(I2C_CHECK_BUS == BUS_BUSY) {}

	//NVIC_EnableIRQ(I2C_IRQs[id]);

	return I2C_NO_FAULT;

}



I2C_FAULT I2C_TXBlocking(I2C_INSTANCES id, uint8_t data_size, uint8_t register_address)
{
	//NVIC_DisableIRQ(I2C_IRQs[id]);

	i2cCtr[id].register_address = register_address;
	i2cCtr[id].data_size = data_size;
	i2cCtr[id].data_index = 0;

	if(I2C_CHECK_BUS != BUS_BUSY)
	{
		i2cPtr[id]->C1 = (i2cPtr[id]->C1 & ~I2C_C1_TXAK_MASK) | I2C_C1_TXAK(0);
		i2cPtr[id]->C1 = (i2cPtr[id]->C1 & ~I2C_C1_TX_MASK) | I2C_C1_TX(1);
		i2cPtr[id]->C1 = (i2cPtr[id]->C1 & ~I2C_C1_MST_MASK) | I2C_C1_MST(1);
		I2C_WRITE_BYTE(i2cCtr[id].device_address_w);

		while(!(I2C_GET_IRQ_FLAG)){}
		if(I2C_GET_TCF != 0){ return I2C_SLAVE_ERROR;}

		if(I2C_GET_RX_ACK == 0)
		{
			I2C_WRITE_BYTE(i2cCtr[id].register_address);

			while(!(I2C_GET_IRQ_FLAG)){}
			if(I2C_GET_TCF != 0){ return I2C_SLAVE_ERROR;}

			while(i2cCtr[id].data_index <= i2cCtr[id].data_size)
			{
				if(I2C_GET_RX_ACK == 0)
				{
					if(i2cCtr[id].data_index == i2cCtr[id].data_size)
					{
						I2C_STOP_SIGNAL;
					}
					else
					{
						I2C_WRITE_BYTE(i2cCtr[id].data[i2cCtr[id].data_index]);

						while(!(I2C_GET_IRQ_FLAG)){}
						if(I2C_GET_TCF != 0){ return I2C_SLAVE_ERROR;}
					}

					++(i2cCtr[id].data_index);
				}
				else
				{
					I2C_STOP_SIGNAL;
					return I2C_SLAVE_ERROR;
				}
			}
		}
		else
		{
			I2C_STOP_SIGNAL;
			return I2C_SLAVE_ERROR;
		}

	}
	else
	{
		return I2C_BUS_BUSY;
	}

	while(I2C_CHECK_BUS == BUS_BUSY) {}

	//NVIC_EnableIRQ(I2C_IRQs[id]);
	return I2C_NO_FAULT;

}


/*******************************************************************************
 *******************************************************************************/



__ISR__ I2C0_IRQHandler(void)
{
	if(!i2cCtr[0].isBlocking)
	{
		I2C_IRQDispatcher(I2C_INSTANCE_0);
	}
}

__ISR__ I2C1_IRQHandler(void)
{
	if(!i2cCtr[1].isBlocking)
	{
		I2C_IRQDispatcher(I2C_INSTANCE_1);
	}
}

__ISR__ I2C2_IRQHandler(void)
{
	if(!i2cCtr[2].isBlocking)
	{
		I2C_IRQDispatcher(I2C_INSTANCE_2);
	}
}


void I2C_IRQDispatcher(I2C_INSTANCES id)
{
	I2C_CLEAR_IRQ_FLAG;
	//If previous transfer was completed
	if(I2C_GET_TCF)
	{
		switch(i2cCtr[id].mode)
		{
			case I2C_MODE_WRITE:
			{
				switch(i2cCtr[id].stage)
				{
					case I2C_TRM_STAGE_WRITE_REG_ADDRESS:
					{
						//If no ACK is detected, error.
						if(I2C_GET_RX_ACK)
						{
							I2C_STOP_SIGNAL;
							i2cCtr[id].fault = I2C_SLAVE_ERROR;
							i2cCtr[id].stage = I2C_TRM_STAGE_NONE;
							//callback error
						}
						else
						{
							I2C_WRITE_BYTE(i2cCtr[id].register_address);
							i2cCtr[id].stage = I2C_TRM_STAGE_WRITE_DATA;
						}
						break;
					}
					case I2C_TRM_STAGE_WRITE_DATA:
					{
						if(I2C_GET_RX_ACK)
						{
							I2C_STOP_SIGNAL;
							i2cCtr[id].fault = I2C_SLAVE_ERROR;
							i2cCtr[id].stage = I2C_TRM_STAGE_NONE;
							//callback error
						}
						else
						{
							//If there are no data to write left, communication finishes.
							if(i2cCtr[id].data_index == i2cCtr[id].data_size)
							{
								I2C_STOP_SIGNAL;
								i2cCtr[id].fault = I2C_NO_FAULT;
								i2cCtr[id].stage = I2C_TRM_STAGE_NONE;
								i2cCtr[id].callback();
							}
							else
							{
								I2C_WRITE_BYTE((i2cCtr[id].data[i2cCtr[id].data_index++]));
							}
						}
					}
					default:
						break;
				}
				break;
			}
			case(I2C_MODE_READ):
			{
				switch(i2cCtr[id].stage)
				{
					case I2C_TRM_STAGE_WRITE_REG_ADDRESS:
					{
						if(I2C_GET_RX_ACK)
						{
							I2C_STOP_SIGNAL;
							i2cCtr[id].fault = I2C_SLAVE_ERROR;
							i2cCtr[id].stage = I2C_TRM_STAGE_NONE;
							//callback error
						}
						else
						{
							i2cCtr[id].stage = I2C_TRM_STAGE_WRITE_DEV_ADDRESS_R;
							I2C_WRITE_BYTE(i2cCtr[id].register_address);
						}
						break;

					}
					//Repeated start
					case I2C_TRM_STAGE_WRITE_DEV_ADDRESS_R:
					{
						if(I2C_GET_RX_ACK)
						{
							I2C_STOP_SIGNAL;
							i2cCtr[id].fault = I2C_SLAVE_ERROR;
							i2cCtr[id].stage = I2C_TRM_STAGE_NONE;
							//callback error
						}
						else
						{
							i2cCtr[id].stage = I2C_TRM_STAGE_READ_DUMMY_DATA;
							I2C_REPEAT_START_SIGNAL;
							I2C_WRITE_BYTE(i2cCtr[id].device_address_r);
						}
						break;
					}
					case I2C_TRM_STAGE_READ_DUMMY_DATA:
					{
						if(I2C_GET_RX_ACK)
						{
							I2C_STOP_SIGNAL;
							i2cCtr[id].fault = I2C_SLAVE_ERROR;
							i2cCtr[id].stage = I2C_TRM_STAGE_NONE;
							//callback error
						}
						else
						{
							i2cCtr[id].stage = I2C_TRM_STAGE_READ_DATA;
							//Set reading transmission mode.
							I2C_SET_RX_MODE;
							I2C_READ_BYTE;
						}
						break;
					}
					case I2C_TRM_STAGE_READ_DATA:
					{
						//Send stop signal
						if(i2cCtr[id].data_index == i2cCtr[id].data_size)
						{
							I2C_STOP_SIGNAL;
							I2C_CLEAR_NACK;
							i2cCtr[id].fault = I2C_NO_FAULT;
							i2cCtr[id].stage = I2C_TRM_STAGE_NONE;
							i2cCtr[id].callback();
						}
						else
						{
							//Last data
							if(i2cCtr[id].data_index == i2cCtr[id].data_size-1)
							{
								I2C_SET_NACK;
							}
							i2cCtr[id].data[i2cCtr[id].data_index++] = I2C_READ_BYTE;
						}
						break;
					}
					default:
						break;
				}
				break;
			}
		}
	}
}


/*******************************************************************************
 ******************************************************************************/
