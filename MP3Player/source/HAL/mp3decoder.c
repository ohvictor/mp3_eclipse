/***************************************************************************//**
  @file     mp3decoder.c
  @brief    MP3 file decoder
  @author   Grupo 3
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "mp3decoder.h"
#include "fatfs/ff.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define MP3_SAMPLE_RATE				44100

#define MP3_FRAME_BUFFER_BYTES  	6913            //MP3 buffer size (in bytes)

#define DEFAULT_ID3_TAG	      		"Unknown"


/******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct {
	// Helix structures
	HMP3Decoder 					decoder;                           					//Helix MP3 decoder object
	MP3FrameInfo 					lastFrameInfo;                    					//Frame info
	// MP3 file data
	FIL 							mp3File;											//MP3 file object
	uint32_t 						fileSize;                           				//File size
	uint16_t 						lastFrameLength;                    				//Last frame length
	uint32_t 						bytesRemaining; 									//Encoded MP3 bytes remaining to be processed
	bool 							isFileOpen;
	// MP3-encoded buffer
	uint8_t 						frameBuffer[MP3_FRAME_BUFFER_BYTES]; 				// buffer for MP3-encoded frames
	uint32_t 						topIndex; 											//Current pos in frame buffer
	uint32_t 						bottomIndex;       									//Current pos in frame buffer
	// ID3 tag
	bool 							hasID3;
	mp3decoder_id3_tag_data_t 		ID3Data;                 //Data from ID3 tag

} mp3decoder_t;


/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/




/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static mp3decoder_t MP3Decoder;


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/**
 * @brief  Inits the MP3  decoder from Helix library and resets the context data
 * @return 0 if error.
*/
bool  MP3DecoderInit(void)
{
	resetContext();
	MP3Decoder.decoder = MP3InitDecoder();
	return mp3decoder.decoder == NULL;
}

/**
 * @brief  Selects and loads the MP3 file.
 * @param  filepath: a pointer to the MP3 file path.
 * @return returns true if it successfully opened the file, false otherwise.
*/
bool  MP3LoadSong(const char* filepath)
{
	bool result = false;

	if (MP3Decoder.isFileOpen == true) {
        // Close the file
        f_close(&MP3Decoder.decoder);
		resetContext();
	}
	if (openFile(filepath)) {
		MP3Decoder.isFileOpen = true;
		MP3Decoder.fileSize = getFileSize();
		MP3Decoder.bytesRemaining = MP3Decoder.fileSize;
		readID3Tag();
		fileToBuffer();
		result = true;
	}
	return result;
}

bool MP3LastFrame(MP3FrameInfo* data)
{
    bool result = false;
    if (MP3Decoder.bytesRemaining < MP3Decoder.fileSize)
    {
        //copyFrameInfo(data, &MP3Decoder.lastFrameInfo);
    	data = MP3Decoder.lastFrameInfo;
        result = true;
    }

    return result;
}

bool MP3NextFrame(MP3FrameInfo* data)
{
    bool result = false;
    MP3FrameInfo nextFrame;
    //int MP3FindSyncWord(unsigned char *buf, int nBytes);
    offset = MP3FindSyncWord(MP3Decoder.frameBuffer + MP3Decoder.topIndex, MP3Decoder.bottomIndex - MP3Decoder.topIndex);
    if (offset >= 0)
    {
    	//int MP3GetNextFrameInfo(HMP3Decoder hMP3Decoder, MP3FrameInfo *mp3FrameInfo, unsigned char *buf);
        if (MP3GetNextFrameInfo(MP3Decoder.decoder, &nextFrame, MP3Decoder.frameBuffer + MP3Decoder.topIndex + offset) == 0)
        {
        	data = nextFrame;
            result = true;
        }
    }
    return result;
}

decoder_state_t MP3DecodedFrame(short* outBuffer, uint16_t bufferSize, uint16_t* decodedSamples)
{
	decoder_state_t result = MP3DECODER_OK;

    if (!MP3Decoder.isFileOpen)
    {
    	result = MP3DECODER_NO_FILE;
    }

    //checks if there is still a part of the file to be decoded
    else if (MP3Decoder.bytesRemaining)
    {
        //checks if the conditions are fine
        if ((MP3Decoder.topIndex > 0) && ((MP3Decoder.bottomIndex - MP3Decoder.topIndex) > 0) && (MP3Decoder.bottomIndex - MP3Decoder.topIndex < MP3_FRAME_BUFFER_BYTES))
        {
            memmove(MP3Decoder.frameBuffer, MP3Decoder.frameBuffer + MP3Decoder.topIndex, MP3Decoder.bottomIndex - MP3Decoder.topIndex);
            MP3Decoder.bottomIndex = MP3Decoder.bottomIndex - MP3Decoder.topIndex;
            MP3Decoder.topIndex = 0;
        }

        // Read data from file
        fileToBuffer();

        // search the mp3 header
        int offset = MP3FindSyncWord(MP3Decoder.frameBuffer + MP3Decoder.topIndex, MP3Decoder.bottomIndex);

        if (offset >= 0)
        {
            // check errors in searching for sync words
        	MP3Decoder.topIndex += offset;
        	MP3Decoder.bytesRemaining -= offset;  //subtract garbage
        }

        // check samples in next frame
        MP3FrameInfo nextFrameInfo;

        // with this function we store the nextFrameInfo data in our struct
        int nextFrameResult = MP3GetNextFrameInfo(MP3Decoder.decoder, &nextFrameInfo, MP3Decoder.frameBuffer + MP3Decoder.topIndex);

        if (nextFrameResult == 0)
        {
            if (nextFrameInfo.outputSamps > bufferSize)
            {
                return DECODER_OVERFLOW;
            }
        }

        // we can now decode a frame
        uint8_t* pointer = MP3Decoder.frameBuffer + MP3Decoder.topIndex;
        int bytesLeft = MP3Decoder.bottomIndex - MP3Decoder.topIndex;

        // the next funtion autodecrements fileSize with bytes decoded and updated bytesLeft
        int decodeResult = MP3Decode(MP3Decoder.decoder, &pointer, &(bytesLeft), outBuffer, DECODER_NORMAL_MODE);

        // if everything worked okey
        if (decodeResult == ERR_MP3_NONE)
        {
            // we calculate the bytes decoded
            uint16_t decodedBytes = MP3Decoder.bottomIndex - MP3Decoder.topIndex - bytesLeft;
            MP3Decoder.lastFrameLength = decodedBytes;

            // update pointers and the numb of bytes that left to decode.
            MP3Decoder.topIndex += decodedBytes;
            MP3Decoder.bytesRemaining -= decodedBytes;

            // update last frame decoded data
            MP3GetLastFrameInfo(MP3Decoder.decoder, &(MP3Decoder.lastFrameLength));

            // update num of samples decoded
            *decodedSamples = MP3Decoder.lastFrameInfo.outputSamps;
            result = MP3DECODER_OK;
        }
        else if (decodeResult == ERR_MP3_INDATA_UNDERFLOW || decodeResult == ERR_MP3_MAINDATA_UNDERFLOW)
        {
            if (MP3Decoder.bytesRemaining == 0)
            {
                return MP3DECODER_END_OF_FILE;
            }
        }
        else
        {
            if (MP3Decoder.bytesRemaining <= MP3Decoder.lastFrameLength)
            {
                //if you are here it means that you ended the file
                return MP3DECODER_END_OF_FILE;
            }
            else
            {
            	return MP3DECODER_ERROR;
            }
        }
    }
    else
    {
        // if you are here it means that you ended the file
    	result = MP3DECODER_END_OF_FILE;
    }
    return result;
}

bool MP3GetTagData(mp3_id3_tag_data_t* data)
{
    bool result = false;
    if (MP3Decoder.hasID3Tag)
    {
        strcpy(data->album, MP3Decoder.ID3Data.album);
        strcpy(data->artist, MP3Decoder.ID3Data.artist);
        strcpy(data->title, MP3Decoder.ID3Data.title);
        strcpy(data->trackNum, MP3Decoder.ID3Data.trackNum);
        strcpy(data->year, MP3Decoder.ID3Data.year);
        result = true;
    }
    return result;
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void resetContext()
{
	  MP3Decoder.mp3File = NULL;
	  MP3Decoder.fileSize = 0;
	  MP3Decoder.isFileOpen = false;
	  MP3Decoder.bytesRemaining = 0;
	  MP3Decoder.bottomIndex= 0;
	  MP3Decoder.topIndex = 0;
	  MP3Decoder.hasID3 = false;
}

static bool openFile(const char *filepath) {

    FRESULT frame = f_open(&MP3Decoder.mp3File, _T(filepath), FA_READ);
    if (frame == FR_OK)
    {
        return true;
    }
    return false;
}

void fileToBuffer()
{
    // Fill buffer with the info of the mp3
    uint8_t* destination = MP3Decoder.framesBuffer + MP3Decoder.bottomIndex;
    if (MP3_FRAME_BUFFER_BYTES - MP3Decoder.bottomIndex > 0)
    {
        uint16_t bytesRead = readFile(destination, (MP3_FRAME_BUFFER_BYTES - MP3Decoder.bottomIndex));
    	// Update bottom_index pointer
        MP3Decoder.bottomIndex += bytesRead;
    }
}

uint32_t getFileSize(void) {
	uint32_t result = 0;
	if (MP3Decoder.isFileOpen) {
		result = f_size(MP3Decoder.mp3File);
	}
	return result;
}

//REVISAR Y COMPARAR CON LA QUE LEE TOOOOODA LA FUNCION
uint16_t readFile(void *buffer, uint16_t count) {
	UINT result = 0, read = 0;
    FRESULT frame;
	if (MP3Decoder.isFileOpen) {
		if (f_read(&MP3Decoder.mp3File, ((uint8_t*) buffer), count, &read) == FR_OK) {
			result = read;
		}
	}
	return result;
}

void copyFrameInfo(MP3FrameInfo* data, MP3FrameInfo* currentData)
{
	data->bitrate = currentData->bitrate;
	data->bitsPerSample = currentData->bitsPerSample;
	data->nChans = currentData->nChans;
	data->samprate = currentData->samprate;
	data->outputSamps = currentData->outputSamps;
}

void readID3Tag(void)
{

    if (has_ID3_tag(MP3Decoder.mp3File))
    {
    	MP3Decoder.hasID3Tag = true;

        if (!read_ID3_info(TITLE_ID3, MP3Decoder.ID3Data.title, ID3_MAX_FIELD_SIZE, MP3Decoder.mp3File))
            strcpy(MP3Decoder.ID3Data.title, DEFAULT_ID3_TAG);

        if (!read_ID3_info(ALBUM_ID3, MP3Decoder.ID3Data.album, ID3_MAX_FIELD_SIZE, MP3Decoder.mp3File))
            strcpy(MP3Decoder.ID3Data.album, DEFAULT_ID3_TAG);

        if (!read_ID3_info(ARTIST_ID3, MP3Decoder.ID3Data.artist, ID3_MAX_FIELD_SIZE, MP3Decoder.mp3File))
            strcpy(MP3Decoder.ID3Data.artist, DEFAULT_ID3_TAG);

        if (!read_ID3_info(YEAR_ID3, MP3Decoder.ID3Data.year, ID3_MAX_FIELD_SIZE, MP3Decoder.mp3File))
            strcpy(MP3Decoder.ID3Data.year, DEFAULT_ID3_TAG);

        if (!read_ID3_info(TRACK_NUM_ID3, MP3Decoder.ID3Data.trackNum, ID3_MAX_FIELD_SIZE, MP3Decoder.mp3File))
            strcpy(MP3Decoder.ID3Data.trackNum, DEFAULT_ID3_TAG);

        unsigned int tagSize = get_ID3_size(dec.mp3File);

        fileSeek(tagSize);
        MP3Decoder.bytesRemaining -= tagSize;

    }
    else
    {
    	f_rewind(MP3Decoder.mp3File);
    }
}

/******************************************************************************/
