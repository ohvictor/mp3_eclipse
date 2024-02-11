#ifndef VUMETER_H_
#define VUMETER_H_

#include <stdint.h>
#define F_BAND_N 8

void vu_init(void);

void vu_set_power(uint8_t* input);

/**
 * @param bands: Potencia de cada banda de frecuencia (8 bandas) en un numero de 0-255
 */
//void vu_update(uint8_t bands[F_BAND_N]);
void vu_update(void);

void vu_screen_clean();


#endif /* VUMETER_H_ */
