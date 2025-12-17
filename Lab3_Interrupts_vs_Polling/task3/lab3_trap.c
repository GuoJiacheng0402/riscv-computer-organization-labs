#include <stdio.h>
#include "lab3_mmio.h"

extern void lab3_trap_entry(void);

#define RB_SIZE 64
volatile char rx_buffer[RB_SIZE];
volatile int rb_head = 0;
volatile int rb_tail = 0;

void rb_put(char c) {
    int next = (rb_head + 1) % RB_SIZE;
    if (next != rb_tail) {
        rx_buffer[rb_head] = c;
        rb_head = next;
    }
}

int rb_get(char *c) {
    if (rb_head == rb_tail) {
        return 0;
    }
    *c = rx_buffer[rb_tail];
    rb_tail = (rb_tail + 1) % RB_SIZE;
    return 1;
}

void lab3_plic_init(void) {
    mmio_write32(LAB_PLIC_PRI_ADDR(PLIC_UART0_ID), 1);
    uint32_t val = mmio_read32(LAB_PLIC_ENABLE_ADDR);
    val |= (1u << PLIC_UART0_ID);
    mmio_write32(LAB_PLIC_ENABLE_ADDR, val);
    mmio_write32(LAB_PLIC_THRES_ADDR, 0);
    unsigned long mie_val = read_csr(mie);
    mie_val |= MIE_MEIE;
    write_csr(mie, mie_val);
}

void lab3_uart_init_irq(void) {
    mmio_write32(LAB_UART0_BASE + UART_IER_OFS, UART_IER_RX_ENABLE);
}

void lab3_system_init(void) {
    write_csr(mtvec, &lab3_trap_entry);
    lab3_plic_init();
    lab3_uart_init_irq();
    unsigned long mstatus_val = read_csr(mstatus);
    mstatus_val |= MSTATUS_MIE;
    write_csr(mstatus, mstatus_val);
}

void handle_uart0_irq(void) {
    while (mmio_read32(LAB_UART0_BASE + UART_LSR_OFS) & UART_LSR_RX_READY) {
        char c = (char)mmio_read32(LAB_UART0_BASE + UART_RBR_OFS);
        rb_put(c);
    }
}

void lab3_trap_handler(unsigned long mcause, unsigned long mepc) {
    int is_interrupt = (mcause & 0x80000000UL);
    int cause_code = (mcause & 0x7FFFFFFFUL);

    if (is_interrupt) {
        switch (cause_code) {
            case 11: {
                uint32_t id = mmio_read32(LAB_PLIC_CLAIM_ADDR);
                if (id == PLIC_UART0_ID) {
                    handle_uart0_irq();
                }
                mmio_write32(LAB_PLIC_CLAIM_ADDR, id);
                break;
            }
            case 3:
                mmio_write32(LAB_CLINT_MSIP_ADDR, 0);
                break;
            default:
                break;
        }
    } else {
        while(1);
    }
}

void lab3_trigger_msi(void) {
    mmio_write32(LAB_CLINT_MSIP_ADDR, 1);
}
