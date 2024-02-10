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
#include "clock_config.h"
#include "board.h"
#include "sdmmc_config.h"

#include "filesystem.h"
#include "node.h"
/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define SD_ARRAY_SIZE 100

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/
const TCHAR driver_number_buffer[4U] = {SDDISK + '0', ':', '/', '\0'};
static FIL file_object;
FRESULT error;
static data_t file;
static Node *node;

element_info_t elements[SD_ARRAY_SIZE];
static bool SD_hostInitDone = false;
static bool SD_error = false;
static FATFS g_fileSystem;

int files_count = 0;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static bool node_append(Node *node, element_info_t *element);

static bool map_files(char *dir, Node *node);

void SD_callback(bool isInserted, void *userData);

void filesystem_map_files_handler(char * filepath);

status_t sdcardWaitCardInsert(void);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
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
	const TCHAR driver_number_buffer[4U] = {SDDISK + '0', ':', '/', '\0'};
	f_mount(NULL, driver_number_buffer, 1U);
	g_sd.isHostReady = false;
    SD_hostInitDone = false;
}


bool filesystem_map_files(void)
{
    files_count = 0;
    if (node)
    {
        nodeDestroy(node);
    }

    strcpy(elements[0].name, "filelsys");
    strcpy(elements[0].path, "");

    node = nodeNew(elements[0].name, elements[0].path);
    bool state = map_files(elements[0].path, node);
    return state;
}

/*
 * @brief Gets the amount of files that exist in the file system.
 */
int filesystem_get_files_count(void)
{
    return files_count;
}

char* filesystem_open_folder(void)
{
    if (!node || !node->children)
    {
        return NULL;
    }
    if (node->children)
    {
        node = node->children;
        return node->name;
    }
    return NULL;
}

char* filesystem_close_folder(void) {
    if (!node || !node->parent)
    {
        return NULL;
    }
    if (node->parent)
    {
        node = node->parent;
        if (!node->parent)
        {
            return NULL;
        }
        return node->name;
    }
    return NULL;
}

data_t* filesystem_open_file(void)
{
    if (!node)
    {
        return NULL;
    }
    error = f_open(&file_object, node->path, FA_READ);
    if (!error)
    {
        error = f_read(&file_object, file.buffer_read, sizeof(file.buffer_read), &file.bytes_read);
        if (!error)
        {
            f_close(&file_object);
            return &file;
        }
    }
    return NULL;
}

char* filesystem_get_path(void)
{
    if (!node)
    {
        return NULL;
    }
    else
    {
        return node->path;
    }
}

char* filesystem_show_next(void)
{
    if (!strcmp(node->path, "") && node->children)
    {
        node = node->children;
        return node->name;
    }
    if (!node || !node->next)
    {
        if (node)
        {
            if (node->prev)
            {
                while (node->prev)
                {
                    node = node->prev;
                }
            }
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        node = node->next;
    }
    return node->name;
}

char* filesystem_show_previous(void) {
    if (!strcmp(node->path, "") && node->children) {
        node = node->children;
        return node->name;
    }
    if (!node || !node->prev) {
        if (node) {
            if (node->next) {
                while (node->next) {
                    node = node->next;
                }
            }
        } else {
            return NULL;
        }
    } else {
        node = node->prev;
    }
    return node->name;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static bool map_files(char *dir, Node *node)
{
    DIR dir_aux;
    FILINFO file_information_aux;
    static int i = 1;
    char tempStr[12 + 1];

    if (f_opendir(&dir_aux, dir))
    {
        //printf("Open directory failed.\r\n");
        return false;
    }
    Node *node_aux = node->children;

    while (1) {
        error = f_readdir(&dir_aux, &file_information_aux);

        if ((error != FR_OK) || (file_information_aux.fname[0U] == 0U)) {
            break;
        }
        if (file_information_aux.fname[0] == '.') {
            continue;
        }

        if (file_information_aux.fattrib & AM_DIR) {
            if (strcmp(file_information_aux.fname, "SYSTEM~1"))
            {
                tempStr[0] = '-';
                tempStr[1] = '-';
                tempStr[2] = '>';
                tempStr[3] = '\0';

                strcat(tempStr, file_information_aux.fname);

                memcpy(elements[i].name, tempStr, strlen(tempStr));

                elements[i].name[strlen(tempStr)] = '\0';

                memcpy(elements[i].path, dir, strlen(dir));

                elements[i].path[strlen(dir)] = '/';

                elements[i].path[strlen(dir) + 1] = '\0';

                strcat(elements[i].path, file_information_aux.fname);

                if (!node_append(node, elements + i))
                {
                    return false;
                }
                if (!node_aux)
                {
                    node_aux = node->children;
                }
                else
                {
                    node_aux = node_aux->next;
                }

                i++;

                map_files(elements[i - 1].path, node_aux);
            }
        }
        else
        {
            memcpy(elements[i].name, file_information_aux.fname, strlen(file_information_aux.fname));

            memcpy(elements[i].path, dir, strlen(dir));

            elements[i].path[strlen(dir)] = '/';

            elements[i].path[strlen(dir) + 1] = '\0';

            strcat(elements[i].path, file_information_aux.fname);

            if (!node_append(node, elements + i))
            {
                return false;
            }

            i++;
            files_count += 1;

            if (!node_aux)
            {
                node_aux = node->children;
            }
            else
            {
                node_aux = node_aux->next;
            }
        }
    }

    return true;
}

static bool node_append(Node *node, element_info_t *element)
{
    if (!nodeAppend(node, nodeNew(element->name, element->path)))
    {
        node = NULL;
    }
    if (!node)
    {
        return false;
    }
    return true;
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

