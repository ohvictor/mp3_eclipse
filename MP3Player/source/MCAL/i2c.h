/*******************************************************************************
  @file     i2c.h
  @brief    I2C driver
  @author   Grupo 3
 ******************************************************************************/

#ifndef _I2C_H_
#define _I2C_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define I2C_MODULES_NUM		3

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

// I2C IRQ Callback
typedef void (*i2c_callback)(void);


//I2C MODE OF TRANSMITION
typedef enum
{
	 I2C_MODE_READ = 0,
	 I2C_MODE_WRITE,
} I2C_MODE;

//I2C FAULT STATES
typedef enum
{
	 I2C_NO_FAULT = 0,
	 I2C_BUS_BUSY,
	 I2C_TIMEOUT,
	 I2C_SLAVE_ERROR,
} I2C_FAULT;

//I2C_INSTANCES STATES
typedef enum {
  I2C_INSTANCE_0,
  I2C_INSTANCE_1,
  I2C_INSTANCE_2,
} I2C_INSTANCES;


//I2C CONTROL TRANSMISSION STAGE STRUCT
typedef enum
{
	I2C_TRM_STAGE_NONE = 0,
	I2C_TRM_STAGE_WRITE_DATA,
	I2C_TRM_STAGE_WRITE_REG_ADDRESS,
	I2C_TRM_STAGE_WRITE_DEV_ADDRESS_R,
	I2C_TRM_STAGE_READ_DUMMY_DATA,
	I2C_TRM_STAGE_READ_DATA,
	I2C_TRM_STAGE_PROCESS_DATA,
} I2C_TRM_STAGE;


/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Init I2C peripheral driver in Master Mode.
 * @param id: I2C Module number.
 */
void I2C_Init(uint8_t id);

/**
 * @brief Set Communication Control Structure for the I2C transmission.
 * @param
 */
void I2C_ConfigCom(I2C_INSTANCES id, uint8_t * data, uint8_t slave_address, i2c_callback callback);


/**
 * @brief Starts a transaction on the I2C bus for NON BLOCKING communication.
 * @param
 */
void I2C_TXRX_NonBlocking(I2C_INSTANCES id, I2C_MODE mode, uint8_t data_size, uint8_t register_address);

/**
 * @brief Getter: stage of transmission.
 * @param
 */
I2C_TRM_STAGE I2C_ComState(I2C_INSTANCES instance);

/**
 * @brief Getter: return error state.
 * @param
 */
I2C_FAULT I2C_ErrorState(I2C_INSTANCES instance);

/**
 * @brief Blocking reading transaction service.
 * @param
 */
I2C_FAULT I2C_RXBlocking(I2C_INSTANCES instance, uint8_t data_size, uint8_t register_address);

/**
 * @brief Blocking writing transaction service.
 * @param
 */
I2C_FAULT I2C_TXBlocking(I2C_INSTANCES instance, uint8_t data_size, uint8_t register_address);


/*******************************************************************************
 ******************************************************************************/

#endif // _I2C_H_

