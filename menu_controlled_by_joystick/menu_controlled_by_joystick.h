#define JOY_Y 26
#define BTN_JOY_PIN 22 

#define ADC_MAX 4095

#define I2C_SDA 14
#define I2C_SCL 15

enum screens {screen_1, screen_2, screen_3};

// Functions definition 
void setup_gpios_ssd1603(void);
void button_callback(uint gpio, uint32_t events);
void intro_animation(void);
void base_screen(void);