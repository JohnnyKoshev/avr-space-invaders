//Hong Jeong
//20230202 base
//20230204 prediction

#ifdef IOT

#include "config.h"

#define F_CPU 16000000UL
#define BAUD 9600
#define MYUBRR (F_CPU/16/BAUD-1)

// UART
#define serialBuffer_SIZE 100
volatile char serialBuffer[serialBuffer_SIZE];
volatile uint8_t serialBufferPosition = 0;

// Lights
volatile int illuminationStatus = 0;  // 0 for OFF, 1 for ON
volatile int ventilationState = 0;    // 0 for OFF, 1 for ON

// DC Motor Timer0
#define DC_MOTOR_PWM PB4 // Example PWM pin
volatile uint8_t dcMotorVelocity = 0;

// servo motor Timer1
#define PWM_FREQUENCY 50 // 50 Hz or 20ms period
#define PRESCALER 8      // Prescaler for the timer
#define SERVO_PWM PB5					// OC1A, PWM pin for Servo motor
volatile int servoMotorPosition = 0;	// Starting angle

// SR04 Timer2/ADC1
// Global variables to store the echo pulse duration and distance
volatile uint16_t echoPulseDuration = 0;
volatile uint16_t measuredDistanceCm = 0;
volatile uint16_t pulse_start = 0;
volatile uint16_t pulse_end = 0;
volatile uint16_t distance = 0;
volatile uint8_t measuring = 0;

// Joystick
volatile unsigned int joystickXCoord = 0;
volatile unsigned int joystickYCoord = 0;
volatile unsigned char joystickSwitchStatus = 0;

// Game
volatile int playMode = 0; // 0 for normal mode, 1 for prediction game mode
#define ALG_AVERAGE 1
#define ALG_LINEAR_REGRESSION 2

volatile int chosenAlgorithm = ALG_AVERAGE; // Default to averaging


// Function Prototypes
void init_interrupts(void);
void initializeUARTInterrupt(void);
void initializeADC0Interrupt(void);
void showStartupInfo(void);
void transmitCharUART(char data);
void transmitStringUART(const char* str);
void processSerialCommand(const char* command);
void displayMenu(void);
int convertToTemperature(uint16_t adc_value);
void moveStepperMotor(int, int);
void initializeDCMotor(void);
void initializeServoMotor(void);
void setServoMotorAngle(int);
void setupTimer2(void);
void setupINT1(void);
int forecastNextValue(int);


// Main Function
void main_iot(void) {
	lcd_init();
	showStartupInfo();
	initializeUARTInterrupt();
	init_interrupts();
	initializeADC0Interrupt();
	initializeDCMotor(); //Timer0
	initializeServoMotor(); // Timer1
	setupTimer2();// Timer2
	setupINT1();// SR04 Echo input

	displayMenu();
	DDRB = 0xFF; // Configure PORTB as output for stepper motor
	// Configure PD7 as input with pull-up
	DDRD &= ~(1<<PD7);
	PORTD |= (1<<PD7);

	while(1) {
		// Main loop can handle other tasks or enter sleep mode
		

	}
}

/************* UART1 ***********************************************/
// UART initialization with interrupt
void initializeUARTInterrupt(void) {
	UBRR1H = (MYUBRR >> 8);
	UBRR1L = MYUBRR;
	UCSR1B = (1<<RXEN1)|(1<<TXEN1)|(1<<RXCIE1);
	UCSR1C = (1<<UCSZ11)|(1<<UCSZ10);
	sei();
}

// UART RX Complete interrupt service routine
ISR(USART1_RX_vect) {
	char receivedChar = UDR1;
	if (receivedChar == '\n' || receivedChar == '\r') {
		serialBuffer[serialBufferPosition] = '\0'; // Null-terminate the string
		
		if (playMode) {
			if (strcmp((const char*) serialBuffer, "exit") == 0 || strcmp((const char*) serialBuffer, "quit") == 0) {
				playMode = 0;
				displayMenu(); // Call function to display main menu
				transmitStringUART("Exited game mode.\n");
				} else {
				int receivedNumber = atoi((const char*) serialBuffer);
				int predictedNumber = forecastNextValue(receivedNumber);

				char buffer[20];
				sprintf(buffer, "Predicted: %d\n", predictedNumber);
				transmitStringUART(buffer);
			}
			} else {
			processSerialCommand((const char*) serialBuffer);
		}

		serialBufferPosition = 0; // Reset buffer position
		} else if (serialBufferPosition < serialBuffer_SIZE - 1) {
		serialBuffer[serialBufferPosition++] = receivedChar;
	}
}


// Function to transmit a string over UART
void transmitStringUART(const char* str) {
	while (*str) {
		transmitCharUART(*str++);
	}
}

// Function to transmit a single character over UART
void transmitCharUART(char data) {
	while (!(UCSR1A & (1<<UDRE1)));
	UDR1 = data;
}

/************* Menu & Commands ***********************************************/
// Send Menu
void displayMenu(void) {
	
	transmitStringUART("\n==========   SOC3050 IoT   ==========\n");
	transmitStringUART("\nSelect an option by typing the command:\n");
	transmitStringUART("1. light0/1 - Turn Light OFF/ON\n");
	transmitStringUART("2. fan0/1 - Turn Fan OFF/ON\n");
	transmitStringUART("3. servo0~servo180 - Set Servo Angle\n");
	transmitStringUART("4. stepper[-X to X] - Move Stepper\n");
	transmitStringUART("5. dc0~dc255 - Set DC Motor Speed\n");
	transmitStringUART("6. Joystick control\n");
	transmitStringUART("7. Ultrasonic control\n");
	transmitStringUART("8. Start game (Type 'exit' or 'quit' to return to menu)\n");
	transmitStringUART("Enter your choice: ");
	transmitStringUART("\n=====================================\n");
}
	

// Process UART command
void processSerialCommand(const char* command) {
	char buffer[100];

	// Light control
	if (strncmp(command, "light", 5) == 0) {
		illuminationStatus = atoi(command + 5);
		lcd_string(2, 0, illuminationStatus ? "Lights: ON " : "Lights: OFF");
		sprintf(buffer, "Light %s\n", illuminationStatus ? "turned on" : "turned off");
	}
	// Fan control
	else if (strncmp(command, "fan", 3) == 0) {
		ventilationState = atoi(command + 3);
		lcd_string(2, 12, ventilationState ? "Fan: ON " : "Fan: OFF");
		sprintf(buffer, "Fan %s\n", ventilationState ? "turned on" : "turned off");
	}
	// Stepper motor control
	else if (strncmp(command, "stepper", 7) == 0) {
		int steps = atoi(command + 7);
		moveStepperMotor(steps, 100);
		sprintf(buffer, "Stepper motor moved %d steps\n", steps);
	}
	// DC motor control
	else if (strncmp(command, "dc", 2) == 0) {
		int speed = atoi(command + 2);
		if (speed >= 0 && speed <= 255) {
			OCR0 = speed; // Set DC motor speed
			sprintf(buffer, "DC Motor speed set to %d\n", speed);
			} else {
			strcpy(buffer, "Invalid DC speed. Enter 'dc' followed by speed 0-255.\n");
		}
	}
	// Servo motor control
	else if (strncmp(command, "servo", 5) == 0) {
		int angle = atoi(command + 5);
		if (angle >= 0 && angle <= 180) {
			setServoMotorAngle(angle);
			sprintf(buffer, "Servo angle set to %d degrees\n", angle);
			} else {
			strcpy(buffer, "Invalid servo angle. Please type 'servo' followed by angle 0-180.\n");
		}
	}  else if (strcmp(command, "8") == 0) { // Game mode activation
		playMode = 1;
		transmitStringUART("Prediction game mode activated. Enter a number:\n");
		return;
	} else if (strcmp(command, "9") == 0) {
		 // Select Averaging Algorithm
		chosenAlgorithm = ALG_AVERAGE;
		transmitStringUART("Averaging algorithm selected.\n");
		return;
	} else if (strcmp(command, "10") == 0) {
		// Select Linear Regression Algorithm
		chosenAlgorithm = ALG_LINEAR_REGRESSION;
		transmitStringUART("Linear regression algorithm selected.\n");
		return;
	} else {
			strcpy(buffer, "Invalid command. Please try again.\n");
		}

	transmitStringUART(buffer);
	_delay_ms(2000);
	displayMenu();
}

/************* Monitor ***********************************************/
// Function to display initial information on the LCD
void showStartupInfo(void) {
	lcd_clear();
	lcd_string(0, 0, "===== Smart Room ====");
	lcd_string(1, 0, "   ATmega128 System  ");
	lcd_string(2, 0, "Lights: OFF Fan: OFF");
	lcd_string(3, 0, "Door: Closed");
	// Prepare line for joystick info
	lcd_string(4, 0, "x=--, y=--, sw=-");
}

/************* ADC ***********************************************/
// Initialize ADC with interrupt for Potentiometer (Temperature)
void initializeADC0Interrupt(void) {
	ADMUX = (1<<REFS0) | (1<<ADLAR) | (0<<MUX0);
	ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0) | (1<<ADIE);
	ADCSRA |= (1<<ADSC);  // Start the first conversion
}


// ADC Conversion Complete Interrupt Service Routine
// Temperature, Joystick
ISR(ADC_vect) {
	static uint8_t adc_channel = 0;
	uint16_t adc_result = ADC;
	char buf[40];
	switch (adc_channel) {
		case 0: // Temperature
		sprintf(buf, "temperature: %d", convertToTemperature(adc_result));
		lcd_string(4, 0, buf);
		ADMUX = (ADMUX & 0xF8) | 3; 
		adc_channel = 3;
		break;

		case 3: // X position
		joystickXCoord = adc_result * 5 / 1023 * 2;
		// Switch to ADC4 (Y position)
		ADMUX = (ADMUX & 0xF8) | 4;
		adc_channel = 4;
		break;

		case 4: // Y position
		joystickYCoord = adc_result * 5 / 1023;
		// Update joystick info on LCD
		char joystickInfo[40];
		sprintf(joystickInfo, "x=%d, y=%d, sw=%d", joystickXCoord, joystickYCoord, joystickSwitchStatus);
		lcd_string(5, 0, joystickInfo);
		// Switch back to ADC0 (temperature)
		ADMUX = (ADMUX & 0xF8);
		adc_channel = 0;
		break;
	}

	// Trigger next conversion
	ADCSRA |= (1<<ADSC);
}


// Convert ADC value to temperature
int convertToTemperature(uint16_t adc_value) {
	return (int)(adc_value * 40.0 / 255.0);  // Example conversion
}

/************* Stepper Motor ***********************************************/
void executeSoftwareDelay(int duration) {
	for (int i = 0; i < duration; i++) {
		_delay_ms(1); // 1 ms delay
	}
}
void moveStepperMotor(int c_steps, int delay) {
	// Assuming a full-step sequence
	uint8_t steps[] = {0x66, 0xCC, 0x99, 0x33};
	int num_steps = sizeof(steps) / sizeof(steps[0]);
	int step_index = 0;

	for (int i = 0; i < abs(c_steps); i++) {
		PORTB = steps[step_index];
		executeSoftwareDelay(delay);

		// Increment or decrement the step index based on direction
		if (c_steps > 0) {
			step_index = (step_index + 1) % num_steps;
			} else {
			step_index = (step_index - 1 + num_steps) % num_steps;
		}
	}
}

/************* DC Motor ***********************************************/
void initializeDCMotor() {
	// Set the motor control pin as output
	DDRB |= (1<<DC_MOTOR_PWM); //OC0

	// Initialize PWM for motor speed control (using Timer 0)
	TCCR0 = (1<<WGM00) | (1<<WGM01) | (1<<COM01) | (1<<CS00); // Fast PWM mode, no prescaling
	OCR0 = 0; // Start with motor stopped
}

/************* Servo Motor ***********************************************/
void initializeServoMotor() {
	// Set the servo control pin as output
	DDRB |= (1<<SERVO_PWM);

	// Initialize Timer 1 for PWM
	ICR1 = F_CPU / (PRESCALER * PWM_FREQUENCY) - 1; // Setting TOP for a 20ms period
	TCCR1A = (1<<COM1A1) | (1<<WGM11); // Non-inverting mode, Mode 14 (Fast PWM)
	TCCR1B = (1<<WGM13) | (1<<WGM12) | (1<<CS11); // Prescaler set to 8
}

void setServoMotorAngle(int angle) {
	servoMotorPosition = angle;

	// Constrain the angle to the valid range
	if (servoMotorPosition > 180) servoMotorPosition = 180;
	if (servoMotorPosition < 0) servoMotorPosition = 0;

	// Map the angle to a pulse width between 1ms (1000) and 2ms (2000)
	// ICR1 is set to 19999 for a 20ms period, so the pulse width should be a fraction of this
	uint16_t min_pulse_ticks = ICR1 / 20; // Minimum pulse width (1ms)
	uint16_t max_pulse_ticks = ICR1 / 10; // Maximum pulse width (2ms)
	uint16_t pulse_width_ticks = ((servoMotorPosition / 180.0) * (max_pulse_ticks - min_pulse_ticks)) + min_pulse_ticks;

	OCR1A = pulse_width_ticks; // Set pulse width for OC1A
}

/************* Distance Meter ***********************************************/
//Ultrasonic sensor SR04 Trigger signal: Timer2 PWM at OC2(PB7), Echo: INT1(PD1)
//Timer2 setup for generating trigger signal
void setupTimer2() {
	// Configure Timer2 in Fast PWM mode
	TCCR2 = (1 << WGM21) | (1 << WGM20); // Fast PWM mode
	TCCR2 |= (1 << COM21); // Non-inverting mode

	// Set prescaler to 8 (assuming F_CPU is 16MHz)
	// PWM frequency = F_CPU / (prescaler * 256) = ~7.8 kHz
	TCCR2 |= (1 << CS21);

	// Set OC2 (PB7) as output
	DDRD |= (1 << PB7);

	// Set the compare value for a 10-microsecond pulse
	// Pulse width = (OCR2 + 1) * prescaler * (1 / F_CPU)
	// Example: for F_CPU = 16MHz, prescaler = 8, a value of 19 gives ~10 microseconds
	OCR2 = 19;
}

// INT1 setup for receiving Echo
void setupINT1(void){
	 // Configure PD1 as input for echo (INT1)
	 DDRD &= ~(1 << PD1);
	 PORTD |= (1 << PD1); // Enable pull-up

	 // External interrupt setup for INT1 (PD1)
	 EICRA |= (1 << ISC10); // Trigger on any logic change for INT1
	 EIMSK |= (1 << INT1);  // Enable INT1

	 sei(); // Enable global interrupts

}

ISR(INT1_vect) {
	if (measuring == 0) {
		// Rising edge detected, start timer
		pulse_start = TCNT2; // Assuming Timer2 is configured as a timer
		measuring = 1;
		} else {
		// Falling edge detected, stop timer and calculate distance
		pulse_end = TCNT2;
		distance = (pulse_end - pulse_start) * 0.034 / 2;
		measuring = 0;

		// Update LCD with distance info
		char buffer[16];
		sprintf(buffer, "Dist: %u cm", distance);
		lcd_string(6, 0, buffer); // Update the specific line on LCD
	}
}

/************* Joystick Switch ***********************************************/
//INT0 Joystick switch
void init_interrupts(void) {
	// Configure PD0 as input
	DDRD &= ~(1 << PD0);
	PORTD |= (1 << PD0);  // Enable pull-up resistor

	// Configure INT0 to trigger on falling edge (active low switch)
	EICRA |= (1 << ISC01);
	EICRA &= ~(1 << ISC00);

	// Enable INT0 interrupt
	EIMSK |= (1 << INT0);

	// Enable global interrupts
	sei();
}

// ISR for INT0 - Joystick Switch
ISR(INT0_vect) {
	// Basic debouncing
	_delay_ms(50);

	// Read the switch state
	// Assuming the switch is active low (pressed = low)
	joystickSwitchStatus = (PIND & (1 << PD0)) ? 0 : 1;

	// Update joystick info on LCD
	char joystickInfo[40];
	sprintf(joystickInfo, "x=%d, y=%d, sw=%d", joystickXCoord, joystickYCoord, joystickSwitchStatus);
	lcd_string(4, 0, joystickInfo);
}

/************* Game ***********************************************/
// Prediction algorithm
// Global Variables
#define NUM_HISTORY 5
int numberHistory[NUM_HISTORY];
int historyIndex = 0;

// Function to add number to history and compute moving average
int forecastNextValue(int newNumber) {
	// Add new number to history
	numberHistory[historyIndex] = newNumber;
	historyIndex = (historyIndex + 1) % NUM_HISTORY;

	// Compute moving average
	int sum = 0;
	for (int i = 0; i < NUM_HISTORY; i++) {
		sum += numberHistory[i];
	}
	return sum / NUM_HISTORY;
}
#endif