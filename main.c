#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include <stdio.h>
#include <string.h>

#include "include/config.h"
#include "include/lora.h"
#include "include/display.h"
#include "include/led_rgb.h"
#include "include/buttons.h"  

// Enum para os comandos recebidos
typedef enum { CMD_NONE, CMD_OPEN, CMD_STOP, CMD_CLOSE } Command;

// Variáveis de controle globais
volatile Command received_command = CMD_NONE;
ssd1306_t display;
// --- FUNÇÕES DE INICIALIZAÇÃO DE HARDWARE ---

/**
 * @brief Inicializa o barramento I2C1 para o display OLED.
 */
void setup_i2c_display() {
    i2c_init(I2C_PORT, I2C_BAUDRATE);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
    printf("I2C1 (Display) inicializado nos pinos SDA=%d, SCL=%d.\n", I2C_SDA_PIN, I2C_SCL_PIN);
}

/**
 * @brief Inicializa o barramento SPI0 e os pinos GPIO associados para o LoRa.
 * ESTA FUNÇÃO É A CORREÇÃO CRÍTICA.
 */
void setup_spi_lora() {
    // Inicializa o periférico SPI em si
    spi_init(LORA_SPI_PORT, 5 * 1000 * 1000); // 5 MHz

    // Informa aos pinos GPIO para serem controlados pelo periférico SPI
    gpio_set_function(LORA_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(LORA_MOSI_PIN, GPIO_FUNC_SPI);
    gpio_set_function(LORA_MISO_PIN, GPIO_FUNC_SPI);
    
    printf("SPI0 (LoRa) inicializado nos pinos SCK=%d, MOSI=%d, MISO=%d.\n", LORA_SCK_PIN, LORA_MOSI_PIN, LORA_MISO_PIN);
}

/**
 * @brief Função helper para enviar um comando LoRa e atualizar a UI.
 */
void send_command(const char* command) {
    printf("Enviando comando: %s\n", command);
    lora_send((uint8_t*)command, strlen(command), LORA_ADDRESS_RECEIVER);
}

int main() {
    stdio_init_all();
    sleep_ms(3000);
    printf("--- Iniciando Transmissor de Controle Remoto ---\n");
    setup_spi_lora();
    setup_i2c_display();
    display_init(&display);
    rgb_led_init();

    // b. Mostra a tela de startup
    printf("Display. Mostrando tela de inicio...\n");
    display_startup_screen(&display);
    sleep_ms(2000);

    lora_config_t config = {
        .spi_port = LORA_SPI_PORT, .cs_pin = LORA_CS_PIN,
        .interrupt_pin = LORA_INTERRUPT_PIN, .reset_pin = LORA_RESET_PIN,
        .freq = LORA_FREQUENCY, .tx_power = LORA_TX_POWER,
        .this_address = LORA_ADDRESS_TRANSMITTER
    };
    if (!lora_init(&config)) {
        printf("ERRO FATAL: Falha na inicializacao do LoRa.\n");
        rgb_led_set_color(COR_LED_VERMELHO);
        while (1);
    }
    printf("Modulo LoRa OK.\n");

    buttons_init();

    printf("Inicializacao completa. Aguardando pressionamento de botoes...\n");
    display_update_command(&display, "Pronto");
    rgb_led_set_color(COR_LED_VERDE);

    while (true) {
        rgb_led_set_color(COR_LED_MAGENTA);
        printf("dentro do loop");

        if (check_button_a_pressed()) { 
            send_command("CMD_OPEN");
            display_update_command(&display, "Abrir");
            printf("botao A pressionado e comando enviado\n");
        }

        if (check_button_b_pressed()) {
            send_command("CMD_STOP");
            display_update_command(&display, "Parar");
            printf("botao B pressionado e comando enviado\n");
        }
        
        if (check_button_joystick_pressed()) {
            display_update_command(&display, "Fechar");
            send_command("CMD_CLOSE");
            printf("botao Joystick pressionado e comando enviado\n");
        }
        

        sleep_ms(3000);
    }
}