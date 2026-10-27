#include <stdint.h>

#define SYSCTL_RCGCGPIO_R  (*((volatile uint32_t *)0x400FE608))
#define GPIO_PORTF_DIR_R   (*((volatile uint32_t *)0x40025400))
#define GPIO_PORTF_DEN_R   (*((volatile uint32_t *)0x4002551C))
#define GPIO_PORTF_DATA_R  (*((volatile uint32_t *)0x400253FC))

void delay(volatile uint32_t count) {
    while (count--) {
        __asm("nop");
    }
}

int main(void) {
    // Enable clock to Port F
    SYSCTL_RCGCGPIO_R |= (1 << 5);
    volatile uint32_t dummy = SYSCTL_RCGCGPIO_R;
    (void)dummy;

    // Set PF1 as digital output
    GPIO_PORTF_DIR_R |= (1 << 1);
    GPIO_PORTF_DEN_R |= (1 << 1);

    while (1) {
        GPIO_PORTF_DATA_R ^= (1 << 1); // Toggle Red LED
        delay(800000);
    }
}