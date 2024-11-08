#include "config.h"


/* Timer Registers
	TCNT0
	OCR0
	TIFR:  OCF0 TOV0
	TIMSK: OCIE0 TOIE0
	TCCR0: WGM00 WGM01 CS02 CS01 CS00
*/
/*
TCCR0:
+------------------------------------------------------------------------------+
| Bit  | Name  | Description                                                   |
+------------------------------------------------------------------------------+
| 7    | FOC0  | Force Output Compare for Channel 0:                           |
|      |       |   0: Disabled (used for debugging, not toggled in PWM mode)   |
|      |       |   1: Enabled                                                  |
+------------------------------------------------------------------------------+
| 6    | WGM00 | Waveform Generation Mode bit 0                                |
+------------------------------------------------------------------------------+
| 5-4  | COM01 | Compare Output Mode for Channel 0 bits:                       |
|      | COM00 |   00: Normal port operation (OC0 disconnected)                |
|      |       |   01: Toggle OC0 on compare match                             |
|      |       |   10: Clear OC0 on compare match                              |
|      |       |   11: Set OC0 on compare match                                |
+------------------------------------------------------------------------------+
| 3    | WGM01 | Waveform Generation Mode bit 1                                |
+------------------------------------------------------------------------------+
| 2-0  | CS02  | Clock Select bits:                                            |
|      | CS01  |   000: No clock source (Timer/Counter stopped)                |
|      | CS00  |   001: clk_io (No prescaling)                                 |
|      |       |   010: clk_io/8 (From prescaler)                              |
|      |       |   011: clk_io/64 (From prescaler)                             |
|      |       |   100: clk_io/256 (From prescaler)                            |
|      |       |   101: clk_io/1024 (From prescaler)                           |
|      |       |   110: External clock source on T0 pin (falling edge)         |
|      |       |   111: External clock source on T0 pin (rising edge)          |
+------------------------------------------------------------------------------+

Waveform Generation Modes:
+---------------------+--------+-----------------+
| WGM01 | WGM00       | Mode   | Description     |
+---------------------+--------+-----------------+
| 0     | 0           | Normal | Top: 0xFF       |
| 0     | 1           | PWM, Phase Correct     |
|       |             |        | Top: 0xFF       |
| 1     | 0           | CTC    | Top: OCR0       |
| 1     | 1           | Fast PWM               |
|       |             |        | Top: 0xFF       |
+---------------------+--------+-----------------+
*/


#ifdef TIMER_NORMAL
/* 1. Timer/Counter0 Normal Mode
TIFR:  OCF2|TOV2|ICF1|OCF1B|TOV1|OCF0|TOV0
TCCR0: FOC0|WGM00|COM01|CON00|WGM01|CS02|CS01|CS00
*/

void main_timer_normal (void)
{
	// Graphics
	init_devices();
	lcd_string(0,0,"12345 Hong Jeong");
	
	// Toggle PORTB.0
	DDRB = 1<<7;

	while (1)
	{
		// delay
		for (char i = 0; i < 255; i++){		// delay 100 times
			TCNT0 = 0x00;					// 0~255
			// Set the clock source to internal clock/1024
			TCCR0 |= (1<<CS02) | (0<< CS01) | (1<< CS00); // clock/1024
			while(!(TIFR&(1<<TOV0)));		// wait
			TCCR0 = 0;						// stop
			TIFR |= (1<<TOV0);				// reset TOV
		}
		// application
		PORTB ^= (1<<7);	
	}
}
#endif


#ifdef TIMER_COUNTER
void main_timer_counter(void)
{
	init_devices();
	// Toggle PORTB.7
	DDRB = 1<<7;

	while(1)
	{
		TCNT1 = 0xFFFD;				// Set Timer1 counter	
		// Set the clock source to external clock on T1 pin (PD6)
		TCCR1B |= (1<<CS12) | (1<<CS11) | (0<<CS10); // 111 rising, 110 falling edge
		while(!(TIFR&(1<<TOV1)));	// Wait for overflow
		TCCR1B = 0;					// Stop Timer1
		TIFR |= (1<<TOV1);			// Reset TOV1

		// application
		PORTB ^= (1<<7);
	}
}
#endif


#ifdef TIMER_CTC
/* Timer0: CTC Mode */
void main_timer_ctc (void)
{
	// Graphics
	init_devices();
	lcd_string(0,0,"12345 Hong Jeong");
	
	// application
	DDRB = 1<<7;
	
	while (1)
	{	
		// delay
		for (int i=0; i < 100; i++){
			TCCR0 = (1<<WGM01) | (1<<CS02) | (0<< CS01) | (1<< CS00); // CTC and Clock/1024
			OCR0 = 255;						// 0~255
			while(!(TIFR&(1<<OCF0)));	// wait
			TCCR0 = 0;						// stop
			TIFR |= (1<<OCF0);				// reset flag
			lcd_xy(4,0); GLCD_4DigitDecimal(i);
		}
		// application
		PORTB ^= (1<<7);
	}
}
#endif


#ifdef TIMER_FASTPWM
/* Timer0: Fast PWM mode (PB4: OC0)*/
void main_timer_fastpwm (void)
{
	// Graphics
	init_devices();
	
	// application
	DDRB = 1<<4; //OC0 PWM output
	
	while(1){
		for (int offset=255; offset >0; offset -= 10)
		{
			// delay
			for (int i=0; i < 100; i++){
				TCCR0 = (1<<WGM01) | (1<<WGM00) | (1<<COM01) | (1<<COM00) | (1<<CS02) | (0<< CS01) | (1<< CS00); // fast PWM, OC0, and Clock/1024
				OCR0 = offset;					// 0~255 input signal
				while((TIFR&(1<<OCF0))==0){}	// wait
				TCCR0 = 0;						// stop
				TIFR |= (1<<OCF0);				// reset flag
			}	
		}
	}
}
#endif

