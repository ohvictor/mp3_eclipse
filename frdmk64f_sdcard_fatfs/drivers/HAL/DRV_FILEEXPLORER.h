/***************************************************************************//**
  @file     DRV_FILEEXPLORER.h
  @brief    Functions to explore the filesystem.
  @author   Matias Bergerman, Pablo Gonzalez Leiro, Milagros Moutin, Pedro Carranza Velez
 ******************************************************************************/

#ifndef HAL_DRV_FILEEXPLORER_H_
#define HAL_DRV_FILEEXPLORER_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "fsl_common.h"
#include "ff.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

//#define DEBUG_FILEEXPLORER

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/**
 * @brief METADATA struct with the information of the current file.
 * 
 */
typedef struct {
	char name[FF_LFN_BUF];
	char altname[FF_SFN_BUF];
	uint16_t year;
	uint8_t	month;
	uint8_t	day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	bool	amdir;
} METADATA_t;


/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/




/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initialize the file explorer module.
 * 
 * @param path to the folder to explore. The default is the root folder with path==NULL.
 * @return status_t 
 */
status_t fileExplorerInit(const char* path);

/**
 * @brief Get the Curr File object. 
 * 
 * @return FILINFO* 
 */
FILINFO* getCurrFile(void);

/**
 * @brief Get the Prev File object. Works as a circular buffer.
 * 
 * @return FILINFO* 
 */
FILINFO* getPrevFile(void);


/**
 * @brief Get the Next File object. Works as a circular buffer.
 * 
 * @return FILINFO* 
 */
FILINFO* getNextFile(void);

/**
 * @brief Get the Current Metadata object
 * 
 * @return METADATA_t 
 */
METADATA_t getMetadata(void);

/**
 * @brief Get the Path object
 * 
 * @param path 
 */
void getPath(char *path);

/**
 * @brief Access current file (only if it is a folder)
 * 
 * @return status_t 
 */
status_t accessFolder(void);

/**
 * @brief Go to the previous folder
 * 
 * @return status_t 
 */
status_t goBack(void);


/**
 * @brief Test module
 * 
 */
void testFileExplorer(void);

/*******************************************************************************
 ******************************************************************************/

#endif // HAL_DRV_FILEEXPLORER_H_


