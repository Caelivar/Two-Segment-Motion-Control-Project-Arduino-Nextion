#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "nextion_calib.h"
#include "usart.h"

#define NX_END() do { uart_putchar(0xFF); uart_putchar(0xFF); uart_putchar(0xFF); } while(0)

#define LOG_LINES 8
#define LINE_MAX  32

static char g_lines[LOG_LINES][LINE_MAX];
static uint8_t g_head = 0;  
static uint8_t g_count = 0;  

static void nx_set_text(const char *obj, const char *txt)
{
    printf("%s.txt=\"%s\"", obj, txt);
    NX_END();
}

static void nx_set_val(const char *obj, uint16_t v)
{
    printf("%s.val=%u", obj, v);
    NX_END();
}

void nextion_goto_page(uint8_t page)
{
    printf("page %u", page);
    NX_END();
}

void nextion_wait_for_start(void)
{

    for (;;)
    {
        char b = uart_getchar();
        if ((uint8_t)b == 0x55) return;
    }
}

void nextion_set_duty(uint8_t duty)
{
    nx_set_val("page1.nDuty", duty);
}

static void format_fixed2(char *out, uint16_t x100)
{
    uint16_t ip = x100 / 100;
    uint16_t fp = x100 % 100;
    sprintf(out, "%u.%02u", ip, fp);
}

void nextion_set_speed_mps_x100(uint16_t speed_mps_x100)
{
    char buf[12];
    format_fixed2(buf, speed_mps_x100);
    nx_set_text("page1.tSpeed", buf);
}

void nextion_set_voltage_mv(uint16_t mv)
{
    uint16_t v100 = (uint16_t)(mv / 10); 
    char buf[12];
    sprintf(buf, "%u.%02uV", v100 / 100, v100 % 100);
    nx_set_text("page1.tVolt", buf);
}

void nextion_set_status(const char *msg)
{
    nx_set_text("page1.tStatus", msg);
}

void nextion_log_clear(void)
{
    memset(g_lines, 0, sizeof(g_lines));
    g_head = 0;
    g_count = 0;
    nx_set_text("page1.tLog", "");
}

static void nextion_log_render(void)
{
    // \r (Nextion new line). :contentReference[oaicite:3]{index=3}
    char out[LOG_LINES * LINE_MAX];
    out[0] = '\0';

    uint8_t start = (g_count < LOG_LINES) ? 0 : g_head;
    for (uint8_t i = 0; i < g_count; i++)
    {
        uint8_t idx = (start + i) % LOG_LINES;
        if (g_lines[idx][0] == '\0') continue;

        if (out[0] != '\0') strncat(out, "\r", sizeof(out) - strlen(out) - 1);
        strncat(out, g_lines[idx], sizeof(out) - strlen(out) - 1);
    }

    nx_set_text("page1.tLog", out);
}

void nextion_log_add(uint8_t duty, uint16_t speed_mps_x100, uint16_t mv)
{
    //"015 -> 0.42 m/s  7.80V"
    char spd[12];
    format_fixed2(spd, speed_mps_x100);

    uint16_t v100 = (uint16_t)(mv / 10);

    char line[LINE_MAX];
    snprintf(line, sizeof(line), "%03u -> %s m/s  %u.%02uV",
             duty, spd, v100 / 100, v100 % 100);

    strncpy(g_lines[g_head], line, LINE_MAX - 1);
    g_lines[g_head][LINE_MAX - 1] = '\0';

    g_head = (g_head + 1) % LOG_LINES;
    if (g_count < LOG_LINES) g_count++;

    nextion_log_render();
}
