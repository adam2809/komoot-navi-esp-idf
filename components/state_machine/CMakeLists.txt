file(GLOB_RECURSE SRC_GEN src-gen/*.c)
file(GLOB_RECURSE SRC src/*.c)
file(GLOB_RECURSE LIB_GEN lib-gen/*.c)

idf_component_register(
    SRCS ${SRC_GEN} ${LIB_GEN} ${SRC}
    INCLUDE_DIRS src-gen
    REQUIRES anti_theft_esp32 display
)