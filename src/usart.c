#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <stdio.h>

#include "usart.h"

#define BAUD       115200UL
#define UBRR_VALUE ((F_CPU / (8UL * BAUD)) - 1)

static int uart_putchar_stdio(char c, FILE *stream);
static int uart_getchar_stdio(FILE *stream);


static FILE uart_output = FDEV_SETUP_STREAM(uart_putchar_stdio, NULL, _FDEV_SETUP_WRITE);
static FILE uart_input  = FDEV_SETUP_STREAM(NULL, uart_getchar_stdio, _FDEV_SETUP_READ);

void uart_init(void)
{
    UCSR0A |= (1 << U2X0);

    UBRR0H = (uint8_t)(UBRR_VALUE >> 8);
    UBRR0L = (uint8_t)(UBRR_VALUE & 0xFF);

    UCSR0B = (1 << RXEN0) | (1 << TXEN0);

    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void io_redirect(void)
{
    stdout = &uart_output;
    stdin  = &uart_input;
}

void uart_putchar(char c)
{
    if (c == '\n')
        uart_putchar('\r');

    while (!(UCSR0A & (1 << UDRE0)))
        ;
    UDR0 = c;
}

char uart_getchar(void)
{
    while (!(UCSR0A & (1 << RXC0)))
        ;
    return UDR0;
}


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

uint8_t uart_available(void)
{
    return (UCSR0A & (1 << RXC0)) ? 1 : 0;
}

uint8_t uart_read_byte(void)
{
    return UDR0;
}
