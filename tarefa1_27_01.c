// Bibliotecas necessárias
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/timer.h"
#include "ws2812.pio.h"

// Definindo os pinos utilizados
#define RED_LED 13
#define LED_MATRIX 7
#define AButton 5
#define BButton 6

// Definindo a matriz de leds
#define NLED 25
static uint32_t LEDS[NLED];

uint number = 0; // Número a ser mostrado no display
uint32_t last_time = 0; // Tempo do último evento (us)

// Números a serem mostrados
static uint32_t numbers[10][NLED] = {
    {0, 1, 1, 1, 0,  0, 1, 0, 1, 0,  0, 1, 0, 1, 0,  0, 1, 0, 1, 0,  0, 1, 1, 1, 0}, // 0
    {0, 1, 1, 1, 0,  0, 0, 1, 0, 0,  0, 0, 1, 0, 0,  0, 1, 1, 0, 0,  0, 0, 1, 0, 0}, // 1
    {0, 1, 1, 1, 0,  0, 1, 0, 0, 0,  0, 1, 1, 0, 0,  0, 0, 0, 1, 0,  0, 1, 1, 1, 0}, // 2
    {0, 1, 1, 1, 0,  0, 0, 0, 1, 0,  0, 1, 1, 1, 0,  0, 0, 0, 1, 0,  0, 1, 1, 1, 0}, // 3
    {0, 1, 0, 0, 0,  0, 0, 0, 1, 0,  0, 1, 1, 1, 0,  0, 1, 0, 1, 0,  0, 1, 0, 1, 0}, // 4
    {0, 1, 1, 1, 0,  0, 0, 0, 1, 0,  0, 0, 1, 1, 0,  0, 1, 0, 0, 0,  0, 1, 1, 1, 0}, // 5
    {0, 1, 1, 1, 0,  0, 1, 0, 1, 0,  0, 1, 1, 1, 0,  0, 1, 0, 0, 0,  0, 1, 1, 0, 0}, // 6
    {0, 0, 1, 0, 0,  0, 0, 1, 0, 0,  0, 0, 1, 0, 0,  0, 0, 0, 1, 0,  0, 1, 1, 1, 0}, // 7
    {0, 1, 1, 1, 0,  0, 1, 0, 1, 0,  0, 1, 1, 1, 0,  0, 1, 0, 1, 0,  0, 1, 1, 1, 0}, // 8
    {0, 0, 1, 1, 0,  0, 0, 0, 1, 0,  0, 1, 1, 1, 0,  0, 1, 0, 1, 0,  0, 1, 1, 1, 0}  // 9
};


// Protótipos das funções
void init_gpio(uint gpio, uint dir);
void red_blink(uint gpio, uint freq_ms);
static inline uint32_t color(uint8_t r, uint8_t g, uint8_t b);
static inline void put_pixel(uint32_t pixel_grb);
void display_number(uint8_t num);
void display_handler(uint gpio, uint32_t events);


int main() {
    stdio_init_all();

    // Inicia o led vermelho e os botões
    init_gpio(RED_LED, GPIO_OUT);
    init_gpio(AButton, GPIO_IN);
    init_gpio(BButton, GPIO_IN);

    // Coloca os botões em pull up
    gpio_pull_up(AButton);
    gpio_pull_up(BButton);

    // Seleciona o PIO e a state machine
    PIO pio = pio0;
    uint sm = 0;

    // Configura a máquina de estados PIO para começar a controlar os LEDs WS2812.
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, LED_MATRIX, 800000, false);

    // Seta a função de callback quando os botões a e b forem pressionados
    gpio_set_irq_enabled_with_callback(AButton, GPIO_IRQ_EDGE_FALL, true, &display_handler);
    gpio_set_irq_enabled_with_callback(BButton, GPIO_IRQ_EDGE_FALL, true, &display_handler);

    while (true) {
        display_number(number);
        red_blink(RED_LED, 100);
    }
}


// Inicia e seta se o pino é entrada ou saida
void init_gpio(uint gpio, uint dir){
    gpio_init(gpio);
    gpio_set_dir(gpio, dir);
}


// Função que pisca o LED vermelho 5 vezes por segundo
void red_blink(uint gpio, uint freq_ms) {
    gpio_put(gpio, 1);
    sleep_ms(freq_ms);  // Liga o led por x ms
    gpio_put(gpio, 0);
    sleep_ms(freq_ms);  

}


// Função para mostrar o numero na matriz
void display_number(uint8_t num) {
    uint32_t purple = color(5, 0, 16);

    // Segue a matriz numbers como referência para plotar os leds na matriz
    for (uint j = 0; j < NLED; j++) {
        if (numbers[num][j]) put_pixel(purple);
        else put_pixel(0);
    }
}


// Função para colocar a cor do LED  na matriz
static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb);
}


// Função para representar a cor em formato RGB
static inline uint32_t color(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)(g) << 24) | ((uint32_t)(r) << 16) | ((uint32_t)(b) << 8);
}


// Função para alterar o display
void display_handler(uint gpio, uint32_t events) {
    uint32_t current_time = to_us_since_boot(get_absolute_time());

    // Verifica se passou 250 ms depois do ultimo evento
    if(current_time - last_time > 250000) {
        last_time = current_time;

        // Adiciona + 1, se o botão A for precionado e o numero mostrado for diferente de 9
        if (gpio == AButton && number != 9 )
            number += 1;

        // Subtrai - 1, se o botão B for precionado e o numero mostrado for diferente de 0
        if (gpio == BButton && number != 0)
            number -= 1;
    }

}

