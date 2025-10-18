#include "tcp_client.h"
#include "telemetry.h"
#include "esp_log.h"
#include "esp_system.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "sdkconfig.h"

#define TAG "TCP_CLIENT"

#define SERVER_IP   CONFIG_NET_SERVER_IP
#define SERVER_PORT CONFIG_NET_SERVER_PORT
#define SEND_PERIOD CONFIG_SEND_PERIOD_MS

static void tcp_client_task(void *pv)
{
    struct sockaddr_in dest_addr = {
        .sin_addr.s_addr = inet_addr(SERVER_IP),
        .sin_family = AF_INET,
        .sin_port = htons(SERVER_PORT),
    };

    char tx_buf[128];
    int sock;

    for (;;) {
        // Create socket
        sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            vTaskDelay(pdMS_TO_TICKS(2000));
            continue;
        }

        ESP_LOGI(TAG, "Connecting to %s:%d", SERVER_IP, SERVER_PORT);
        int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err != 0) {
            ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
            close(sock);
            vTaskDelay(pdMS_TO_TICKS(2000));
            continue;
        }

        ESP_LOGI(TAG, "Connected!");

        telemetry_sample_t sample;
        while (1) {
#if CONFIG_SEND_MODE_PERIODIC
            vTaskDelay(pdMS_TO_TICKS(SEND_PERIOD));
#endif
            while (telemetry_get_next(&sample)) {

#if CONFIG_INCLUDE_TIMESTAMP
#if CONFIG_PAYLOAD_FORMAT_CSV
                int len = snprintf(tx_buf, sizeof(tx_buf),
                                   "%lu,%u,%u,%u\n",
                                   (unsigned long)sample.timestamp_ms,
                                   sample.rpm_ref,
                                   sample.rpm_read,
                                   sample.pwm);
#elif CONFIG_PAYLOAD_FORMAT_JSON
                int len = snprintf(tx_buf, sizeof(tx_buf),
                                   "{\"t\":%lu,\"ref\":%u,\"rpm\":%u,\"pwm\":%u}\n",
                                   (unsigned long)sample.timestamp_ms,
                                   sample.rpm_ref, sample.rpm_read, sample.pwm);
#else
                int len = snprintf(tx_buf, sizeof(tx_buf),
                                   "%lu,%u,%u,%u\n",
                                   (unsigned long)sample.timestamp_ms,
                                   sample.rpm_ref, sample.rpm_read, sample.pwm);
#endif
#else
                int len = snprintf(tx_buf, sizeof(tx_buf),
                                   "%u,%u,%u\n",
                                   sample.rpm_ref, sample.rpm_read, sample.pwm);
#endif

                int sent = send(sock, tx_buf, len, 0);
                if (sent < 0) {
                    ESP_LOGE(TAG, "Send failed: errno %d", errno);
                    break; // reconnect
                }

#if CONFIG_SEND_MODE_FAST
                vTaskDelay(pdMS_TO_TICKS(10));
#endif
            }
        }

        ESP_LOGW(TAG, "Disconnected. Reconnecting...");
        close(sock);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

esp_err_t tcp_client_start(void)
{
    xTaskCreatePinnedToCore(tcp_client_task, "tcp_client_task", 4096, NULL, 4, NULL, 1);
    return ESP_OK;
}
