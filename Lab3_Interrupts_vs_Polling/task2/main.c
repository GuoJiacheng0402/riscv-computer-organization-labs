#include <stdio.h>
#include <stdio.h>
#include "lab3_mmio.h"

// ===== External functions from lab3_trap.c =====
extern void lab3_trap_init(void);
extern void lab3_trigger_msi(void);
extern volatile uint32_t g_msi_count;

// ===== UART Transmit Functions =====
void uart0_putc(char c) {
    while ((mmio_read32(LAB_UART0_BASE + UART_LSR_OFS) & UART_LSR_TX_EMPTY) == 0);
    mmio_write32(LAB_UART0_BASE + UART_THR_OFS, c);
}

void uart0_puts(const char *s) {
    while (*s) {
        if (*s == '\n') uart0_putc('\r');
        uart0_putc(*s++);
    }
}

// ===== UART Receive (Non-blocking) =====
int uart0_getc(char *c) {
    if (mmio_read32(LAB_UART0_BASE + UART_LSR_OFS) & UART_LSR_RX_READY) {
        *c = (char)mmio_read32(LAB_UART0_BASE + UART_RBR_OFS);
        return 1;
    }
    return 0;
}

// ===== Simple Delay =====
void delay_short() {
    for (int i = 0; i < 50000; i++) __asm__ volatile("nop");
}

// ===== Main Program =====
int main(void) {
    lab3_trap_init();

    uart0_puts("\n==============================\n");
    uart0_puts("[Lab3 Task2] Trap & MSI Test\n");
    uart0_puts("Press 't' to trigger Software Interrupt.\n");
    uart0_puts("==============================\n");

    char rx_char;
    uint32_t last_count = 0;

    while (1) {
        if (uart0_getc(&rx_char)) {
            uart0_putc(rx_char);

            if (rx_char == 't' || rx_char == 'T') {
                uart0_puts("\n[CMD] Triggering MSI...\n");
                lab3_trigger_msi();
            }
        }

        if (g_msi_count != last_count) {
            uart0_puts("[SUCCESS] MSI happened! Count = ");
            uart0_putc('0' + (g_msi_count % 10));
            uart0_puts("\n");
            last_count = g_msi_count;
        }

        delay_short();
    }

    return 0;
}
