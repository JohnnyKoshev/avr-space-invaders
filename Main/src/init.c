#include "config.h"

void init_joystick_button()
{
    DDRC &= ~(1 << PC2); 
    PORTC |= (1 << PC2); 
}

void initialize_touch_sensor(void) {
    // Set PC3 as an input pin
    DDRC &= ~(1 << PC3);  // Clear the PC3 bit in DDRC to make it an input pin

    // Enable the internal pull-up resistor on PC3 (optional, only if the sensor needs it)
    PORTC |= (1 << PC3);  // Set the PC3 bit in PORTC to enable the pull-up resistor
}

// void initialize_button(void) {
//     DDRD &= ~(1 << PD3);  // Set PD3 as input (clear the corresponding bit in DDRD)
//     PORTD |= (1 << PD3);  // Enable the pull-up resistor on PD3 (assuming button pulls low when pressed)
// }

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
    initialize_touch_sensor();
    // initialize_button();
    // EIMSK |= (1 << INT0); // Enable INT0
	// EICRA = 0x02; // Falling edge of INT0 generates an interrupt request
    sei(); 
}