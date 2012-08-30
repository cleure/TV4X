#pragma once
#ifndef TV4X_YIQ_H
#define TV4X_YIQ_H

#include <stdint.h>

#ifdef __cplusplus
    extern "C" {
#endif

/* Convert RGB15/16/24 to YIQ. */
void tv4x_rgb_to_yiq(
        struct tv4x_rgb_format *fmt,
        uint32_t rgb,
        float *y,
        float *i,
        float *q);

/* Convert YIQ to RGB15/16/24, unpacked. */
void tv4x_yiq_to_rgb_unpacked(
        struct tv4x_rgb_format *fmt,
        uint8_t *ro,
        uint8_t *go,
        uint8_t *bo,
        float y,
        float i,
        float q);

#ifdef __cplusplus
    }
#endif
#endif
