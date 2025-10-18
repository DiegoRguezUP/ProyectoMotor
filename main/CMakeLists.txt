idf_component_register(
    SRCS "app_main.c"
    INCLUDE_DIRS "."
    REQUIRES HAL
    PRIV_REQUIRES driver esp_timer esp_system freertos
)
