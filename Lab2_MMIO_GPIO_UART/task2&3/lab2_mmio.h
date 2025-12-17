#ifndef APPLICATION_LAB2_MMIO_H_
#define APPLICATION_LAB2_MMIO_H_

#include <stdint.h>
#include "hbird_sdk_soc.h"
#include "board_ddr200t.h"

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
#define LAB_UART_LSR_OFS        0x14u

#define LAB_GPIOA_BASE          0x10012000u
#define LAB_UART0_BASE          0x10013000u

#define LAB_GPIO_PADDIR_ADDR    (LAB_GPIOA_BASE + LAB_GPIO_PADDIR_OFS)
#define LAB_GPIO_PADIN_ADDR     (LAB_GPIOA_BASE + LAB_GPIO_PADIN_OFS)
#define LAB_GPIO_PADOUT_ADDR    (LAB_GPIOA_BASE + LAB_GPIO_PADOUT_OFS)
#define LAB_GPIO_IOFCFG_ADDR    (LAB_GPIOA_BASE + LAB_GPIO_IOFCFG_OFS)

#define LAB_UART0_TXDATA_ADDR   (LAB_UART0_BASE + LAB_UART_TXDATA_OFS)
#define LAB_UART0_LSR_ADDR      (LAB_UART0_BASE + LAB_UART_LSR_OFS)

#ifndef SOC_LED_GPIO_MASK
    #define SOC_LED_OFFSET      20
    #define SOC_LED_GPIO_MASK   (0x3F << 20)
#endif

#ifndef SOC_SW_GPIO_MASK
    #define SOC_SW_OFFSET       26
    #define SOC_SW_GPIO_MASK    (0x3F << 26)
#endif

#define LAB_UART_LSR_THRE       (1u << 5)

#endif /* APPLICATION_LAB2_MMIO_H_ */
