#include "telemetry.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_timer.h"

#define TELEMETRY_QUEUE_LEN 128

static QueueHandle_t telemetry_queue = NULL;

void telemetry_init(void)
{
    telemetry_queue = xQueueCreate(TELEMETRY_QUEUE_LEN, sizeof(telemetry_sample_t));
}

void telemetry_push(uint16_t rpm_ref, uint16_t rpm_read, uint16_t pwm)
{
    if (!telemetry_queue) return;

    telemetry_sample_t sample;
    sample.timestamp_ms = (uint32_t)(esp_timer_get_time() / 1000ULL);
    sample.rpm_ref = rpm_ref;
    sample.rpm_read = rpm_read;
    sample.pwm = pwm;

    xQueueSend(telemetry_queue, &sample, 0);
}

bool telemetry_get_next(telemetry_sample_t *out_sample)
{
    if (!telemetry_queue) return false;
    return xQueueReceive(telemetry_queue, out_sample, pdMS_TO_TICKS(10));
}
