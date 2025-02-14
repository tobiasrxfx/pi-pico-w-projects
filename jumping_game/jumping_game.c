#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "inc/ssd1306.h"
#include "jumping_game.h"


// Create display struct for configuration 
ssd1306_t disp;

int main()
{
    stdio_init_all();

    // Init ADC 
    adc_init();
    adc_gpio_init(JOY_Y);

    // Init GPIO for the joystick button
    gpio_init(BTN_JOY_PIN);
    gpio_set_dir(BTN_JOY_PIN, GPIO_IN);
    gpio_pull_up(BTN_JOY_PIN);

    // Init SSD1603 OLED display 
    setup_gpios_ssd1603();
    ssd1306_init(&disp, 128, 64, 0x3C, i2c1);
    ssd1306_clear(&disp);
    disp.external_vcc=false;

    Player player = {10, GROUND_Y, 10, 10, 0, false};
    Obstacle obstacle_0 = {100, GROUND_Y, 6, 3, 4};
    Obstacle obstacle_1 = {100, GROUND_Y+5, 3, 5, 1};
    int score_cnt = 0;
    bool jumped = false; 
    char score_string[15];

    while (true) {

        // Select JOY_Y as input 
        adc_select_input(0); 
        uint adc_y_raw = adc_read();

        //ssd1306_draw_string(&disp, 10+24 , 10, 1,"CRIADO POR");
        
        if (adc_y_raw > 3*ADC_MAX/4){
            jumped = jump(&player);
        }

        // Update game objects
        update_player(&player);
        update_obstacle(&obstacle_1);

        // Draw everything on OLED
        draw_game(&player, &obstacle_1);

        if(check_collision(&player, &obstacle_1)){
            ssd1306_draw_string(&disp, 50, 30 ,1, "GAME OVER");
            ssd1306_show(&disp);
            sleep_ms(5000);
            score_cnt = 0;
            ssd1306_clear(&disp); 
        }else if(jumped == true){
            score_cnt++;
            sprintf(score_string, "SCORE: %d", score_cnt);
            ssd1306_clear_square(&disp, 0,0,128, 30);
            ssd1306_draw_string(&disp, 40, 10 ,1, score_string);
            ssd1306_show(&disp);
        }
        
        //sleep_ms(20);
    }
}

void setup_gpios_ssd1603(void) {
    i2c_init(i2c1, 400000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}

void update_player(Player *player) {
    if (player->is_jumping) {
        player->y += player->velocity_y;
        player->velocity_y += GRAVITY; // Apply gravity

        // Prevent the player from falling through the ground
        if (player->y >= GROUND_Y) {
            player->y = GROUND_Y;
            player->is_jumping = false;
            player->velocity_y = 0;
        }
    }
}

// Function to make the player jump
bool jump(Player *player) {
    if (!player->is_jumping) {  // Only allow jumping if on the ground
        player->is_jumping = true;
        player->velocity_y = JUMP_STRENGTH;
        return true;
    }
    return false;
}



void update_obstacle(Obstacle *obstacle) {
    obstacle->x -= obstacle->speed;

    // Reset obstacle when it moves out of the screen
    if (obstacle->x + obstacle->width < 0) {
        obstacle->x = 128; // Reset to the right side of the screen
    }
}



void draw_game(Player *player, Obstacle *obstacle) {

    ssd1306_clear_square(&disp, 0, 0, 20, 30);
    ssd1306_clear_square(&disp, 0, 20, 128, 44);

    // Draw player
    ssd1306_draw_square(&disp, player->x, player->y, player->width, player->height);

    // Draw obstacle
    ssd1306_draw_square(&disp, obstacle->x, obstacle->y, obstacle->width, obstacle->height);
    

    ssd1306_show(&disp);
}

// Axis-Aligned Bounding Box (AABB) collision detection
bool check_collision(Player *player, Obstacle *obstacle) {
    return (player->x < obstacle->x + obstacle->width &&  // Player's left edge is before obstacle's right edge
            player->x + player->width > obstacle->x &&    // Player's right edge is after obstacle's left edge
            player->y < obstacle->y + obstacle->height && // Player's top edge is above obstacle's bottom edge
            player->y + player->height > obstacle->y);    // Player's bottom edge is below obstacle's top edge
}