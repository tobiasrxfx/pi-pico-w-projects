#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "inc/ssd1306.h"
#include "jumping_game.h"
#include "hardware/pwm.h"

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

    int score_cnt = 0;
    char score_string[15];

    init_buzzer();

    Player player = {10, GROUND_Y, 10, 10, 0, false};
    Obstacle obstacle_0 = {100, GROUND_Y, 6, 3, 3};
    Obstacle obstacle_1 = {100, GROUND_Y+5, 3, 5, 2};
    
    // Waits for the user press the to start the game
    while(gpio_get(BTN_JOY_PIN)){
        ssd1306_draw_string(&disp, 17, 20 ,1, "PRESS THE BUTTON");
        ssd1306_draw_string(&disp, 15, 30 ,1, "TO PLAY THE GAME");
        ssd1306_show(&disp);
    }

    intro_animation();


    while (true) {

        
        // Select JOY_Y as input 
        adc_select_input(0); 
        uint adc_y_raw = adc_read();
        
        // Detect jump by taking the joystcick adc input
        if (adc_y_raw > 3*ADC_MAX/4){
            jump(&player);
        }

        // Update player and the active obstacle
        update_player(&player);

        update_obstacle(&obstacle_1);
        draw_game(&player, &obstacle_1);

        // Draw everything on OLED
        // draw_game(&player, &obstacle_1);
        
        if (check_collision(&player, &obstacle_1)){

            ssd1306_draw_string(&disp, 50, 30 ,1, "GAME OVER");
            ssd1306_show(&disp);
            defeat_sound();
            sleep_ms(2000);
            score_cnt = 0;
            ssd1306_clear(&disp); 
            ssd1306_show(&disp);
            
            while(gpio_get(BTN_JOY_PIN)){
                ssd1306_draw_string(&disp, 17, 20 ,1, "PRESS THE BUTTON");
                ssd1306_draw_string(&disp, 17, 30 ,1, " TO PLAY AGAIN");
                ssd1306_show(&disp);
            }

            reset_objects_positions(&player, &obstacle_1);
            reset_objects_positions(&player, &obstacle_0);
            intro_animation();
        }else if(obstacle_1.x == 128){
            score_cnt++;
            sprintf(score_string, "SCORE: %d", score_cnt);
            ssd1306_clear_square(&disp, 0,0,128, 30);
            ssd1306_draw_string(&disp, 40, 10 ,1, score_string);
            ssd1306_show(&disp);
        }

        
        //sleep_ms(20);
    }
}

void reset_objects_positions(Player *player, Obstacle *obstacle){
    player->x = 10;
    player->y = GROUND_Y;

    obstacle ->x = 100;
    obstacle ->y = GROUND_Y + 5;
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
void jump(Player *player) {
    if (!player->is_jumping) {  // Only allow jumping if on the ground
        player->is_jumping = true;
        player->velocity_y = JUMP_STRENGTH;
    }
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

void intro_animation(void){

    ssd1306_clear(&disp);
    ssd1306_draw_string(&disp, 30, 15 ,1, "STARTING IN");
    ssd1306_draw_string(&disp, 60, 30 ,2, "3");
    ssd1306_show(&disp);
    sleep_ms(750);
    ssd1306_clear_square(&disp, 60,30,16,10);
    ssd1306_draw_string(&disp, 60, 30 ,2, "2");
    ssd1306_show(&disp);
    sleep_ms(750);
    ssd1306_clear_square(&disp, 60,30,16,10);
    ssd1306_draw_string(&disp, 60, 30 ,2, "1");
    ssd1306_show(&disp);
    sleep_ms(750);
    ssd1306_clear_square(&disp, 55, 30, 32,20);
    ssd1306_draw_string(&disp, 55, 30 ,2, "GO");
    ssd1306_show(&disp);
    sleep_ms(750);

    ssd1306_clear(&disp);
    ssd1306_show(&disp);
}

void init_buzzer() {
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    pwm_set_clkdiv(slice_num, 4.0f); // Set clock divider
    pwm_set_enabled(slice_num, true); // Keep PWM enabled
}


void set_tone(uint16_t frequency) {
    if (frequency == 0) {
        pwm_set_gpio_level(BUZZER_PIN, 0);  // Silence
        return;
    }

    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    uint16_t wrap = 125000000 / frequency - 1;
    pwm_set_wrap(slice_num, wrap);
    pwm_set_gpio_level(BUZZER_PIN, wrap / 2);  // 50% duty cycle
}

void defeat_sound() {
    uint16_t tones[][2] = {
        {660, 200},  // E5
        {440, 200},  // A4
        {330, 400},  // E4
        {220, 600},  // A3 (deep note)
    };

    for (int i = 0; i < 4; i++) {
        set_tone(tones[i][0]);
        sleep_ms(tones[i][1]);
    }

    set_tone(0);  // Stop sound
}
