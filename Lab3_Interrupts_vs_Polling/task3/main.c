#include <stdio.h>
#include "lab3_mmio.h"

extern void lab3_system_init(void);
extern void lab3_trigger_msi(void);
extern int rb_get(char *c);

void uart0_putc(char c) {
    while ((mmio_read32(LAB_UART0_BASE + UART_LSR_OFS) & UART_LSR_TX_EMPTY) == 0);
    mmio_write32(LAB_UART0_BASE + UART_THR_OFS, c);
}

void uart0_puts(const char *s) {
    while (*s) { if(*s=='\n') uart0_putc('\r'); uart0_putc(*s++); }
}

void delay_short() {
    for(volatile int i=0; i<10000; i++);
}

int main(void) {
    lab3_system_init();
    
    uart0_puts("\n==============================\n");
    uart0_puts("[Lab3 Task3] Interrupt-Driven UART RX\n");
    uart0_puts("Type anything! It will be echoed back via ISR.\n");
    uart0_puts("==============================\n");

    char c;
    
    while (1) {
        if (rb_get(&c)) {
            uart0_puts("RX IRQ: ");
            uart0_putc(c);
            uart0_puts("\n");

            if (c == 't') {
                uart0_puts("   -> Triggering MSI Test...\n");
                lab3_trigger_msi();
            }
        }
        delay_short();
    }

    return 0;
}
