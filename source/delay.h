#ifndef DELAY_H
#define DELAY_H

#include "MKL46Z4.h"

//simple delay
static inline void delay_ms(int ms) {
	//goes through cycles 8 MHz to ms
    for (int i=0; i < ms*800; i++) {
    	//this No Operation goes through one cycle
        __NOP();
    }
}

#endif
