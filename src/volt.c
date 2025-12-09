#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "volt.h"

// Коэффициент делителя напряжения.
// Если на A0 через делитель приходит 1/3 от реального напряжения -> SCALE = 3.
// Подправите под ваш делитель в лабе.
#define VOLTAGE_SCALE 3

void volt_init(void)
{
    // A0 (PC0) как вход
    DDRC &= ~(1 << PC0);

    // АЦП: опора AVcc (5В), вход ADC0, предделитель 128 (125 кГц)
    ADMUX  = (1 << REFS0);                      // AVcc
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t volt_read_mv(void)
{
    // Канал A0 (ADC0)
    ADMUX = (ADMUX & 0xF0) | 0;

    // Старт измерения
    ADCSRA |= (1 << ADSC);

    // Ждём завершения
    while (ADCSRA & (1 << ADSC)) {}

    uint16_t adc = ADC; // 0..1023

    // 0..5В -> 0..5000 мВ
    uint32_t mv = (uint32_t)adc * 5000UL / 1023UL;

    // Учитываем делитель
    mv *= VOLTAGE_SCALE;

    return (uint16_t)mv;
}
