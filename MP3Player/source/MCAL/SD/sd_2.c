/***************************************************************************//**
  @file		
  @brief	
  @author	
  @date		
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "sd.h"
#include "gpio.h"
#include "MK64F12.h"

#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

// SD Pinout

#define SD_PORT				PORTE

#define SD_SWITCH_PIN		PORTNUM2PIN(PE, 6)
#define SD_DCLK_PIN		  	2
#define SD_CMD_PIN		  	3

#define SD_D0_PIN			1
#define SD_D1_PIN			0
#define SD_D2_PIN			5
#define SD_D3_PIN			4

#define SD_MUX_MODE 		PORT_mAlt4	// 4 

#define SD_CARD_PRESENT_STATE	HIGH

#define SD_MAX_BLOCK_SIZE			1024
#define SD_CLOCK_FREQUENCY		  	(96000000U)
#define SD_RESET_TIMEOUT			4 * SD_CLOCK_FREQUENCY

#define SD_RESPONSE_NONE				0UL		// No response
#define SD_RESPONSE_LONG				1UL		// 136 bit
#define SD_RESPONSE_SHORT				2UL		// 48 bit
#define SD_RESPONSE_SHORT_BUSY			3UL		// Short response with busy

#define SDHC_RESPONSE_LENGTH_NONE	0UL
#define SDHC_RESPONSE_LENGTH_48		1UL
#define SDHC_RESPONSE_LENGTH_136	2UL
#define SDHC_RESPONSE_LENGTH_48BUSY	3UL

#define SDHC_COMMAND_CHECK_CCR		SDHC_XFERTYP_CCCEN(0b1)
#define SDHC_COMMAND_CHECK_INDEX	SDHC_XFERTYP_CICEN(0b1)

#define SD_RESPONSE_NONE		0UL		// No response expected
#define SD_RESPONSE_LONG		1UL		// Long response (136-bit)
#define SD_RESPONSE_SHORT		2UL		// Short response (48-bit)
#define SD_RESPONSE_SHORT_BUSY	3UL		// Short response with busy signal (48-bit)

#define OCR_VDD32_33		(1U << 20U)
#define OCR_VDD33_34		(1U << 21U)
#define OCR_BUSY_MASK		(0x80000000U)

#define SDHC_OCR		(OCR_VDD32_33 | OCR_VDD33_34)

#define SDHC_DTW_4BIT		(1U)

#define SDHC_IRQSTAT_ERRORS_MASK	(0x117F0000U)

#define SD_BLKSIZE			(512U)

#define RCA_ARG_SHIFT		(16U)
#define RCA_ARG(rca)		((rca) << RCA_ARG_SHIFT)

#define CMD8_ARG			(0x1AAU)

#define ACMD41_HCS_SHIFT	(30U)
#define ACMD41_HCS(x)		((x) << ACMD41_HCS_SHIFT)
#define ACMD41_XPC_SHIFT	(28U)
#define ACMD41_XPC(x)		((x) << ACMD41_XPC_SHIFT)
#define ACMD41_S18R_SHIFT	(24U)
#define ACMD41_S18R(x)		((x) << ACMD41_S18R_SHIFT)
#define ACMD41_OCR_SHIFT	(0U)
#define ACMD41_OCR(x)		((x) << ACMD41_OCR_SHIFT)
#define ACMD41_ARG(hcs, xpc, s18r, ocr)		(ACMD41_HCS(hcs) | ACMD41_XPC(xpc) | ACMD41_S18R(s18r) | ACMD41_OCR(ocr))

#define ACMD6_4BIT_ARG		(2U)

#define CMD_GO_IDLE_STATE			0
#define CMD_SEND_IF_COND			8
#define CMD_APP_CMD					55
#define CMD_SD_SEND_OP_COND			41
#define CMD_ALL_SEND_CID			2
#define CMD_SEND_RELATIVE_ADDR		3
#define CMD_SEND_CSD				9
#define CMD_SELECT_DESELECT_CARD	7
#define CMD_APP_CMD					55
#define CMD_SET_BUS_WIDTH			6
#define CMD_SET_BLOCKLEN			16
#define CMD_READ_SINGLE_BLOCK		17
#define CMD_READ_MULTIPLE_BLOCKS	18

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum {	SD_NO_RESPONSE,
				SD_RESPONSE_R1,
				SD_RESPONSE_R1b,
				SD_RESPONSE_R2,
				SD_RESPONSE_R3,
				SD_RESPONSE_R4,
				SD_RESPONSE_R5,
				SD_RESPONSE_R6,
				SD_RESPONSE_R7,
				SD_RESPONSE_DATA_SINGLE,
				SD_RESPONSE_DATA_MULTI
				} SDRESPONSE_TYPE;

typedef struct {
	uint8_t  status;
	uint8_t  highCapacity;
	uint8_t  tranSpeed;
	uint32_t address;
	uint32_t numBlocks;
	uint32_t lastCardStatus;
} SDDESCRIPTOR;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void 		sd_init(void);

static void 		sd_card_inserted_handler();

static uint32_t 	sd_send_cmd(uint8_t cmd, uint32_t argument, SDRESPONSE_TYPE rspIndex, uint32_t* response);

static bool 		is_sd_card_inserted();

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static DSTATUS SDState = STA_NOINIT | STA_NODISK;	// Set NOINIT when reset

static uint16_t relative_card_address = 0x0U;		

static SDDESCRIPTOR SDDescriptor;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/*
* Required functions for fatfs library
*/

DSTATUS sd_disk_status () {
	return SDState;
}

DSTATUS sd_disk_initialize() {

    // Check if the SD card is not initialized
    if (SDState & STA_NOINIT) {
        SDState |= STA_NOINIT;  // Set STA_NOINIT flag
        sd_init();  			// Initialize hardware
        SDHC->SYSCTL |= SDHC_SYSCTL_RSTA_MASK;  // Software Reset SDHC

        // Configure SDHC for little-endian and 1-bit mode
        SDHC->PROCTL = SDHC_PROCTL_EMODE(2U);
        SDHC->IRQSIGEN = 0U;  // Disable all IRQs
        SDHC->VENDOR = 0U;  // Disable external DMA
        SDHC->BLKATTR = SDHC_BLKATTR_BLKSIZE(SD_BLKSIZE);

        // Check if an SD card is inserted
        if (is_sd_card_inserted()) {
            // Set SD card clock to approximately 400kHz
            SDHC->SYSCTL = SDHC_SYSCTL_SDCLKFS(0x80) | SDHC_SYSCTL_SDCLKEN_MASK;
            while (SDHC->PRSSTAT & (SDHC_PRSSTAT_CIHB_MASK | SDHC_PRSSTAT_CDIHB_MASK));  // Wait CMD and DAT

            // Send INIT signal to the card
            SDHC->SYSCTL |= SDHC_SYSCTL_INITA_MASK;
            uint32_t response[4], error;

            // CMD0: GO_IDLE_STATE
            error = sd_send_cmd(CMD_GO_IDLE_STATE, 0x0, SD_NO_RESPONSE, NULL);
            if (error) {
                return SDState;  // Initialization failed
            }

            // CMD8: SEND_IF_COND (check voltage)
            error = sd_send_cmd(CMD_SEND_IF_COND, CMD8_ARG, SD_RESPONSE_R7, response);
            if (response[0] != CMD8_ARG || error) {
                return SDState;  // Voltage not supported or error
            }

            uint32_t rca = 0x0U;  // Reset Relative Card Address (RCA)
            uint32_t tries = 1000;  // Timeout for ACMD41

            do {
                // CMD55: APP_CMD
                error = sd_send_cmd(CMD_APP_CMD, RCA_ARG(rca), SD_RESPONSE_R1, response);
                if (error) {
                    return SDState;  // Error in CMD55
                }

                // ACMD41: SD_SEND_OP_COND
                error = sd_send_cmd(CMD_SD_SEND_OP_COND, ACMD41_ARG(1U, 1U, 0U, SDHC_OCR), SD_RESPONSE_R3, response);
                if (!(response[0] & SDHC_OCR) || error) {
                    return SDState;  // Initialization failed
                }

                uint32_t cont = 0xFFFFF;
                while (cont--);
            } while (!(response[0] & OCR_BUSY_MASK) && --tries);  // Repeat while busy

            // CMD2: ALL_SEND_CID
            error = sd_send_cmd(CMD_ALL_SEND_CID, 0x0U, SD_RESPONSE_R2, response);
            if (error) {
                return SDState;  // Error in CMD2
            }

            // CMD3: SEND_RELATIVE_ADDR
            error = sd_send_cmd(CMD_SEND_RELATIVE_ADDR, 0x0U, SD_RESPONSE_R6, response);
            if (error) {
                return SDState;  // Error in CMD3
            }
            rca = response[0] >> RCA_ARG_SHIFT;  // Get Relative Card Address (RCA)

            // CMD9: SEND_CSD
            error = sd_send_cmd(CMD_SEND_CSD, RCA_ARG(rca), SD_RESPONSE_R2, response);
            if (error) {
                return SDState;  // Error in CMD9
            }

            // Change SDHC clock to 25MHz
            SDHC->SYSCTL &= ~SDHC_SYSCTL_SDCLKEN_MASK;  // Disable CLK
            SDHC->SYSCTL = SDHC_SYSCTL_SDCLKFS(2U) | SDHC_SYSCTL_DTOCV(0xEU);  // Set Frequency to 25MHz
            while (!(SDHC->PRSSTAT & SDHC_PRSSTAT_SDSTB_MASK));  // Wait stable CLK
            SDHC->SYSCTL |= SDHC_SYSCTL_SDCLKEN_MASK;  // Enable CLK

            // CMD7: SELECT/DESELECT CARD
            error = sd_send_cmd(CMD_SELECT_DESELECT_CARD, RCA_ARG(rca), SD_RESPONSE_R1b, response);
            if (error) {
                return SDState;  // Error in CMD7
            }

            // CMD55: APP_CMD
            error = sd_send_cmd(CMD_APP_CMD, RCA_ARG(rca), SD_RESPONSE_R1, response);
            if (error) {
                return SDState;  // Error in CMD55
            }

            // ACMD6: SET_BUS_WIDTH
            error = sd_send_cmd(CMD_SET_BUS_WIDTH, ACMD6_4BIT_ARG, SD_RESPONSE_R1, response);
            if (error) {
                return SDState;  // Error in ACMD6
            }
            SDHC->PROCTL |= SDHC_PROCTL_DTW(SDHC_DTW_4BIT);  // Change SDHC to 4-bit

            // CMD16: SET_BLOCKLEN
            error = sd_send_cmd(CMD_SET_BLOCKLEN, SD_BLKSIZE, SD_RESPONSE_R1, response);
            if (error) {
                return SDState;  // Error in CMD16
            }

            SDState = 0U;  // Initialization succeed
        } else {
            // No SD card inserted
            SDState |= STA_NODISK;
        }
    }
    return SDState;  // Return SD card state
}

/*
DSTATUS sd_disk_initialize ()
{
	//Set Initial State and Initialize Hardware:
	SDState |= STA_NOINIT; // Set the SD card state to not initialized.
	sd_init(); // Initialize the SD card hardware.

	//Perform a software reset on the SDHC (Secure Digital Host Controller)
	SDHC->SYSCTL |= SDHC_SYSCTL_RSTA_MASK | SDHC_SYSCTL_SDCLKFS(0x80);
	while (SDHC->SYSCTL & SDHC_SYSCTL_RSTA_MASK); 	// wait

	//Configure SD
	SDHC->PROCTL = SDHC_PROCTL_EMODE(2U); //Set the endianness and data bus width (1-bit mode).
	SDHC->IRQSIGEN = 0U; //Disable all interrupts.
	SDHC->VENDOR = 0U; //Disable external DMA.
	SDHC->BLKATTR = SDHC_BLKATTR_BLKSIZE(SD_MAX_BLOCK_SIZE); //Set the block size.
	
	//Check if SD Card is Inserted:
	if (is_sd_card_inserted()) { 

		// Configure Clock and Wait for stability	
		SDHC->SYSCTL = SDHC_SYSCTL_SDCLKFS(0x80) | SDHC_SYSCTL_SDCLKEN_MASK; 		//Set the clock to approximately 400kHz.
		while (SDHC->PRSSTAT & (SDHC_PRSSTAT_CIHB_MASK | SDHC_PRSSTAT_CDIHB_MASK)); //Wait for command and data lines to be idle.
		
		// Send Initialization Signal:
		SDHC->SYSCTL |= SDHC_SYSCTL_INITA_MASK; //end the initialization signal to the card.

		// Send CMD0 (Go Idle State)
		sd_send_cmd(0, 0x0, SDNoResponse, NULL);

		// Send CMD8 (Send Interface Condition)
		sd_send_cmd(8, CMD8_ARG, SDResponseR7, res); //Send CMD8 to check the voltage range.

		// Send ACMD41 (Send Operation Condition)
		// Card Identification and Initialization
		// Send CMD55 and ACMD41 in a loop until the card is ready
		while sd_ACMD41_SendOperationCond(0) != SDHC_RESULT_OK);

		// Send CMD2 to get the card identification.
		sd_send_cmd(2, 0x0U, SDResponseR2, res);

		// Send CMD9 to get card parameters:
		sd_send_cmd(9, RCA_ARG(rca), SDResponseR2, res);

		// Change Clock to 25MHz


		// Send CMD7 to select the card.
		sd_send_cmd(7, RCA_ARG(rca), SDResponseR1b, res);


		// Change to 4-bit Mode:
		sd_send_cmd(55, RCA_ARG(rca), SDResponseR1, res);
		//Send ACMD6 to set the bus width to 4 bits.
		sd_send_cmd(6, ACMD6_4BIT_ARG, SDResponseR1, res);
		SDHC->PROCTL |= SDHC_PROCTL_DTW(SDHC_DTW_4BIT); //Change SDHC to 4-bit mode.

		// Send CMD16 to set the block size.
		sd_send_cmd(16, SD_BLKSIZE, SDResponseR1, res);

		//Update Initialization Status:
		SDState = 0U;
	}
	else {
		// Set the "no disk" state if no SD card is inserted
		SDState |= STA_NODISK;
	}
	return SDState;
}
*/

/*
dataBuffer: Pointer to the read data buffer.
startSector: Start sector number.
sectorCount: Number of sectors to read.
*/
DRESULT sd_disk_read(BYTE* dataBuffer, LBA_t startSector, UINT sectorCount) {
    // Check if the SD card is not initialized
    if (SDState & STA_NOINIT) {
        return RES_NOTRDY;  // Card not ready
    }

    // Initialize local variables
    uint32_t response;
    uint32_t errorStatus = 0xFFFFFFFF;
    uint32_t dataIndex = 0;
    uint8_t readWatermarkLevel = SDHC->WML & SDHC_WML_RDWML_MASK;

    // Check if it's a single sector read
    if (sectorCount == 1) {
        errorStatus = sd_send_cmd(CMD_READ_SINGLE_BLOCK, startSector, SD_RESPONSE_DATA_SINGLE, &response);
        if (errorStatus) {
            return RES_ERROR;  // Operation failed
        }
    }
    // For multiple sector read
    else {
        // Set up SDHC for multiple block read
        SDHC->BLKATTR = SDHC_BLKATTR_BLKCNT(sectorCount) | SDHC_BLKATTR_BLKSIZE(SD_BLKSIZE);
        errorStatus = sd_send_cmd(CMD_READ_MULTIPLE_BLOCKS, startSector, SD_RESPONSE_DATA_MULTI, &response);
        if (errorStatus) {
            return RES_ERROR;  // Operation failed
        }
    }

    // Data Transfer Loop
    while (!(SDHC->IRQSTAT & SDHC_IRQSTAT_TC_MASK)) {
        if (SDHC->PRSSTAT & SDHC_PRSSTAT_BREN_MASK) {
            // Read data from DATPORT in 32-bit chunks
            for (int i = 0; i < readWatermarkLevel; i++) {
                ((uint32_t*)dataBuffer)[dataIndex] = SDHC->DATPORT;
                dataIndex++;
            }
        }
    }

    // Check for errors
    errorStatus = SDHC->IRQSTAT & SDHC_IRQSTAT_ERRORS_MASK;
    SDHC->IRQSTAT |= SDHC_IRQSTAT_TC_MASK;  // Clear Transfer Complete flag

    // Return status
    return errorStatus ? RES_ERROR : RES_OK;  // Return success or error
}


/***************************** LOW Level SDHC interface ********************************/


void sd_init()
{
	if (!SDDescriptor.status)
	{

		/* INIT HARDWARE */

		// Initialize port connections to detect switch
    	gpioMode(SD_SWITCH_PIN, INPUT_PULLDOWN);						
		gpioIRQ(SD_SWITCH_PIN, GPIO_IRQ_MODE_BOTH_EDGES, sd_card_inserted_handler);

    	// PCR registers
		SD_PORT->PCR[SD_D0_PIN] 	= 	PORT_PCR_MUX(SD_MUX_MODE) | PORT_PCR_PE(1) | PORT_PCR_PS(1);
		SD_PORT->PCR[SD_D1_PIN] 	= 	PORT_PCR_MUX(SD_MUX_MODE) | PORT_PCR_PE(1) | PORT_PCR_PS(1);
		SD_PORT->PCR[SD_D2_PIN] 	= 	PORT_PCR_MUX(SD_MUX_MODE) | PORT_PCR_PE(1) | PORT_PCR_PS(1);
		SD_PORT->PCR[SD_D3_PIN] 	= 	PORT_PCR_MUX(SD_MUX_MODE) | PORT_PCR_PE(1) | PORT_PCR_PS(1);

		SD_PORT->PCR[SD_CMD_PIN] 	= 	PORT_PCR_MUX(SD_MUX_MODE) | PORT_PCR_PE(1) | PORT_PCR_PS(1);
		SD_PORT->PCR[SD_DCLK_PIN] 	= 	PORT_PCR_MUX(SD_MUX_MODE) | PORT_PCR_PE(1) | PORT_PCR_PS(1);

		// Clock gating for both SD & PORTE peripherals
		SIM->SCGC3 = (SIM->SCGC3 & ~SIM_SCGC3_SDHC_MASK)  | SIM_SCGC3_SDHC(1);
		SIM->SCGC5 = (SIM->SCGC5 & ~SIM_SCGC5_PORTE_MASK) | SIM_SCGC5_PORTE(1);

		// Clear interrupt status
		SDHC->IRQSTAT 	= 	0xFFFFFFFF;

		// Enable requests 
		SDHC->IRQSTATEN =  	SDHC_IRQSTAT_CRM_MASK | SDHC_IRQSTATEN_CIESEN_MASK | SDHC_IRQSTATEN_TCSEN_MASK | SDHC_IRQSTATEN_CCSEN_MASK;
		NVIC_EnableIRQ(SDHC_IRQn);

		sd_card_inserted_handler();	// Check if card is inserted

		// Initialization successful
    	SDDescriptor.status = true;
	}
}

/*
command: 			The command index to be sent to the SD card.
argument: 			The argument associated with the command.
responseType: 		An enumeration indicating the type of response expected.
responseBuffer: 	A pointer to an array where the response data will be stored.
*/
static uint32_t sd_send_cmd(uint8_t command, uint32_t argument, SDRESPONSE_TYPE responseType, uint32_t* responseBuffer) {
    uint32_t errorStatus = 0xFFFFFFFF;
    uint32_t transferType = SDHC_XFERTYP_CMDINX(command);
    uint8_t responseLength = 0;

    // Determine the expected response length based on responseType
    switch (responseType) {
        case SD_NO_RESPONSE:
            responseLength = SDHC_RESPONSE_LENGTH_NONE;
			transferType |=  SDHC_XFERTYP_RSPTYP(SD_RESPONSE_NONE);
            break;
        case SD_RESPONSE_R1:
        case SD_RESPONSE_R5:
        case SD_RESPONSE_R6:
        case SD_RESPONSE_R7:
            responseLength = SDHC_RESPONSE_LENGTH_48;
            transferType |= SDHC_XFERTYP_RSPTYP(SD_RESPONSE_SHORT) | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_CICEN_MASK;
            break;
        case SD_RESPONSE_R1b:
            responseLength = SDHC_RESPONSE_LENGTH_48;
            transferType |= SDHC_XFERTYP_RSPTYP(SD_RESPONSE_SHORT) | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_CICEN_MASK;
            break;
        case SD_RESPONSE_R2:
            responseLength = SDHC_RESPONSE_LENGTH_136;
            transferType |= SDHC_XFERTYP_RSPTYP(SD_RESPONSE_LONG) | SDHC_XFERTYP_CCCEN_MASK;
            break;
        case SD_RESPONSE_R3:
        case SD_RESPONSE_R4:
            responseLength = SDHC_RESPONSE_LENGTH_48;
			transferType |= SDHC_XFERTYP_RSPTYP(SD_RESPONSE_SHORT);
            break;
        case SD_RESPONSE_DATA_SINGLE:
            responseLength = SDHC_RESPONSE_LENGTH_48;
            transferType |= SDHC_XFERTYP_RSPTYP(SD_RESPONSE_SHORT) | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_CICEN_MASK |
							SDHC_XFERTYP_DPSEL_MASK | SDHC_XFERTYP_DTDSEL_MASK;
            break;
        case SD_RESPONSE_DATA_MULTI:
            responseLength = SDHC_RESPONSE_LENGTH_48;
            transferType |= SDHC_XFERTYP_RSPTYP(SD_RESPONSE_SHORT) | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_CICEN_MASK |
							SDHC_XFERTYP_DPSEL_MASK | SDHC_XFERTYP_DTDSEL_MASK |
							SDHC_XFERTYP_BCEN_MASK | SDHC_XFERTYP_MSBSEL_MASK | SDHC_XFERTYP_AC12EN_MASK;
            break;
        default:
            return errorStatus;
    }

    // Check if command and data lines are not busy
    if (!(SDHC->PRSSTAT & (SDHC_PRSSTAT_CIHB_MASK | SDHC_PRSSTAT_CDIHB_MASK | SDHC_PRSSTAT_WTA_MASK))) {
        // Set command argument and transfer type
        SDHC->CMDARG = argument;
        SDHC->XFERTYP = transferType;

        // Wait for command complete
        while (!(SDHC->IRQSTAT & SDHC_IRQSTAT_CC_MASK));

        // Get and handle errors
        errorStatus = SDHC->IRQSTAT & SDHC_IRQSTAT_ERRORS_MASK;
        SDHC->IRQSTAT |= errorStatus;
        errorStatus &= 0xFFFFFFFE;

        // If a response is expected and a response buffer is provided, retrieve the response data
        if (responseLength != SDHC_RESPONSE_LENGTH_NONE && responseBuffer != NULL) {
            responseBuffer[0] = SDHC->CMDRSP[0];
            if (responseLength == SDHC_RESPONSE_LENGTH_NONE) {
                responseBuffer[1] = SDHC->CMDRSP[1];
                responseBuffer[2] = SDHC->CMDRSP[2];
                responseBuffer[3] = SDHC->CMDRSP[3];
            }
        }
    }

    return errorStatus;
}

static void sd_card_inserted_handler()
{
  if (gpioRead(SD_SWITCH_PIN) == HIGH) {
		SDState = STA_NOINIT;
  }
  else {
    	SDState = STA_NOINIT | STA_NODISK;
  }
}


bool is_sd_card_inserted() {
	return !(SDState & STA_NODISK);
}

