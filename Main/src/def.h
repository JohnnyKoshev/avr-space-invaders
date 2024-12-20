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

unsigned char alien32n32[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x01, 0xc0, 0x03, 0x80, 0x01, 0xc0, 0x03, 0x80, 0x01, 0xc0,
    0x00, 0x70, 0x06, 0x00, 0x00, 0x70, 0x06, 0x00, 0x00, 0xf0, 0x0f, 0x00, 0x03, 0xff, 0xff, 0xc0,
    0x03, 0xff, 0xff, 0xc0, 0x07, 0xff, 0xff, 0xe0, 0x1f, 0x8f, 0xf9, 0xf8, 0x1f, 0x8f, 0xf9, 0xf8,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xe7,
    0xe3, 0xff, 0xff, 0xc7, 0xe3, 0xff, 0xff, 0xc7, 0xe3, 0x80, 0x01, 0xc7, 0xe3, 0x80, 0x01, 0xc7,
    0xe3, 0x80, 0x01, 0xc7, 0x00, 0x7c, 0x3e, 0x00, 0x00, 0x7e, 0x3e, 0x00, 0x00, 0x7e, 0x3e, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

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

#define MAX_BULLETS 3

typedef struct
{
    int x;
    int y;
    int active;
} Bullet;

Bullet bullets[MAX_BULLETS];

#define ALIEN_UFO_COUNT 8
typedef struct
{
    int x;
    int y;
    int speed;
    int health;
} AlienUFO;

AlienUFO alien_ufo_positions[ALIEN_UFO_COUNT];

int spaceship_health = 3;

int GAME_STATE = -1;

#define LIGHT_DETECTION_THRESHOLD 512
#define BONUS_STAR_X 30
#define BONUS_STAR_Y 60
#define DEBOUNCE_DELAY 500
#define BONUS_STAR_SPEED 1

static int bonus_star_x = 0;
static int bonus_star_y = 0;
static int bonus_star_active = 0;

unsigned int Count_Of_Timer2 = 0;
unsigned int Task1_Of_Timer2 = 0;
unsigned int Time_Of_Timer2 = 500;

volatile uint16_t game_time_seconds = 0;
volatile uint16_t count_of_timer2 = 0;
volatile uint16_t time_of_timer2 = 1000;

int enemies_destroyed = 0;
int time_spent = 0;
int bonus_stars_collected = 0;
