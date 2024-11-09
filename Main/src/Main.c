#include "config.h"
#include "image_utils.c"

#define JOYSTICK_DEAD_ZONE 10

unsigned char spaceShip16n16[] = {
	0x00, 0x00, 0x01, 0x80, 0x03, 0xc0, 0x06, 0x60, 0x0c, 0x30, 0x18, 0x18, 0x30, 0x0c, 0x20, 0x04,
	0x60, 0x06, 0x40, 0x02, 0x43, 0xc2, 0x46, 0x62, 0x5d, 0xba, 0x71, 0x8e, 0x01, 0x80, 0x00, 0x00};

int x_position = 48;
int y_position = 0;
unsigned int joystick_y;

void update_spaceship_position(void)
{
	joystick_y = 127 - Read_Adc_Data(4) / 8;

	if (abs(joystick_y - 63) > JOYSTICK_DEAD_ZONE)
	{
		y_position += (joystick_y > 63) ? 2 : -2; // change speed by changing the increment value
	}

	if (y_position > 111)
	{
		y_position = 111;
	}
	else if (y_position < 0)
	{
		y_position = 0;
	}
}

void main(void)
{
	init_devices();
	lcd_init();
	lcd_clear();
	ScreenBuffer_clear();

	GLCD_DrawImageWithRotation(x_position, y_position, spaceShip16n16, 16, 16, ROTATE_90);

	while (1)
	{
		update_spaceship_position();

		lcd_clear();
		ScreenBuffer_clear();
		GLCD_DrawImageWithRotation(x_position, y_position, spaceShip16n16, 16, 16, ROTATE_90);

		_delay_ms(75);
	}
}