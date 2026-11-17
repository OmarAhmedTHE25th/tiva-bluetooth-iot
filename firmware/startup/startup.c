#include <stdint.h>

extern uint32_t _estack;
extern uint32_t _sidata;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;

int main(void);

void Reset_Handler(void) {
    // Copy .data section to SRAM
    uint32_t *pSrc = &_sidata;
    uint32_t *pDst = &_sdata;
    while (pDst < &_edata) {
        *pDst++ = *pSrc++;
    }

    // Zero out .bss section in SRAM
    pDst = &_sbss;
    while (pDst < &_ebss) {
        *pDst++ = 0;
    }

    // Call main
    main();

    while (1);
}

void Default_Handler(void) {
    while (1);
}

__attribute__((section(".isr_vector"), used))
void (* const g_pfnVectors[])(void) = {
    (void (*)(void))((uint32_t)&_estack),
    Reset_Handler,
    Default_Handler, // NMI
    Default_Handler, // HardFault
    Default_Handler, // MemManage
    Default_Handler, // BusFault
    Default_Handler, // UsageFault
    0, 0, 0, 0,
    Default_Handler, // SVCall
    Default_Handler, // Debug Monitor
    0,
    Default_Handler, // PendSV
    Default_Handler  // SysTick
};