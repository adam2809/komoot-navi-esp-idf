file(GLOB_RECURSE SOURCES src/*.c)

idf_component_register(
    SRCS ${SOURCES} 
    INCLUDE_DIRS src
    REQUIRES i2c_manager esp32-button display esp-idf-ttgo-sim7000-lib
)