#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <stdio.h>

#include "usart.h"

// Скорость UART (должна совпадать с настройкой Nextion, по умолчанию 9600)
#define BAUD       9600UL
#define UBRR_VALUE ((F_CPU / (16UL * BAUD)) - 1)

// Прототипы для stdio
static int uart_putchar_stdio(char c, FILE *stream);
static int uart_getchar_stdio(FILE *stream);

// FILE-объекты для stdio
static FILE uart_output = FDEV_SETUP_STREAM(uart_putchar_stdio, NULL, _FDEV_SETUP_WRITE);
static FILE uart_input  = FDEV_SETUP_STREAM(NULL, uart_getchar_stdio, _FDEV_SETUP_READ);

void uart_init(void)
{
    // Настраиваем скорость
    UBRR0H = (uint8_t)(UBRR_VALUE >> 8);
    UBRR0L = (uint8_t)(UBRR_VALUE & 0xFF);

    // Включаем приёмник и передатчик
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);

    // Режим 8N1: 8 бит, без чётности, 1 стоп-бит
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void io_redirect(void)
{
    stdout = &uart_output;
    stdin  = &uart_input;
}

void uart_putchar(char c)
{
    // Перевод строки: \n -> \r\n (удобно для терминала)
    if (c == '\n')
        uart_putchar('\r');

    // Ждём, пока буфер передачи освободится
    while (!(UCSR0A & (1 << UDRE0)))
        ;
    UDR0 = c;
}

char uart_getchar(void)
{
    // Ждём байт
    while (!(UCSR0A & (1 << RXC0)))
        ;
    return UDR0;
}

// Обёртки для stdio

static int uart_putchar_stdio(char c, FILE *stream)
{
    (void)stream;
    uart_putchar(c);
    return 0;
}

static int uart_getchar_stdio(FILE *stream)
{
    (void)stream;
    return uart_getchar();
}
