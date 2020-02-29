/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#include "lcd_lpm013m126c.hpp"

// static void
// gpio_setup(void) {

// 	/* Enable GPIOC clock. */
// 	rcc_periph_clock_enable(RCC_GPIOG);

// 	/* Set GPIO9 (in GPIO port C) to 'output push-pull'. */
//     gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO9);
// }


int
main(void) {
	// Enable needed clocks
	//rcc_periph_clock_enable(RCC_AFIO);  // LCD SPI
	rcc_periph_clock_enable(RCC_GPIOC);  // BACKLIGHT, VIBRO ...
	rcc_periph_clock_enable(RCC_SPI3);  // LCD SPI over GPIOG

	// Intialize the LCD
	// backlight_port, backlight_pin, lcd_port, sclk_pin, mosi_pin, cs_pin
	lpm013m126c::LCD lcd(GPIOC, GPIO9, GPIOG, GPIO9, GPIO11, GPIO12);

	int i;



	int max = 1000;
	int on = 0;
	int off = max;
	bool up = true;
	for (;;) {

		gpio_clear(GPIOC,GPIO9);	/* LED off */

		for (i = 0; i < off; i++) __asm__("nop");

		gpio_set(GPIOC,GPIO9);		/* LED on */

		for (i = 0; i < on; i++) __asm__("nop");

		if (up) {
			if (on < max) on++;
			else off--;

			if (off <= 0) up = false;
		} else {
			if (off < max) off++;
			else on--;

			if (on <= 0) up = true;
		}
	}
	return 0;
}