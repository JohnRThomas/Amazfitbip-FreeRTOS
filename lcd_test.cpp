#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>

//#include "lcd_lpm013m126c.hpp"

static void clock_setup(void) {
    // HSI16 at 16Mhz
    rcc_set_hpre(RCC_CFGR_HPRE_DIV2); // Set to 16Mhz/2 = 8Mhz
    rcc_set_ppre1(RCC_CFGR_PPRE1_DIV2); // Set to 8Mhz/2 = 4Mhz
    rcc_set_ppre2(RCC_CFGR_PPRE2_DIV2); // Set to 8Mhz/2 = 4Mhz

    rcc_osc_on(RCC_HSI16);
    rcc_wait_for_osc_ready(RCC_HSI16);
    rcc_set_sysclk_source(RCC_CFGR_SW_HSI16);

    rcc_ahb_frequency  = 8000000;
    rcc_apb1_frequency = 4000000;
    rcc_apb2_frequency = 4000000;
}

static void gpio_setup(void) {

    // Enable the main button.
	rcc_periph_clock_enable(RCC_GPIOA);
	gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO0);

	/* Enable GPIOB clock. */
	rcc_periph_clock_enable(RCC_GPIOB);
    gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12);
    gpio_set(GPIOB, GPIO12);

    // PC7 : PWM 2.8v ON/OFF
    // PC8 : ??
    // PC9 : BACK LIGHT ON/OFF
	rcc_periph_clock_enable(RCC_GPIOC);
	gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO7 | GPIO8 | GPIO9);
    gpio_set(GPIOC, GPIO7);
    gpio_clear(GPIOC, GPIO8);
}

static void spi_setup(void) {
	rcc_periph_clock_enable(RCC_GPIOG);
	rcc_periph_clock_enable(RCC_SPI3);

    // PG9  : LCD_1 - SCLK
    // PG10 : MISO (Not used)
    // PG11 : LCD_2 - MOSI
    // PG12 : LCD_3 - SCS
    // PG13 : LCD_5 - ON/OFF
    // PG14 : LCD_4 - EXTCOM IN

    // Turn the display on.
    gpio_mode_setup(GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13 | GPIO14);
    gpio_set(GPIOG, GPIO13);
    gpio_clear(GPIOG, GPIO14);

    uint16_t G_SPI_PINS = GPIO9 | GPIO10 | GPIO11 | GPIO12;
    gpio_mode_setup(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, G_SPI_PINS);
	gpio_set_af(GPIOG, GPIO_AF6, G_SPI_PINS);

    uint16_t C_SPI_PINS = GPIO10 | GPIO11 | GPIO12;
    gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, C_SPI_PINS);
	gpio_set_af(GPIOC, GPIO_AF6, C_SPI_PINS);


	// Set the baudrate to 4Mhz APB1 / 2 = 2Mhz
    spi_reset(SPI3);
	spi_init_master(SPI3, SPI_CR1_BAUDRATE_FPCLK_DIV_2, SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE, SPI_CR1_CPHA_CLK_TRANSITION_1, SPI_CR1_MSBFIRST);

    spi_disable_software_slave_management(SPI3);
	spi_enable_ss_output(SPI3); /* Required, see NSS, 25.3.1 section. */

    spi_enable(SPI3);
}

int main(void) {
    int i;
    int polarity_toggle = 0;

    clock_setup();
    gpio_setup();
    spi_setup();

    // Indicate startup is complete
    gpio_set(GPIOC, GPIO9);
    for (i = 0; i < 2500000; i++) __asm__("nop");	/* Wait a bit. */
    gpio_clear(GPIOC, GPIO9);

    bool sent = false;

    for (;;) {
        if (!gpio_get(GPIOA, GPIO0)) {
            gpio_set(GPIOC, GPIO9);

            if (!sent){
                gpio_set(GPIOG, GPIO12);
                spi_send(SPI3, 0x2004);
                spi_send(SPI3, 0x0000);
                gpio_clear(GPIOG, GPIO12);

                sent = true;
            }

        } else {
            gpio_clear(GPIOC, GPIO9);

            if (sent){
                gpio_set(GPIOG, GPIO12);
                spi_send(SPI3, 0x2004 | 0x1428);
                spi_send(SPI3, 0x0000);
                gpio_clear(GPIOG, GPIO12);

                sent = false;
            }
        }

        if (polarity_toggle++ > 1000000) {
            // TOGGLE Display polarity
            gpio_set(GPIOG, GPIO14);
            for (i = 0; i < 1000; i++) __asm__("nop");	/* Wait a bit. */
            gpio_clear(GPIOG, GPIO14);
            polarity_toggle = 0;
        }
    }

    return 0;
}
