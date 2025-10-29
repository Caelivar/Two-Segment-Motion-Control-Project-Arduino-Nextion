#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ---- Settings LCD ----
#define LCD_ADDR 0x27
#define LCD_COLS 16
#define LCD_ROWS 2
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

// ---- INPUT INTO SERIAL (дефакто откладка) ----
static int uart_putchar(char c, FILE*) { Serial.write(c); return 0; }
static FILE uartout;

// ---- Vcc inner res. 1.1 V ----
long readVcc_mV() {
  ADMUX  = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1); // 1.1V vs Vcc
  delay(2);
  ADCSRA |= _BV(ADSC);
  while (bit_is_set(ADCSRA, ADSC)) {}
  uint16_t adc = ADCL | (ADCH << 8);
  if (adc == 0) adc = 1;
  return 1125300L / adc; 
}

// ---- A0 ----
uint16_t readA0_mV(uint8_t samples = 16, uint8_t delay_ms = 2) {
  uint32_t sum = 0;
  for (uint8_t i = 0; i < samples; i++) { sum += analogRead(A0); delay(delay_ms); }
  float avg = sum / (float)samples;
  float mV  = (avg * 5000.0f) / 1023.0f;     
  return (uint16_t)(mV + 0.5f);
}

// ---- "A0 empty" ----
bool isA0LikelyDisconnected(uint8_t samples = 16, uint8_t delay_ms = 2) {
  int minv = 1023, maxv = 0;
  for (uint8_t i = 0; i < samples; i++) {
    int v = analogRead(A0);
    if (v < minv) minv = v;
    if (v > maxv) maxv = v;
    delay(delay_ms);
  }
  int span = maxv - minv;            
  return (span > 100);                
}

// ---- Smart LCD no blush ----
String prevLine0, prevLine1;

void lcdPrintFixed(uint8_t row, const String& text) {
  String s = text;
  if (s.length() > LCD_COLS) s.remove(LCD_COLS);
  while (s.length() < LCD_COLS) s += ' ';
  lcd.setCursor(0, row);
  lcd.print(s);
}

void setup() {
  Serial.begin(9600);
  fdev_setup_stream(&uartout, uart_putchar, NULL, _FDEV_SETUP_WRITE);
  stdout = &uartout;

  analogReference(DEFAULT);  

  // I2C LCD
  lcd.init();
  lcd.backlight();
  lcdPrintFixed(0, "Starting...");
  lcdPrintFixed(1, "LCD @0x27 16x2"); 
}

void loop() {
  bool a0_empty  = isA0LikelyDisconnected();
  uint16_t a0_mV = readA0_mV();
  long vcc_mV    = readVcc_mV();

  String line0, line1;
  if (!a0_empty) {
    float a0_V = a0_mV / 1000.0f;
    char buf0[17];
    snprintf(buf0, sizeof(buf0), "A0: %4.2f V", (double)a0_V);
    line0 = String(buf0);
  } else {
    line0 = "A0 empty";
  }

  char buf1[17];
  snprintf(buf1, sizeof(buf1), "Vcc: %4.2f V", (double)(vcc_mV / 1000.0));
  line1 = String(buf1);

  if (line0 != prevLine0) { lcdPrintFixed(0, line0); prevLine0 = line0; }
  if (line1 != prevLine1) { lcdPrintFixed(1, line1); prevLine1 = line1; }

  // Logs
  if (!a0_empty) {
    printf("A0=%u mV  Vcc=%.2f V\n", a0_mV, (double)vcc_mV/1000.0);
  } else {
    printf("A0 empty  Vcc=%.2f V\n", (double)vcc_mV/1000.0);
  }

  delay(500);
}
