#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"

// Inicialización Wi-Fi (modo estación)
void wifi_init_sta(void);

// Tarea de socket TCP (envía RPM y PWM cada cierto tiempo)
void socket_task(void *pvParameters);
