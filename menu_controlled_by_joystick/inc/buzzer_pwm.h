#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

#define BUZZER_PIN 21

void pwm_init_buzzer(uint pin); 

void play_tone(uint pin, uint frequency, uint duration_ms);

void play_star_wars(uint pin, volatile bool *running);