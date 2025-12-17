#ifndef APPLICATION_LAB3_MMIO_H_
#define APPLICATION_LAB3_MMIO_H_

#include <stdint.h>

static inline void mmio_write32(uint32_t addr, uint32_t val) {
    *(volatile uint32_t *)(uintptr_t)addr = val;
}

static inline uint32_t mmio_read32(uint32_t addr) {
    return *(volatile uint32_t *)(uintptr_t)addr;
}

#define LAB_GPIOA_BASE      0x10012000u
#define LAB_UART0_BASE      0x10013000u

#define GPIO_PADDIR_OFS     0x00u
#define GPIO_PADIN_OFS      0x04u
#define GPIO_PADOUT_OFS     0x08u
#define GPIO_IOFCFG_OFS     0x1Cu
#define GPIO_INTEN_OFS      0x20u
#define GPIO_INTTYPE0_OFS   0x24u
#define GPIO_INTTYPE1_OFS   0x28u
#define GPIO_INTSTATUS_OFS  0x2Cu

#define UART_RBR_OFS        0x00u
#define UART_THR_OFS        0x00u
#define UART_IER_OFS        0x04u
#define UART_IIR_OFS        0x08u
#define UART_FCR_OFS        0x08u
#define UART_LSR_OFS        0x14u

#define UART_LSR_RX_READY   (1u << 0)
#define UART_LSR_TX_EMPTY   (1u << 5)

#define LED_GPIO_MASK       (0x3Fu << 20)

#endif /* APPLICATION_LAB3_MMIO_H_ */
