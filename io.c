#include <libopencm3/stm32/usart.h>

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
