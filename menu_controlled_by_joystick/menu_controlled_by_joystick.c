
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "inc/ssd1306.h"
#include "inc/buzzer_pwm.h"
#include "inc/pwm_led.h"
#include "inc/joystick_led.h"
#include "menu_controlled_by_joystick.h"

// Screen's enum
enum screens actual_screen; 

// Variable to act as a flag for the interrupt 
volatile bool running = false;

// Create display struct for configuration 
ssd1306_t disp;


int main() {
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
    disp.external_vcc=true;

    // Init buzzer
    pwm_init_buzzer(BUZZER_PIN);

    // Enable interruption by the joystick_button
    gpio_set_irq_enabled_with_callback(BTN_JOY_PIN, GPIO_IRQ_EDGE_FALL, true, &button_callback);

    intro_animation();

    base_screen();
    
    while (1) {

        // Select JOY_Y as input 
        adc_select_input(0); 
        uint adc_y_raw = adc_read();

        switch (actual_screen)
        {
            case screen_1:
                ssd1306_clear_square(&disp, 10, 15, 16, 27); 
                ssd1306_draw_string(&disp, 10 , 15, 1,"->");
                ssd1306_show(&disp);

                if (adc_y_raw < ADC_MAX/4){
                    actual_screen = screen_2;
                } else if (adc_y_raw > 3*ADC_MAX/4) {
                    actual_screen = screen_3;    
                } else {
                    actual_screen = screen_1;
                }

                if(running) {
                    execute_joystick_led(&running);
                    running = false;
                }
                break;
            case screen_2:
                ssd1306_clear_square(&disp, 10, 15, 16, 27);
                ssd1306_draw_string(&disp, 10 , 25, 1,"->");
                ssd1306_show(&disp);
                
                if (adc_y_raw < ADC_MAX/4){
                    actual_screen = screen_3;
                } else if (adc_y_raw > 3*ADC_MAX/4) {
                    actual_screen = screen_1;
                } else {
                    actual_screen = screen_2;
                } 

                if(running) {
                    play_star_wars(BUZZER_PIN, &running);
                    running = false;
                }
                break;
            case screen_3:
                ssd1306_clear_square(&disp, 10, 15, 16, 27);
                ssd1306_draw_string(&disp, 10 , 35, 1,"->");
                ssd1306_show(&disp);
                
                if (adc_y_raw < ADC_MAX/4){
                    actual_screen = screen_1;
                } else if (adc_y_raw > 3*ADC_MAX/4) {
                    actual_screen = screen_2;
                } else {
                    actual_screen = screen_3;
                }
                
                if(running) {
                    execute_pwm_led(&running);
                    running = false;
                }

                break;
            default:
                actual_screen = screen_1;
                break;
        }

        sleep_ms(175);

    }
}

void setup_gpios_ssd1603(void) {
    i2c_init(i2c1, 400000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}


// Interrupt handler function
void button_callback(uint gpio, uint32_t events) {
    

    // Anti-bouncing system
    static uint32_t last_press_time = 0;
    uint32_t current_time = to_ms_since_boot(get_absolute_time());

    if (current_time - last_press_time < 300)  // Ignore presses < 300ms apart
        return;

    last_press_time = current_time;  // Update last press time
    
    // Each time the button is pressed it change the state
    running = !running;
   
}


void intro_animation(void){

    for(int y=0; y<64; ++y) {
        ssd1306_draw_string(&disp, 10+24 , y, 1,"CRIADO POR");
        ssd1306_show(&disp);
        sleep_ms(5);
        ssd1306_clear(&disp);
    }

    ssd1306_draw_string(&disp, 10 , 25, 1,"TOBIAS");
    ssd1306_show(&disp);
    sleep_ms(600);
    ssd1306_clear(&disp);

    ssd1306_draw_string(&disp, 10 , 25, 2,"TOBIAS");
    ssd1306_show(&disp);
    sleep_ms(600);
    ssd1306_clear(&disp);

    ssd1306_draw_string(&disp, 10 , 25, 3,"TOBIAS");
    ssd1306_show(&disp);
    sleep_ms(600);
    ssd1306_clear(&disp);
}

void base_screen(void){
    
    ssd1306_draw_string(&disp, 46 , 2, 1,"MENU :)");
    ssd1306_draw_line(&disp, 0, 12, 127, 12);

    ssd1306_draw_string(&disp, 10 , 15, 1,"   Joystick LED ");
    ssd1306_draw_string(&disp, 10 , 25, 1,"   Buzzer PWM ");
    ssd1306_draw_string(&disp, 10 , 35, 1,"   PWM LED ");

    ssd1306_draw_line(&disp, 0, 44, 127, 44);
    ssd1306_draw_string(&disp, 10 , 47, 1,"Pressione o botao");
    ssd1306_draw_string(&disp, 10 , 55, 1,"para rodar/parar");
    ssd1306_show(&disp);
}