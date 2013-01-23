#ifndef TV2X_H
#define TV2X_H

#include <stdint.h>

#ifdef __cplusplus
    extern "C" {
#endif

#ifndef TVXX_PACKAGED
    #include "tvxx_internal.h"
#endif

struct tv2x_kernel {
    float brightness;
    float contrast;
    float scan_brightness;
    float scan_contrast;
    float rgb_matrix[3][2][3];
    
    uint16_t brcn_table[1536]; /* 512*3 = 1536 */
    uint16_t *brcn_ptrs[6];
    
    struct tvxx_rgb_format *in_format;
    struct tvxx_rgb_format *out_format;
};

int tv2x_init_kernel(
            struct tv2x_kernel *kernel,
            float brightness,
            float contrast,
            float scan_brightness,
            float scan_contrast,
            float *rgb_levels,
            float *scan_rgb_levels,
            float rgb_matrix_min[3],
            float rgb_matrix_max[3],
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