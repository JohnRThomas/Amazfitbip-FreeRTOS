#include <cstring>
#include <cstdio>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

static void setup_clock(void) {
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

void setup_gpio() {
    // PC9  : BACKLIGHT ON/OFF
    // PC10 : UART TX
    // PC11 : UART TX
    rcc_periph_clock_enable(RCC_GPIOC);
    rcc_periph_clock_enable(RCC_UART4);
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO9);
    gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO10 | GPIO11);
    gpio_set_af(GPIOC, GPIO_AF8 /* UART_4 */, GPIO10 | GPIO11);
}

void setup_uart() {
    usart_set_baudrate(UART4_BASE, 9600);
    usart_set_databits(UART4_BASE, 8);
    usart_set_stopbits(UART4_BASE, USART_STOPBITS_1);
    usart_set_mode(UART4_BASE, USART_MODE_TX);
    usart_set_parity(UART4_BASE, USART_PARITY_NONE);
    usart_set_flow_control(UART4_BASE, USART_FLOWCONTROL_NONE);
    usart_enable(UART4_BASE);
}

int _write(int fd, char *ptr, int len) {
    int i = 0;

    /*
     * Write "len" of char from "ptr" to file id "fd"
     * Return number of char written.
     *
     * Only work for STDOUT, STDIN, and STDERR
     */
    if (fd > 2) {
        return -1;
    }

    while (*ptr && (i < len)) {
        usart_send_blocking(UART4_BASE, *ptr);
        if (*ptr == '\n') {
            usart_send_blocking(UART4_BASE, '\r');
        }
        i++;
        ptr++;
    }
    return i;
}

int main(void) {

    setup_clock();
    setup_gpio();
    setup_uart();

    while (true) {
        printf("Hello World!\n");
        for (int i = 0; i < 2500000; i++) __asm__("nop");	/* Wait a bit. */
    }

    return 0;
}


