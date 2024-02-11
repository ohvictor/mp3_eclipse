/***************************************************************************//**
  @file		mp3decoder.c
  @brief
  @author
  @date
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILE
 ******************************************************************************/

#include "mp3decoder.h"
#include "stdlib.h"
#include "mp3dec.h"
#include "read_id3.h"
#include "ff.h"
#include "gpio.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define TAG_SIZE 128
#define FRAME_BYTES 2560U

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

typedef struct {
 HMP3Decoder 		    mp3Decoder;
 FIL 				    mp3FileHandler;
 MP3FrameInfo 		    mp3FrameInfo;
 bool 				    isFileOpen;
 uint16_t 			    fileSize;

 mp3_decoder_states_t   decoderState;

 BYTE 				    mp3FrameBuffer[INPUT_BUFFER_SIZE];
 
 UINT                   mp3BytesRead;
 UINT                   mp3BytesLeft;

 uint32_t               mp3TopIndex;                                    // current position in frame buffer
 uint32_t               mp3BottomIndex;                                 // current position at info end in frame buffer
 uint32_t               mp3LastFrameLength;                                
} MP3DecoderContext_t;

static MP3DecoderContext_t context;

ID3Tag_t id3Tag;

bool id3TagRead = false;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void     convert_stereo_to_mono(int16_t* outputBuffer, int16_t* temporaryBuffer);
static void     mp3_decoder_to_buffer(void);
static size_t   mp3_decoder_read_file(void* buffer, size_t count);
static void     mp3_reset_context();
static uint16_t mp3_file_size();
static void     mp3_read_id3tag(void);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/**
 * @brief  Initialize the MP3 decoder using the Helix MP3 decoder library and resets the context data.
 * @return false if error.
*/
bool  mp3_decoder_init(void)
{
	context.mp3Decoder = MP3InitDecoder();
	mp3_reset_context();
    return context.mp3Decoder == NULL;
}

/**
 * @brief  Selects and loads the MP3 file.
 * @param  filepath: a pointer to the MP3 file path.
 * @return returns true if it successfully opened the file, false otherwise.
*/
bool  mp3_load_song(const char* filepath)
{
    // Error: no file selected
    if (context.isFileOpen)
    {
        f_close(&context.mp3FileHandler);
    	mp3_reset_context();
    }
    else if (f_open(&context.mp3FileHandler, filepath, FA_READ) == FR_OK)
    {
    	context.isFileOpen = true;

        context.fileSize = mp3_file_size();
        
        context.mp3BytesRead = 0; 

        context.mp3BytesLeft = context.fileSize;

        // Read ID3 tag if it exits
        mp3_read_id3tag();

        // Flush file to buffer
        mp3_decoder_to_buffer();

        return true;
    }
    
    return false;

}

/**
 * @brief Decodes current loaded song
 * @param @outBuffer      to be filled with the decoded data.
 * @return Decoder state
*/
mp3_decoder_states_t mp3_decoded_frame(short* outputBuffer, uint16_t bufferSize, uint16_t* decodedSamples)
{
    context.decoderState = MP3DECODER_WORKED;

    if (!context.isFileOpen)
    {
        context.decoderState = MP3DECODER_NO_FILE;
    }
    // checks if there is still a part of the file to be decoded
    else if (context.mp3BytesLeft)
    {
        if ((context.mp3TopIndex > 0) && ((context.mp3BottomIndex - context.mp3TopIndex) > 0) && (context.mp3BottomIndex - context.mp3TopIndex < FRAME_BYTES))
        {
            memmove(context.mp3FrameBuffer, context.mp3FrameBuffer + context.mp3TopIndex, context.mp3BottomIndex - context.mp3TopIndex);
            context.mp3BottomIndex = context.mp3BottomIndex - context.mp3TopIndex;
            context.mp3TopIndex = 0;
        }

        mp3_decoder_to_buffer(); // Read from file

        // Search sync word in header
        int offset = MP3FindSyncWord(context.mp3FrameBuffer + context.mp3TopIndex, context.mp3BottomIndex);
        if (offset >= 0)
        {
            context.mp3TopIndex += offset;
            context.mp3BytesLeft -= offset; 
        }        

        // Store next frame info
        MP3FrameInfo nextFrameInfo;
        int error = MP3GetNextFrameInfo(context.mp3Decoder, &nextFrameInfo, context.mp3FrameBuffer + context.mp3TopIndex);

        if (!error)
        {
            if (nextFrameInfo.outputSamps > bufferSize)
            {
                return MP3DECODER_OVERFLOW;
            }
        }

        // Decode frame
        uint8_t* dPtr = context.mp3FrameBuffer + context.mp3TopIndex;
        int bytesLeftAux = context.mp3BottomIndex - context.mp3TopIndex;

        int result = MP3Decode(context.mp3Decoder, &dPtr, &(bytesLeftAux), outputBuffer, 0);

        // If the decoder worked
        switch(result)
        {
            case ERR_MP3_NONE:
                // we calculate the bytes decoded
                uint16_t decodedBytes = context.mp3BottomIndex - context.mp3TopIndex - bytesLeftAux;
                context.mp3LastFrameLength = decodedBytes;

                // update pointers and the numb of bytes that left to decode.
                context.mp3TopIndex += decodedBytes;
                context.mp3BytesLeft -= decodedBytes;

                // update last frame decoded data
                MP3GetLastFrameInfo(context.mp3Decoder, &(context.mp3FrameInfo));

                // update num of samples decoded
                *decodedSamples = context.mp3FrameInfo.outputSamps;

                context.decoderState = MP3DECODER_WORKED;

                break;
            case ERR_MP3_INDATA_UNDERFLOW:
            case ERR_MP3_MAINDATA_UNDERFLOW:
                if (context.mp3BytesLeft == 0)
                {
                    return MP3DECODER_END_OF_FILE;
                }
                break;
            default:
                if (context.mp3BytesLeft <= context.mp3LastFrameLength)
                {
                    // File ended
                    context.decoderState = MP3DECODER_END_OF_FILE;
                    return context.decoderState;
                }
                else
                {
                    //printf("Error");
                }
                break;
        }
    }
    else
    {
        // if you are here it means that you ended the file
        context.decoderState = MP3DECODER_END_OF_FILE;
    }
    return context.decoderState;
}

bool mp3_get_last_frame_channel_count(uint8_t* channelCount)
{
    bool result = false;
    if (context.mp3BytesLeft < context.fileSize)
    {
        *channelCount = context.mp3FrameInfo.nChans;
        result = true;
    }
    return result;
}


/**
 * @brief Get MP3 file tag data.
 */
ID3Tag_t* mp3_get_tag_data()
{
	return &id3Tag;
}

/**
 * @brief Check if file has ID3 tag.
 */
bool mp3_has_tag(void)
{
    return id3TagRead;
}

/**
 * @brief: Get the current file title.
 * @return: true if hasID3 returns true.
 */
char* mp3_get_tag_title(void)
{
    return id3Tag.title;
}


/**
 * @brief: Get the current file's album.
 * @param album_: is a pointer to the char pointer (array of chars) that forms the album word.
 * @return: true if hasID3 returns true.
 */
char* mp3_get_tag_album(void)
{
    return id3Tag.album;
}


/**
 * @brief: Get the current file's artist.
 * @param artist_: is a pointer to the char pointer (array of chars) that forms the artist word.
 * @return: true if hasID3 returns true.
 */
char* mp3_get_tag_artist(void)
{
    return id3Tag.artist;
}


/**
 * @brief: Get the current file's year.
 * @param year_: is a pointer to the char pointer (array of chars) that forms the year word.
 * @return: true if hasID3 returns true.
 */
char* mp3_get_tag_year(void)
{
    return id3Tag.year;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


void mp3_decoder_to_buffer(void)
{
	uint16_t bytesRead;
    uint8_t* auxBuffer = &context.mp3FrameBuffer[context.mp3BottomIndex];
    bytesRead = mp3_decoder_read_file(auxBuffer, (FRAME_BYTES - context.mp3BottomIndex));
    context.mp3BottomIndex += bytesRead; // Update bottom pointer with the bytes read
}

size_t mp3_decoder_read_file(void* buffer, size_t count)
{
    size_t read = 0;
    size_t auxCount, i, j = count % 512;
    char * auxBuffer = buffer;
    FRESULT fr;

    for(i = 0; i < count/512; i++)
    {
    	fr = f_read(&context.mp3FileHandler, auxBuffer, 512, &auxCount);
    	if (fr != FR_OK)
    	     break;
    	auxBuffer += 512;
    	read += auxCount;
    }

    if(fr == FR_OK && j != 0)
    {
    	fr = f_read(&context.mp3FileHandler, auxBuffer, j, &auxCount);
    	if (fr == FR_OK)
    		read += auxCount;
    }

    return fr == FR_OK ? read : 0;
}


static void convert_stereo_to_mono(int16_t* outputBuffer, int16_t* auxBuffer) 
{
    for (int i = 0; i < OUTPUT_BUFFER_SIZE; i++) 
    {
        outputBuffer[i] = (int16_t)(((int32_t)auxBuffer[2 * i] + (int32_t)auxBuffer[2 * i + 1]) / 2);
    }
}

static uint16_t mp3_file_size()
{
	uint16_t filesize = 0;
    if (context.isFileOpen)
    {
        filesize = f_size(&context.mp3FileHandler);
    }
    return filesize;
}

static void mp3_reset_context()
{
	context.mp3BottomIndex = 0;
	context.mp3TopIndex = 0;
	context.isFileOpen = false;
	context.fileSize = 0;
    context.mp3BytesRead = 0;
    id3TagRead =    false;
    context.decoderState = MP3DECODER_NO_FILE;

    context.mp3TopIndex = 0;
    context.mp3BottomIndex = 0;
    context.mp3LastFrameLength = 0;
}


static void mp3_read_id3tag(void)
{
    if(!context.isFileOpen)
    {
        return;
    } 
    // Checks if the file has an ID3 Tag. (ID3 library)
    if (has_ID3_tag(&context.mp3FileHandler))
    {
        id3TagRead = true;

        if (!read_ID3_info(TITLE_ID3, id3Tag.title, ID3_MAX_NUM_CHARS, &context.mp3FileHandler))
            strcpy(id3Tag.title, DEFAULT_ID3TAG);

        if (!read_ID3_info(ALBUM_ID3, id3Tag.album, ID3_MAX_NUM_CHARS, &context.mp3FileHandler))
            strcpy(id3Tag.album, DEFAULT_ID3TAG);

        if (!read_ID3_info(ARTIST_ID3, id3Tag.artist, ID3_MAX_NUM_CHARS, &context.mp3FileHandler))
            strcpy(id3Tag.artist, DEFAULT_ID3TAG);

        if (!read_ID3_info(YEAR_ID3, id3Tag.year, 10, &context.mp3FileHandler))
            strcpy(id3Tag.year, DEFAULT_ID3TAG);

        unsigned int tagSize = get_ID3_size(&context.mp3FileHandler);

        // Position the internal file pointer where the data starts
        f_lseek(&context.mp3FileHandler, tagSize);
        context.mp3BytesLeft -= tagSize;
    }
    else
    {
    	// Position the internal file pointer where the data starts (the beginning)
        f_rewind(&context.mp3FileHandler);
    }
}
