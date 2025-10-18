idf_component_register(
    SRCS
        "wifi_sta.c"
        "telemetry.c"
        "tcp_client.c"
        "Simulink2ESP32.c"
    INCLUDE_DIRS
        "."                      # <-- do NOT list protocol_examples_common/include here
    PRIV_REQUIRES
        protocol_examples_common # header + helpers live here
        esp_wifi
        lwip
        nvs_flash
        esp_netif
        esp_timer
)
