/*
 * controls.h
 *
 *  Created on: 19 ene. 2024
 *      Author: ohvic
 */

#ifndef CONTROLS_H_
#define CONTROLS_H_

#include "board.h"

// Inicializa el módulo de controladores utilizando interrupciones gpio para el accionamiento de los botones.
// Utilizamos los pines conectados a la izquierda donde se encontrarían los pines analógicos, pero dado que no los usamos están libres para usar.
void control_init();

#endif /* CONTROLS_H_ */
