#ifndef APPLICATION_LAB2_MMIO_H_
#define APPLICATION_LAB2_MMIO_H_

#include <stdint.h>
#include "hbird_sdk_soc.h"

static inline void mmio_write32(uintptr_t addr, uint32_t val) {
    *(volatile uint32_t *)addr = val;
}

static inline uint32_t mmio_read32(uintptr_t addr) {
    return *(volatile uint32_t *)addr;
}

#define LAB_GPIO_PADDIR_OFS     0x00u
#define LAB_GPIO_PADIN_OFS      0x04u
#define LAB_GPIO_PADOUT_OFS     0x08u
#define LAB_GPIO_IOFCFG_OFS     0x1Cu

#define LAB_UART_TXDATA_OFS     0x00u
#define LAB_UART_RXDATA_OFS     0x00u
#define LAB_UART_LSR_OFS        0x14u

#define LAB_GPIO_BASE_ADDR      0x10012000u
#define LAB_UART0_BASE_ADDR     0x10013000u

#define LAB_GPIO_PADDIR_ADDR    (LAB_GPIO_BASE_ADDR + LAB_GPIO_PADDIR_OFS)
#define LAB_GPIO_PADOUT_ADDR    (LAB_GPIO_BASE_ADDR + LAB_GPIO_PADOUT_OFS)
#define LAB_GPIO_IOFCFG_ADDR    (LAB_GPIO_BASE_ADDR + LAB_GPIO_IOFCFG_OFS)

#define LAB_UART0_TXDATA_ADDR   (LAB_UART0_BASE_ADDR + LAB_UART_TXDATA_OFS)
#define LAB_UART0_LSR_ADDR      (LAB_UART0_BASE_ADDR + LAB_UART_LSR_OFS)

#define LAB_UART_LSR_THRE_MASK  (1u << 5)
#define LAB_UART_LSR_RXVAL_MASK (1u << 0)

#endif /* APPLICATION_LAB2_MMIO_H_ */
