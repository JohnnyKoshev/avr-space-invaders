#include "config.h"
#include "render.c"
#include "init.c"
#include <avr/io.h>
#include <avr/interrupt.h>

#define MAX_BUFFER_SIZE 64

ISR(USART1_RX_vect)
{
	static char buffer[MAX_BUFFER_SIZE];
	static unsigned char index = 0;
	unsigned char received_data = UDR1;

	if (received_data == '\r' || received_data == '\n')
	{
		buffer[index] = '\0';
		UART_send_string(buffer);
		index = 0;
	}
	else
	{
		if (index < MAX_BUFFER_SIZE - 1)
		{
			buffer[index++] = received_data;
		}
	}
}

// ISR(INT0_vect)
// {
//     // Check if the interrupt was caused by the joystick button press
//     if (is_joystick_button_pressed())
//     {
//         turn_on_all_leds();
//         _delay_ms(500);
//         turn_off_all_leds();
//         fire_bullet();
//     }
// }


int is_joystick_button_pressed()
{
	return !(PINC & (1 << PC2)); 
}


void initialize_system(void)
{
	initialize_devices();
	lcd_clear();
	ScreenBuffer_clear();
	UART_init();

	init_graphics();
}

void main_loop(void)
{
	sei();
	UART_send_string("UART Communication Initialized...\n");
	while (1)
	{
		detect_light_intensity();
		if (GAME_STATE == 1)
		{
			if (is_joystick_button_pressed())
			{
				fire_bullet();
			}

			handle_movement();
		}
	}
}

void main(void)
{
	initialize_system();
	main_loop();
}
