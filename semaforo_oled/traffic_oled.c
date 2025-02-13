#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "inc/ssd1306.h"
#include "hardware/i2c.h"

// Define para identificação dos LEDs e Botão 
#define LED_R_PIN 13
#define LED_G_PIN 11
#define LED_B_PIN 12

#define BTN_A_PIN 5

// Declaração das funções 
int WaitWithRead(int timeMS);
void SinalFechado();
void SinalAtencao();
void SinalAberto();

// Instanciação das variaveis globais 
const uint I2C_SDA = 14;
const uint I2C_SCL = 15;

int A_state = 0;  //Botao A está pressionado?

uint8_t ssd[ssd1306_buffer_length];

// Strutura para preparar área de renderização para o display (ssd1306_width pixels por ssd1306_n_pages páginas)
struct render_area frame_area = {
    start_column : 0,
    end_column : ssd1306_width - 1,
    start_page : 0,
    end_page : ssd1306_n_pages - 1
};

int main()
{
    // Inicializa os tipos stdio padrão presentes ligados ao binário
    stdio_init_all(); 
    
    // Inicialização dos LEDs
    gpio_init(LED_R_PIN);
    gpio_set_dir(LED_R_PIN, GPIO_OUT);
    gpio_init(LED_G_PIN);
    gpio_set_dir(LED_G_PIN, GPIO_OUT);
    gpio_init(LED_B_PIN);
    gpio_set_dir(LED_B_PIN, GPIO_OUT);

    // Inicialização do botão
    gpio_init(BTN_A_PIN);
    gpio_set_dir(BTN_A_PIN, GPIO_IN);
    gpio_pull_up(BTN_A_PIN);

    // Inicialização do i2c
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Processo de inicialização completo do OLED SSD1306
    ssd1306_init();

    calculate_render_area_buffer_length(&frame_area);

    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);

restart:

    ssd1306_draw_line(ssd, 10, 35, 117, 35, true);
    render_on_display(ssd, &frame_area);


    while(true){

        SinalFechado(); // Sinal fechado (vermelho) para pedestres e aberto(verde) para carros
        A_state = WaitWithRead(8000); // Espera pedestre apertar o botão para que o sinal fique verde antes do tempo normal

        if(A_state){ // Alguém apertou o botão - semaforo muda para outro estado
            // Sinal fica amarelo para carros e pedestres por 5s
            SinalAtencao();
            sleep_ms(5000);

            // Sinal fica verde para pedestres e vermelho para carros 
            SinalAberto();
            sleep_ms(10000);

        }else{ // Ninguém apertou o botão - semaforo normal 
                                      
            SinalAtencao();
            sleep_ms(2000);

            // Sinal verde para pedestre e vermelho para carros por 8s
            SinalAberto();
            sleep_ms(8000);
        }
                
    }

    return 0;
}

// Logica de LEDs do sinal aberto e mensagens para tela OLED 
void SinalAberto(){
    gpio_put(LED_R_PIN, 0);
    gpio_put(LED_G_PIN, 1);
    gpio_put(LED_B_PIN, 0); 


    ssd1306_draw_string(ssd, 5, 25, "    ABERTO   ");
    ssd1306_draw_string(ssd, 5, 40, " ATRAVESSE COM");
    ssd1306_draw_string(ssd, 5, 50, "    CUIDADO    ");
   
    render_on_display(ssd, &frame_area);  
}

// Logica de LEDs do sinal atenção e mensagens para tela OLED 
void SinalAtencao(){
    gpio_put(LED_R_PIN, 1);
    gpio_put(LED_G_PIN, 1);
    gpio_put(LED_B_PIN, 0);

    ssd1306_draw_string(ssd, 5, 25, "    ATENCAO ");
    ssd1306_draw_string(ssd, 5, 40, "  PREPARE SE   ");
    ssd1306_draw_string(ssd, 5, 50, "                ");
    render_on_display(ssd, &frame_area);  
}

// Logica de LEDs do sinal fechado e mensagens para tela OLED 
void SinalFechado(){
    gpio_put(LED_R_PIN, 1);
    gpio_put(LED_G_PIN, 0);
    gpio_put(LED_B_PIN, 0);

    ssd1306_draw_string(ssd, 5, 20, "     SINAL   ");
    ssd1306_draw_string(ssd, 5, 25, "    FECHADO ");
    ssd1306_draw_string(ssd, 5, 40, "    AGUARDE     ");
    ssd1306_draw_string(ssd, 5, 50, "                ");
    render_on_display(ssd, &frame_area);  
}


// Leitura do botão A 
int WaitWithRead(int timeMS){
    for(int i = 0; i < timeMS; i = i+100){
        A_state = !gpio_get(BTN_A_PIN);
        if(A_state == 1){
            return 1;
        }
        sleep_ms(100);
    }
    return 0;
}