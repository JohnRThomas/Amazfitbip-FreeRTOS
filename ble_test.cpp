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
    // PC10 : UART4 TX (stdout)
    // PC11 : UART4 RX (stdin)
    rcc_periph_clock_enable(RCC_GPIOC);
    int UART4_PINS = GPIO11;
    gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, UART4_PINS);
    gpio_set_af(GPIOC, GPIO_AF8 /* UART_4 */, UART4_PINS);

    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO10);
    gpio_set(GPIOC, GPIO10);

    // PC4 : 1.8v enable
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO4);
    gpio_set(GPIOC, GPIO4);

    // PA09 : USART1 TX (da14580)
    // PA10 : USART1 RX (da14580)
    rcc_periph_clock_enable(RCC_GPIOA);
    int USART1_PINS = /*GPIO9 | */GPIO10;
    // gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, USART1_PINS);
    // gpio_set_af(GPIOA, GPIO_AF7 /* USART_1 */, USART1_PINS);
    gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, USART1_PINS);

    // da14580 RST
    rcc_periph_clock_enable(RCC_GPIOB);
    gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, GPIO2);
    gpio_clear(GPIOB, GPIO2);

}

void uart_setup () {
    // stdin/stdout over uart
    rcc_periph_clock_enable(RCC_UART4);
    usart_set_baudrate(UART4_BASE, 9600);
    usart_set_databits(UART4_BASE, 8);
    usart_set_stopbits(UART4_BASE, USART_STOPBITS_1);
    usart_set_mode(UART4_BASE, USART_MODE_RX);
    usart_set_parity(UART4_BASE, USART_PARITY_NONE);
    usart_set_flow_control(UART4_BASE, USART_FLOWCONTROL_NONE);
    usart_enable(UART4_BASE);

    // da14580 over uart
    rcc_periph_clock_enable(RCC_USART1);
    usart_set_baudrate(USART1, 115200);
    usart_set_databits(USART1, 8);
    usart_set_stopbits(USART1, USART_STOPBITS_1);
    usart_set_mode(USART1, USART_MODE_TX_RX);
    usart_set_parity(USART1, USART_PARITY_NONE);
    usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
    // usart_enable_rx_inversion(USART1);
    usart_enable(USART1);
}

int main(void) {
    clock_setup();
    gpio_setup();
    //uart_setup();

    //printf("Welcome to Bip\n");
    for (int i = 0; i < 1000000; i++) __asm__("nop");	/* Wait a bit. */
    gpio_clear(GPIOC, GPIO10);

    // while (true) {
    //     gpio_set(GPIOC, GPIO10);
    //     for (int i = 0; i < 10000; i++) __asm__("nop");	/* Wait a bit. */
    //     gpio_clear(GPIOC, GPIO10);

    //     // Reset the ble chip.
    //     gpio_set(GPIOB, GPIO2);
    //     // Sleep for 3us (16Mhz -> 16 instructions/ usec)
    //     for (int i = 0; i < 100; i++) __asm__("nop");	/* Wait a bit. */
    //     gpio_clear(GPIOB, GPIO2);

    //     for (int i = 0; i < 10000000; i++){
    //         if (gpio_get(GPIOA, GPIO10)) {
    //             gpio_set(GPIOC, GPIO10);
    //         } else {
    //             gpio_clear(GPIOC, GPIO10);
    //         }
    //     }
    // }


    // // Read a char from usart1.
    // char c = usart_recv_blocking(USART1);

    // // Print the char to stdout for debug.
    // printf("Recieved %d\n", c);

    // // Read a char from usart1.
    // c = usart_recv_blocking(USART1);

    // // Print the char to stdout for debug.
    // printf("Recieved %d\n", c);

    // while (true) {
    //     for (int i = 0; i < 10000000; i++) __asm__("nop");	/* Wait a bit. */
    //     printf("pulse\n");
    // }

    return 0;
}