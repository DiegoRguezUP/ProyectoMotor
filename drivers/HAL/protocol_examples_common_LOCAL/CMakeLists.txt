idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    idf_component_register(SRCS protocol_examples_utils.c
                           INCLUDE_DIRS include)
    return()
endif()

set(srcs
    "stdin_out.c"
    "addr_from_stdin.c"
    "connect.c"
    "wifi_connect.c"
    "protocol_examples_utils.c"
)

if(CONFIG_EXAMPLE_PROVIDE_WIFI_CONSOLE_CMD)
    list(APPEND srcs "console_cmd.c")
endif()
if(CONFIG_EXAMPLE_CONNECT_ETHERNET)
    list(APPEND srcs "eth_connect.c")
endif()
if(CONFIG_EXAMPLE_CONNECT_THREAD)
    list(APPEND srcs "thread_connect.c")
endif()
if(CONFIG_EXAMPLE_CONNECT_PPP)
    list(APPEND srcs "ppp_connect.c")
endif()

idf_component_register(
    SRCS "${srcs}"
    INCLUDE_DIRS "include"   # <== exports headers to dependents
    PRIV_REQUIRES esp_netif esp_driver_gpio esp_driver_uart esp_wifi vfs console esp_eth openthread
)
