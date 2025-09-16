#ifndef BUTTONS_H
#define BUTTONS_H

#include "pico/stdlib.h"

/**
 * @brief Inicializa os pinos dos botões e as interrupções GPIO.
 */
void buttons_init();

/**
 * @brief Verifica se o botão A foi pressionado.
 * @return true se o botão foi pressionado, false caso contrário.
 */
bool check_button_a_pressed();

/**
 * @brief Verifica se o botão B foi pressionado.
 * @return true se o botão foi pressionado, false caso contrário.
 */
bool check_button_b_pressed();

/**
 * @brief Verifica se o botão do joystick foi pressionado.
 * @return true se o botão foi pressionado, false caso contrário.
 */
bool check_button_joystick_pressed();

#endif // BUTTONS_H