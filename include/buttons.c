#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <stdio.h>

#include "buttons.h"
#include "config.h"
#include "debouncer.h"

// Variáveis voláteis para sinalizar o estado dos botões
static volatile bool btn_a_pressed = false;
static volatile bool btn_b_pressed = false;
static volatile bool btn_joystick_pressed = false;

// Variáveis de tempo para o debounce
static uint32_t last_press_time_A = 0;
static uint32_t last_press_time_B = 0;
static uint32_t last_press_time_JOY = 0;

// Variável para o lockout do botão A, se necessário
static absolute_time_t button_a_lockout_until;

static void gpio_callback(uint gpio, uint32_t events) {
    if (gpio == BUTTON_A_PIN) {
        if (check_debounce(&last_press_time_A, BUTTON_DEBOUNCE_US)) {
            btn_a_pressed = true;
        }
    } else if (gpio == BUTTON_B_PIN) {
        if (check_debounce(&last_press_time_B, BUTTON_DEBOUNCE_US)) {
            btn_b_pressed = true;
        }
    } else if (gpio == BUTTON_JOYSTICK_PIN) {
        if (check_debounce(&last_press_time_JOY, BUTTON_DEBOUNCE_US)) {
            btn_joystick_pressed = true;
        }
    }
}

void buttons_init() {
    // Inicializa a trava como já "vencida"
    button_a_lockout_until = get_absolute_time();
    
    // Configuração dos pinos dos botões
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);
    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);
    gpio_init(BUTTON_JOYSTICK_PIN);
    gpio_set_dir(BUTTON_JOYSTICK_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_JOYSTICK_PIN);

    // Configura as interrupções para a borda de descida (pressionar o botão)
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(BUTTON_JOYSTICK_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
}

bool check_button_a_pressed() {
    if (btn_a_pressed) {
        btn_a_pressed = false;
        return true;
    }
    return false;
}

bool check_button_b_pressed() {
    if (btn_b_pressed) {
        btn_b_pressed = false;
        return true;
    }
    return false;
}

bool check_button_joystick_pressed() {
    if (btn_joystick_pressed) {
        btn_joystick_pressed = false;
        return true;
    }
    return false;
}