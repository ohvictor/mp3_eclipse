/***************************************************************************//**
  @file     DRV_AudioControl.c
  @brief    Control mp3 with encoder and push buttons.
  @author   Matias Bergerman, Pablo Gonzalez Leiro, Milagros Moutin, Pedro Carranza Velez
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "DRV_AudioControl.h"
#include "DRV_FILEEXPLORER.h"
#include "DRV_Encoder.h"
#include "DRV_Buttons.h"
#include "audio_manager.h"
//#include "lcd_display/lcd_display_2.h"
#include "lcd_display/lcd_display.h"
#include "EQ_FIR.h"

#include "DRV_Timers.h"
#include "fsl_debug_console.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum {
	MODE_EXPLORE = 0,
	MODE_PLAY,
	MODE_COUNT
} state_t;

typedef enum {
	EVENT_NONE = 0,
	EVENT_ENCODER_LEFT,
	EVENT_ENCODER_RIGHT,
	EVENT_BUTTON1_SHORT,
	EVENT_BUTTON1_LONG,
	EVENT_BUTTON2_SHORT,
	EVENT_BUTTON2_LONG,
	EVENT_BUTTONENC_SHORT,
	EVENT_BUTTONENC_LONG,
	EVENT_COUNT
} event_t;

typedef void (*eventHandler_t)(void);


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static event_t getEvent(void);

//event handlers
static void eventHandlerNoneModeExplore(void);
static void eventHandlerNoneModePlay(void);
static void eventHandlerEncoderLeftModeExplore(void);
static void eventHandlerEncoderLeftModePlay(void);
static void eventHandlerEncoderRightModeExplore(void);
static void eventHandlerEncoderRightModePlay(void);
static void eventHandlerButton1ShortModeExplore(void);
static void eventHandlerButton1ShortModePlay(void);
static void eventHandlerButton1LongModeExplore(void);
static void eventHandlerButton1LongModePlay(void);
static void eventHandlerButton2ShortModeExplore(void);
static void eventHandlerButton2ShortModePlay(void);
static void eventHandlerButton2LongModeExplore(void);
static void eventHandlerButton2LongModePlay(void);
static void eventHandlerButtonEncShortModeExplore(void);
static void eventHandlerButtonEncShortModePlay(void);
static void eventHandlerButtonEncLongModeExplore(void);
static void eventHandlerButtonEncLongModePlay(void);

static void reprintName(void);


/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static const eventHandler_t eventHandlerTable[EVENT_COUNT][MODE_COUNT] = {
	[EVENT_NONE] = {
		[MODE_EXPLORE] = eventHandlerNoneModeExplore,
		[MODE_PLAY] = eventHandlerNoneModePlay
	},
	[EVENT_ENCODER_LEFT] = {
		[MODE_EXPLORE] = eventHandlerEncoderLeftModeExplore,
		[MODE_PLAY] = eventHandlerEncoderLeftModePlay
	},
	[EVENT_ENCODER_RIGHT] = {
		[MODE_EXPLORE] = eventHandlerEncoderRightModeExplore,
		[MODE_PLAY] = eventHandlerEncoderRightModePlay
	},
	[EVENT_BUTTON1_SHORT] = {
		[MODE_EXPLORE] = eventHandlerButton1ShortModeExplore,
		[MODE_PLAY] = eventHandlerButton1ShortModePlay
	},
	[EVENT_BUTTON1_LONG] = {
		[MODE_EXPLORE] = eventHandlerButton1LongModeExplore,
		[MODE_PLAY] = eventHandlerButton1LongModePlay
	},
	[EVENT_BUTTON2_SHORT] = {
		[MODE_EXPLORE] = eventHandlerButton2ShortModeExplore,
		[MODE_PLAY] = eventHandlerButton2ShortModePlay
	},
	[EVENT_BUTTON2_LONG] = {
		[MODE_EXPLORE] = eventHandlerButton2LongModeExplore,
		[MODE_PLAY] = eventHandlerButton2LongModePlay
	},
	[EVENT_BUTTONENC_SHORT] = {
		[MODE_EXPLORE] = eventHandlerButtonEncShortModeExplore,
		[MODE_PLAY] = eventHandlerButtonEncShortModePlay
	},
	[EVENT_BUTTONENC_LONG] = {
		[MODE_EXPLORE] = eventHandlerButtonEncLongModeExplore,
		[MODE_PLAY] = eventHandlerButtonEncLongModePlay
	}
};


/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static tim_id_t reprint_id;

static state_t state = MODE_EXPLORE;

static METADATA_t metadata;

static uint8_t volume;

static uint8_t eq;

static char path[260];
#ifdef AUDIOCONTROL_DEBUG
#endif

static char lcd_buffer[32];
static bool updated = false;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void audioControllerInit(void) {
	initTimers();
	initEncoder();
	buttonsInit();
	lcd_display_init();
	//HD44780LcdInit();
	//while(!HD44780LcdInitReady()){};

	reprint_id = timerGetId();

}

void loopAudioControl(void) {
	int eventHandler = getEvent();
	eventHandlerTable[eventHandler][state]();
	if (!updated){
		//audio_play_pause();
		//HD44780ClearDisplay();
		lcd_display_clear();
		//snprintf(lcd_buffer, 32, "TEST >:(");
		//HD44780WriteString(0, 0, lcd_buffer, 32);
		lcd_display_set_string(lcd_buffer);
		//audio_play_pause();
		updated = true;
	}
}

void audioControllerLCDConfigOn(void) {
	//HD44780ClearDisplay();
	snprintf(lcd_buffer, 32, "MP3_PLAYER");
	lcd_display_config();
	lcd_display_set_string(lcd_buffer);
	//HD44780WriteString(0, 0, lcd_buffer, 10);
	updated = false;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static event_t getEvent(void) {

	if( encoderGetStatus() ) {
		switch( encoderGetEvent() ) {
			case ENC_LEFT:
				return EVENT_ENCODER_LEFT;
			case ENC_RIGHT:
				return EVENT_ENCODER_RIGHT;
			default:
				break;
		}
	}
	pressstatus_t button1Status = getButton1Status();
	if( button1Status ) {
		switch( button1Status ) {
			case SHORTPRESS:
				return EVENT_BUTTON1_SHORT;
			case LONGPRESS:
				return EVENT_BUTTON1_LONG;
			default:
				break;
		}
	}

	pressstatus_t button2Status = getButton2Status();
	if( button2Status ) {
		switch( button2Status ) {
			case SHORTPRESS:
				return EVENT_BUTTON2_SHORT;
			case LONGPRESS:
				return EVENT_BUTTON2_LONG;
			default:
				break;
		}
	}

	pressstatus_t buttonEncoderStatus = getButtonEncoderStatus();
	if( buttonEncoderStatus ) {
		switch( buttonEncoderStatus ) {
			case SHORTPRESS:
				return EVENT_BUTTONENC_SHORT;
			case LONGPRESS:
				return EVENT_BUTTONENC_LONG;
			default:
				break;
		}
	}
	return EVENT_NONE;
}


/* EVENT_NONE */
static void eventHandlerNoneModeExplore(void) {
	//do nothing
}

static void eventHandlerNoneModePlay(void) {
	//do nothing
}

/* EVENT_ENCODER_LEFT */
static void eventHandlerEncoderLeftModeExplore(void) {
	//go to previous song
	getPrevFile();
	metadata = getMetadata();
	#ifdef AUDIOCONTROL_DEBUG
	PRINTF("%s\n", metadata.name);
	#endif
	// *** TODO ***
	snprintf(lcd_buffer, 32, "%s", metadata.name);
	updated = false;
}

static void eventHandlerEncoderLeftModePlay(void) {
	//decrease volume
	volume = decrease_volume();
	#ifdef AUDIOCONTROL_DEBUG
	PRINTF("V0LUME %d\n", volume);
	#endif
	// *** TODO ***
	snprintf(lcd_buffer, 32, "V0LUME %d", volume);
	updated = false;
	timerStart(reprint_id, 1000, TIM_MODE_SINGLESHOT, reprintName);
}

/* EVENT_ENCODER_RIGHT */

static void eventHandlerEncoderRightModeExplore(void) {
	//go to next song
	getNextFile();
	metadata = getMetadata();
	#ifdef AUDIOCONTROL_DEBUG
	PRINTF("%s\n", metadata.name);
	#endif
	snprintf(lcd_buffer, 32, "%s", metadata.name);
	updated = false;

}

static void eventHandlerEncoderRightModePlay(void) {
	//increase volume
	volume = increase_volume();
	#ifdef AUDIOCONTROL_DEBUG
	PRINTF("V0LUME %d\n", volume);
	#endif
	// *** TODO ***
	snprintf(lcd_buffer, 32, "V0LUME %d", volume);
	updated = false;
	timerStart(reprint_id, 1000, TIM_MODE_SINGLESHOT, reprintName);
}


/* EVENT_BUTTON1_SHORT */
static void eventHandlerButton1ShortModeExplore(void) {
	// switch between shuffle and in-order play
	#ifdef AUDIOCONTROL_DEBUG
	PRINTF("SWITCH SHUFFLE/IN ORDER PLAY MODE\n");
	#endif
	// *** TODO ***
	snprintf(lcd_buffer, 32, "Shuffle/In 0rder");
	updated = false;
	timerStart(reprint_id, 1000, TIM_MODE_SINGLESHOT, reprintName);
}

static void eventHandlerButton1ShortModePlay(void) {
	//reproduce the previous song / rewind the current song
	getPrevFile();
	metadata = getMetadata();
	#ifdef AUDIOCONTROL_DEBUG
	PRINTF("PLAY PREV %s\n", metadata.name);
	#endif
	snprintf(lcd_buffer, 32, "PLAYING %s", metadata.name);
	updated = false;
	// *** TODO ***
	if( !metadata.amdir ) {
		if(audio_load_song(metadata.name)){
			#ifdef AUDIOCONTROL_DEBUG
			audio_print_tag_data();
			#endif
			audio_start_playing();
		}
		timerStart(reprint_id, 2000, TIM_MODE_SINGLESHOT, reprintName);
	}
}

/* EVENT_BUTTON1_LONG */
static void eventHandlerButton1LongModeExplore(void) {
	//toggle mode
	state ^= 0b1; //toggle state
	#ifdef AUDIOCONTROL_DEBUG
	if(state == MODE_EXPLORE) {
		PRINTF("M0DE EXPLORE\n");
	}
	else {
		PRINTF("M0DE PLAY\n");
	}
	#endif
	if(state == MODE_EXPLORE) {
		snprintf(lcd_buffer, 32, "M0DE EXPL0RE");
		updated = false;
		//timerStart(reprint_id, 2000, TIM_MODE_SINGLESHOT, reprintName);
	}
	else {
		snprintf(lcd_buffer, 32, "M0DE PLAY");
		updated = false;
		//timerStart(reprint_id, 2000, TIM_MODE_SINGLESHOT, reprintName);
	}
}

static void eventHandlerButton1LongModePlay(void) {
	eventHandlerButton1LongModeExplore();
}

/* EVENT_BUTTON2_SHORT */
static void eventHandlerButton2ShortModeExplore(void) {
	/* TODO: switch equalizer (jazz/rock/pop/classic) */
	eq += 1;
	if (eq > CLASSIC_EQ){
		eq = NO_EQ;
	}
	#ifdef AUDIOCONTROL_DEBUG
	PRINTF("SWITCH EQUALIZER\n");
	switch(eq){
	case(NO_EQ): PRINTF("N0 EQUALIZER\n"); break;
	case(JAZZ_EQ): PRINTF("JAZZ EQUALIZER\n"); break;
	case(ROCK_EQ): PRINTF("R0CK EQUALIZER\n"); break;
	case(POP_EQ): PRINTF("P0P EQUALIZER\n"); break;
	case(CLASSIC_EQ): PRINTF("CLASSIC EQUALIZER\n"); break;
	}
	#endif
	audio_set_eq(eq);

	updated = false;
	switch(eq){
		case(NO_EQ): snprintf(lcd_buffer, 32, "N0 EQUALIZER"); break;
		case(JAZZ_EQ): snprintf(lcd_buffer, 32, "JAZZ EQUALIZER"); break;
		case(ROCK_EQ): snprintf(lcd_buffer, 32, "R0CK EQUALIZER"); break;
		case(POP_EQ): snprintf(lcd_buffer, 32, "P0P EQUALIZER"); break;
		case(CLASSIC_EQ): snprintf(lcd_buffer, 32, "CLASSIC EQUALIZER");  break;
	}
	timerStart(reprint_id, 2000, TIM_MODE_SINGLESHOT, reprintName);

}

static void eventHandlerButton2ShortModePlay(void) {
	//play the next mp3 song
	getNextFile();
	metadata = getMetadata();
	#ifdef AUDIOCONTROL_DEBUG
	PRINTF("PLAY NEXT %s\n", metadata.name);
	#endif
	if( !metadata.amdir ) {
		snprintf(lcd_buffer, 32, "PLAYING %s", metadata.name);
		updated = false;
		if(audio_load_song(metadata.name)){
			#ifdef AUDIOCONTROL_DEBUG
			audio_print_tag_data();
			#endif
			audio_start_playing();
		}
		timerStart(reprint_id, 2000, TIM_MODE_SINGLESHOT, reprintName);
	}
}

/* EVENT_BUTTON2_LONG */
static void eventHandlerButton2LongModeExplore(void) {
	// *** TODO: on/off ***
	#ifdef AUDIOCONTROL_DEBUG
	PRINTF("ON/OFF\n");
	#endif
}

static void eventHandlerButton2LongModePlay(void) {
	eventHandlerButton2LongModeExplore();
}

/* EVENT_BUTTONENC_SHORT */
static void eventHandlerButtonEncShortModeExplore(void) {
	//play the current song / enter the current folder
	metadata = getMetadata();
	if( metadata.amdir ) {
		//enter the folder
		accessFolder();
		getPath(path);
		#ifdef AUDIOCONTROL_DEBUG
		PRINTF("ENTER FOLDER %s\n", path);
		#endif
		snprintf(lcd_buffer, 32, "F0LDER %s", path);
		updated = false;
	}
	else {
		// *** TODO: play song ***
		#ifdef AUDIOCONTROL_DEBUG
		PRINTF("PLAY %s Switch to MODE PLAY\n", metadata.name);
		#endif	
		snprintf(lcd_buffer, 32, "PLAYING %s", metadata.name);
		updated = false;
		state = MODE_PLAY;
		if(audio_load_song(metadata.name)){
			#ifdef AUDIOCONTROL_DEBUG
			audio_print_tag_data();
			#endif
			audio_start_playing();
		}
		timerStart(reprint_id, 2000, TIM_MODE_SINGLESHOT, reprintName);
	}
}

static void eventHandlerButtonEncShortModePlay(void) {
	//play/pause the current song
	bool playing = audio_play_pause();
	#ifdef AUDIOCONTROL_DEBUG
	PRINTF("PLAY/PAUSE\n", playing);
	#endif
	if(playing){
		snprintf(lcd_buffer, 32, "PLAYING %s", metadata.name);
		updated = false;
		timerStart(reprint_id, 2000, TIM_MODE_SINGLESHOT, reprintName);
	}
	else{
		snprintf(lcd_buffer, 32, "PAUSE", metadata.name);
		updated = false;
		//timerStart(reprint_id, 2000, TIM_MODE_SINGLESHOT, reprintName);
	}
}

/* EVENT_BUTTONENC_LONG */
static void eventHandlerButtonEncLongModeExplore(void) {
	//go to previous folder
	goBack();
	getPath(path);
	#ifdef AUDIOCONTROL_DEBUG
	PRINTF("GO BACK TO %s\n", path);
	#endif
	// *** TODO ***
	// show in lcd
	snprintf(lcd_buffer, 32, "BACK T0 %s", path);
	updated = false;
	//timerStart(reprint_id, 2000, TIM_MODE_SINGLESHOT, reprintName);
}

static void eventHandlerButtonEncLongModePlay(void) {
	//toggle mode
	state ^= 0b1; //toggle state
	#ifdef AUDIOCONTROL_DEBUG
	if(state == MODE_EXPLORE) {
		PRINTF("MODE EXPLORE\n");
	}
	else {
		PRINTF("MODE PLAY\n");
	}
	#endif
	if(state == MODE_EXPLORE) {
		snprintf(lcd_buffer, 32, "M0DE EXPL0RE");
		updated = false;
		//timerStart(reprint_id, 2000, TIM_MODE_SINGLESHOT, reprintName);
	}
	else {
		snprintf(lcd_buffer, 32, "M0DE PLAY");
		updated = false;
		//timerStart(reprint_id, 2000, TIM_MODE_SINGLESHOT, reprintName);
	}
}

static void reprintName(void){
	snprintf(lcd_buffer, 32, "%s", metadata.name);
	updated = false;
	timerStop(reprint_id);
}
