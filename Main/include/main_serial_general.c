

#include "config.h" // Include your configuration file

#ifdef SERIAL_GENERAL

void UART1_init(void) {
	// Set baud rate (9600 8N1)
	UBRR1H = (F_CPU / 16 / BAUD - 1) >> 8;  // Set the high byte of the UBRR register
	UBRR1L = (F_CPU / 16 / BAUD - 1);       // Set the low byte of the UBRR register

	// Enable receiver and transmitter
	UCSR1B = (1 << RXEN1) | (1 << TXEN1);

	// Set frame format: 8 data bits, no parity, 1 stop bit (8N1)
	UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);
}

void UART1_transmit(unsigned char data) {
	// Wait until the transmit buffer is empty
	while (!(UCSR1A & (1 << UDRE1)));
	// Put data into the buffer, which sends the data
	UDR1 = data;
}

unsigned char UART1_receive(void) {
	// Wait until data has been received
	while (!(UCSR1A & (1 << RXC1)));
	// Get and return received data from the buffer
	return UDR1;
}

void UART1_send_string(const char *str) {
	while (*str) {
		UART1_transmit(*str++);  // Transmit each character in the string
	}
}

int main_serial_general(void) {
	UART1_init();  // Initialize UART1
	sei();         // Enable global interrupts (though not strictly needed in polling mode)

	unsigned char received_data;
	
	// Send a welcome message
	UART1_send_string("UART Communication Initialized...\n");

	while (1) {
		// Receive data
		received_data = UART1_receive();
		
		// Send the received data back (Echo)
		UART1_transmit(received_data);
		
		// Optionally: send a confirmation message
		UART1_send_string("\nData received and echoed back.\n");
	}
	
	return 0;
}
#endif