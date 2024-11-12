#include "config.h"

void init_joystick_button()
{
    DDRC &= ~(1 << PC2); 
    PORTC |= (1 << PC2); 
}


void initialize_devices(void) 
{
    cli(); 

    Port_init(); 
    Interrupt_init();
    Timer2_init(); 
    Uart1_init();
    Adc_init();
    lcd_init(); 
    init_joystick_button();

    sei(); 
}