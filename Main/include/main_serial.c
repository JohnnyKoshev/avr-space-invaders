/* Serial communication*/
/* http://ebook.pldworld.com/_Semiconductors/Atmel/Databook%20CDROM/Atmel/acrobat/doc1451.pdf */

/* Atmega128 board: PD2,3:RXD1,TXD1 */
/* 	UART Registers:
	UCSRA: RXC   TXC   UDRE	
	UCSRB: RXCIE TXCIE UDRIE RXEN TXEN UCSZ2 
	UCSRC: UCSZ1 UCSZ0 
*/	

#include "config.h"

/* Serial communications UART1 for Atmega128 board: Polling */
#ifdef SERIAL_POLLING

void main_serial_polling(void)										
{
	unsigned char data;
	// UART1: 9600 8N1
	UBRR1H = (F_CPU/16/BAUD-1)>>8;		// UBRR
	UBRR1L = F_CPU/16/BAUD-1;		
	UCSR1B = (1<<RXEN1)|(1<<TXEN1);		// receiver and transmitter
	UCSR1C = (1<<UCSZ11)|(1<<UCSZ10);	// 8-bit
	sei();								//never forget
		
	DDRB = 0xFF;
	PORTB = 0xAA;	
	while (1){
		while (!(UCSR1A & (1<<RXC1)));	// receiver polling
		data = UDR1;
		PORTB = ~PORTB;
		_delay_ms(10);
		
		while(!(UCSR1A & (1<<UDRE1)));	// transmitter polling
		UDR1 = data;
		_delay_ms(10);
		PORTB = ~PORTB;
	}	
}
#endif

/* Serial communications UART1 for SimulIDE: Polling */
#ifdef SERIAL_POLLING_SIMULIDE

void main_serial_polling_simulide(void)
{
	unsigned char data;
	// UART1: 9600 8N1
	UBRR0H = (F_CPU/16/BAUD-1)>>8;		// UBRR
	UBRR0L = F_CPU/16/BAUD-1;
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);		// receiver and transmitter
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);	// 8-bit
	sei();								//never forget
	
	DDRB = 0xFF;
	PORTB = 0xAA;
	while (1){
		while (!(UCSR0A & (1<<RXC0)));	// receiver polling
		data = UDR0;
		_delay_ms(1000);
		
		while(!(UCSR0A & (1<<UDRE0)));	// transmitter polling
		UDR0 = data + 2;
		_delay_ms(1000);
	}
}
#endif

/* Serial communication: Interrupt */
/* Atmega128 board: PD2,3:RXD1,TXD1 */
/* 	UART Registers:
	UCSRA: RXC   TXC   UDRE	
	UCSRB: RXCIE TXCIE UDRIE RXEN TXEN UCSZ2 
	UCSRC: UCSZ1 UCSZ0 
*/	
#ifdef SERIAL_INTERRUPT

void main_serial_interrupt (void)
{
	DDRB = 0xFF;
	PORTB = 0xAA;

	// UART1: 9600 8N1
	UBRR1H = (F_CPU/16/BAUD-1)>>8;		// UBRR
	UBRR1L = F_CPU/16/BAUD-1;
	UCSR1B |= (1<<RXCIE1)|(1<<RXEN1)|(1<<TXEN1); // receiver/ transmit interrupt
	UCSR1C = (1<<UCSZ11)|(1<<UCSZ10);			// character size 8
	sei();
	
	while(1);

}

SIGNAL(USART1_RX_vect)					// Serial ISR
{
	static unsigned char data = 'a';
	data = UDR1;
	if ((data >= 0x41)&&(data <=0x5A)) data = data + 0x20;
	else if ((data >= 0x61) && (data <= 0x7A)) data = data - 0x20;
	UDR1 = data;			
}
#endif



#ifdef SERIAL_INTERRUPT_LAB
/*Receive two numbers,from Data Visualizer, multiply them, and display like "3 x 4 = 12" on GLCD"
*/
/* SimulIDE Arduino Uno Test */
#define BAUD_RATE 9600
void usart_init (void) 
{ 
	UCSR0B = (1<<TXEN0); 
	UCSR0C = (1<< UCSZ01)|(1<<UCSZ00); 
	UBRR0L = F_CPU/16/BAUD_RATE-1; //UBRR0L = 103; 
	
}
void usart_send (char ch) 
{ 
	while (! (UCSR0A & (1<<UDRE0))); //wait until UDR0 is empty 
	UDR0 = ch; //transmit ch 
}
void usart_sendStr(char str[]) { 
	unsigned char i = 0;
	while(str[i]) { 
		usart_send(str[i]); 
		i++; 
	} 
}
	
void main_serial_interrupt_lab (void) { 
	usart_init(); //initialize the USART 
	while(1) { 
		usart_sendStr("Hello World!\n\r"); 
		_delay_ms(1000); 
	}

}
#endif


#ifdef SERIAL_BUFFER
/* Serial Communication with Buffer */
/* Atmega128 board: PD2,3:RXD1,TXD1 */
/* 	UART Registers:
	UCSRA: RXC   TXC   UDRE	
	UCSRB: RXCIE TXCIE UDRIE RXEN TXEN UCSZ2 
	UCSRC: UCSZ1 UCSZ0 
*/	
/* UART Buffer Defines */

#define UART_RX_BUFFER_SIZE 128 /* 1,2,4,8,16,32,64,128 or 256 bytes */
#define UART_RX_BUFFER_MASK ( UART_RX_BUFFER_SIZE - 1 )
#if ( UART_RX_BUFFER_SIZE & UART_RX_BUFFER_MASK )
#error RX buffer size is not a power of 2
#endif
#define UART_TX_BUFFER_SIZE 128 /* 1,2,4,8,16,32,64,128 or 256 bytes */
#define UART_TX_BUFFER_MASK ( UART_TX_BUFFER_SIZE - 1 )
#if ( UART_TX_BUFFER_SIZE & UART_TX_BUFFER_MASK )
#error TX buffer size is not a power of 2
#endif
/* Static Variables */
static unsigned char UART_RxBuf[UART_RX_BUFFER_SIZE];
static volatile unsigned char UART_RxHead;
static volatile unsigned char UART_RxTail;
static unsigned char UART_TxBuf[UART_TX_BUFFER_SIZE];
static volatile unsigned char UART_TxHead;
static volatile unsigned char UART_TxTail;
/* Prototypes */
void InitUART( int baudrate );
unsigned char ReceiveByte( void );
void TransmitByte( unsigned char data );
/* main - a simple test program*/
void main_serial_buffer ( void )
{
	init_devices();
	lcd_clear();
	 DDRB = 0xFF;
	 PORTB = 0x00;
	 InitUART( 9600 ); /* set the baudrate to 9600 bps using a 16MHz crystal */
	 sei(); /* enable interrupts => enable UART interrupts */
	 
	 while ( 1 ) /* forever */
	 {
		TransmitByte( ReceiveByte() ); /* echo the received character */
	 }
}
/* initialize UART */
void InitUART( int baudrate )
{
	 unsigned char x;
	 UBRR1H = (F_CPU/16/baudrate-1)>>8;		// UBRR
	 UBRR1L = F_CPU/16/baudrate-1;

	 /* enable UART receiver and transmitter, and
	 receive interrupt */
	 UCSR1B = (1<<RXCIE)|(1<<RXEN1)|(1<<TXEN1);		// receiver and transmitter
	 UCSR1C = (1<<UCSZ11)|(1<<UCSZ10);	// 8-bit
	 x = 0; /* flush receive buffer */
	 UART_RxTail = x;
	 UART_RxHead = x;
	 UART_TxTail = x;
	 UART_TxHead = x;
}
/* interrupt handlers */
SIGNAL (USART1_RX_vect)
{
	PORTB ^= (1<<0);
	unsigned char data;
	unsigned char tmphead;
	data = UDR1; /* read the received data */
	/* calculate buffer index */
	tmphead = ( UART_RxHead + 1 ) & UART_RX_BUFFER_MASK;
	UART_RxHead = tmphead; /* store new index */
	if ( tmphead == UART_RxTail )
	{
		/* ERROR! Receive buffer overflow */
	}

	UART_RxBuf[tmphead] = data; /* store received data in buffer */
	lcd_xy(3,0);
	lcd_char(data);
}
  
SIGNAL (USART1_UDRE_vect)
{
	unsigned char tmptail;
	/* check if all data is transmitted */
	if ( UART_TxHead != UART_TxTail )
	{
		/* calculate buffer index */
		tmptail = ( UART_TxTail + 1 ) & UART_TX_BUFFER_MASK;
		UART_TxTail = tmptail; /* store new index */
		UDR1 = UART_TxBuf[tmptail]; /* start transmmition */
	}
	else
	{
		UCSR1B &= ~(1<<UDRIE); /* disable UDRE interrupt */
	}
}
/* Read and write functions */
unsigned char ReceiveByte( void )
{
	unsigned char tmptail;
	while ( UART_RxHead == UART_RxTail ); /* wait for incoming data */
	tmptail = ( UART_RxTail + 1 ) & UART_RX_BUFFER_MASK;/* calculate buffer index */
	UART_RxTail = tmptail; /* store new index */
	return UART_RxBuf[tmptail]; /* return data */
}
void TransmitByte( unsigned char data )
{
	unsigned char tmphead;
	/* calculate buffer index */
	tmphead = ( UART_TxHead + 1 ) & UART_TX_BUFFER_MASK; /* wait for free space in buffer */
	while ( tmphead == UART_TxTail );
	UART_TxBuf[tmphead] = data; /* store data in buffer */
	UART_TxHead = tmphead; /* store new index */
	UCSR1B |= (1<<UDRIE); /* enable UDRE interrupt */
}
unsigned char DataInReceiveBuffer( void )
{
	return ( UART_RxHead != UART_RxTail ); /* return 0 (FALSE) if the receive buffer is empty */
}
#endif

#ifdef SERIAL_WORD
/************** serial word communication  ************************/
#include <stdbool.h>
#define TX_BUFFER_SIZE 64

// Transmission buffer global variables
volatile char tx_buffer[TX_BUFFER_SIZE];
volatile uint8_t tx_write_pos = 0;
volatile uint8_t tx_read_pos = 0;

// Function prototypes
void init_UART(void);
void send_string(const char *s);
int string_in_list(const char *received, const char *commands[], int size);

// UART helper functions
uint8_t is_tx_buffer_empty() {
	return tx_read_pos == tx_write_pos;
}

void write_tx_buffer(char c) {
	if ((tx_write_pos + 1) % TX_BUFFER_SIZE != tx_read_pos) {
		tx_buffer[tx_write_pos] = c;
		tx_write_pos = (tx_write_pos + 1) % TX_BUFFER_SIZE;
	}
	// Buffer overflow can be handled here if needed
}

ISR(USART1_UDRE_vect) {
	if (!is_tx_buffer_empty()) {
		UDR1 = tx_buffer[tx_read_pos];
		tx_read_pos = (tx_read_pos + 1) % TX_BUFFER_SIZE;
		} else {
		// No more data, disable this interrupt until new data is written
		UCSR1B &= ~(1<<UDRIE1);
	}
}

void send_string(const char *s) {
	while (*s) {
		write_tx_buffer(*s++);
	}
	// Enable the Data Register Empty interrupt
	UCSR1B |= (1<<UDRIE1);
}

// List of acceptable commands
const char *commands[] = {"start", "d", "a", "w", "s", "j", "stop"};

void init_UART(void) {
	UBRR1H = (F_CPU/16/BAUD-1)>>8;
	UBRR1L = F_CPU/16/BAUD-1;
	UCSR1B |= (1<<RXCIE1)|(1<<RXEN1)|(1<<TXEN1)|(1<<UDRIE1);
	UCSR1C = (1<<UCSZ11)|(1<<UCSZ10);
}

int string_in_list(const char *received, const char *commands[], int size) {
	for (int i = 0; i < size; i++) {
		if (strcmp(received, commands[i]) == 0) {
			return 1;
		}
	}
	return 0;
}

SIGNAL(USART1_RX_vect) {
	static char buffer[16];
	static uint8_t i = 0;

	char data = UDR1;

	if (data == '\r' || data == '\n') {
		if (i == 0) return;

		buffer[i] = '\0';
		send_string("\nCommand received: ");
		send_string(buffer);
		send_string("\n");

		if (string_in_list(buffer, commands, sizeof(commands)/sizeof(commands[0]))) {
			send_string("OK\n");
			} else {
			send_string("Command not recognized.\n");
		}
		i = 0;
		} else {
		if (i < sizeof(buffer) - 1) {
			buffer[i++] = data;
		}
	}
}

void main_serial_word(void) {
	init_UART();
	send_string("UART initialized...\n");
	sei();

	while(1) {
		// Your main code would go here
	}

}
#endif


