#include "def.h"
#include "_glcd.h"

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
                        // Alien or UFO destroyed
                        alien_ufo_positions[j].x = -20; // Move off-screen
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


void init_graphics(void)
{
    for (int i = 0; i < STAR_COUNT; i++)
    {
        star_positions[i][0] = 5 + rand() % 60;
        star_positions[i][1] = 5 + rand() % 116;
    }

    for (int i = 0; i < ALIEN_UFO_COUNT; i++)
    {
        alien_ufo_positions[i].x = rand() % 60;
        alien_ufo_positions[i].y = 5 + (i * 10);
        alien_ufo_positions[i].speed = (i % 2 == 0) ? 1 : -1; 
        alien_ufo_positions[i].health = (i % 2 == 0) ? 3 : 5; // UFOs have 2 health, Aliens have 3 health
    }

    GLCD_DrawImageWithRotation(x_position, y_position, spaceShip16n16, 16, 16, ROTATE_90);
    initialize_bullets();
}


void handle_movement(void)
{
    render_spaceship();
    render_stars();
    render_aliens_and_ufos();
    render_bullets();

    _delay_ms(125);
}