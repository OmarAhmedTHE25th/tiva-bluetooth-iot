#include "uart_bt.h"
#include "tm4c123gh6pm.h"

static char rx_buffer[BT_RX_BUFFER_SIZE];
static uint16_t rx_index = 0;
static volatile bool line_ready = false;

void UART_BT_Init(void) {
    // 1. Enable clocks for UART1 and GPIO Port B
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R1;
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;
    while ((SYSCTL_PRUART_R & SYSCTL_PRUART_R1) == 0) {}
    while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R1) == 0) {}

    // 2. Disable UART1 during configuration
    UART1_CTL_R &= ~UART_CTL_UARTEN;

    // 3. Configure PB0 (U1Rx) and PB1 (U1Tx)
    GPIO_PORTB_AMSEL_R &= ~0x03;        // Disable analog on PB0, PB1
    GPIO_PORTB_AFSEL_R |=  0x03;        // Enable alternate function on PB0, PB1
    GPIO_PORTB_PCTL_R   = (GPIO_PORTB_PCTL_R & 0xFFFFFF00) | 0x00000011; // Mux = 1 (UART1)
    GPIO_PORTB_DEN_R   |=  0x03;        // Enable digital I/O on PB0, PB1

    // 4. Configure 9600 baud @ 16 MHz PIOSC:
    // BRD = 16,000,000 / (16 * 9600) = 104.16667 -> IBRD = 104, FBRD = 11
    UART1_CC_R   = UART_CC_CS_PIOSC;    // Use 16 MHz precision internal oscillator
    UART1_IBRD_R = 104;
    UART1_FBRD_R = 11;

    // 5. 8-bit word length, no parity, 1 stop bit, FIFOs enabled
    UART1_LCRH_R = UART_LCRH_WLEN_8 | UART_LCRH_FEN;

    // 6. Enable UART1, TXE, and RXE
    UART1_CTL_R  = UART_CTL_UARTEN | UART_CTL_TXE | UART_CTL_RXE;
}

void UART_BT_Poll(void) {
    while (((UART1_FR_R & UART_FR_RXFE) == 0) && !line_ready) {
        char c = (char)(UART1_DR_R & 0xFF);

        if (c == '\r') {
            continue;
        }

        if (c == '\n') {
            rx_buffer[rx_index] = '\0';
            rx_index = 0;
            line_ready = true;
            break;
        }

        if (rx_index < (BT_RX_BUFFER_SIZE - 1)) {
            rx_buffer[rx_index++] = c;
        } else {
            rx_index = 0;
        }
    }
}

bool UART_BT_HasLine(void) {
    return line_ready;
}

void UART_BT_GetLine(char *dest, uint16_t max_len) {
    if (!line_ready || max_len == 0) {
        return;
    }

    uint16_t i = 0;
    while (rx_buffer[i] != '\0' && i < (max_len - 1)) {
        dest[i] = rx_buffer[i];
        i++;
    }
    dest[i] = '\0';
    line_ready = false;
}

void UART_BT_SendChar(char c) {
    while ((UART1_FR_R & UART_FR_TXFF) != 0) {}
    UART1_DR_R = (uint32_t)c;
}

void UART_BT_SendString(const char *str) {
    while (*str != '\0') {
        UART_BT_SendChar(*str++);
    }
}

void UART_BT_SendLine(const char *str) {
    UART_BT_SendString(str);
    UART_BT_SendChar('\n');
}