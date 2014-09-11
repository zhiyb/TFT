#include <avr/io.h>
#include <util/delay.h>

#define ON	400
#define OFF	100

#define ONMIN	200

#define PINS	(_BV(5) | _BV(6) | _BV(7))

#if 1
const uint8_t seq[6][2] = {
	{_BV(7),		_BV(7)},
	{_BV(6) | _BV(7),	_BV(6) | _BV(7)},
	{_BV(6),		_BV(6)},
	{_BV(5) | _BV(6),	_BV(5) | _BV(6)},
	{_BV(5),		_BV(5)},
	{_BV(5) | _BV(7),	_BV(5) | _BV(7)},
};
#else
const uint8_t seq[6][2] = {
	{_BV(5),		_BV(5)},
	{_BV(5) | _BV(6),	_BV(5) | _BV(6)},
	{_BV(6),		_BV(6)},
	{_BV(6) | _BV(7),	_BV(6) | _BV(7)},
	{_BV(7),		_BV(7)},
	{_BV(5) | _BV(7),	_BV(5) | _BV(7)},
};
#endif

void init(void)
{
	MCUCR |= 0x80;					// Disable JTAG
	MCUCR |= 0x80;

	DDRB |= _BV(7);
	PORTB = 0;

	DDRD |= PINS;
	PORTD = 0x00;
	DDRC &= ~PINS;
	PORTC |= PINS;

	DDRB |= _BV(4);
	TCCR0B = 0;
	TCCR0A = _BV(COM0B1) | _BV(WGM00) | _BV(WGM01);	// Fast PWM
	OCR0B = 128;
	TIMSK0 = 0;
	TCCR0B = _BV(CS00);				// Start PWM
}

int main(void)
{
	init();

	while (1) {
		uint8_t i, j;
		for (i = 0; i < 6; i++) {
			uint8_t k = 0;
			do {
				PORTD = seq[i][0];
				_delay_us(ON);
				PORTD = 0;
				for (j = 0; j < OFF; j++) {
					_delay_us(1);
					if ((PINC & PINS) == seq[i][1])
						break;
				}
				k++;
			} while ((PINC & PINS) != seq[i][1] && k != 255);
		}
	}

	return 1;
}