#include <stdio.h>
#include "lab3_mmio.h"

extern void lab3_trap_entry(void);

volatile uint32_t g_msi_count = 0;

void lab3_trap_init(void) {
    write_csr(mtvec, &lab3_trap_entry);

    unsigned long val = read_csr(mie);
    val |= MIE_MSIE;
    write_csr(mie, val);

    val = read_csr(mstatus);
    val |= MSTATUS_MIE;
    write_csr(mstatus, val);
}

void lab3_trigger_msi(void) {
    mmio_write32(LAB_CLINT_MSIP_ADDR, 1);
}

void lab3_trap_handler(unsigned long mcause, unsigned long mepc) {
    int is_interrupt = (mcause & 0x80000000UL);
    int cause_code = (mcause & 0x7FFFFFFFUL);

    if (is_interrupt) {
        switch (cause_code) {
            case 3:
                g_msi_count++;
                mmio_write32(LAB_CLINT_MSIP_ADDR, 0);
                break;
            default:
                break;
        }
    } else {
        while(1);
    }
}
