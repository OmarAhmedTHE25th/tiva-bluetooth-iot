#ifndef UART_BT_H
#define UART_BT_H

#include <stdint.h>
#include <stdbool.h>

#define BT_RX_BUFFER_SIZE 64

void UART_BT_Init(void);
void UART_BT_Poll(void);
bool UART_BT_HasLine(void);
void UART_BT_GetLine(char *dest, uint16_t max_len);

void UART_BT_SendChar(char c);
void UART_BT_SendString(const char *str);
void UART_BT_SendLine(const char *str);

#endif // UART_BT_H