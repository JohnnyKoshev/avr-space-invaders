#include "config.h"
#include "render.c"
#include "init.c"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <string.h>

#define MAX_BUFFER_SIZE 1024
static int is_start_cmd_received = 0;
static int is_stop_cmd_received = 0;
static int last_cmd_was_stats = 0;
static int is_init_graphics = 0;
static int is_start_menu_shown = 0;

#define EEPROM_ADDR_ENEMIES_DESTROYED 0
#define EEPROM_ADDR_TIME_SPENT 2
#define EEPROM_ADDR_BONUS_COLLECTED 4
#define EEPROM_ADDR_SPACESHIP_HEALTH 6

void save_stats_to_eeprom()
{
	eeprom_update_word((uint16_t *)EEPROM_ADDR_ENEMIES_DESTROYED, enemies_destroyed);
	eeprom_update_word((uint16_t *)EEPROM_ADDR_TIME_SPENT, time_spent);
	eeprom_update_word((uint16_t *)EEPROM_ADDR_BONUS_COLLECTED, bonus_stars_collected);
	eeprom_update_word((uint16_t *)EEPROM_ADDR_SPACESHIP_HEALTH, spaceship_health);
}

void load_stats_from_eeprom()
{
	// Check if the EEPROM value for enemies destroyed is uninitialized
	enemies_destroyed = eeprom_read_word((uint16_t *)EEPROM_ADDR_ENEMIES_DESTROYED);
	if (enemies_destroyed == 0xFFFF)
	{
		enemies_destroyed = 0; // Assign default value
	}

	// Check if the EEPROM value for time spent is uninitialized
	time_spent = eeprom_read_word((uint16_t *)EEPROM_ADDR_TIME_SPENT);
	if (time_spent == 0xFFFF)
	{
		time_spent = 0; // Assign default value
	}

	// Check if the EEPROM value for bonus stars collected is uninitialized
	bonus_stars_collected = eeprom_read_word((uint16_t *)EEPROM_ADDR_BONUS_COLLECTED);
	if (bonus_stars_collected == 0xFFFF)
	{
		bonus_stars_collected = 0; // Assign default value
	}

	// Check if the EEPROM value for spaceship health is uninitialized
	spaceship_health = eeprom_read_word((uint16_t *)EEPROM_ADDR_SPACESHIP_HEALTH);
	if (spaceship_health == 0xFFFF)
	{
		spaceship_health = 3; // Assign default value (example: full health)
	}
}

void reset_microcontroller()
{
	save_stats_to_eeprom();
	wdt_enable(WDTO_15MS);
	while (1)
		;
}

void process_received_cmd(char *cmd)
{
	if (cmd == NULL || *cmd == '\0')
	{
		return;
	}

	if (strcmp(cmd, "1") == 0)
	{
		if (last_cmd_was_stats)
		{
			reset_microcontroller();
		}
		else
		{
			if (GAME_STATE == 1)
			{
				UART1_send_string("Game is already running. Please finish or stop the game to start a new game\n");
				return;
			}
			else if (GAME_STATE == -1)
			{
				UART1_send_string("Press Push Button to start the game.\n");
				return;
			}
			lcd_clear();
			ScreenBuffer_clear();
			UART1_send_string("Command received: 1. Start New Game\n");
			game_time_seconds = 0;
			enemies_destroyed = 0;
			bonus_stars_collected = 0;
			time_spent = 0;
			is_stop_cmd_received = 0;
			is_start_cmd_received = 1;
		}
	}
	else if (strcmp(cmd, "2") == 0)
	{
		UART1_send_string("Command received: 2. View Stats of the Previous Game\n");
		if (GAME_STATE == 1)
		{
			UART1_send_string("Game is still running. Please stop the game to view stats\n");
		}
		else
		{
			char enemies_destroyed_str[20];
			sprintf(enemies_destroyed_str, "Enemies Destroyed: %d", enemies_destroyed);

			char time_spent_str[20];
			sprintf(time_spent_str, "Time Spent: %ds", time_spent);

			char bonus_stars_collected_str[20];
			if (bonus_stars_collected < 0)
			{
				bonus_stars_collected = 0;
			}
			sprintf(bonus_stars_collected_str, "Bonus Gained: %d", bonus_stars_collected);

			char spaceship_health_str[20];
			if (spaceship_health < 0)
			{
				spaceship_health = 0;
			}
			sprintf(spaceship_health_str, "Health: %d", spaceship_health);

			UART1_send_string(enemies_destroyed_str);
			UART1_transmit('\n');
			UART1_send_string(time_spent_str);
			UART1_transmit('\n');
			UART1_send_string(bonus_stars_collected_str);
			UART1_transmit('\n');
			UART1_send_string(spaceship_health_str);
			UART1_transmit('\n');

			lcd_clear();
			ScreenBuffer_clear();

			lcd_string(0, 0, "PREVIOUS GAME STATS:");
			lcd_string(2, 0, enemies_destroyed_str);
			lcd_string(3, 0, time_spent_str);
			lcd_string(4, 0, bonus_stars_collected_str);
			lcd_string(5, 0, spaceship_health_str);

			last_cmd_was_stats = 1;
		}
	}
	else if (strcmp(cmd, "3") == 0)
	{
		if (GAME_STATE == 0 || GAME_STATE == -1)
		{
			UART1_send_string("The game is not running.\n");
			return;
		}
		lcd_clear();
		ScreenBuffer_clear();
		UART1_send_string("Command received: 3. Quit Game\n");
		GAME_STATE = 0;
		is_start_cmd_received = 0;
		is_stop_cmd_received = 1;
	}
	else
	{
		UART1_send_string("Unknown command received\n");
	}
}

ISR(USART1_RX_vect)
{
	static char buffer[MAX_BUFFER_SIZE];
	static unsigned char index = 0;
	unsigned char received_data = UDR1;

	if (received_data == '\r' || received_data == '\n')
	{
		buffer[index] = '\0';
		process_received_cmd(buffer);
		index = 0;
	}
	else if (received_data >= 32 && received_data <= 126)
	{
		if (index < MAX_BUFFER_SIZE - 1)
		{
			buffer[index++] = received_data;
		}
		else
		{
			index = 0;
		}
	}
	else
	{
		return;
	}
}

SIGNAL(TIMER2_OVF_vect)
{
	TCNT2 = 0x1A;
	count_of_timer2++;

	if (count_of_timer2 >= time_of_timer2)
	{
		count_of_timer2 = 0;

		if (GAME_STATE == 1)
		{
			game_time_seconds++;
			time_spent = game_time_seconds;

			if (game_time_seconds >= 60)
			{
				GAME_STATE = 0;
				lcd_clear();
				ScreenBuffer_clear();
				save_stats_to_eeprom();
				lcd_string(4, 1, "TIME UP! GAME OVER");
				if (spaceship_health < 0)
				{
					spaceship_health = 0;
				}
				turn_off_all_leds();
			}
		}
	}
}

void main_loop(void)
{
	UART1_send_string("\nGame started!\n1. Start New Game\n2. View Stats of the Previous Game\n3. Stop Game\n");
	init_graphics();
	while (1)
	{
		if (GAME_STATE == -1)
		{
			if (is_start_menu_shown == 0)
			{
				lcd_clear();
				ScreenBuffer_clear();
				lcd_string(1, 3, "SPACE INVADERS");
				GLCD_DrawImageWithRotation(20, 46, alien32n32, 32, 32, ROTATE_90);
				lcd_string(7, 1, "Press Push Button");
				is_start_menu_shown = 1;
			}
			if (is_joystick_button_pressed())
			{
				GAME_STATE = 1;
			}
		}
		else if (GAME_STATE == 1)
		{
			if (is_init_graphics == 0)
			{
				init_graphics();
				is_init_graphics = 1;
			}
			if (is_joystick_button_pressed())
			{
				fire_bullet();
			}

			handle_movement();
		}
		else
		{
			if (is_stop_cmd_received == 1)
			{
				is_stop_cmd_received = 0;
				lcd_clear();
				ScreenBuffer_clear();
				turn_off_all_leds();
				save_stats_to_eeprom();
				lcd_string(4, 1, "YOU QUIT THE GAME!");
			}
			else if (is_start_cmd_received == 1)
			{
				UART1_send_string("Game started!\n1. Start New Game\n2. View Stats of the Previous Game\n3. Stop Game\n");
				GAME_STATE = -1;
				// init_graphics();

				is_start_cmd_received = 0;
			}
		}
	}
}

void clear_stats()
{
	// Clear enemies destroyed count
	eeprom_write_word((uint16_t *)EEPROM_ADDR_ENEMIES_DESTROYED, 0xFFFF); // Default uninitialized value

	// Clear time spent
	eeprom_write_word((uint16_t *)EEPROM_ADDR_TIME_SPENT, 0xFFFF); // Default uninitialized value

	// Clear bonus stars collected
	eeprom_write_word((uint16_t *)EEPROM_ADDR_BONUS_COLLECTED, 0xFFFF); // Default uninitialized value

	// Clear spaceship health
	eeprom_write_word((uint16_t *)EEPROM_ADDR_SPACESHIP_HEALTH, 0xFFFF); // Default uninitialized value
}

void main(void)
{
	initialize_devices();
	load_stats_from_eeprom();
	main_loop();
}
