#ifndef USART_H
#define USART_H

#include <stdio.h>
#include <stdint.h>

void uart_init(void);
void io_redirect(void);

void uart_putchar(char c);
char uart_getchar(void);

uint8_t uart_rx_ready(void);

#endif
