#ifndef F_CPU
#define F_CPU 16000000UL

#endif

#define WALLE_X_MIN      0    
#define WALLE_X_MAX      280   
#define WALLE_Y          156   
#define WALLE_PIC_ID     0    

#define WALLE_TRACK_X    0
#define WALLE_TRACK_Y    140
#define WALLE_TRACK_W    220   
#define WALLE_TRACK_H    40    
#define WALLE_BG_COLOR 65535U

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>

#include "usart.h"
#include "nextion.h"

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

// public

void nextion_init(void)
{
    _delay_ms(500);
    nx_send_cmd("page 0");
    printf("System started. Waiting for data frame from Nextion...\n");
}

void nextion_goto_page(uint8_t page)
{
    char buf[16];
    snprintf(buf, sizeof(buf), "page %u", (unsigned)page);
    nx_send_cmd(buf);
}

void nextion_wait_for_values(uint32_t *d1, uint32_t *t1,
                             uint32_t *d2, uint32_t *t2)
{
    uint8_t b;

    do {
        b = nx_read_byte();
    } while (b != 0x55);

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

// page1.nProg.val
void nextion_set_progress(uint8_t percent)
{
    if (percent > 100) percent = 100;
    printf("page1.nProg.val=%u%c%c%c", percent, 0xFF, 0xFF, 0xFF);
}

// page1.nSpeed.val 
void nextion_set_speed(uint16_t speed)
{
    printf("page1.nSpeed.val=%u%c%c%c", speed, 0xFF, 0xFF, 0xFF);
}

// page1.nSpeedTarget.val
void nextion_set_target_speed(uint16_t speed)
{
    printf("page1.nSpeedTarget.val=%u%c%c%c", speed, 0xFF, 0xFF, 0xFF);
}

// page1.nDist.val
void nextion_set_distance(uint16_t dist_cm)
{
    printf("page1.nDist.val=%u%c%c%c", dist_cm, 0xFF, 0xFF, 0xFF);
}

// page1.nTime.val
void nextion_set_time(uint16_t seconds)
{
    printf("page1.nTime.val=%u%c%c%c", seconds, 0xFF, 0xFF, 0xFF);
}

// page1.tVolt.val
void nextion_set_voltage(uint16_t mv)
{
    printf("page1.tVolt.val=%u%c%c%c", mv, 0xFF, 0xFF, 0xFF);
}

//page1.picWalle.val
void nextion_set_wall_e(uint8_t percent)
{
    if (percent > 100) percent = 100;

    uint16_t x = WALLE_X_MIN +
        (uint16_t)((uint32_t)(WALLE_X_MAX - WALLE_X_MIN) * percent / 100);

    printf("fill %u,%u,%u,%u,%u%c%c%c",
           WALLE_TRACK_X,
           WALLE_TRACK_Y,
           WALLE_TRACK_W,
           WALLE_TRACK_H,
           (unsigned int)WALLE_BG_COLOR, 
           0xFF, 0xFF, 0xFF);

    printf("pic %u,%u,%u%c%c%c",
           x,
           WALLE_Y,
           WALLE_PIC_ID,
           0xFF, 0xFF, 0xFF);
}

void nextion_reset_start_button(void)
{
    printf("page0.bStart.val=0%c%c%c", 0xFF, 0xFF, 0xFF);
}