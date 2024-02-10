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
#define FILENAME_LENGTH 255

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct
{
  char  file_path[FILENAME_LENGTH];
  int   file_index;
} file_t;

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
 * @brief
 * @param
*/
void filesystem_map_files(void);

/*
 * @brief Prints all the files of the file system.
 * @param completePath.
 * 				true: prints the complete path of the file.
 * 				false: prints only the file's name.
 *
 * */
void filesystem_print_files(void);

/*
 * @brief Reset the file system and return a new first file.
 *
 * */
file_t filesystem_reset_files(void);

/*
 * @brief Gets the amount of files that exist in the file system.
 */
int filesystem_get_files_count(void);

/*
 * @brief Gets the first file of the file system.
 *
 * */
file_t filesystem_get_first_file(void);

/**
 * @brief Go to next file/folder in a circular order.
 * @return Next file/folder name. Return NULL if not possible.
*/
file_t filesystem_get_next_file(file_t current_file);

/**
 * @brief Go to previous file/folder in a circular order.
 * @return Previous file/folder name. Return NULL if not possible.
*/
file_t filesystem_get_previous_file(file_t current_file);

/*
 * @brief Adds a file to the file system.
 * @param path: complete file's path.
 *
 * */
void filesystem_add_files(char *path);

/*
 * @brief
 * @param
 * @return
 * */
char* filesystem_get_file_name(file_t file);

/*
 * @brief Detects if a file's path corresponds to a .mp3 file.
 * */
bool filesystem_is_MP3file(char *filepath);


/*******************************************************************************
 ******************************************************************************/

#endif // _FILESYSTEM_H_


