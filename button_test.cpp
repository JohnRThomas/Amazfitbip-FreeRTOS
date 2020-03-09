#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

//#include "lcd_lpm013m126c.hpp"

static void gpio_setup(void) {

	rcc_periph_clock_enable(RCC_GPIOA);
	gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO0);

    // PC7 : PWM 2.8v ON/OFF
    // PC8 : ??
    // PC9 : BACK LIGHT ON/OFF
	rcc_periph_clock_enable(RCC_GPIOC);
	gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO7 | GPIO9);
    gpio_set(GPIOC, GPIO7);
}

int main(void) {
    int i;

    gpio_setup();

    for (;;) {

        // If the button is pressed, turn the back light on.
        if (gpio_get(GPIOA, GPIO0)) {
            gpio_clear(GPIOC,GPIO9);
        } else {
            gpio_set(GPIOC,GPIO9);
        }
	}

	return 0;
 }
