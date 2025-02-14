#define JOY_Y 26
#define BTN_JOY_PIN 22 

#define ADC_MAX 4095

#define I2C_SDA 14
#define I2C_SCL 15


#define GRAVITY 1.5      // Gravity force
#define JUMP_STRENGTH -10 // Jump initial velocity
#define GROUND_Y 40    // Ground level (adjust based on OLED screen)

typedef struct {
    int x, y;
    int width, height;
    int velocity_y;
    bool is_jumping;
} Player;

typedef struct {
    int x, y;
    int width, height;
    int speed;
} Obstacle;

// Functions definition 
void setup_gpios_ssd1603(void);

void update_player(Player *player);
void jump(Player *player);
void update_obstacle(Obstacle *obstacle);
void reset_objects_positions(Player *player, Obstacle *obstacle);

void draw_game(Player *player, Obstacle *obstacle);
bool check_collision(Player *player, Obstacle *obstacle);

void intro_animation(void);
