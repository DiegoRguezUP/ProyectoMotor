#include "wifi_sta.h"
#include "protocol_examples_common/connect.h"
#include "esp_log.h"

#define TAG "WIFI_STA"

esp_err_t wifi_sta_init(void)
{
    ESP_LOGI(TAG, "Connecting to Wi-Fi...");
    esp_err_t err = example_connect();   // From protocol_examples_common
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Wi-Fi connected.");
    } else {
        ESP_LOGE(TAG, "Wi-Fi connection failed: %s", esp_err_to_name(err));
    }
    return err;
}
