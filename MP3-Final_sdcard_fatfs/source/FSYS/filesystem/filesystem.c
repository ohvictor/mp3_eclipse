/***************************************************************************//**
  @file		fileSystem.c
  @brief	Manejo de archivos y directorios en la tarjeta SD
  @author
  @date
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILE
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include "ff.h"
#include "diskio.h"
#include "fsl_port.h"
#include "fsl_sd.h"
#include "fsl_sd_disk.h"
#include "sdmmc_config.h"
#include "fsl_sysmpu.h"
#include "filesystem.h"
#include "clock_config.h"
#include "board.h"
#include "sdmmc_config.h"



/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define FILE_ARRAY_SIZE 100

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

// SD flags and status variables
static bool SD_connected = false;
static bool SD_error = false;
static bool SD_hostInitDone = false;
static FATFS g_fileSystem;
static uint8_t SD_status = 0;

// Filesystem flags and status variables
FRESULT error;
const TCHAR driver_number_buffer[3U] = {SDDISK + '0', ':', '/'}; 	// Path name
char ch                            = '0';

//const TCHAR driver_number_buffer[4U] = {SDDISK + '0', ':', '/', '\0'};
FATFS file_system;
FIL file_object;
DIR directory; /* Directory object */
FILINFO fileInformation;
UINT bytesWritten;
UINT bytesRead;

// Filesystem
file_t file_array[FILE_ARRAY_SIZE] = {};
int files_count = 0;

BYTE work[FF_MAX_SS];



/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/


void SD_callback(bool isInserted, void *userData);

void filesystem_map_files_handler(char * filepath);

status_t sdcardWaitCardInsert(void);



/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
/**
 * @brief Init filesystem module
*/
bool filesystem_init(void)
{
    if(!SD_hostInitDone)
    {

		BOARD_InitPins();
		BOARD_BootClockRUN();
	    BOARD_InitDebugConsole();
		SYSMPU_Enable(SYSMPU, false);

	    if (sdcardWaitCardInsert() != kStatus_Success)
	    {
	        return false;
	    }
	    /*
        SYSMPU->CESR &= ~SYSMPU_CESR_VLD_MASK;

        BOARD_SD_Config(&g_sd, SD_callback, BOARD_SDMMC_SD_HOST_IRQ_PRIORITY, NULL);

        if (SD_HostInit(&g_sd) != kStatus_Success)
        {
            //printf("\r\nSD host init fail\r\n");
            SD_error = true;
        }
        */

    	SD_hostInitDone = true;

        if (f_mount(&g_fileSystem, driver_number_buffer, 0U))
	    {
            SD_error = true;
            return false;
	    }
        error = f_chdrive((char const *)&driver_number_buffer[0U]);
        if (error)
        {
            SD_error = true;
            return false;
        }
        /*
		#if FF_USE_MKFS
			//PRINTF("\r\nMake file system......The time may be long if the card capacity is big.\r\n");
			if (f_mkfs(driver_number_buffer, 0, work, sizeof work))
			{
				//PRINTF("Make file system failed.\r\n");
				return false;
			}
		#endif
		*/

        return true;
    }
    else{
    	return false;
    }
}

/**
 * @brief Deinit filesystem module
*/
void filesystem_deinit(void)
{
	const TCHAR driver_number_buffer[3U] = {SDDISK + '0', ':', '/'};
	f_mount(NULL, driver_number_buffer, 1U);
	g_sd.isHostReady = false;
    SD_hostInitDone = false;
}


/**
 * @brief
 * @param
*/
void filesystem_map_files(void)
{
	files_count = 0;
	char buffer[FILE_ARRAY_SIZE] = {0U};
    filesystem_map_files_handler(buffer);
}

/*
 * @brief Prints all the files of the file system.
 * */
void filesystem_print_files(void)
{
    printf("Files in SD card: \n");
	for (int i = 0; i < files_count; i++)
	{
		printf("Track: %d -> %s\n", i, file_array[i].file_path);
	}
	printf("\n");
}

/*
 * @brief Reset the file system and return a new first file
 *
 * */
file_t filesystem_reset_files(void)
{
    files_count = 0;
	return filesystem_get_first_file();
}

/*
 * @brief
 * @param
 * @return
 * */
char* filesystem_get_file_name(file_t file)
{
    char str[FILENAME_LENGTH];
	strcpy(str, file.file_path);
	char* pch = strtok(str, "/"); // pointer to character
	char filename_str[FILENAME_LENGTH];
	while (pch != NULL)
	{
		//printf("%s\n", pch);
		strcpy(filename_str, pch);
		pch = strtok(NULL, "/");
	}
	char* file_name;
	file_name = strtok(filename_str, ".");
	return file_name;
}


/*
 * @brief Gets the amount of files that exist in the file system.
 */
int filesystem_get_files_count(void)
{
    return files_count;
}

/*
 * @brief Detects if a file's path corresponds to a .mp3 file.
 * */
bool filesystem_is_MP3file(char *filepath)
{
    char *extension;
	if ((extension = strrchr(filepath, '.')) != NULL)
	{
		if (strcmp(extension, ".mp3") == 0)
		{
			return true;
		}
	}
	return false;
}

/*
 * @brief Gets the first file of the file system.
 *
 * */
file_t filesystem_get_first_file(void)
{
    if (files_count == 0)
	{
		file_t file_null = {.file_index = -1, .file_path = ""};
		return file_null;
	}
	return file_array[0];
}

/**
 * @brief Go to next file/folder in a circular order.
 * @return Next file/folder name. Return NULL if not possible.
*/
file_t filesystem_get_next_file(file_t current_file)
{
    int next_file_index = current_file.file_index + 1;
	if (next_file_index == files_count)
	{
		next_file_index = 0;
	}
	return file_array[next_file_index];
}

/**
 * @brief Go to previous file/folder in a circular order.
 * @return Previous file/folder name. Return NULL if not possible.
*/
file_t filesystem_get_previous_file(file_t current_file)
{
	int prev_file_index = current_file.file_index - 1;
	if (current_file.file_index == 0)
	{
		prev_file_index = files_count - 1;
	}
	return file_array[prev_file_index];
}

/*
 * @brief Adds a file to the file system.
 * @param path: complete file's path.
 *
 * */
void filesystem_add_files(char *filepath)
{
    file_t *new_file = &file_array[files_count];
	strcpy(new_file->file_path, filepath);
	new_file->file_index = files_count;
	files_count++;
}


/*************************************************************************************
 * 		LOCAL FUNCTIONS DECLARATIONS
 ************************************************************************************/

void SD_callback(bool isInserted, void *userData)
{
	if(SD_connected == isInserted)
	{
		SD_error = true;
	}
	else if(isInserted)
	{
		SD_status = 1;
	}
	else
	{
		SD_status = 2;
	}

	SD_connected = isInserted;
}

void filesystem_map_files_handler(char * filepath)
{
	if (f_opendir(&directory, filepath))
	{
		//printf("Open directory failed.\r\n");
		return;
	}
	for (;;)
	{
		error = f_readdir(&directory, &fileInformation);
		if ((error != FR_OK) || (fileInformation.fname[0U] == 0U))
		{
			break;
		}
		if (fileInformation.fname[0] == '.')
		{
			continue;
		}
		if (fileInformation.fattrib & AM_DIR)
		{
			int i = strlen(filepath);
			char * fn = fileInformation.fname;
			*(filepath+i) = '/'; strcpy(filepath+i+1, fn);
			filesystem_map_files_handler(filepath);
			*(filepath+i) = 0;
		}
		else
		{
			int i = strlen(filepath);
			char * fn = fileInformation.fname;
			*(filepath+i) = '/'; strcpy(filepath+i+1, fn);

			if (filesystem_is_MP3file(filepath))
				filesystem_add_files(filepath);

			*(filepath+i) = 0;
		}
	}
	f_closedir(&directory);
}

status_t sdcardWaitCardInsert(void)
{
    BOARD_SD_Config(&g_sd, NULL, BOARD_SDMMC_SD_HOST_IRQ_PRIORITY, NULL);

    /* SD host init function */
    if (SD_HostInit(&g_sd) != kStatus_Success)
    {
        //PRINTF("\r\nSD host init fail\r\n");
        return kStatus_Fail;
    }

    /* wait card insert */
    if (SD_PollingCardInsert(&g_sd, kSD_Inserted) == kStatus_Success)
    {
        //PRINTF("\r\nCard inserted.\r\n");
        /* power off card */
        SD_SetCardPower(&g_sd, false);
        /* power on the card */
        SD_SetCardPower(&g_sd, true);
    }
    else
    {
        //PRINTF("\r\nCard detect fail.\r\n");
        return kStatus_Fail;
    }

    return kStatus_Success;
}

