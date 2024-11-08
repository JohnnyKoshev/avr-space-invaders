#include "config.h" 

int main(void)
{
	init_devices();
	lcd_clear();
	ScreenBuffer_clear();
	lcd_init();
	lcd_string(0, 0, "Hello from Koshev!");

	return 0;
}
