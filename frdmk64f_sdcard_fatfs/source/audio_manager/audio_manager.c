/***************************************************************************//**
  @file     audio_manager.c
  @brief    Audio Manager
  @author   Matias Bergerman, Pablo Gonzalez Leiro, Milagros Moutin, Pedro Carranza Velez
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdio.h>
#include "audio_manager.h"
#include "mp3decoder.h"
#include "DRV_Wavegen.h"
#include "EQ_FIR_Q15.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define AUDIO_BUFFER_SIZE   (576*2)
#define DEFAULT_SAMPLERATE  (24000)


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void audio_raise_decode_flag(void);
static void audio_decode_next_frame(void);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static bool audio_playing = false;
static bool decode_flag = false;
static int16_t audio_sample_buffer[AUDIO_BUFFER_SIZE];
static int16_t decoded_frame_buffer[AUDIO_BUFFER_SIZE*2];

static q15_t pre_eq_buffer[AUDIO_BUFFER_SIZE];

static uint8_t volume = MAX_VOLUME;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void init_audio_manager(void){
    MP3DecoderInit();
    wavegen_init(audio_sample_buffer, AUDIO_BUFFER_SIZE, DEFAULT_SAMPLERATE);
    wavegen_set_callback(audio_raise_decode_flag);
    eq_init(AUDIO_BUFFER_SIZE);
}

bool audio_load_song(char* song_path){
    audio_playing = false;
    return MP3LoadFile(song_path);
}

void audio_get_tag_data(mp3decoder_tag_data_t* ID3Data){
    MP3GetTagData(ID3Data);
}

void audio_print_tag_data(){
    mp3decoder_tag_data_t ID3Data;
    MP3GetTagData(&ID3Data);
    printf("\nSONG INFO\n");
    printf("TITLE: %s\n", ID3Data.title);
    printf("ARTIST: %s\n", ID3Data.artist);
    printf("ALBUM: %s\n", ID3Data.album);
    printf("TRACK NUM: %s\n", ID3Data.trackNum);
    printf("YEAR: %s\n", ID3Data.year);
}

void audio_start_playing(void){
    audio_playing = true;
    audio_decode_next_frame();
    wavegen_start();
}

bool audio_decode_loop(){
    if(decode_flag){
        decode_flag = false;
        audio_decode_next_frame();
    }
    return audio_playing;
}

void audio_stop_playing(void){
    audio_playing = false;
    wavegen_stop();
}

bool audio_is_playing(void){
    return audio_playing;
}

uint8_t increase_volume(void){
    if(volume < MAX_VOLUME){
        volume++;
    }
    return volume;
}

uint8_t decrease_volume(void){
    if(volume > MIN_VOLUME){
        volume--;
    }
    return volume;
}

bool audio_play_pause(void) {
    if(audio_playing){
        audio_stop_playing();
    }else{
        audio_start_playing();
    }
    return audio_playing;
}

void audio_set_eq(uint8_t eq){
	eq_set_eq(eq);
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void audio_raise_decode_flag(void){
    decode_flag = true;
}

static void audio_decode_next_frame(void){

    static uint16_t sample_count = 0;
	static mp3decoder_frame_data_t frame_data;
    static uint32_t sr = 0;
    static uint8_t ch = 0;
    static q15_t sample = 0;
    //static uint32_t sample = 0;

    static int16_t filter_out = 0;

    if(audio_playing){
        mp3decoder_result_t res = MP3GetDecodedFrame(decoded_frame_buffer, MP3_DECODED_BUFFER_SIZE, &sample_count);
        if (res == 0){
            MP3GetLastFrameData(&frame_data);
            sr = frame_data.sampleRate;

            ch = frame_data.channelCount;
            sample_count = sample_count / ch;



            int16_t* wavegen_buffer = wavegen_get_buffer_addr();
            //for (int32_t j = 0; j < sample_count; j++) {
            //    
            //    if(ch == 1){
            //        // Shift sample from int16 to uint16
            //        sample = (int32_t) decoded_frame_buffer[j] + 32768;
            //    }else{
            //    	sample = (int32_t)(decoded_frame_buffer[ch*j] + decoded_frame_buffer[ch*j + 1])/2  + 32768;
            //    }
            //    wavegen_buffer[j] = ( (sample * volume) / MAX_VOLUME ) >> 4;     // DAC uses 12 bits
            //}
            for (int32_t j = 0; j < sample_count; j++) {
            
                if(ch == 1){
                	// Apply filter to sample
                	//eq_filter_sample(decoded_frame_buffer[j], &filter_out);
                    // Shift sample from int16 to uint16
                    //sample = (int32_t) filter_out + 32768;
                	sample = decoded_frame_buffer[j];
                }else{
                	// Apply filter to sample
					//eq_filter_sample((decoded_frame_buffer[ch*j] + decoded_frame_buffer[ch*j + 1])/2 , &filter_out);
					// Shift sample from int16 to uint16
                	//sample = (int32_t) (filter_out)  + 32768;
                	sample = ((decoded_frame_buffer[ch*j] + decoded_frame_buffer[ch*j + 1])/2);
                }
                pre_eq_buffer[j] = (q15_t)(((sample * volume) / MAX_VOLUME ));
                //wavegen_buffer[j] = ( (sample * volume) / MAX_VOLUME ) >> 4;     // DAC uses 12 bits
            }
            wavegen_set_samplerate(2*sr);

            eq_filter_frame(pre_eq_buffer, wavegen_buffer, sample_count);

            wavegen_increase_buffer_index(sample_count);

        }else if (res == MP3DECODER_FILE_END){
            audio_playing = false;
            wavegen_stop();
        }
    }

}
