#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>

#include "usart.h"
#include "nextion.h"

// ---------- Внутренние функции ----------

static void nx_send_cmd(const char *cmd)
{
    while (*cmd)
    {
        printf("%c", *cmd);
        cmd++;
    }
    printf("%c%c%c", 0xFF, 0xFF, 0xFF);
}

static uint8_t nx_read_byte(void)
{
    int c;
    do {
        c = getchar();
    } while (c == EOF);

    return (uint8_t)c;
}

// ---------- Публичные функции ----------

void nextion_init(void)
{
    _delay_ms(500);
    nx_send_cmd("page 0");
    printf("System started. Waiting for data frame from Nextion...\n");
}

// Ждём кадр: 0x55, 0x04, D1(4),T1(4),D2(4),T2(4), 0xAA
void nextion_wait_for_values(uint32_t *d1, uint32_t *t1,
                             uint32_t *d2, uint32_t *t2)
{
    uint8_t b;

    // 1) Стартовый байт
    do {
        b = nx_read_byte();
    } while (b != 0x55);

    // 2) Кол-во чисел
    uint8_t count = nx_read_byte();
    if (count != 4)
        return;

    uint32_t vals[4];

    for (uint8_t i = 0; i < 4; i++)
    {
        uint8_t b0 = nx_read_byte();
        uint8_t b1 = nx_read_byte();
        uint8_t b2 = nx_read_byte();
        uint8_t b3 = nx_read_byte();

        vals[i] =  (uint32_t)b0
                 | ((uint32_t)b1 << 8)
                 | ((uint32_t)b2 << 16)
                 | ((uint32_t)b3 << 24);
    }

    uint8_t end = nx_read_byte();
    (void)end;

    *d1 = vals[0];
    *t1 = vals[1];
    *d2 = vals[2];
    *t2 = vals[3];
}

// page0.nProg.val = percent
void nextion_set_progress(uint8_t percent)
{
    if (percent > 100) percent = 100;
    printf("page0.nProg.val=%u%c%c%c", percent, 0xFF, 0xFF, 0xFF);
}

// page0.nSpeed.val = speed
void nextion_set_speed(uint16_t speed)
{
    printf("page0.nSpeed.val=%u%c%c%c", speed, 0xFF, 0xFF, 0xFF);
}

// page0.tVolt.val = mv  (например, 7300 = 7.3V)
void nextion_set_voltage(uint16_t mv)
{
    printf("page0.tVolt.val=%u%c%c%c", mv, 0xFF, 0xFF, 0xFF);
}
