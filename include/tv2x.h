#ifndef TV2X_H
#define TV2X_H

#include <stdint.h>

#ifdef __cplusplus
    extern "C" {
#endif

/* Input/Output Data Types */
#ifndef tv2x_in_type
    #define tv2x_in_type uint32_t
#endif

#ifndef tv2x_out_type
    #define tv2x_out_type uint32_t
#endif

struct tv2x_kernel {
    float brightness;
    float contrast;
    float scan_brightness;
    float scan_contrast;
    
    uint32_t brcn_table_r[256];
    uint32_t brcn_table_g[256];
    uint32_t brcn_table_b[256];
    
    struct tv4x_rgb_format *in_format;
    struct tv4x_rgb_format *out_format;
};

int tv2x_init_kernel(
            struct tv2x_kernel *kernel,
            float brightness,
            float contrast,
            float scan_brightness,
            float scan_contrast,
            struct tv4x_rgb_format *in_fmt);

void tv2x_process(
            struct tv2x_kernel *k,
            tv2x_in_type * __restrict__ in,
            tv2x_out_type * __restrict__ out,
            int in_pitch,
            int out_pitch,
            int in_width,
            int in_height);

#ifdef __cplusplus
    }
#endif

/* TV2X_H */ #endif