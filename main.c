#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"

#define F_CPU 16000000UL
#define ACS712_OFFSET 2.5 // Tegangan referensi pada arus nol
#define ACS712_SENSITIVITY 185 // Sensitivitas sensor (185mV/A untuk ACS712-05B)

void ADC_init() {
	ADMUX = (1 << REFS0); // Reference voltage on AVCC
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // ADC enable, prescaler 128
}

uint16_t ADC_read(uint8_t ch) {
	ch &= 0b00000000; // select ADC channel ch must be 0-7
	ADMUX = (ADMUX & 0xF8) | ch; // clear the bottom 3 bits before ORing
	ADCSRA |= (1 << ADSC); // start conversion
	while (ADCSRA & (1 << ADSC)); // wait for conversion to complete
	return ADCW;
}

float calculate_current(uint16_t adc_value) {
	float voltage = (adc_value / 1024.0) * 5.0;
	float current = (voltage - ACS712_OFFSET) / ACS712_SENSITIVITY;
	return current;
}

float calculate_voltage(uint16_t adc_value) {
	float voltage = (adc_value / 1024.0) * 5.0; // Assuming a 5V reference voltage
	return voltage;
}

int main(void) {
	lcd_init();
	lcd_clear();

	ADC_init();

	while (1) {
		uint16_t adc_value = ADC_read(0); // Read from ADC0 (PC0)
		float current = calculate_current(adc_value);
		float voltage = calculate_voltage(adc_value);

		char buffer[16];

		// Display current
		lcd_set_cursor(0, 0);
		lcd_write_string("Current: ");
		dtostrf(current, 5, 2, buffer);
		lcd_write_string(buffer);
		lcd_write_string(" A");

		// Display voltage
		lcd_set_cursor(1, 0);
		lcd_write_string("Voltage: ");
		dtostrf(voltage, 5, 2, buffer);
		lcd_write_string(buffer);
		lcd_write_string(" V");

		_delay_ms(5000);
	}

	return 0;
}