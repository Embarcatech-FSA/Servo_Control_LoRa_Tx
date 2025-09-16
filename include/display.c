#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "display.h"
#include "config.h"

/**
 * @brief Inicializa o objeto do display SSD1306.
 * A inicialização do hardware I2C é feita separadamente no main.
 */
void display_init(ssd1306_t *ssd) {
    // Inicializa o objeto ssd1306, associando-o ao barramento I2C correto
    ssd1306_init(ssd, DISPLAY_WIDTH, DISPLAY_HEIGHT, false, DISPLAY_I2C_ADDR, I2C_PORT);

    // Envia a sequência de comandos de configuração para o display
    ssd1306_config(ssd);

    // Limpa o buffer interno e atualiza a tela
    ssd1306_fill(ssd, false);
    ssd1306_send_data(ssd);
    printf("Display inicializado.\n");
}

void display_startup_screen(ssd1306_t *ssd) {
    ssd1306_fill(ssd, false);
    const char *line1 = "Controle Remoto";
    uint8_t pos1 = (ssd->width - strlen(line1) * 8) / 2;
    ssd1306_draw_string(ssd, line1, pos1, 28);
    ssd1306_send_data(ssd);
}

void display_update_command(ssd1306_t *ssd, const char* command) {
    ssd1306_fill(ssd, false);
    ssd1306_draw_string(ssd, "Ultimo Comando:", 4, 16);
    uint8_t pos = (ssd->width - strlen(command) * 8) / 2;
    ssd1306_draw_string(ssd, command, pos, 36);
    ssd1306_send_data(ssd);
}