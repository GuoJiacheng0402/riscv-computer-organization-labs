#include <stdio.h>
#include "lab3_mmio.h"

void delay_loops(uint32_t loops) {
    while (loops > 0) {
        loops--;
        __asm__ volatile ("nop"); 
    }
}

void uart0_putc(char c) {
    uint32_t lsr_addr = LAB_UART0_BASE + UART_LSR_OFS;
    uint32_t thr_addr = LAB_UART0_BASE + UART_THR_OFS;
    while ((mmio_read32(lsr_addr) & UART_LSR_TX_EMPTY) == 0) {}
    mmio_write32(thr_addr, (uint32_t)c);
}

void uart0_puts(const char *s) {
    while (*s) {
        if (*s == '\n') uart0_putc('\r');
        uart0_putc(*s++);
    }
}

void gpio_init_leds(void) {
    uint32_t iofcfg_addr = LAB_GPIOA_BASE + GPIO_IOFCFG_OFS;
    uint32_t val = mmio_read32(iofcfg_addr);
    val &= ~LED_GPIO_MASK;
    mmio_write32(iofcfg_addr, val);

    uint32_t dir_addr = LAB_GPIOA_BASE + GPIO_PADDIR_OFS;
    val = mmio_read32(dir_addr);
    val |= LED_GPIO_MASK;
    mmio_write32(dir_addr, val);

    uint32_t out_addr = LAB_GPIOA_BASE + GPIO_PADOUT_OFS;
    val = mmio_read32(out_addr);
    val |= LED_GPIO_MASK; 
    mmio_write32(out_addr, val);
}

int main(void) {
    gpio_init_leds();
    uart0_puts("\n==============================\n");
    uart0_puts("[Lab3 Task1] Polling Baseline\n");
    uart0_puts("==============================\n");

    uint32_t count = 0;
    uint32_t led_state = 0;
    uint32_t out_addr = LAB_GPIOA_BASE + GPIO_PADOUT_OFS;

    while (1) {
        uint32_t current_val = mmio_read32(out_addr);
        current_val ^= LED_GPIO_MASK; 
        mmio_write32(out_addr, current_val);

        if (count % 10 == 0) {
            uart0_puts("Polling Loop: LED Toggle\n");
        } else {
            uart0_putc('.');
        }

        count++;
        delay_loops(200000);
    }

    return 0;
}
