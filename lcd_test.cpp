#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/pwr.h>

#define LCD_CS            GPIOG, GPIO12
#define LCD_SPI           SPI3

static void gpio_setup(void) {
    // PC7 : 2.8v ON/OFF
    // PC8 : VIBRO PWM
    // PC9 : BACKLIGHT ON/OFF
    rcc_periph_clock_enable(RCC_GPIOC);
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO7 | GPIO8 | GPIO9);
    gpio_clear(GPIOC, GPIO8 | GPIO9);
    gpio_set(GPIOC, GPIO7); // Enable power for the backlight, screen and vibro. Screen will fade to black if this is disabled.

    // Since the GPIOG port is powered by VDDIO2 on L4 chips, we need to specifically
    // enable power for this port by setting the IOSV bit in PWR_CR2.
    rcc_periph_clock_enable(RCC_PWR);
    uint32_t pwr_reg32 = PWR_CR2;
    pwr_reg32 |= PWR_CR2_IOSV;
    PWR_CR2 = pwr_reg32;

    rcc_periph_clock_enable(RCC_GPIOG);

    // PG9  : LCD_1 - SCLK
    // PG10 : MISO (Not used)
    // PG11 : LCD_2 - MOSI
    // PG12 : LCD_3 - SCS
    // PG13 : LCD_5 - ON/OFF
    // PG14 : LCD_4 - EXTCOM IN
    uint16_t SPI3_PINS = GPIO9 | GPIO11;
    gpio_mode_setup(GPIOG, GPIO_MODE_AF, GPIO_PUPD_PULLDOWN, SPI3_PINS);
    gpio_mode_setup(GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12 | GPIO13 | GPIO14);
	gpio_set_af(GPIOG, GPIO_AF6, SPI3_PINS);

    // Turn the display on.
    gpio_set(GPIOG, GPIO13);

    // Clear Chip select and polarity bit.
    gpio_clear(GPIOG, GPIO12 | GPIO14);
}

static void spi_setup() {
    rcc_periph_clock_enable(RCC_SPI3);
	// Set the baudrate to 4Mhz APB1 / 2 = 2Mhz
    spi_init_master(LCD_SPI, SPI_CR1_BAUDRATE_FPCLK_DIV_2, SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
                   SPI_CR1_CPHA_CLK_TRANSITION_1, SPI_CR1_MSBFIRST);
    spi_disable_software_slave_management(LCD_SPI);
    spi_set_bidirectional_transmit_only_mode(LCD_SPI);
    spi_set_data_size(LCD_SPI, SPI_CR2_DS_8BIT);
	spi_enable_ss_output(LCD_SPI);
    spi_enable(LCD_SPI);
}

void send_line(uint8_t line, uint8_t data) {
    // Enable CS
    gpio_set(LCD_CS);

    spi_send8(LCD_SPI, 0x90); // 4-bit data mode
    spi_send8(LCD_SPI, line+1);

    // 4-bit * 176px / 8-bit
    for(int x = 0 ; x < 176 * 4 / 8; ++x){
        spi_send8(LCD_SPI, data);
    }

    // 2 dummy bytes to complete transfer
    spi_send8(LCD_SPI, 0x0);
    spi_send8(LCD_SPI, 0x0);
    while (SPI_SR(LCD_SPI) & SPI_SR_BSY);

    // Disable CS
    gpio_clear(LCD_CS);
}

int main(void) {

    //clock_setup();
    gpio_setup();
    spi_setup();

    // Turn on the backlight.
    gpio_set(GPIOC, GPIO9);

    // Send the clear all command.
    gpio_set(LCD_CS);
    spi_send8(LCD_SPI, 0x20);
    spi_send8(LCD_SPI, 0x00);
    gpio_clear(LCD_CS);

    uint8_t k = 0;
    uint8_t colors[] = {0x00, 0x88, 0x44, 0x22, 0x66, 0xAA, 0xCC, 0xFF};
    uint8_t color = 0;

    while(true) {

        send_line(k++, colors[color]);

        if (k >= 176) {
            k = 0;
            if (++color > 7) color = 0;
        }

        // Periodically invert the polarity bit so the crystals don't degrade.
        gpio_set(GPIOG, GPIO14);
        for (int i = 0; i < 1000; i++) __asm__("nop");	/* Wait a bit. */
        gpio_clear(GPIOG, GPIO14);
    }

    return 0;
}
