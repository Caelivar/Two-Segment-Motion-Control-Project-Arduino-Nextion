#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

/* --- Team modules 
#include "usart.h"    
#include "pwm.h"      
#include "adc.h"     
#include "icp.h"      
#include "nextion.h"  
#include "systime.h"  

*/

#ifndef LED_PORT
#define LED_PORT PORTD
#define LED_DDR  DDRD
#define LED_PIN  PD7           
#endif

/* --- Debug --- */
#ifndef NDEBUG
  #define DBG(...) do { printf(__VA_ARGS__); } while(0)
#else
  #define DBG(...) do {} while(0)
#endif

int main(void)
{
    app_init();
}
