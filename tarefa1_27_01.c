// Bibliotecas necessárias
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "ws2812.pio.h"

// Definindo os pinos utilizados
#define RED_LED 13
#define LED_MATRIX 7

// Definindo a matriz de leds
#define NLED 25
static uint32_t LEDS[NLED];


// Protótipos das funções
void init_gpio(uint gpio, uint dir);
void red_blink(uint gpio, uint freq_ms);
static inline uint32_t color(uint8_t r, uint8_t g, uint8_t b);
static inline void put_pixel(uint32_t pixel_grb);


int main() {
    stdio_init_all();
    init_gpio(RED_LED, GPIO_OUT);

    PIO pio = pio0;
    uint sm = 0;

    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, LED_MATRIX, 800000, false);

    uint32_t white = color(96, 96, 96);

    while (true) {
        LEDS[12] = white;
        for (int i = 0; i < NLED; i++)
            put_pixel(LEDS[i]);
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

// Função para colocar a cor do LED  na matriz
static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb);
}

// Função para representar a cor em formato RGB
static inline uint32_t color(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)(g) << 24) | ((uint32_t)(r) << 16) | ((uint32_t)(b) << 8);
}


