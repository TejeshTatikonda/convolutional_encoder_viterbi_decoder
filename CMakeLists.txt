cmake_minimum_required(VERSION 3.10)

project(FEC_ENC_DEC)

set(SOURCES
    fec.c
    encode.c
    decode.c
)

add_library(fec_lib STATIC ${SOURCES})

add_executable(fec_test fec_tests.c)

target_link_libraries(fec_test PRIVATE fec_lib m)

target_compile_options(fec_test PRIVATE -Wall -Wextra -g)