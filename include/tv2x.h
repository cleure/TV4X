#ifndef TV2X_H
#define TV2X_H

#include <stdint.h>

#ifdef __cplusplus
    extern "C" {
#endif

#include "tvxx_internal.h"

struct tv2x_kernel {
    float brightness;
    float contrast;
    float scan_brightness;
    float scan_contrast;
    
    uint32_t brcn_table_r[256];
    uint32_t brcn_table_g[256];
    uint32_t brcn_table_b[256];
    
    struct tvxx_rgb_format *in_format;
    struct tvxx_rgb_format *out_format;
};

int tv2x_init_kernel(
            struct tv2x_kernel *kernel,
            float brightness,
            float contrast,
            float scan_brightness,
            float scan_contrast,
            struct tvxx_rgb_format *in_fmt);

void tv2x_process(
            struct tv2x_kernel *k,
            tv2x_in_type * TVXX_RESTRICT in,
            tv2x_out_type * TVXX_RESTRICT out,
            uint32_t in_pitch,
            uint32_t out_pitch,
            uint32_t in_width,
            uint32_t in_height);

#ifdef __cplusplus
    }
#endif

/* TV2X_H */ #endif