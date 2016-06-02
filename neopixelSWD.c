/** @file	neopixelSWD.c
 * 	@brief	Software driver for WS2812 neooixel in ChibiOS.
 *
 * 	Uses software bitbanging.
 */

#include <hal.h>
#include "neopixelSWD.h"

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------


/*
 * The below values work fine with OPEN DRAIN mode.
 * #define HIGH_TIME_1 38 // 800 ns // 40
 * #define LOW_TIME_1 22 // 450 ns // 20
 * #define HIGH_TIME_0 18 // 400 ns // 15 // 12 för lågt, 24 för högt
 * #define LOW_TIME_0 43 // 850 ns // 40
 *
 * With push-pull, the behaviour is much less sensitive to voltage
 * variations caused by other components. However, for some reasons
 * (probably the hardware specifics of the internal resistors of the STM32,
 * maybe the signal is just very fast at going high, but slower when going
 * low) the zeroes are understood as ones if we use the same HIGH_TIME_0!
 * Here we need to use a significantly lower HIGH_TIME_0.
 *
 * In general, the time seems to be around 21 ms per NOP - maybe it's because
 * one oscillation in 68mhz is 5.95 ns, and it usually takes three for each
 * iteration of the NOP for loops (the NOP, the boolean check and the counter
 * increase)?
 */

/**
 * The number of NOP loop iterations the signal should be high when sending a 1.
 */
#define HIGH_TIME_1 38

/**
 * The number of NOP loop iterations the signal should be low when sending a 1.
 */
#define LOW_TIME_1 22 // 450 ns // 20

/**
 * The number of NOP loop iterations the signal should be high when sending a 0.
 */
#define HIGH_TIME_0 5 // 400 ns // 15 // 12 för lågt, 24 för högt

/**
 * The number of NOP loop iterations the signal should be low when sending a 0.
 */
#define LOW_TIME_0 55 // 850 ns // 40

/**
 * The number of NOP loop iterations the signal should be high when sending a 1.
 */
#define RESET_TIME_CLEAR 2400 // 50 microseconds


//-----------------------------------------------------------------------------
// Public interface
//-----------------------------------------------------------------------------


/**
 * Initializes the color buffer and the pin selected in the neopixelConfig.
 */
void neopixelInit(neopixelConfig* cfg, uint8_t** colorBuffer) {
	palSetPadMode(cfg->port, cfg->pin, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
	palClearPad(cfg->port, cfg->pin);

	// Allocate memory, set all elements to 0
	uint8_t* allocatedBuffer = chHeapAlloc(NULL, sizeof(unsigned char) * cfg->numberOfLEDs * 3);
	*colorBuffer = allocatedBuffer;

	uint32_t i;
	for (i = 0; i < cfg->numberOfLEDs * 3; i++) allocatedBuffer[i] = 0;
}


/**
 * @brief Nice way of setting colors by bit masks, for example LED(1) | LED(2).
 *
 * Works for the 32 first LEDs. Be responsible, no error checking with the
 * configured LED count.
 */
void neopixelSetColor(uint8_t* colorBuffer, uint32_t ledMask,
		unsigned char R, unsigned char G, unsigned char B) {
	uint32_t i;
	for (i = 0; i < 32; i++) {
		if (ledMask & (1 << i)) {
			//GRB =  (G << 16) | (R << 8) | B;
			colorBuffer[i * 3] = G;
			colorBuffer[i * 3 + 1] = R;
			colorBuffer[i * 3 + 2] = B;
		}
	}
}

/**
 * Writes the color buffer to the LEDs.
 */
void neopixelWrite(neopixelConfig* cfg, uint8_t* colorBuffer) {
	// For timing reasons, we prepare variables before we start writing
	uint32_t currentLED;
	uint32_t currentLEDColor;
	int c = 0;
	int bit = 0;
	ioportid_t port = cfg->port;
	ioportmask_t pin = cfg->pin;

	chSysLock();
	for (currentLED = 0; currentLED < cfg->numberOfLEDs; currentLED++) {

		currentLEDColor = colorBuffer[currentLED * 3] << 16 | colorBuffer[currentLED * 3 + 1] << 8 |
				colorBuffer[currentLED * 3 + 2];

		for (bit = 23; bit >= 0; bit--) {
			// See if bit i is set
			if ((currentLEDColor & (uint32_t)(0x01 << (bit))) == (uint32_t)(0x01 << (bit))) {
				// It's a 1 - 0.8 ms high, 0.45 ms low
				palSetPad(port, pin);
				for (c = 0; c < HIGH_TIME_1; c++) __asm__("nop");
						// Note probably just the loop consumes significant time
				palClearPad(port, pin);
				for (c = 0; c < LOW_TIME_1; c++) __asm__("nop");
			} else {
				// It's a 0 - 0.4 ms high, 0.85 ms low
				palSetPad(port, pin);
				for (c = 0; c < HIGH_TIME_0 ; c++) __asm__("nop"); // Note probably just the loop consumes significant time
				palClearPad(port, pin);
				for (c = 0; c < LOW_TIME_0 ; c++) __asm__("nop");
			}
		}
	}

	if (cfg->enforceLatchTime) {
		// Make sure to keep the signal low for at least 50 us
		for (c = 0; c < RESET_TIME_CLEAR; c++) __asm__("nop");
	}
	chSysUnlock();
}
