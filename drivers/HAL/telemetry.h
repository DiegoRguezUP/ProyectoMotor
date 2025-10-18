#pragma once
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t timestamp_ms;
    uint16_t rpm_ref;
    uint16_t rpm_read;
    uint16_t pwm;
} telemetry_sample_t;

void telemetry_init(void);
void telemetry_push(uint16_t rpm_ref, uint16_t rpm_read, uint16_t pwm);
bool telemetry_get_next(telemetry_sample_t *out_sample);

#ifdef __cplusplus
}
#endif
