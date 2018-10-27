/*
 * GccApplication1.c
 *
 * Created: 25/10/2018 10:39:36 PM
 * Author : Trystan D Jones
 */ 

#include <avr/io.h>

#define F_CPU	1000000UL

#include <util/delay.h>

#define LED_NUMBERS		16

unsigned char SPITransaction(unsigned char temp)
{
	for(unsigned char i=8; i--; temp <<= 1) {
		if(temp & 0x80) PORTB |=  (1 << PORTB1);
		else         PORTB &= ~(1 << PORTB1);
		PORTA |=  (1 << PORTA7);
		PORTA &= ~(1 << PORTA7);
	}
	return 0;
}

void LEDStartPreamble(void)
{
	// START FRAME
	unsigned char i;
	for(i=0; i < 4; i++)
		SPITransaction(0x00);
}

void LEDwrite(unsigned char brightness, unsigned char red, unsigned char green, unsigned char blue)
{
	SPITransaction(0xE0 | (brightness & 0x1F));
	SPITransaction(blue);
	SPITransaction(green);
	SPITransaction(red);
}

#define BRIGHTNESS	0x01 // Between 0 and 31(0x1F)


void LEDEndPreamble(void)
{
	// End Frame
	unsigned char i;
	for(i=0; i<((LED_NUMBERS + 15) / 16); i++)
		SPITransaction(0xFF);
}

typedef struct  
{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
} Colour;

const unsigned char pattern[LED_NUMBERS] = {
	1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1
};

Colour array[LED_NUMBERS] = {
	{0x00, 0x00, 0xFF},
	{0x00, 0x00, 0xFF},
	{0x00, 0x00, 0x00},
	{0x00, 0x00, 0xFF},
	{0x00, 0x00, 0xFF},
	{0x00, 0x00, 0xFF},
	{0x00, 0x00, 0xFF},
	{0x00, 0x00, 0x00},
	{0x00, 0x00, 0xFF},
	{0x00, 0x00, 0x00},
	{0x00, 0x00, 0xFF},
	{0x00, 0x00, 0xFF},
	{0x00, 0x00, 0xFF},
	{0x00, 0x00, 0xFF},
	{0x00, 0x00, 0xFF},
	{0x00, 0x00, 0xFF},
};

static Colour global = {0x00, 0x00, 0xFF};
static unsigned short cycles = 0;

int main(void)
{
	unsigned char i, j;
    /* Replace with your application code */
	CLKPSR = 0;
	DDRB |= (1 << 1);
	DDRA |= (1 << 7);
	LEDStartPreamble();
	for(i=0; i < LED_NUMBERS; i++)
		LEDwrite(BRIGHTNESS, 0x00, 0x00, 0x00);
	LEDEndPreamble();
	_delay_ms(500);
	for(i=0; i < LED_NUMBERS; i++) {
		LEDStartPreamble();
		for(j=0; j < LED_NUMBERS; j++) {
			if (j <= i)
				LEDwrite(BRIGHTNESS, 0xFF, 0, 0);
			else
				LEDwrite(BRIGHTNESS, 0, 0, 0);
		}
		LEDEndPreamble();
		_delay_ms(50);
	}
	
	_delay_ms(500);
	for(i=0; i < LED_NUMBERS; i++) {
		LEDStartPreamble();
		for(j=0; j < LED_NUMBERS; j++) {
			if (j <= i)
			LEDwrite(BRIGHTNESS, 0, 0xFF, 0);
			else
			LEDwrite(BRIGHTNESS, 0xFF, 0, 0);
		}
		LEDEndPreamble();
		_delay_ms(50);
	}
	
	_delay_ms(500);
	for(i=0; i < LED_NUMBERS; i++) {
		LEDStartPreamble();
		for(j=0; j < LED_NUMBERS; j++) {
			if (j <= i)
			LEDwrite(BRIGHTNESS, 0, 0, 0xFF);
			else
			LEDwrite(BRIGHTNESS, 0, 0xFF, 0);
		}
		LEDEndPreamble();
		_delay_ms(50);
	}
	_delay_ms(500);
	j = 0;
	
    while (1) 
    {
		_delay_ms(50);
		LEDStartPreamble();
		for(i=0; i < LED_NUMBERS; i++) {
			if (pattern[j] == 1)
				LEDwrite(BRIGHTNESS, global.red, global.green, global.blue);
			else
				LEDwrite(BRIGHTNESS, 0, 0, 0);
			if (++j >= LED_NUMBERS)
				j = 0;
		}
		if (++j >= LED_NUMBERS)
			j = 0;
		LEDEndPreamble();
		cycles++;
		if (global.blue == 0xFF && global.red == 0x00 && cycles >= 300) {
			global.red = 0xFF;
			global.green = 0x00;
			global.blue = 0x00;
			cycles = 0;
		} else if ((global.blue == 0x00 && global.red == 0xFF && cycles >= 30) || (global.blue == 0xFF && global.red == 0xFF) || (global.blue == 0x00 && global.red == 0x00)) {
			global.red = 0x00;
			global.green = 0x00;
			global.blue = 0xFF;
			cycles = 0;
		}
    }
}

