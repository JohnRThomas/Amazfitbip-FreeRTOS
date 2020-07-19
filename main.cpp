#include <cstdio>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/usart.h>

#define LCD_SPI       SPI3
#define LCD_SPI_CLOCK RCC_SPI3

void print_SPI_REGS() {
    printf("---------------\n");
    printf("SPI3_CTR1: 0x%04X\n", SPI_CR1(LCD_SPI));
    printf("SPI3_CTR2: 0x%04X\n", SPI_CR2(LCD_SPI));
    printf("SPI3_SR:   0x%04X\n", SPI_SR(LCD_SPI));
    printf("SPI3_DR:   0x%04X\n", SPI_DR(LCD_SPI));
}

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

    // PC7 : PWM 2.8v ON/OFF
    // PC8 : ??
    // PC9  : BACKLIGHT ON/OFF
    // PC10 : UART TX
    // PC11 : UART TX
    rcc_periph_clock_enable(RCC_GPIOC);
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO7 | GPIO8 | GPIO9);
    gpio_set(GPIOC, GPIO7);

    int UART4_PINS = GPIO10 | GPIO11;
    gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, UART4_PINS);
    gpio_set_af(GPIOC, GPIO_AF8 /* UART_4 */, UART4_PINS);

    // PG13 : LCD_5 - ON/OFF
    // PG14 : LCD_4 - EXTCOM IN
    // Turn the display on.
    // gpio_mode_setup(GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13 | GPIO14);
    // gpio_clear(GPIOG, GPIO13);
    // gpio_clear(GPIOG, GPIO14);

    // PG9  : LCD_1 - SCLK
    // PG10 : MISO (Not used)
    // PG11 : LCD_2 - MOSI
    // PG12 : LCD_3 - SCS
    uint16_t SPI3_PINS = GPIO9 | GPIO10 | GPIO11 | GPIO12;
    rcc_periph_clock_enable(RCC_GPIOG);
    gpio_set_output_options(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH, SPI3_PINS);
    gpio_mode_setup(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, SPI3_PINS);
	gpio_set_af(GPIOG, GPIO_AF6, SPI3_PINS);
}

static void spi_setup() {
	rcc_periph_clock_enable(LCD_SPI_CLOCK);

	// Set the baudrate to 4Mhz APB1 / 4 = 1Mhz
    spi_reset(LCD_SPI);
	spi_init_master(LCD_SPI, SPI_CR1_BAUDRATE_FPCLK_DIV_4, SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
                    SPI_CR1_CPHA_CLK_TRANSITION_1, SPI_CR1_MSBFIRST);
    spi_disable_software_slave_management(LCD_SPI);
	spi_enable_ss_output(LCD_SPI); /* Required, see NSS, 25.3.1 section. */
    spi_enable(LCD_SPI);
}

void uart_setup () {
    rcc_periph_clock_enable(RCC_UART4);
    usart_set_baudrate(UART4_BASE, 9600);
    usart_set_databits(UART4_BASE, 8);
    usart_set_stopbits(UART4_BASE, USART_STOPBITS_1);
    usart_set_mode(UART4_BASE, USART_MODE_TX_RX);
    usart_set_parity(UART4_BASE, USART_PARITY_NONE);
    usart_set_flow_control(UART4_BASE, USART_FLOWCONTROL_NONE);
    usart_enable(UART4_BASE);
}

const char *bit_rep[16] = {
    [ 0] = "0000", [ 1] = "0001", [ 2] = "0010", [ 3] = "0011",
    [ 4] = "0100", [ 5] = "0101", [ 6] = "0110", [ 7] = "0111",
    [ 8] = "1000", [ 9] = "1001", [10] = "1010", [11] = "1011",
    [12] = "1100", [13] = "1101", [14] = "1110", [15] = "1111",
};

int main(void) {
    clock_setup();
    gpio_setup();
    uart_setup();
    spi_setup();

    printf("Welcome to Bip\n");

    int* address;
    int value;

    while(true) {
        char c;
        if (scanf("%c", &c)) {
            switch(c) {
                case 'r':
                    scanf(" %x", &address);
                    printf("0x%x\n", *address);
                    break;
                case 'w':
                    scanf(" %x %x", &address, &value);
                    *address = value;
                    printf("0x%x = 0x%x\n", address, *address);
                    break;
                case 'o':
                    scanf("%c", &c);
                    if (c == 'n') {
                        gpio_set(GPIOC,GPIO9);
                    } else {
                        gpio_clear(GPIOC,GPIO9);
                    }
                    break;
                case 'p':
                    print_SPI_REGS();
                    break;
            }
        }
    }

    return 0;
}
