/***************************************************************************//**
  @file     DRV_AudioControl.h
  @brief    Control mp3 with encoder and push buttons.
  @author   Matias Bergerman, Pablo Gonzalez Leiro, Milagros Moutin, Pedro Carranza Velez
 ******************************************************************************/

#ifndef HAL_DRV_AUDIOCONTROL_H_
#define HAL_DRV_AUDIOCONTROL_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/



/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

//#define AUDIOCONTROL_DEBUG

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/



/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

// +ej: extern unsigned int anio_actual;+


/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief initialize audio controller
 * 
 */
void audioControllerInit(void);

/**
 * @brief Loop to control audio
 * 
 */
void loopAudioControl(void);


void audioControllerLCDConfigOn(void);

/*******************************************************************************
 ******************************************************************************/

#endif // HAL_DRV_AUDIOCONTROL_H_
