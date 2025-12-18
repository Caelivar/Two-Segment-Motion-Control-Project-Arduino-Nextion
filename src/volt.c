#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <stdint.h>
#include "volt.h"

#define VOLTAGE_SCALE 3UL

void volt_init(void)
{
    DDRC &= ~(1 << PC0);    

    ADMUX  = (1 << REFS0);   
    ADCSRA = (1 << ADEN)   
           | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); 
}

uint16_t volt_read_mv(void)
{
    // ADC0 (PC0)
    ADMUX = (ADMUX & 0xF0) | 0;

    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC)) {;}

    uint16_t adc = ADC; // 0..1023
    uint32_t mv  = (uint32_t)adc * 5000UL / 1023UL; // mV at pin
    mv *= VOLTAGE_SCALE; // restore Vin

    return (uint16_t)mv;
}
