#include "config.h"

void turn_on_led(uint8_t led_number)
{
    if (led_number < 8)
    {
        PORTB &= ~(1 << led_number);
    }
}

void turn_off_led(uint8_t led_number)
{
    if (led_number < 8)
    {
        PORTB |= (1 << led_number);
    }
}

void turn_on_all_leds(void)
{
    PORTB = 0x00;
}

void turn_off_all_leds(void)
{
    PORTB = 0xFF;
}
