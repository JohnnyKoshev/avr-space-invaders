#include "def.h"

void DrawStar(unsigned char x, unsigned char y)
{
    GLCD_Line(x, y - 2, x, y + 2);
    GLCD_Line(x - 2, y, x + 2, y);
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

void update_spaceship_position(void)
{
    joystick_y = 127 - Read_Adc_Data(4) / 8;

    if (abs(joystick_y - 63) > JOYSTICK_DEAD_ZONE)
    {
        y_position += (joystick_y > 63) ? 2 : -2; // change speed by changing the increment value
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

void update_alien_ufo_positions(void)
{
    for (int i = 0; i < ALIEN_UFO_COUNT; i++)
    {
        alien_ufo_positions[i][1] += alien_ufo_positions[i][2]; // Update x position based on direction

        if (alien_ufo_positions[i][1] < 0 || alien_ufo_positions[i][1] > 116)
        {
            alien_ufo_positions[i][2] *= -1; // Change direction if it hits the boundary
            alien_ufo_positions[i][0] += 10; // Move down a row
        }

        if (alien_ufo_positions[i][0] > 60)
        {
            alien_ufo_positions[i][0] = 5; // Reset to top if it goes out of screen
        }
    }
}

void draw_aliens_and_ufos(void)
{
    for (int i = 0; i < ALIEN_UFO_COUNT; i++)
    {
        if (i % 2 == 0)
        {
            GLCD_DrawImageWithRotation(alien_ufo_positions[i][0], alien_ufo_positions[i][1], ufo16n16, 16, 16, ROTATE_90);
        }
        else
        {
            GLCD_DrawImageWithRotation(alien_ufo_positions[i][0], alien_ufo_positions[i][1], alien16n16, 16, 16, ROTATE_90);
        }
    }
}

void render_aliens_and_ufos(void)
{
    update_alien_ufo_positions();
    draw_aliens_and_ufos();
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

void handle_movement(void)
{
    render_spaceship();
    render_stars();
    render_aliens_and_ufos();

    _delay_ms(100);
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
        alien_ufo_positions[i][0] = rand() % 60;
        alien_ufo_positions[i][1] = 5 + (i * 10);
        alien_ufo_positions[i][2] = (i % 2 == 0) ? 1 : -1; // Initial direction
    }

    GLCD_DrawImageWithRotation(x_position, y_position, spaceShip16n16, 16, 16, ROTATE_90);
}