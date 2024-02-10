/***************************************************************************//**
  @file		mp3decoder.c
  @brief
  @author
  @date
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILE
 ******************************************************************************/

#include "Mp3decoder.h"
#include "stdlib.h"
#include "../../lib/helix/pub/mp3dec.h"
#include "ff.h"
#include "gpio.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

typedef struct {
 HMP3Decoder 		mp3Decoder;
 FIL 				mp3FileHandler;
 MP3FrameInfo 		mp3FrameInfo;
 bool 				isFileOpen;
 uint16_t 			fileSize;

 uint8_t 			mp3FrameBuffer[INPUT_BUFFER_SIZE];
 int32_t    		mp3OutputBufferIndex;
 int32_t     		mp3InputBufferIndex;

 UINT               mp3BytesRead;
 UINT               mp3BytesLeft;
} MP3DecoderContext_t;

static MP3DecoderContext_t context;


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void convert_stereo_to_mono(int16_t* outputBuffer, int16_t* temporaryBuffer);
static void mp3_reset_context();
static uint16_t mp3_file_size();


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
    else if (f_open(&context.mp3FileHandler, _T(filepath), FA_READ) == FR_OK)
    {
    	context.isFileOpen = true;

        context.fileSize = mp3_file_size();

        int error = 1, offset;

        // Read until EOF or valid frame is found
        while (!f_eof(&context.mp3FileHandler) && error) {

            // Save file content in buffer
            if (f_read(&context.mp3FileHandler, context.mp3FrameBuffer, INPUT_BUFFER_SIZE, &context.mp3BytesRead) == FR_OK) 
            {

                offset = MP3FindSyncWord(context.mp3FrameBuffer, context.mp3BytesRead);

                // If valid frame found
                if (offset >= 0) {
                    error = MP3GetNextFrameInfo(context.mp3FrameBuffer, &context.mp3FrameInfo, context.mp3BytesRead + offset);

                    if (error) 
                    {
                        // Move file pointer next to new frame sync found
                        f_lseek(&context.mp3FileHandler, f_tell(&context.mp3FileHandler) - context.mp3BytesRead + offset + 1);
                    }
                }
            } 
            else 
            {
                break;
            }
        }
		// Check if audio is accepted
        if (!error && context.mp3FrameInfo.samprate == MP3_SAMPLE_RATE) 
        {
            if (f_lseek(&context.mp3FileHandler, f_tell(&context.mp3FileHandler) - context.mp3BytesRead + offset) == FR_OK) 
            {
                return true;
            }
        }

        f_close(&context.mp3FileHandler);
    	mp3_reset_context();
    }
    
    return false;

}

/**
 * @brief   Decodes current loaded song.
 * @param   outputBuffer to be filled with the decoded data.
 * @return  MP3FrameInfo struct with outputSamps bytes written in output buffer.
*/
uint16_t mp3_get_next_frame(int16_t* outputBuffer)
{
    if(!context.isFileOpen) {
        return 0;
    }

    int16_t auxBuffer[2 * OUTPUT_BUFFER_SIZE];
    int error, offset;
    uint8_t* decodeBuffer;

    bool needSync = false;

    while (!f_eof(&context.mp3FileHandler)) 
    {
        if (f_read(&context.mp3FileHandler, context.mp3FrameBuffer, INPUT_BUFFER_SIZE, &context.mp3BytesRead) == FR_OK) {
            context.mp3BytesLeft = context.mp3BytesRead;
            decodeBuffer = context.mp3FrameBuffer;

            if (needSync) {
                offset = MP3FindSyncWord(decodeBuffer, context.mp3BytesLeft);
                if (offset < 0) {
                    continue;
                }
                needSync = false;
                context.mp3BytesLeft -= offset;
                decodeBuffer += offset;
            }

            int checkpoint = context.mp3BytesLeft;
            error = MP3Decode(context.mp3Decoder, &decodeBuffer, &context.mp3BytesLeft, context.mp3FrameInfo.nChans == 1 ? outputBuffer : auxBuffer, 0U);

            switch (error) 
            {
                case ERR_MP3_NONE:

                    MP3GetLastFrameInfo(context.mp3Decoder, &context.mp3FrameInfo);
                    f_lseek(&context.mp3FileHandler, f_tell(&context.mp3FileHandler) - context.mp3BytesLeft);

                    if (context.mp3FrameInfo.nChans == 2) {
                        convert_stereo_to_mono(outputBuffer, auxBuffer);
                    }

                    return context.mp3FrameInfo.outputSamps / context.mp3FrameInfo.nChans;

                    break;
                case ERR_MP3_INDATA_UNDERFLOW:
                case ERR_MP3_MAINDATA_UNDERFLOW:
                    if (!f_eof(&context.mp3FileHandler)) 
                    {
                        f_lseek(&context.mp3FileHandler, f_tell(&context.mp3FileHandler) - checkpoint);
                    }
                    break;
                case ERR_MP3_INVALID_FRAMEHEADER:
                case ERR_MP3_INVALID_HUFFCODES:

                    needSync = true;

                    f_lseek(&context.mp3FileHandler, f_tell(&context.mp3FileHandler) - checkpoint + 1);

                    break;

                default:

                    needSync = true;

                    f_lseek(&context.mp3FileHandler, f_tell(&context.mp3FileHandler) - checkpoint + 1);

                    break;
            }

        } 
        else 
        {
            break;
        }
    }

    return 0;
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
	context.mp3OutputBufferIndex = 0;
	context.mp3InputBufferIndex = 0;
	context.isFileOpen = false;
	context.fileSize = 0;
}
