file(GLOB_RECURSE SOURCES src/*.c)

idf_component_register(
    SRCS ${SOURCES} 
    INCLUDE_DIRS src
    REQUIRES anti_theft_esp32 lvgl komoot_ble_client
)