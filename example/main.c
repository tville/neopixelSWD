/** @file	main.c
 * Exampe file for how to use the neopixelSWD software driver.
 */


// ChibiOS includes
#include <ch.h>
#include <hal.h>

// Local includes
#include "../neopixelSWD.h"


/**
 * Initializes ChibiOS and then outputs colors to two LEDs.
 */
int main(void) {
	// ChibiOS initialization
	halInit();
	chSysInit();

	// The color buffer is used to keep track of colors for each NeoPixel
	uint8_t* colorBuffer = 0;
	
	// Configuration: GPIO group, pin number, number of LEDs, and finally
	// a boolean determining wether to make sure the latch time (0 digital 
	// state) is output to the NeoPixels while still in chSysLock - normally
	// not needed, provided you make sure not to write again too soon
	// after each write
	neopixelConfig cfg = {
		GPIOA,
		6,
		2,
		false
	};
	
	// Initialize pin and color buffer with these settings
	neopixelInit(&cfg, &colorBuffer);
	
	// It can be useful to wait a short while after powering up the
	// STM32 before communicating with the NeoPixels, to make sure
	// voltage levels have stabilized.
	chThdSleepMilliseconds(200);
	
	while(true) {
		// Alternate between blue and red. Blue first
		neopixelSetColor(colorBuffer, LED(1) | LED(2), 0, 0, 255);
		neopixelWrite(&cfg, colorBuffer);
		chThdSleepMilliseconds(1000);
	
		// Red
		neopixelSetColor(colorBuffer, LED(1) | LED(2), 255, 0, 0);
		neopixelWrite(&cfg, colorBuffer);
		chThdSleepMilliseconds(1000);
 	}
	return 0;
}
