# neopixelSWD
Software (bit-banging) driver for controlling NeoPixels in ChibiOS. Tested on STM32F407. Uses chSysLock while writing to the NeoPixels. 

Used in a university project with a miniature car, where we had one 8 pixel strip (undocumented, but presumably with WS2812 pixels)  in the front and another in the rear, with each pixel corresponding to a light such as brake light etc.

Let me know if you get this working on other boards, other versions of the NeoPixels, if you find timing adjustments are needed for other versions or if you can provide insight into my timing assumptions discussed in the source files.
