#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "soc/gpio_struct.h"

#define TAG "MOTOR_UART"

// ================== Pines ==================
#define PIN_ENC_A      GPIO_NUM_25
#define PIN_ENC_B      GPIO_NUM_26
#define PIN_PWM        GPIO_NUM_13

// ================== UART ===================
#define UART_PORT      UART_NUM_0
#define UART_BAUD      115200

// ================== Encoder ===================
#define PULSES_PER_REV 199
#define SAMPLE_MS       10
#define ALPHA           0.1f   // filtro pasa bajas

// ================== Escalado ===================
#define RPM_MAX         10000.0

// ================== Variables globales ===================
static volatile long g_pulse_count = 0;
static volatile uint8_t old_AB = 0;
static const int8_t QEM[16] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
static uint8_t g_pwm_cmd = 0;

// 🔒 Mutex de protección para secciones críticas
static portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;

// ================== ISR encoder ===================
static void IRAM_ATTR encoder_isr(void *arg)
{
    uint32_t gpio_in = GPIO.in;
    old_AB <<= 2;
    uint8_t state_A = (gpio_in >> PIN_ENC_A) & 1;
    uint8_t state_B = (gpio_in >> PIN_ENC_B) & 1;
    uint8_t new_AB = (state_A << 1) | state_B;
    old_AB |= new_AB;
    g_pulse_count += QEM[old_AB & 0x0F];
}

// ================== UART INIT ===================
static void uart_init(void)
{
    const uart_config_t cfg = {
        .baud_rate = UART_BAUD,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    uart_param_config(UART_PORT, &cfg);
    uart_driver_install(UART_PORT, 256, 256, 0, NULL, 0);
}

// ================== PWM INIT ===================
static void pwm_init(void)
{
    ledc_timer_config_t timer = {
        .speed_mode       = LEDC_HIGH_SPEED_MODE,
        .timer_num        = LEDC_TIMER_0,
        .duty_resolution  = LEDC_TIMER_8_BIT,
        .freq_hz          = 100000,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer);

    ledc_channel_config_t ch = {
        .gpio_num   = PIN_PWM,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel    = LEDC_CHANNEL_0,
        .timer_sel  = LEDC_TIMER_0,
        .duty       = 0,
        .hpoint     = 0
    };
    ledc_channel_config(&ch);
}

// ================== Encoder INIT ===================
static void encoder_init(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << PIN_ENC_A) | (1ULL << PIN_ENC_B),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE
    };
    gpio_config(&io_conf);

    old_AB = ((gpio_get_level(PIN_ENC_A) << 1) | gpio_get_level(PIN_ENC_B));

    gpio_install_isr_service(0);
    gpio_isr_handler_add(PIN_ENC_A, encoder_isr, NULL);
    gpio_isr_handler_add(PIN_ENC_B, encoder_isr, NULL);
}

// ================== UART RX Task ===================
static void uart_rx_task(void *arg)
{
    uint16_t raw_val;
    while (1) {
        int len = uart_read_bytes(UART_PORT, (uint8_t *)&raw_val,
                                  sizeof(uint16_t), pdMS_TO_TICKS(10));
        if (len == sizeof(uint16_t)) {
            double duty = ((double)raw_val / 65535.0) * 255.0;
            if (duty > 255.0) duty = 255.0;
            if (duty < 0.0) duty = 0.0;
            g_pwm_cmd = (uint8_t)duty;

            ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, g_pwm_cmd);
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
        }
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

// ================== RPM TX Task ===================
static void rpm_tx_task(void *arg)
{
    const double Ts = SAMPLE_MS / 1000.0;
    double rpm_filt = 0.0;

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(SAMPLE_MS));

        long pulses;
        taskENTER_CRITICAL(&spinlock);
        pulses = g_pulse_count;
        g_pulse_count = 0;
        taskEXIT_CRITICAL(&spinlock);
        
        // cálculo como en Arduino
        double cycles = (double)pulses / 8.0;
        double revolutions = cycles / (double)PULSES_PER_REV;
        double rpm = (revolutions / Ts) * 60.0;

        if (rpm < 0) rpm = -rpm;
        if (rpm > RPM_MAX) rpm = RPM_MAX;

        // Filtro pasa bajas
        rpm_filt = (ALPHA * rpm) + ((1.0 - ALPHA) * rpm_filt);

        uint16_t rpm_val = (uint16_t)rpm_filt;
        uart_write_bytes(UART_PORT, (const char *)&rpm_val, sizeof(uint16_t));
    }
}

// ================== MAIN ===================
void app_main(void)
{
    esp_log_level_set("*", ESP_LOG_WARN);
    uart_init();
    pwm_init();
    encoder_init();

    xTaskCreatePinnedToCore(uart_rx_task, "uart_rx_task", 2048, NULL, 5, NULL, 0);
    xTaskCreatePinnedToCore(rpm_tx_task, "rpm_tx_task", 4096, NULL, 5, NULL, 1);
}
