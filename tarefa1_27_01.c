// Bibliotecas necessárias
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "ws2812.pio.h"

// Definindo os pinos utilizados
#define RED_LED 13
#define LED_MATRIX 7

// Definindo a matriz de leds
#define NLED 25
static const uint32_t LEDS[NLED];


// Protótipos das funções
void init_gpio(uint gpio, uint dir);
void red_blink(uint gpio, uint freq_ms);


int main() {
    stdio_init_all();
    init_gpio(RED_LED, GPIO_OUT);

    PIO pio = pio0;
    uint sm = 0;

    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, LED_MATRIX, 800000, false);

    while (true) {
        red_blink(RED_LED, 100);
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
