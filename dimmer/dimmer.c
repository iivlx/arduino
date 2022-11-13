#define F_CPU 16000000UL
#define __DELAY_BACKWARD_COMPATIBLE__ // for variable _delay_ms();

#include <avr/io.h>
#include <util/delay.h>

#include "dimmer.h"

int main(void) {	
	int delayCycles = 10;
	int delayIncrement = 5;
	
	int delayMin = 10;
	int delayMax = 1000;
	int offTime = 100;
	int delayTime = 5;	
	
	DDRB |= 0B00100000;
		
	while (1) {
		for(int i = 0; i < delayCycles; i++)
			ledOn();

		for(int i = 0; i < delayCycles; i++) {
			ledOff();
		
		adjustDelay(&delayCycles, &delayIncrement, delayMin, delayMax);
		checkOffDelay(delayCycles, delayMin, offTime);
		_delay_ms(delayTime);
    }
}

void ledOn(void) {
	PORTB |= 0B00100000; //0x20;
}

void ledOff(void) {
	PORTB &= 0B11011111; //0xDF;
}

void adjustDelay(int *delayCycles, int *delayIncrement, int delayMin, int delayMax) {
	*delayCycles += *delayIncrement;

	if( (*delayCycles <= delayMin) | (*delayCycles >= delayMax) )
		*delayIncrement = -(*delayIncrement);
}

void checkOffDelay(int delayCycles, int delayMin, int offTime) {
	if( delayCycles <= delayMin )
		_delay_ms(offTime);
}
