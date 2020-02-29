#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>

#include "lcd_lpm013m126c.hpp"

namespace lpm013m126c {

LCD::LCD(uint32_t backlight_port, uint32_t backlight_pin,
        uint32_t lcd_port, uint32_t sclk_pin, uint32_t mosi_pin, uint32_t cs_pin) :
        backlight_port(backlight_port),  backlight_pin(backlight_pin), lcd_port(lcd_port),
        sclk_pin(sclk_pin), mosi_pin(mosi_pin), cs_pin(cs_pin) {

    // Setup the backlight (pwm over gpio)
    gpio_mode_setup(backlight_port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, backlight_pin);
    gpio_set_output_options(backlight_port, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, backlight_pin);

    // Setup the LCD control (spi)
    gpio_mode_setup(lcd_port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, sclk_pin | mosi_pin | cs_pin);
    gpio_set_output_options(backlight_port, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, backlight_pin);

}

void LCD::set_brightness(uint8_t level) {
    // TODO support 0x0-0x100 brightness levels via pwm.
    if (level) {
        gpio_set(backlight_port, backlight_pin);
    } else {
        gpio_clear(backlight_port, backlight_pin);
    }
}
};