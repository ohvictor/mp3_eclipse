/***************************************************************************//**
  @file     DRV_FILEEXPLORER.c
  @brief    Functions to explore the filesystem.
  @author   Matias Bergerman, Pablo Gonzalez Leiro, Milagros Moutin, Pedro Carranza Velez
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "DRV_FILEEXPLORER.h"
#include "fsl_debug_console.h"
#include "string.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define FILESINFO_SIZE 	100
#define DIRSINFO_SIZE	10

#define DATE2YEAR(fdate)	(((fdate)>>9 & 0x7f) + 1980)
#define	DATE2MONTH(fdate)	(((fdate)>>5) & 0x0f)
#define DATE2DAY(fdate)		(((fdate)>>0) & 0x1f)

#define TIME2HOUR(ftime)	((ftime)>>11 & 0x1f)
#define TIME2MINUTE(ftime)	((ftime)>>5 & 0x3f)
#define TIME2SECOND(ftime)	(((ftime)>>0 * 2) & 0x1f)



/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/


/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

// +ej: unsigned int anio_actual;+




/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

// +ej: static const int temperaturas_medias[4] = {23, 26, 24, 29};+


/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static FILINFO files[FILESINFO_SIZE];
static FILINFO *fileptr;

static FRESULT res;
static FILINFO fno;
static DIR dir;
static int nfile, ndir;

static METADATA_t metadata;

char currPath[255] = "/";

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void convertFilinfoToMetadata(void);
static void getCurrentMetadataAsString(char* buf);
static void remove_last_folder(char* path);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

status_t fileExplorerInit(const char* path) {
	if( path == NULL ){
		path = "..";
	}
	strcpy(currPath, path);
	fileptr = files;
	#ifdef DEBUG_FILEEXPLORER
	PRINTF("%s\n", path);
	#endif
	res = f_opendir(&dir, path);
	if( res != FR_OK ) {
		PRINTF("FILEEXPLORER INIT ERROR: f_opendir returned %d", res);
	}
	if (res == FR_OK) {
		nfile = ndir = 0;
		for (;;) {
			res = f_readdir(&dir, &fno);                   /* Read a directory item */
			if (res != FR_OK || fno.fname[0] == 0) break;  /* Error or end of dir */

			if ( !(fno.fattrib & AM_SYS) ) {
				files[nfile++] = fno;
			}

			#ifdef DEBUG_FILEEXPLORER
			if (fno.fattrib & AM_DIR) {            /* Directory */
				PRINTF("   <DIR>   %s\n", fno.fname);
				ndir++;
			}
			else {  /* File */
				PRINTF("%10u %s\n", fno.fsize, fno.fname);
			}
			#endif
		}
		f_closedir(&dir);

		#ifdef DEBUG_FILEEXPLORER
		PRINTF("%d dirs, %d files.\n", ndir, nfile);
		#endif

	} else {
		PRINTF("Failed to open \"%s\". (%u)\n", path, res);
	}

	return res;
}


// Explore files in folder
FILINFO* getCurrFile(void) {
	return fileptr;
}

FILINFO* getNextFile(void) {
	if (fileptr - files == nfile - 1) {
		fileptr = files;
	}
	else {
		fileptr++;
	}
	return fileptr;
}

FILINFO* getPrevFile(void) {
	if (fileptr - files == 0) {
		fileptr = files + nfile - 1;
	}
	else {
		fileptr--;
	}
	return fileptr;
}

// Access folders in current folder

status_t accessFolder(void) {
	if(fileptr->fattrib == AM_DIR) {
		strcat(currPath, "/");
		strcat(currPath, fileptr->fname);
		res = fileExplorerInit(currPath);
	} else {
		res = kStatus_Fail;
	}
	return res;
}

status_t goBack(void) {
	remove_last_folder(currPath);
	res = fileExplorerInit(currPath);
	return res;
}


// Getters

METADATA_t getMetadata(void) {
	convertFilinfoToMetadata();
	return metadata;
}

void getPath(char *buf) {
	strcpy(buf, currPath);
}

// !! testFileExplorer hardcoded for my SD files
void testFileExplorer(void){
	PRINTF("INIT FILE EXPLORER\n");
	char strBuffer[300];
	char pathBuffer[260];

    fileExplorerInit(NULL);

    PRINTF("-----------------------------------------------------------\n");
    PRINTF("DATE       TIME     FILENAME   \n");
    PRINTF("-----------------------------------------------------------\n");

    getPath(pathBuffer);
    PRINTF("\t\t |_>Scroll through the main folder: %s\n", pathBuffer);
    for(int i=0; i<8; i++) {
    	getNextFile();
    	getCurrentMetadataAsString(strBuffer);
    	PRINTF("%s\n", strBuffer);
    }

    accessFolder();
    getPath(pathBuffer);
    PRINTF("\t\t |_>Enter folder and scroll %s\n", pathBuffer);
    for(int i=0; i<2; i++) {
    	getNextFile();
		getCurrentMetadataAsString(strBuffer);
		PRINTF("%s\n", strBuffer);
	}

    goBack();
    getPath(pathBuffer);
    PRINTF("\t\t |_>Go back to previous folder %s\n", pathBuffer);
    for(int i=0; i<5; i++) {
		getNextFile();
		getCurrentMetadataAsString(strBuffer);
		PRINTF("%s\n", strBuffer);
	}

    accessFolder();
    getPath(pathBuffer);
    PRINTF("\t\t |_>Enter folder and scroll %s\n", pathBuffer);
    for(int i=0; i<4; i++) {
		getPrevFile();
		getCurrentMetadataAsString(strBuffer);
		PRINTF("%s\n", strBuffer);
	}

	accessFolder();
	getPath(pathBuffer);
    PRINTF("\t\t |_>Enter folder and scroll %s\n", pathBuffer);
	for(int i=0; i<4; i++) {
		getPrevFile();
		getCurrentMetadataAsString(strBuffer);
		PRINTF("%s\n", strBuffer);
	}

	goBack();
	getPath(pathBuffer);
	PRINTF("\t\t |_>Go back to previous folder %s\n", pathBuffer);
	for(int i=0; i<2; i++) {
		getNextFile();
		getCurrentMetadataAsString(strBuffer);
		PRINTF("%s\n", strBuffer);
	}

	goBack();
	getPath(pathBuffer);
	PRINTF("\t\t |_>Go back to previous folder %s\n", pathBuffer);
	for(int i=0; i<5; i++) {
		getNextFile();
		getCurrentMetadataAsString(strBuffer);
		PRINTF("%s\n", strBuffer);
	}
    PRINTF("-----------------------------------------------------------\n");
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void convertFilinfoToMetadata(void) {
	metadata.year 	= DATE2YEAR(fileptr->fdate);
	metadata.month 	= DATE2MONTH(fileptr->fdate);
	metadata.day 	= DATE2DAY(fileptr->fdate);
	metadata.hour	= TIME2HOUR(fileptr->ftime);
	metadata.minute	= TIME2MINUTE(fileptr->ftime);
	metadata.second = TIME2SECOND(fileptr->ftime);
	metadata.amdir	= fileptr->fattrib & AM_DIR;
	strcpy(metadata.name, fileptr->fname);
	strcpy(metadata.altname, fileptr->altname);
}

static void getCurrentMetadataAsString(char* buf) {
	convertFilinfoToMetadata();
	sprintf(buf, "%02d/%02d/%04d %02d:%02d:%02d %s", metadata.day,
			metadata.month, metadata.year, metadata.hour, metadata.minute,
			metadata.second, metadata.name );
}

static void remove_last_folder(char* path) {
    char *last_slash = strrchr(path, '/');
    if (last_slash != NULL) {
        *last_slash = '\0';  // Truncate the string at the last slash
    }
}

