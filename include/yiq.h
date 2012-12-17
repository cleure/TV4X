#ifndef TVXX_YIQ_H
#define TVXX_YIQ_H

#include <stdint.h>

#ifdef __cplusplus
    extern "C" {
#endif

/* Convert RGB15/16/24 to YIQ. */
void tvxx_rgb_to_yiq(
        struct tvxx_rgb_format *fmt,
        uint32_t rgb,
        float *y,
        float *i,
        float *q);

/* Convert YIQ to RGB15/16/24, unpacked. */
void tvxx_yiq_to_rgb_unpacked(
        struct tvxx_rgb_format *fmt,
        uint8_t *ro,
        uint8_t *go,
        uint8_t *bo,
        float y,
        float i,
        float q);

#ifdef __cplusplus
    }
#endif

/* TVXX_YIQ_H */ #endif
