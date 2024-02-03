/*******************************************************************************
  @file		
  @brief	
  @author	
  @date		
 ******************************************************************************/
#ifndef _SD_H_
#define _SD_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "fatfs/diskio.h"
#include "fatfs/ff.h"
#include <stdbool.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define SD_DISK	1

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

DSTATUS SD_disk_status ();

DSTATUS SD_disk_initialize ();

DRESULT SD_disk_read (
  BYTE* buff,    /* [OUT] Pointer to the read data buffer */
  LBA_t sector,  /* [IN] Start sector number */
  UINT count     /* [IN] Number of sectros to read */
);

/*******************************************************************************
 ******************************************************************************/

#endif // _UART_H_