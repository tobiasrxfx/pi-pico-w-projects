#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define LED_PIN 12            // Pino do LED conectado

void setup_pwm(void);

void execute_pwm_led(volatile bool *running);