/** @file	neopixelSWD.c
 * @brief	Software driver for WS2812 neooixel in ChibiOS.
 *
 *  Uses software bitbanging. Usage example:
 *
 * 	uint8_t* colorBuffer = 0;
 * 	neopixelConfig cfg = {
 * 		GPIOA,
 * 		6,
 * 		16,
 * 		false
 * 	};
 * 	neopixelInit(&cfg, &colorBuffer);
 * 	neopixelSetColor(colorBuffer, LED(1) | LED(2), 255, 220, 0);
 * 	chThdSleepMilliseconds(1000);
 * 	neopixelWrite(&cfg, colorBuffer);
 *
 *  Buffer initialization inspired by omriiluz driver:
 *  https://github.com/omriiluz/WS2812B-LED-Driver-ChibiOS
 *
 * 	TODO:
 * 	To make the driver work well with larger chains, it's good to be able to
 * 	send an ARRAY of the leds we want to affect in setColor. Add one function
 * 	for setColor with just one int argument, and one function that takes both
 * 	an INTEGER ARRAY affectedLeds and an INTEGER ARRAYSIZE affectedLedCount.
 *
 * 	TODO:
 * 	Clean up LED driver and make it a bit more generic, for example, define the timings as
 * 	macros relative to the frequency - IFNDEF FREQUENCY_MHZ DEFINE FREQUENCY_MHZ
 * 	168 should allow anyone to define another frequency.
 * 	OSC_TIME = 1 000 000 000 / (double)(FREQUENCY_MHZ * 000 000) = 5.95.
 * 	TIME_HIGH_1 = 400 // ns
 * 	OPERATIONS_PER_ITERATION 4 // 3 in current code
 * 	ITERATIONS_HIGH_1 = TIME_HIGH_1 / (OPERATIONS_PER_ITERATION * OSC_TIME)
 * 	(For example 400 / 21)
 * 	HOWEVER these calculation may add more steps for each division and multiplication?
 * 	(5 steps?) Maybe the nops are unneccessary also (without them, 4).
 */

#ifndef LEDDRIVER_H_
#define LEDDRIVER_H_

#include <hal.h>


/**
 * @brief Configuration struct for the neopixel SW driver.
 *
 * It should be possible to run different LED chains on different pins with
 * different configs.
 */
typedef struct {
	/**
	 * Which port the LEDs are connected to.
	 */
	ioportid_t port;

	/**
	 * Which pin the LEDs are connected to.
	 */
	ioportmask_t pin;

	/**
	 * The number of LEDs you want to control.
	 */
	uint32_t numberOfLEDs;

	/**
	 * Makes sure to enforce the reset/latch time while still in
	 * the syslock part of write. Usually not needed if you use Write()
	 * at a reasonable pace.
	 */
	bool enforceLatchTime;

} neopixelConfig;

/**
 * Helper macro for the setColor function with bit masks. LEDs start from 0.
 */
#define LED(i) (1 << i)


/**
 * Initializes the color buffer and the pin selected in the neopixelConfig.
 */
void neopixelInit(neopixelConfig* cfg, uint8_t** colorBuffer);

/**
 * @brief Nice way of setting colors by bit masks, for example LED(1) | LED(2).
 *
 * Works for the 32 first LEDs. Be responsible, no error checking with the
 * configured LED count.
 */
void neopixelSetColor(uint8_t* colorBuffer, uint32_t ledMask, unsigned char R, unsigned char G, unsigned char B);

/**
 * Writes the color buffer to the LEDs.
 */
void neopixelWrite(neopixelConfig* cfg, uint8_t* colorBuffer);


#endif /* LEDDRIVER_H_ */
