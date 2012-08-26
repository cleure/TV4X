#pragma once
#include <stdint.h>

void rgb_to_yiq(
        struct rgb_format *fmt,
        uint32_t rgb,
        float *y,
        float *i,
        float *q);

void yiq_to_rgb_unpacked(
        struct rgb_format *fmt,
        uint8_t *ro,
        uint8_t *go,
        uint8_t *bo,
        float y,
        float i,
        float q);
