#pragma once
#include <stdint.h>

// Convert RGB15/16/24 to YIQ.
void rgb_to_yiq(
        struct rgb_format *fmt,
        uint32_t rgb,
        float *y,
        float *i,
        float *q);

// Convert YIQ to RGB15/16/24, unpacked.
void yiq_to_rgb_unpacked(
        struct rgb_format *fmt,
        uint8_t *ro,
        uint8_t *go,
        uint8_t *bo,
        float y,
        float i,
        float q);
