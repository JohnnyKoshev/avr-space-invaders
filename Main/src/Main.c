#include "config.h"
#include "image_utils.c"
#include "render.c"

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

void initialize_system(void)
{
	init_devices();
	lcd_init();
	lcd_clear();
	ScreenBuffer_clear();
	UART_init();
	sei();

	init_graphics();
}

void main_loop(void)
{
	UART_send_string("UART Communication Initialized...\n");
	while (1)
	{
		handle_movement();

		// unsigned char received_data = UART_receive();
		// UART_transmit(received_data);
		// UART_send_string("\nData received and echoed back.\n");
	}
}

void main(void)
{
	initialize_system();
	main_loop();
}
