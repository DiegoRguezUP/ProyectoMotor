#include "socket_task.h"
#include <string.h>
#include <stdio.h>
#include "driver/ledc.h"

// ============ CONFIGURACIÓN ============
// Cambia por tu red Wi-Fi y host destino
#define WIFI_SSID      "S24 Ultra Sergio"
#define WIFI_PASS      "12345678"
#define SERVER_IP      "10.218.214.104"   // IP de tu PC o servidor
#define SERVER_PORT    13000              // Puerto TCP destino
#define SEND_INTERVAL_MS  1000            // Enviar cada segundo

static const char *TAG = "SOCKET_TASK";

// Variables externas definidas en app_main.c
extern double rpm_ref;
extern double rpm_filt;
extern double pwm_val;

// ============ EVENTOS WIFI ============
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
        esp_wifi_connect();

    else if (event_base == WIFI_EVENT &&
             event_id == WIFI_EVENT_STA_DISCONNECTED)
        esp_wifi_connect();

    else if (event_base == IP_EVENT &&
             event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "Got IP:" IPSTR, IP2STR(&event->ip_info.ip));
    }
}

// ============ WIFI INIT ============
void wifi_init_sta(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");
}

// ============ SOCKET TASK ============
void socket_task(void *pvParameters)
{
    struct sockaddr_in dest_addr;
    char payload[64];

    dest_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(SERVER_PORT);

    while (1)
    {
        int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        if (sock < 0)
        {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            vTaskDelay(pdMS_TO_TICKS(5000));
            continue;
        }

        ESP_LOGI(TAG, "Connecting to %s:%d ...", SERVER_IP, SERVER_PORT);
        int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err != 0)
        {
            ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
            close(sock);
            vTaskDelay(pdMS_TO_TICKS(5000));
            continue;
        }

        ESP_LOGI(TAG, "Connected. Sending data...");

        while (1)
        {
            // Construir mensaje CSV: rpm_ref, rpm_filt, pwm_val
            int len = snprintf(payload, sizeof(payload), "%.2f,%.2f,%.2f\n",
                               rpm_ref, rpm_filt, pwm_val);

            if (len > 0)
            {
                int sent = send(sock, payload, len, 0);
                if (sent < 0)
                {
                    ESP_LOGE(TAG, "Send failed: errno %d", errno);
                    break;
                }
                ESP_LOGI(TAG, "Sent: %s", payload);
            }

            vTaskDelay(pdMS_TO_TICKS(SEND_INTERVAL_MS));
        }

        ESP_LOGW(TAG, "Connection lost. Reconnecting...");
        close(sock);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
