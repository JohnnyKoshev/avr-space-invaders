#include "config.h"
#include "image_utils.c"
// #include <stdlib.h>

#define JOYSTICK_DEAD_ZONE 10

unsigned char spaceShip16n16[] = {
	0x00, 0x00, 0x01, 0x80, 0x03, 0xc0, 0x06, 0x60, 0x0c, 0x30, 0x18, 0x18, 0x30, 0x0c, 0x20, 0x04,
	0x60, 0x06, 0x40, 0x02, 0x43, 0xc2, 0x46, 0x62, 0x5d, 0xba, 0x71, 0x8e, 0x01, 0x80, 0x00, 0x00};

int x_position = 48;
int y_position = 0;
unsigned int joystick_y;

void initialize_system(void)
{
	init_devices();
	lcd_init();
	lcd_clear();
	ScreenBuffer_clear();
	UART_init(); // Initialize UART1
	sei();		 // Enable global interrupts (though not strictly needed in polling mode)
	GLCD_DrawImageWithRotation(x_position, y_position, spaceShip16n16, 16, 16, ROTATE_90);
}

void update_spaceship_position(void)
{
	joystick_y = 127 - Read_Adc_Data(4) / 8;

	if (abs(joystick_y - 63) > JOYSTICK_DEAD_ZONE)
	{
		y_position += (joystick_y > 63) ? 2 : -2; // change speed by changing the increment value
	}

	if (y_position > 112)
	{
		y_position = 112;
	}
	else if (y_position < 0)
	{
		y_position = 0;
	}
}

void draw_spaceship(void)
{
	lcd_clear();
	ScreenBuffer_clear();
	GLCD_DrawImageWithRotation(x_position, y_position, spaceShip16n16, 16, 16, ROTATE_90);
}

void handle_spaceship_movement(void)
{
	update_spaceship_position();
	draw_spaceship();
	_delay_ms(75);
}

void main_loop(void)
{
	UART_send_string("UART Communication Initialized...\n");
	while (1)
	{
		handle_spaceship_movement();

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
