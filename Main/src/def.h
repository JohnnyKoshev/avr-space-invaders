#define JOYSTICK_DEAD_ZONE 10

unsigned char spaceShip16n16[] = {
    0x00, 0x00, 0x01, 0x80, 0x03, 0xc0, 0x06, 0x60, 0x0c, 0x30, 0x18, 0x18, 0x30, 0x0c, 0x20, 0x04,
    0x60, 0x06, 0x40, 0x02, 0x43, 0xc2, 0x46, 0x62, 0x5d, 0xba, 0x71, 0x8e, 0x01, 0x80, 0x00, 0x00};

unsigned char ufo16n16[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x7f, 0xfe, 0xff, 0xff, 0x7f, 0xfe,
    0x1f, 0xf8, 0x13, 0xc8, 0x10, 0x08, 0x10, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

unsigned char alien16n16[] = {
    0x00, 0x00, 0x00, 0x00, 0x10, 0x08, 0x0c, 0x30, 0x0c, 0x30, 0x1f, 0xf8, 0x33, 0xce, 0x33, 0xce,
    0xff, 0xff, 0xdf, 0xf9, 0xdf, 0xf9, 0xd0, 0x09, 0x0e, 0x70, 0x0e, 0x70, 0x00, 0x00, 0x00, 0x00};

int x_position = 48;
int y_position = 0;
unsigned int joystick_y;

#define STAR_MIN_DISTANCE 20
#define STAR_COUNT 5
int star_positions[STAR_COUNT][2];


int star_positions[STAR_COUNT][2];


#define JOYSTICK_BUTTON_PIN PIND2
#define JOYSTICK_BUTTON_PORT PORTD
#define JOYSTICK_BUTTON_DDR DDRD
#define JOYSTICK_BUTTON_PINR PIND

#define MAX_BULLETS 3 // Max number of bullets on screen at once

typedef struct
{
    int x;      // Bullet's x-position
    int y;      // Bullet's y-position
    int active; // Whether the bullet is active (0 for inactive, 1 for active)
} Bullet;

Bullet bullets[MAX_BULLETS]; 


#define ALIEN_UFO_COUNT 8
typedef struct {
    int x;
    int y;
    int speed;
    int health;
} AlienUFO;

AlienUFO alien_ufo_positions[ALIEN_UFO_COUNT];


int spaceship_health = 3;

int GAME_STATE = 1;

#define LIGHT_DETECTION_THRESHOLD 512  // Threshold for detecting light
#define BONUS_STAR_X 30  // X-position for bonus star (or randomize it)
#define BONUS_STAR_Y 60  // Y-position for bonus star (or randomize it)
#define DEBOUNCE_DELAY 500  // Delay in milliseconds for debounce

#define BONUS_STAR_SPEED 1   // Speed at which the bonus star falls

static int bonus_star_x = 0;  // Bonus star starting from the top (x = 0)
static int bonus_star_y = 0;  // Randomized starting y position for the bonus star
static int bonus_star_active = 0;  // Flag to track if the bonus star is active
