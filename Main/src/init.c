#include "config.h"

void Timer2_init(void) 
{
    TCCR2 = 0x00; 
    TCNT2 = 0x1A; 
    TCCR2 = 0x03; 
    TIMSK = 0x40; 
}

void init_joystick_button()
{
    DDRC &= ~(1 << PC2);
    PORTC |= (1 << PC2);
}

int is_joystick_button_pressed()
{
    return !(PINC & (1 << PC2));
}

void UART1_transmit(unsigned char data)
{

    while (!(UCSR1A & (1 << UDRE1)))
        ;

    UDR1 = data;
}

void UART1_send_string(const char *str)
{
    while (*str)
    {
        UART1_transmit(*str++);
    }
}

void UART1_initialize(void)
{
    UBRR1H = (F_CPU / 16 / BAUD - 1) >> 8;
    UBRR1L = (F_CPU / 16 / BAUD - 1);
    UCSR1B = (1 << RXEN1) | (1 << TXEN1) | (1 << RXCIE1);
    UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);
}

void initialize_devices(void)
{
    cli();
    Port_init();
    Interrupt_init();
    Timer2_init();
    UART1_initialize();
    Adc_init();
    lcd_init();
    init_joystick_button();
    sei();
}