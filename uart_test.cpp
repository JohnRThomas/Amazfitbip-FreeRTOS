#include <cstdio>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

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
    int UART4_PINS = GPIO10 | GPIO11;
    rcc_periph_clock_enable(RCC_GPIOC);
    gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, UART4_PINS);
    gpio_set_af(GPIOC, GPIO_AF8 /* UART_4 */, UART4_PINS);
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
            }
        }
    }

    return 0;
}
