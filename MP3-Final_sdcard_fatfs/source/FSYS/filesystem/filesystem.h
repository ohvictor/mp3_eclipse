/***************************************************************************//**
  @file     fileSystem.h
  @brief    Manejo de archivos y directorios en la tarjeta SD
  @author   
  @date		
 ******************************************************************************/

#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdbool.h>
#include <stdint.h>
/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define BUFFER_SIZE (513U)
#define FILENAME_LENGTH 255

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct {
	uint8_t buffer_read[BUFFER_SIZE];
	unsigned int bytes_read;
} data_t;

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Init filesystem module
*/
bool filesystem_init(void);

/**
 * @brief Deinit filesystem module
*/
void filesystem_deinit(void);

/**
 * @brief Starts mapping all files in SD
*/
bool filesystem_map_files();

/*
 * @brief Gets the amount of files that exist in the file system.
 */
int filesystem_get_files_count(void);

/**
 * @brief open folder
 * @return Next file/folder name into folder. Return NULL if not possible.
*/
char* filesystem_open_folder();

/**
 * @brief close folder
 * @return Next file/folder name in the previous folder. Return NULL if not possible.
*/
char* filesystem_close_folder();

/**
 * @brief Open file and read data
 * @return File data
*/
data_t* filesystem_open_file();

/**
 * @brief Gives the path
 * @return Path of actual file
*/
char* filesystem_get_path();

/**
 * @brief Go to next file/folder in a circular order.
 * @return Next file/folder name. Return NULL if not possible.
*/
char* filesystem_show_next();

/**
 * @brief Go to previous file/folder in a circular order.
 * @return Previous file/folder name. Return NULL if not possible.
*/
char* filesystem_show_previous();


/*
 * @brief Detects if a file's path corresponds to a .mp3 file.
 * */
bool filesystem_is_MP3file(char *filepath);


/*******************************************************************************
 ******************************************************************************/

#endif // _FILESYSTEM_H_
