

/* include/tvxx_internal.h */

#ifndef TVXX_INTERNAL_H
#define TVXX_INTERNAL_H

#if defined(_MSC_VER)
    #define TVXX_INLINE __forceinline
    #define TVXX_RESTRICT
#elif defined(__GNUC__)
    #define TVXX_INLINE __inline
    #define TVXX_RESTRICT __restrict__
#elif defined(__clang__)
    #define TVXX_INLINE __inline
    #define TVXX_RESTRICT __restrict__
#else
    #define TVXX_INLINE
    #define TVXX_RESTRICT
#endif

/* Input/Output Data Types */
#ifndef tv4x_in_type
    #define tv4x_in_type uint32_t
#endif

#ifndef tv4x_out_type
    #define tv4x_out_type uint32_t
#endif

/* Input/Output Data Types */
#ifndef tv2x_in_type
    #define tv2x_in_type uint32_t
#endif

#ifndef tv2x_out_type
    #define tv2x_out_type uint32_t
#endif

/* TVXX_INTERNAL_H */ #endif

/* include/rgb.h */

#ifndef TVXX_RGB_H
#define TVXX_RGB_H

#include <stdint.h>

#ifdef __cplusplus
    extern "C" {
#endif

/*
    Common packed video formats are as follows:
    
    FMT   | R Bits | G Bits | B Bits  |
    ______|________|________|_________|
    RGB15 |    5   |    5   |    5    |
    RGB16 |    5   |    6   |    5    |
    RGB24 |    8   |    8   |    8    |

*/

/* Packed RGB Format. */
struct tvxx_rgb_format {
    uint8_t r_mask;
    uint8_t g_mask;
    uint8_t b_mask;
    
    uint8_t r_shift;
    uint8_t g_shift;
    uint8_t b_shift;
    
    double (*to_rgb24)[3];
    double (*to_rgb16)[3];
    double (*to_rgb15)[3];
};

/* Various RGB Formats */
extern struct tvxx_rgb_format tvxx_rgb_format_rgb15;
extern struct tvxx_rgb_format tvxx_rgb_format_rgb16;
extern struct tvxx_rgb_format tvxx_rgb_format_rgb24;

extern double tvxx_rgb24_to_rgb24[3];
extern double tvxx_rgb24_to_rgb16[3];
extern double tvxx_rgb24_to_rgb15[3];
extern double tvxx_rgb16_to_rgb24[3];
extern double tvxx_rgb16_to_rgb16[3];
extern double tvxx_rgb16_to_rgb15[3];
extern double tvxx_rgb15_to_rgb24[3];
extern double tvxx_rgb15_to_rgb16[3];
extern double tvxx_rgb15_to_rgb15[3];

/* Get RGB conversion table for in/out combination. Result copied into
   cnvtable, which is a pointer to an array of double with 3 elements. */
int rgb_get_conversion_table(
            struct tvxx_rgb_format *in,
            struct tvxx_rgb_format *out,
            double (*cnvtable)[3]);

/* Convert between RGB formats */
void rgb_convert(
                    struct tvxx_rgb_format *in_fmt,
                    struct tvxx_rgb_format *out_fmt,
                    uint32_t *in,
                    uint32_t *out,
                    uint32_t size);

/* Pack RGB */
#define PACK_RGB(r, g, b, fmt, out)\
    (out) =     (r) << (fmt).r_shift |\
                (g) << (fmt).g_shift |\
                (b) << (fmt).b_shift;

/* Pack RGB with RGB Conversion Matrix */
#define PACK_RGB_CONV(r, g, b, fmt, conv, out)\
    (out) =     (uint8_t)((r) * (conv)[0]) << (fmt).r_shift |\
                (uint8_t)((g) * (conv)[1]) << (fmt).g_shift |\
                (uint8_t)((b) * (conv)[2]) << (fmt).b_shift;

/* Unpack RGB */
#define UNPACK_RGB(r, g, b, fmt, in)\
    (r) =       (in) >> (fmt).r_shift & (fmt).r_mask;\
    (g) =       (in) >> (fmt).g_shift & (fmt).g_mask;\
    (b) =       (in) >> (fmt).b_shift & (fmt).b_mask;

/* Unpack RGB with RGB Conversion Matrix */
#define UNPACK_RGB_CONV(r, g, b, fmt, conv, in)\
    (r) =       ((in) >> (fmt).r_shift & (fmt).r_mask) * (conv)[0];\
    (g) =       ((in) >> (fmt).g_shift & (fmt).g_mask) * (conv)[1];\
    (b) =       ((in) >> (fmt).b_shift & (fmt).b_mask) * (conv)[2];

/* Clamp value macro */
#define CLAMP(in, min, max) {\
    if ((in) > max) {\
        (in) = max;\
    } else if ((in) < min) {\
        (in) = min;\
    }\
}

#ifdef __cplusplus
    }
#endif

/* TVXX_RGB_H */ #endif


/* include/tv4x.h */

#ifndef TV4X_H
#define TV4X_H

#include <stdint.h>

#ifdef __cplusplus
    extern "C" {
#endif

#include "tvxx_internal.h"

/* Input/Output Data Types */
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
    struct tvxx_rgb_format *in_fmt;
    struct tvxx_rgb_format *out_fmt;
    
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
        struct tvxx_rgb_format *in_fmt,
        struct tvxx_rgb_format *out_fmt,
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

/* TV4X_H */ #endif
