
/*
 * main_serial_general_word.c
 *
 * Created: 2024-10-21 오전 12:45:05
 *  Author: hjeong
 */ 



#include "config.h"
#include <string.h> // For string operations

#ifdef SERIAL_GENERAL_WORD_INTERRUPT

#define MAX_BUFFER_SIZE 64  // Define a buffer size for the received word

// Forward declaration of process_received_word
void process_received_word(char *word);

// UART Initialization with Interrupt for receiving data
void UART1_init_general_word(void) {
    // Set baud rate (9600 8N1)
    UBRR1H = (F_CPU / 16 / BAUD - 1) >> 8;  // Set the high byte of the UBRR register
    UBRR1L = (F_CPU / 16 / BAUD - 1);       // Set the low byte of the UBRR register

    // Enable receiver, transmitter, and receive complete interrupt
    UCSR1B = (1 << RXEN1) | (1 << TXEN1) | (1 << RXCIE1);

    // Set frame format: 8 data bits, no parity, 1 stop bit (8N1)
    UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);
}

// Interrupt Service Routine (ISR) for receiving data
ISR(USART1_RX_vect) {
    static char buffer[MAX_BUFFER_SIZE];
    static unsigned char index = 0;
    unsigned char received_data = UDR1;  // Read the received data

    // Check for newline or carriage return (indicating end of word)
    if (received_data == '\r' || received_data == '\n') {
        buffer[index] = '\0';  // Null-terminate the string
        process_received_word(buffer);  // Process the received word
        index = 0;  // Reset the buffer index
    } else {
        // Store the received character into the buffer (if it's not too large)
        if (index < MAX_BUFFER_SIZE - 1) {
            buffer[index++] = received_data;
        }
    }
}

// Function to send data via UART
void UART1_transmit_general_word(unsigned char data) {
    // Wait until the transmit buffer is empty
    while (!(UCSR1A & (1 << UDRE1)));
    // Put data into the buffer, which sends the data
    UDR1 = data;
}

// Function to send a string via UART
void UART1_send_string_general_word(const char *str) {
    while (*str) {
        UART1_transmit_general_word(*str++);  // Transmit each character in the string
    }
}

// Function to process the received word
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

// Main function
void main_serial_general_word_interrupt(void) {
    UART1_init_general_word();  // Initialize UART1
    sei();  // Enable global interrupts

    // Send a welcome message
    UART1_send_string_general_word("UART Communication Initialized...\n");

    while (1) {
        // Main game logic or other tasks can go here
    }

}
#endif