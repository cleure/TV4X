cmake_minimum_required (VERSION 2.6)
project (test)

find_package(PNG REQUIRED)

set (CMAKE_C_FLAGS "-Wall -g -DTV4X_YIQ_BLUR_ENABLED -DTV4X_USE_SSE -O3 -std=c99 -pedantic -funroll-loops -ffast-math -msse2 -fomit-frame-pointer -finline-functions")

#configure_file (
#    "${PROJECT_SOURCE_DIR}/config.h.in"
#    "${PROJECT_BINARY_DIR}/config.h"
#)

include_directories(
    "${PROJECT_SOURCE_DIR}"
    "${PROJECT_SOURCE_DIR}/include"
    ${PNG_INCLUDE_DIR}
)

add_executable(test-filter
    src/png/convert.c
    src/png/pngutil.c
    src/rgb/rgb.c
    src/yiq/yiq.c
    src/tv4x/shadow_masks.c
    src/tv4x/tv4x.c
    tests/test-filter.c
)

target_link_libraries(test-filter ${PNG_LIBRARY})

add_executable(test-filter2
    src/png/convert.c
    src/png/pngutil.c
    src/rgb/rgb.c
    src/tv2x/tv2x.c
    tests/test-filter2.c
)

target_link_libraries(test-filter2 ${PNG_LIBRARY})
