// config.h

#ifndef CONFIG_H_
#define CONFIG_H_

// System clock frequency
#define F_CPU 16000000UL // 16 MHz

// Baud rate for UART
#define BAUD 9600

// AVR system libraries
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// Standard libraries (ensure compatibility with AVR)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// Project-specific module headers
#include "_main.h"
#include "_buzzer.h"
#include "_adc.h"
#include "_eeprom.h"
#include "_init.h"
#include "_interrupt.h"
#include "_port.h"
#include "_timer2.h"
#include "_uart.h"
#include "_glcd.h"


// Uncomment the app you want to compile
// #define ACCELEROMETER
// #define ADC_INTERRUPT
// #define ADC_INTERRUPT_UART_INTERRUPT
// #define ADC_INTERRUPT_UART_POLLING
// #define ADC_POLLING
// #define ADC_POLLING_UART_POLLING
// #define BLINK_PORT
// #define BLINK_PIN
// #define BLINK_ASM
// #define BLINK_ASM_MACRO
// #define BLINK_ASM_RANDOM
// #define BLINK_ASM_RANDOM_DELAY
// #define CDS
// #define GAME_HANGMAN
// #define GAME_OBSTACLE
// #define GAME_OBSTACLE_LEVEL
// #define GAME_PUZZLE
// #define GAME_WORD_PUZZLE
// #define GRAPHICS_BASICS
// #define GRAPHICS_MOVEMENT
// #define GRAPHICS_RANDOM
// #define GRAPHICS_BOUNCING_BALL
// #define GRAPHICS_MOVING_SQUARE
// #define GRAPHICS_SINE_WAVE
// #define INLINE
// #define INTERRUPT_EXTERNAL
// #define INTERRUPT_LAB
// #define INTERRUPT_TIMER
// #define INTERRUPT_TIMER_CTC
// #define IOT
// #define JOYSTICK
// #define MEMORY_EEPROM
// #define MEMORY_PROGRAM
// #define MOTORS_FULLSTEP
// #define MOTORS_FULLSTEP_INTERRUPT
// #define MOTORS_HALFSTEP
// #define MOTORS_HALFSTEP_INTERRUPT
// #define MOTORS_STEPPER_LAB
// #define MOTORS_PWM_FAST
// #define MOTORS_PWM_PHASECORRECT
// #define MOTORS_PWM_LAB
// #define MOTORS_SERVO
// #define MOTORS_SERVO_ADC
// #define MOTORS_SERVO_UART
// #define PORT_BLINKING
// #define PORT_ROTATION
// #define SERIAL_BUFFER
// #define SERIAL_GENERAL
// #define SERIAL_GENERAL_WORD
// #define SERIAL_GENERAL_WORD_INTERRUPT  // Uncomment to compile this app
// #define SERIAL_INTERRUPT
// #define SERIAL_INTERRUPT_LAB
// #define SERIAL_POLLING
// #define SERIAL_POLLING_SIMULIDE
// #define SERIAL_WORD
// #define SOUND
// #define SOUND_ATARI
// #define SOUND_TWINGKLE
// #define TIMER_COUNTER
// #define TIMER_CTC
// #define TIMER_FASTPWM
// #define TIMER_NORMAL

#endif /* CONFIG_H_ */
