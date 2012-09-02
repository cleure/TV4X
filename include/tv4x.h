#pragma once
#ifndef TV4X_TV4X_H
#define TV4X_TV4X_H

#include <stdint.h>

#ifdef __cplusplus
    extern "C" {
#endif

/* Input/Output Data Types */
#ifndef tv4x_in_type
    #define tv4x_in_type uint32_t
#endif

#ifndef tv4x_out_type
    #define tv4x_out_type uint32_t
#endif

#if defined(_MSC_VER)
    #define TV4X_INLINE __forceinline
#elif defined(__GNUC__)
    #define TV4X_INLINE __inline
#elif defined(__clang__)
    #define TV4X_INLINE __inline
#else
    #define TV4X_INLINE
#endif

struct tv4x_setup {
    float y_events;
    float i_events;
    float q_events;
};

extern struct tv4x_setup tv4x_setup_coax;
extern struct tv4x_setup tv4x_setup_composite;
extern struct tv4x_setup tv4x_setup_svideo;
extern struct tv4x_setup tv4x_setup_rgb;

/* CRT Shadow Masks */
extern float tv4x_crt_passthru[2][16];
extern float tv4x_crt_slotmask[2][16];
extern float tv4x_crt_scanline[2][16];
extern float tv4x_crt_silly[2][16];
extern float tv4x_crt_stich[2][16];

/* CRT Phosphors */
extern float tv4x_crt_passthru_phosphor[2][16][3];
extern float tv4x_crt_slotmask_phosphor[2][16][3];
extern float tv4x_crt_scanline_phosphor[2][16][3];
extern float tv4x_crt_silly_phosphor[2][16][3];
extern float tv4x_crt_stich_phosphor[2][16][3];

struct tv4x_kernel {
    /* I/O Formats */
    struct tv4x_rgb_format *in_fmt;
    struct tv4x_rgb_format *out_fmt;
    
    /* Setup Type */
    struct tv4x_setup *setup;
    
    /* Effects */
    float deluma;
    float dechroma;
    
    /* Event Map */
    uint8_t *y_events;
    uint8_t *i_events;
    uint8_t *q_events;
    
    uint32_t rgb_matrix_ev[32768][16];
    uint32_t rgb_matrix_od[32768][16];
};

int tv4x_init_kernel(
        struct tv4x_kernel *k,
        struct tv4x_rgb_format *in_fmt,
        struct tv4x_rgb_format *out_fmt,
        struct tv4x_setup *setup,
        float crt_mask[2][16],
        float crt_rgb[2][16][3],
        float brightness,
        float contrast,
        float scan_brightness,
        float scan_contrast,
        float deluma,
        float dechroma,
        int max_width);

void tv4x_free_kernel(struct tv4x_kernel *k);

void tv4x_process(
            struct tv4x_kernel *k,
            tv4x_in_type *in,
            tv4x_out_type *out,
            int in_pitch,
            int out_pitch,
            int in_width,
            int in_height);

#ifdef __cplusplus
    }
#endif
#endif
