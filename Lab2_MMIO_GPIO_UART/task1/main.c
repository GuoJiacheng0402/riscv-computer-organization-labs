#include <stdio.h>
#include "lab2_mmio.h"

void delay_loops(uint32_t loops) {
    while (loops > 0) {
        loops--;
    }
}

void uart_putc(char c) {
    while ((mmio_read32(LAB_UART0_LSR_ADDR) & LAB_UART_LSR_THRE_MASK) == 0) {
    }
    mmio_write32(LAB_UART0_TXDATA_ADDR, (uint32_t)c);
}

void uart_puts(const char *str) {
    while (*str) {
        if (*str == '\n') {
            uart_putc('\r');
        }
        uart_putc(*str++);
    }
}

int main(void) {
    delay_loops(1000);
    uart_puts("MMIO & UART Lab Start\n");
    uart_puts("Hello, Nuclei! Using direct register access.\n");
    while (1) {
        delay_loops(1000000);
        uart_puts(".");
    }
    return 0;
}
