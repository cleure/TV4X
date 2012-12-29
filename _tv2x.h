

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


/* include/tv2x.h */

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