#ifndef USART_H
#define USART_H

#include <stdio.h>

// Инициализация UART
void uart_init(void);

// Перенаправление printf/getchar на UART
void io_redirect(void);

// Низкоуровневые функции (если захочется использовать напрямую)
void uart_putchar(char c);
char uart_getchar(void);

#endif // USART_H
