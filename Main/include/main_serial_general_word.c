/*
 * main_serial_general_word.c
 *
 * Created: 2024-10-21 오전 12:45:05
 *  Author: hjeong
 */ 



#include "config.h"
#ifdef SERIAL_GRNERAL_WORD
#include "config.h" // Include your configuration file
#include <string.h> // For string operations

#define MAX_BUFFER_SIZE 64  // Define a buffer size for the received word

void UART1_init_general_word(void) {
	// Set baud rate (9600 8N1)
	UBRR1H = (F_CPU / 16 / BAUD - 1) >> 8;  // Set the high byte of the UBRR register
	UBRR1L = (F_CPU / 16 / BAUD - 1);       // Set the low byte of the UBRR register

	// Enable receiver and transmitter
	UCSR1B = (1 << RXEN1) | (1 << TXEN1);

	// Set frame format: 8 data bits, no parity, 1 stop bit (8N1)
	UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);
}

void UART1_transmit_general_word(unsigned char data) {
	// Wait until the transmit buffer is empty
	while (!(UCSR1A & (1 << UDRE1)));
	// Put data into the buffer, which sends the data
	UDR1 = data;
}

unsigned char UART1_receive_general_word(void) {
	// Wait until data has been received
	while (!(UCSR1A & (1 << RXC1)));
	// Get and return received data from the buffer
	return UDR1;
}

void UART1_send_string_general_word(const char *str) {
	while (*str) {
		UART1_transmit_general_word(*str++);  // Transmit each character in the string
	}
}

void process_received_word(char *word) {
	// Check for known commands and respond accordingly
	if (strcmp(word, "start") == 0) {
		UART1_send_string_general_word("Command received: start\n");
		} else if (strcmp(word, "stop") == 0) {
		UART1_send_string_general_word("Command received: stop\n");
		} else if (strcmp(word, "reset") == 0) {
		UART1_send_string_general_word("Command received: reset\n");
		} else {
		UART1_send_string_general_word("Unknown command received\n");
	}
}

int main_serial_general_word(void) {
	UART1_init_general_word();  // Initialize UART1
	sei();         // Enable global interrupts (though not strictly needed in polling mode)

	char buffer[MAX_BUFFER_SIZE];  // Buffer to store the received word
	unsigned char index = 0;       // Index to keep track of the buffer position
	unsigned char received_data;
	
	// Send a welcome message
	UART1_send_string_general_word("UART Communication Initialized...\n");

	while (1) {
		// Receive data
		received_data = UART1_receive_general_word();
		
		// Check for newline or carriage return (indicating end of word)
		if (received_data == '\r' || received_data == '\n') {
			buffer[index] = '\0';  // Null-terminate the string
			process_received_word(buffer);  // Process the received word
			
			// Reset the buffer index
			index = 0;
			} 
		else {
			// Store the received character into the buffer (if it's not too large)
			if (index < MAX_BUFFER_SIZE - 1) {
				buffer[index++] = received_data;
			}
		}
	}
	
	return 0;
}
#endif