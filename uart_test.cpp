#include <cstring>
#include <cstdio>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

int _write(int fd, char *ptr, int len);

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

    setup_gpio();
    setup_uart();

    while (true) {
        usart_send_blocking(UART4_BASE, 'c');
    }

    return 0;
}


