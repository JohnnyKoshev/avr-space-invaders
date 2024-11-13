#include "config.h"
#include "def.h"
#include "_glcd.h"
#include "led_utils.c"
#include "image_utils.c"

void display_lives_with_leds(void)
{
    turn_off_all_leds();
    uint8_t led_mask = (1 << spaceship_health) - 1;
    PORTB = ~led_mask;
}

void DrawStar(unsigned char x, unsigned char y)
{
    GLCD_Line(x, y - 2, x, y + 2);
    GLCD_Line(x - 2, y, x + 2, y);
}

void DrawBonusStar(unsigned char x, unsigned char y)
{
    DrawStar(x, y);
    GLCD_Circle(x, y, 2);
}

int are_stars_overlapping(int star1_x, int star1_y, int star2_x, int star2_y)
{

    int dx = star1_x - star2_x;
    int dy = star1_y - star2_y;
    int distance_squared = dx * dx + dy * dy;

    if (distance_squared < (STAR_MIN_DISTANCE * STAR_MIN_DISTANCE))
    {
        return 1;
    }
    return 0;
}

void update_star_positions(void)
{

    for (int i = 0; i < STAR_COUNT; i++)
    {

        star_positions[i][0] += 1;

        if (star_positions[i][0] > 60)
        {
            star_positions[i][0] = 5;
            star_positions[i][1] = 5 + (rand() % 116);
        }

        int overlap_found = 1;
        while (overlap_found)
        {
            overlap_found = 0;
            for (int j = 0; j < STAR_COUNT; j++)
            {
                if (i != j)
                {
                    if (are_stars_overlapping(star_positions[i][0], star_positions[i][1], star_positions[j][0], star_positions[j][1]))
                    {
                        star_positions[i][1] = 5 + (rand() % 116);
                        overlap_found = 1;
                        break;
                    }
                }
            }
        }
    }
}

void draw_stars(void)
{
    for (int i = 0; i < STAR_COUNT; i++)
    {

        DrawStar(star_positions[i][0], star_positions[i][1]);
    }
}

void update_alien_ufo_positions(void)
{
    for (int i = 0; i < ALIEN_UFO_COUNT; i++)
    {
        alien_ufo_positions[i].y += alien_ufo_positions[i].speed;

        if (alien_ufo_positions[i].y < 0 || alien_ufo_positions[i].y > 116)
        {
            alien_ufo_positions[i].speed *= -1;
            alien_ufo_positions[i].x += 10;
        }

        if (alien_ufo_positions[i].x > 60)
        {
            alien_ufo_positions[i].x = 5;
        }
    }
}

void draw_aliens_and_ufos(void)
{
    for (int i = 0; i < ALIEN_UFO_COUNT; i++)
    {
        if (alien_ufo_positions[i].health > 0)
        {
            if (i % 2 == 0)
            {
                GLCD_DrawImageWithRotation(alien_ufo_positions[i].x, alien_ufo_positions[i].y, ufo16n16, 16, 16, ROTATE_90);
            }
            else
            {
                GLCD_DrawImageWithRotation(alien_ufo_positions[i].x, alien_ufo_positions[i].y, alien16n16, 16, 16, ROTATE_90);
            }
        }
    }
}

void render_aliens_and_ufos(void)
{
    update_alien_ufo_positions();
    draw_aliens_and_ufos();
}

void update_spaceship_position(void)
{
    joystick_y = 127 - Read_Adc_Data(4) / 8;

    if (abs(joystick_y - 63) > JOYSTICK_DEAD_ZONE)
    {
        y_position += (joystick_y > 63) ? 2 : -2;
    }

    if (y_position > 112)
    {
        y_position = 112;
    }
    else if (y_position < 0)
    {
        y_position = 0;
    }
}

void draw_spaceship(void)
{
    lcd_clear();
    ScreenBuffer_clear();
    GLCD_DrawImageWithRotation(x_position, y_position, spaceShip16n16, 16, 16, ROTATE_90);
}

void render_spaceship(void)
{
    update_spaceship_position();
    draw_spaceship();
}

void render_stars(void)
{
    update_star_positions();
    draw_stars();
}

void initialize_bullets(void)
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        bullets[i].active = 0;
    }
}

void fire_bullet(void)
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!bullets[i].active)
        {
            bullets[i].y = y_position + 8;
            bullets[i].x = x_position;
            bullets[i].active = 1;
            break;
        }
    }
}

void update_bullets(void)
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (bullets[i].active)
        {
            bullets[i].x -= 2;
            if (bullets[i].x < 0)
            {
                bullets[i].active = 0;
            }

            for (int j = 0; j < ALIEN_UFO_COUNT; j++)
            {
                if (alien_ufo_positions[j].health > 0 &&
                    bullets[i].active &&
                    bullets[i].x >= alien_ufo_positions[j].x && bullets[i].x <= alien_ufo_positions[j].x + 16 &&
                    bullets[i].y >= alien_ufo_positions[j].y && bullets[i].y <= alien_ufo_positions[j].y + 16)
                {
                    alien_ufo_positions[j].health -= 1;
                    bullets[i].active = 0;

                    if (alien_ufo_positions[j].health <= 0)
                    {

                        alien_ufo_positions[j].x = -20;
                    }
                }
            }
        }
    }
}

void draw_bullets(void)
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (bullets[i].active)
        {
            GLCD_Line(bullets[i].x, bullets[i].y, bullets[i].x + 3, bullets[i].y);
        }
    }
}

void render_bullets(void)
{
    update_bullets();
    draw_bullets();
}

void check_collisions_with_spaceship(void)
{
    for (int i = 0; i < ALIEN_UFO_COUNT; i++)
    {
        if (alien_ufo_positions[i].health > 0 &&
            x_position + 15 >= alien_ufo_positions[i].x && x_position <= alien_ufo_positions[i].x + 15 &&
            y_position + 15 >= alien_ufo_positions[i].y && y_position <= alien_ufo_positions[i].y + 15)
        {
            if (i % 2 == 0)
            {
                spaceship_health -= 1;
            }
            else
            {
                spaceship_health -= 2;
            }

            alien_ufo_positions[i].health = 0;
            alien_ufo_positions[i].x = -20;

            if (spaceship_health <= 0)
            {
                GAME_STATE = 0;
                lcd_clear();
                ScreenBuffer_clear();

                lcd_string(30, 50, "GAME OVER");
            }

            display_lives_with_leds();
        }
    }
}

void init_graphics(void)
{
    for (int i = 0; i < STAR_COUNT; i++)
    {
        star_positions[i][0] = 5 + rand() % 60;
        star_positions[i][1] = 5 + rand() % 116;
    }

    for (int i = 0; i < ALIEN_UFO_COUNT; i++)
    {
        alien_ufo_positions[i].x = 0;
        alien_ufo_positions[i].y = 5 + (i * 16);
        alien_ufo_positions[i].speed = (i % 2 == 0) ? 1 : -2;
        alien_ufo_positions[i].health = (i % 2 == 0) ? 3 : 5;
    }

    GLCD_DrawImageWithRotation(x_position, y_position, spaceShip16n16, 16, 16, ROTATE_90);
    initialize_bullets();
    display_lives_with_leds();
}

int count_remaining_enemies(void)
{
    int count = 0;
    for (int i = 0; i < ALIEN_UFO_COUNT; i++)
    {
        if (alien_ufo_positions[i].health > 0)
        {
            count++;
        }
    }
    return count;
}

void detect_light_intensity(void)
{
    uint16_t light_value = Read_Adc_Data(2);

    if (light_value > LIGHT_DETECTION_THRESHOLD)
    {
        if (!bonus_star_active)
        {
            generate_bonus_star();
        }
    }
}

void generate_bonus_star(void)
{

    bonus_star_y = 5 + rand() % 116;

    bonus_star_x = 3;

    bonus_star_active = 1;
}

void update_bonus_star_position(void)
{
    if (bonus_star_active)
    {

        bonus_star_x += BONUS_STAR_SPEED;

        if (bonus_star_x > 64)
        {
            bonus_star_active = 0;
            bonus_star_x = 0;
            bonus_star_y = 0;
        }
    }
}

void check_bonus_star_collection(void)
{
    if (bonus_star_active &&
        x_position + 16 > bonus_star_x && x_position < bonus_star_x &&
        y_position + 16 > bonus_star_y && y_position < bonus_star_y)
    {

        if (spaceship_health < 8)
        {
            spaceship_health += 1;
        }
        bonus_star_active = 0;
        bonus_star_x = 0;
        bonus_star_y = 0;

        display_lives_with_leds();
    }
}

void draw_bonus_star(void)
{
    if (bonus_star_active)
    {
        DrawBonusStar(bonus_star_x, bonus_star_y);
    }
}

void render_bonus_star(void)
{
    check_bonus_star_collection();
    update_bonus_star_position();
    draw_bonus_star();
}

void display_remaining_enemies(void)
{
    int remaining_enemies = count_remaining_enemies();
    char enemy_count_str[20];
    sprintf(enemy_count_str, "Enemies: %d", remaining_enemies);
    lcd_string(0, 0, enemy_count_str);
}

void handle_movement(void)
{
    detect_light_intensity();

    render_spaceship();
    render_stars();
    render_aliens_and_ufos();
    render_bullets();
    render_bonus_star();

    display_remaining_enemies();

    check_collisions_with_spaceship();

    _delay_ms(125);
}
