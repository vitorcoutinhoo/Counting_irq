#include "pico/stdlib.h"

// Definindo o pino vermelho do led RGB
#define RED_LED 13


// Protótipos das funções
void init_gpio(uint gpio, uint dir);
void red_blink(uint gpio, uint freq_ms);


int main() {
    stdio_init_all();
    init_gpio(RED_LED, GPIO_OUT);

    while (true) {
        red_blink(RED_LED, 200);
    }
}

// Inicia e seta se o pino é entrada ou saida, coloca o pino em nivel baixo
void init_gpio(uint gpio, uint dir){
    gpio_init(gpio);
    gpio_set_dir(gpio, dir);
    gpio_put(gpio, 0);
}

// Função que pisca o LED vermelho 5 vezes por segundo
void red_blink(uint gpio, uint freq_ms){
    gpio_put(gpio, 1);
    sleep_ms(freq_ms);
    gpio_put(gpio, 0);
    sleep_ms(freq_ms);
}
