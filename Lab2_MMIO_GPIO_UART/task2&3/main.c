#include <stdio.h>
#include "lab2_mmio.h"

void delay_ms(uint32_t count) {
    volatile uint32_t i;
    for(; count > 0; count--) {
        for(i = 0; i < 4000; i++);
    }
}

void uart_putc(char c) {
    while ((mmio_read32(LAB_UART0_LSR_ADDR) & LAB_UART_LSR_THRE) == 0);
    mmio_write32(LAB_UART0_TXDATA_ADDR, c);
}

void uart_puts(const char *str) {
    while (*str) {
        if (*str == '\n') uart_putc('\r');
        uart_putc(*str++);
    }
}

void lab_gpio_init(void) {
    uint32_t val;
    val = mmio_read32(LAB_GPIO_IOFCFG_ADDR);
    val &= ~SOC_LED_GPIO_MASK;
    val &= ~SOC_SW_GPIO_MASK;
    mmio_write32(LAB_GPIO_IOFCFG_ADDR, val);
    val = mmio_read32(LAB_GPIO_PADDIR_ADDR);
    val |= SOC_LED_GPIO_MASK;
    val &= ~SOC_SW_GPIO_MASK;
    mmio_write32(LAB_GPIO_PADDIR_ADDR, val);
    val = mmio_read32(LAB_GPIO_PADOUT_ADDR);
    val |= SOC_LED_GPIO_MASK;
    mmio_write32(LAB_GPIO_PADOUT_ADDR, val);
}

// ==========================================
// Subtask A: Running LEDs
// ==========================================
void task2a_running_leds(void) {
    uart_puts("Task 2A: Running LEDs Start...\n");
    uint32_t current_padout;
    for (int round = 0; round < 3; round++) {
        for (int i = 0; i < 6; i++) {
            uint32_t active_led = (1u << (20 + i));
            current_padout = mmio_read32(LAB_GPIO_PADOUT_ADDR);
            uint32_t new_led_state = active_led;
            current_padout = (current_padout & ~SOC_LED_GPIO_MASK) | new_led_state;
            mmio_write32(LAB_GPIO_PADOUT_ADDR, current_padout);
            delay_ms(200);
        }
    }
    uart_puts("Task 2A: Done.\n");
}

// ==========================================
// Subtask B: Switch Controls LEDs
// ==========================================
void task2b_sw_link_led(void) {
    uart_puts("Task 2B: Switch -> LED Mode.\n");
    uart_puts("Please toggle SW0-SW5 on the board.\n");
    uint32_t pin_val, sw_val, led_val, padout_val;
    while (1) {
        pin_val = mmio_read32(LAB_GPIO_PADIN_ADDR);
        sw_val = pin_val & SOC_SW_GPIO_MASK;
        led_val = sw_val >> 6;
        padout_val = mmio_read32(LAB_GPIO_PADOUT_ADDR);
        padout_val = (padout_val & ~SOC_LED_GPIO_MASK) | (led_val & SOC_LED_GPIO_MASK);
        mmio_write32(LAB_GPIO_PADOUT_ADDR, padout_val);
        delay_ms(50);
    }
}

int main(void) {
    lab_gpio_init();
    uart_puts("\nLab 2 GPIO Task Started.\n");
    task2a_running_leds();
    task2b_sw_link_led();
    return 0;
}
