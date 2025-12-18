#ifndef USART_H
#define USART_H

#include <stdio.h>

void uart_init(void);

void io_redirect(void);

void uart_putchar(char c);
char uart_getchar(void);
uint8_t uart_available(void);
uint8_t uart_read_byte(void);

#endif
