

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


/* include/rgb_defines.h */

#ifndef TVXX_RGB_DEFINES_H
#define TVXX_RGB_DEFINES_H

#ifdef __cplusplus
    extern "C" {
#endif

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

/* TVXX_RGB_DEFINES_H */ #endif


/* include/yiq.h */

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


/* src/rgb/rgb.c */

#include <stdint.h>
#include <stdlib.h>
#include <memory.h>

/* RGB Format Conversion Tables */
double tvxx_rgb24_to_rgb24[3] = {1.0000000000000000, 1.0000000000000000, 1.0000000000000000  };
double tvxx_rgb24_to_rgb16[3] = {0.1215686274509804, 0.2470588235294118, 0.1215686274509804  };
double tvxx_rgb24_to_rgb15[3] = {0.1215686274509804, 0.1215686274509804, 0.1215686274509804  };
double tvxx_rgb16_to_rgb24[3] = {8.2258064516129039, 4.0476190476190474, 8.2258064516129039  };
double tvxx_rgb16_to_rgb16[3] = {1.0000000000000000, 1.0000000000000000, 1.0000000000000000  };
double tvxx_rgb16_to_rgb15[3] = {1.0000000000000000, 0.4920634920634920, 1.0000000000000000  };
double tvxx_rgb15_to_rgb24[3] = {8.2258064516129039, 8.2258064516129039, 8.2258064516129039  };
double tvxx_rgb15_to_rgb16[3] = {1.0000000000000000, 2.0322580645161290, 1.0000000000000000  };
double tvxx_rgb15_to_rgb15[3] = {1.0000000000000000, 1.0000000000000000, 1.0000000000000000  };

/* RGB15 Format Definition */
struct tvxx_rgb_format tvxx_rgb_format_rgb15 = {
    0x1f,           /* r_mask */
    0x1f,           /* g_mask */
    0x1f,           /* b_mask */
    
    10,          /* r_shift */
    5,           /* g_shift */
    0,           /* b_shift */
    
    &tvxx_rgb15_to_rgb24,   /* to RGB24 conversion table */
    &tvxx_rgb15_to_rgb16,   /* to RGB16 conversion table */
    &tvxx_rgb15_to_rgb15,   /* to RGB15 conversion table */
};

/* RGB16 Format Definition */
struct tvxx_rgb_format tvxx_rgb_format_rgb16 = {
    0x1f,           /* r_mask */
    0x3f,           /* g_mask */
    0x1f,           /* b_mask */
    
    11,          /* r_shift */
    5,           /* g_shift */
    0,           /* b_shift */
    
    &tvxx_rgb16_to_rgb24,   /* to RGB24 conversion table */
    &tvxx_rgb16_to_rgb16,   /* to RGB16 conversion table */
    &tvxx_rgb16_to_rgb15,   /* to RGB15 conversion table */
};

/* RGB24 Format Definition */
struct tvxx_rgb_format tvxx_rgb_format_rgb24 = {
    0xff,           /* r_mask */
    0xff,           /* g_mask */
    0xff,           /* b_mask */
    
    16,          /* r_shift */
    8,           /* g_shift */
    0,           /* b_shift */
    
    &tvxx_rgb24_to_rgb24,   /* to RGB24 conversion table */
    &tvxx_rgb24_to_rgb16,   /* to RGB16 conversion table */
    &tvxx_rgb24_to_rgb15,   /* to RGB15 conversion table */
};

/* Lookup table for rgb_conversion_table() */
struct format_lookup_table {
    struct tvxx_rgb_format *in;
    struct tvxx_rgb_format *out;
    double (*cnvtable)[3];
};

static struct format_lookup_table fmt_lk_table[] = {
    {&tvxx_rgb_format_rgb15, &tvxx_rgb_format_rgb15, &tvxx_rgb15_to_rgb15},
    {&tvxx_rgb_format_rgb15, &tvxx_rgb_format_rgb16, &tvxx_rgb15_to_rgb16},
    {&tvxx_rgb_format_rgb15, &tvxx_rgb_format_rgb24, &tvxx_rgb15_to_rgb24},
    {&tvxx_rgb_format_rgb16, &tvxx_rgb_format_rgb15, &tvxx_rgb16_to_rgb15},
    {&tvxx_rgb_format_rgb16, &tvxx_rgb_format_rgb16, &tvxx_rgb16_to_rgb16},
    {&tvxx_rgb_format_rgb16, &tvxx_rgb_format_rgb24, &tvxx_rgb16_to_rgb24},
    {&tvxx_rgb_format_rgb24, &tvxx_rgb_format_rgb15, &tvxx_rgb24_to_rgb15},
    {&tvxx_rgb_format_rgb24, &tvxx_rgb_format_rgb16, &tvxx_rgb24_to_rgb16},
    {&tvxx_rgb_format_rgb24, &tvxx_rgb_format_rgb24, &tvxx_rgb24_to_rgb24},
};

/**
* Get conversion table for in/out format, using simple array search.
*
* @param    struct tv4x_rgb_format *in
* @param    struct tv4x_rgb_format *out
* @param    double (*cnvtable)[3]
* @return   1 on success, 0 on error
**/
int rgb_get_conversion_table(
            struct tvxx_rgb_format *in,
            struct tvxx_rgb_format *out,
            double (*cnvtable)[3]) {
    
    int i, len;
    
    len = sizeof(fmt_lk_table)/sizeof(fmt_lk_table[0]);
    for (i = 0; i < len; i++) {
        if (in == fmt_lk_table[i].in && out == fmt_lk_table[i].out) {
            memcpy(cnvtable, fmt_lk_table[i].cnvtable, sizeof(*cnvtable));
            return 1;
        }
    }
    
    return 0;
}

/**
* Convert between various RGB formats.
*
* @param    struct rgb_convert *config
* @param    uint32_t *in
* @param    uint32_t *out
* @param    uint32_t size
* @return   void
**/
void rgb_convert(
                    struct tvxx_rgb_format *in_fmt,
                    struct tvxx_rgb_format *out_fmt,
                    uint32_t *in,
                    uint32_t *out,
                    uint32_t size) {
    
    uint32_t i;
    float r, g, b;
    
    /* Calculate scales */
    float   rs = (float)out_fmt->r_mask / (float)in_fmt->r_mask,
            gs = (float)out_fmt->g_mask / (float)in_fmt->g_mask,
            bs = (float)out_fmt->b_mask / (float)in_fmt->b_mask;
    
    for (i = 0; i < size; i++) {
        /* Unpack RGB */
        r = in[i] >> in_fmt->r_shift & in_fmt->r_mask;
        g = in[i] >> in_fmt->g_shift & in_fmt->g_mask;
        b = in[i] >> in_fmt->b_shift & in_fmt->b_mask;
        
        /* Scale */
        r *= rs;
        g *= gs;
        b *= bs;
        
        /* Clamp */
        CLAMP(r, 0, out_fmt->r_mask);
        CLAMP(g, 0, out_fmt->g_mask);
        CLAMP(b, 0, out_fmt->b_mask);
        
        /* Pack into out buffer */
        out[i] =    (uint8_t)r << out_fmt->r_shift |
                    (uint8_t)g << out_fmt->g_shift |
                    (uint8_t)b << out_fmt->b_shift;
    }
}


/* src/yiq/yiq.c */


#ifdef TV4X_USE_SSE
    #include <xmmintrin.h>
#endif

/* RGB to YIQ Matrix */
static const float yiq_in_matrix[3][3] = {
    {0.299f,     0.587f,    0.114f},
    {0.596f,    -0.274f,   -0.322f},
    {0.212f,    -0.523f,    0.311f}
};

/* YIQ to RGB Matrix */
static const float yiq_out_matrix[3][3] = {
    {1.0f,      0.956f,     0.621f},
    {1.0f,     -0.272f,    -0.647f},
    {1.0f,     -1.105f,     1.702f}
};

/**
* Convert RGB15/16/24 to YIQ.
*
* @param    struct rgb_format *in_fmt
* @param    uint32_t rgb
* @param    float *y
* @param    float *i
* @param    float *q
* @return   void
**/
void TVXX_INLINE tvxx_rgb_to_yiq(
        struct tvxx_rgb_format *fmt,
        uint32_t rgb,
        float *y,
        float *i,
        float *q) {
    
    float r, g, b;
    UNPACK_RGB(r, g, b, *fmt, rgb);
    
    *y = (yiq_in_matrix[0][0] * r) +
         (yiq_in_matrix[0][1] * g) +
         (yiq_in_matrix[0][2] * b);
    
    *i = (yiq_in_matrix[1][0] * r) +
         (yiq_in_matrix[1][1] * g) +
         (yiq_in_matrix[1][2] * b);
    
    *q = (yiq_in_matrix[2][0] * r) +
         (yiq_in_matrix[2][1] * g) +
         (yiq_in_matrix[2][2] * b);
}

/**
* Convert YIQ to RGB15/16/24, unpacked.
*
* @param    struct rgb_format *in_fmt
* @param    uint8_t *r
* @param    uint8_t *g
* @param    uint8_t *b
* @param    float y
* @param    float i
* @param    float q
* @return   void
**/
void TVXX_INLINE tvxx_yiq_to_rgb_unpacked(
        struct tvxx_rgb_format *fmt,
        uint8_t *ro,
        uint8_t *go,
        uint8_t *bo,
        float y,
        float i,
        float q) {

    float r, g, b;
    
    #ifdef TV4X_USE_SSE
        __m128 maxclamp;
        __m128 minclamp;
        __m128 result;
        float *rgb;
    #endif
    
    /* Get R */
    r = (y) +
        (yiq_out_matrix[0][1] * i) +
        (yiq_out_matrix[0][2] * q);
    
    /* Get G */
    g = (y) +
        (yiq_out_matrix[1][1] * i) +
        (yiq_out_matrix[1][2] * q);

    /* Get B */
    b = (y) +
        (yiq_out_matrix[2][1] * i) +
        (yiq_out_matrix[2][2] * q);
    
    /* Round */
    /*
    r += 0.5;
    g += 0.5;
    b += 0.5;
    */
    
    /* Clamp */
    #ifdef TV4X_USE_SSE
        maxclamp = _mm_setr_ps(
                        (float)fmt->r_mask,
                        (float)fmt->g_mask,
                        (float)fmt->b_mask, 0.0f);
        minclamp = _mm_setr_ps( 0.0f,  0.0f,  0.0f, 0.0f);
        result   = _mm_setr_ps(r, g, b, 0.0f);
    
        result = _mm_min_ps(maxclamp, result);
        result = _mm_max_ps(minclamp, result);
    
        rgb = ((float *)&result);
    
        r = *(rgb+0);
        g = *(rgb+1);
        b = *(rgb+2);
    #else
        CLAMP(r, 0, fmt->r_mask);
        CLAMP(g, 0, fmt->g_mask);
        CLAMP(b, 0, fmt->b_mask);
    #endif
    
    *ro = (uint8_t)r;
    *go = (uint8_t)g;
    *bo = (uint8_t)b;
}


/* src/tv4x/shadow_masks.c */


/*

One of the advantages to using matrix multiplication for scaling, is that it's
incredibly simple to create new pixel layouts. Each pixel layout consists of
two components: a primary matrix, which is applied to all RGB channels, and a
secondary "Phosphor" matrix, which can be used to apply phosphor on top of the
primary matrix. For debugging, there are passthru matrices, for both primary
and phosphor, so that it's easier to track down problems while building out
new pixel layouts.

*/

/* Pasthru Phosphor */
float tv4x_crt_passthru_phosphor[2][16][3] = {{
        {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
}, {
        {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
}};

/* Passthru Matrix */
float tv4x_crt_passthru[2][16] = {{
        1.00f, 1.00f, 1.00f, 1.00f,
        1.00f, 1.00f, 1.00f, 1.00f,
        1.00f, 1.00f, 1.00f, 1.00f,
        1.00f, 1.00f, 1.00f, 1.00f,
    }, {
        1.00f, 1.00f, 1.00f, 1.00f,
        1.00f, 1.00f, 1.00f, 1.00f,
        1.00f, 1.00f, 1.00f, 1.00f,
        1.00f, 1.00f, 1.00f, 1.00f,
}};

/* Slot Mask RGB Phosphor Layout */
float tv4x_crt_slotmask_phosphor[2][16][3] = {{
        {5.0f, -5.0f, -5.0f}, {-5.0f, 5.0f, -5.0f}, {-5.0f, -5.0f, 5.0f}, {0.0f, 0.0f, 0.0f},
        {5.0f, -5.0f, -5.0f}, {-5.0f, 5.0f, -5.0f}, {-5.0f, -5.0f, 5.0f}, {0.0f, 0.0f, 0.0f},
        {5.0f, -5.0f, -5.0f}, {-5.0f, 5.0f, -5.0f}, {-5.0f, -5.0f, 5.0f}, {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
}, {
        {-5.0f, -5.0f, 5.0f}, {0.0f, 0.0f, 0.0f}, {5.0f, -5.0f, -5.0f}, {-5.0f, 5.0f, -5.0f},
        {-5.0f, -5.0f, 5.0f}, {0.0f, 0.0f, 0.0f}, {5.0f, -5.0f, -5.0f}, {-5.0f, 5.0f, -5.0f},
        {-5.0f, -5.0f, 5.0f}, {0.0f, 0.0f, 0.0f}, {5.0f, -5.0f, -5.0f}, {-5.0f, 5.0f, -5.0f},
        {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
}};

/* Slot Mask Matrix */
float tv4x_crt_slotmask[2][16] = {{
        1.25f, 1.25f, 1.25f, 1.00f,
        1.30f, 1.30f, 1.30f, 1.00f,
        0.85f, 0.85f, 0.85f, 0.75f,
        0.85f, 0.85f, 0.85f, 0.75f
    }, {
        1.25f, 1.00f, 1.25f, 1.25f,
        1.30f, 1.00f, 1.30f, 1.30f,
        0.85f, 0.75f, 0.85f, 0.85f,
        0.85f, 0.75f, 0.85f, 0.85f
}};

/* Scanline Phosphor */
float tv4x_crt_scanline_phosphor[2][16][3] = {{
        {5.0f, -5.0f, -5.0f}, {-5.0f, 5.0f, -5.0f}, {-5.0f, -5.0f, 5.0f}, {0.0f, 0.0f, 0.0f},
        {5.0f, -5.0f, -5.0f}, {-5.0f, 5.0f, -5.0f}, {-5.0f, -5.0f, 5.0f}, {0.0f, 0.0f, 0.0f},
        {5.0f, -5.0f, -5.0f}, {-5.0f, 5.0f, -5.0f}, {-5.0f, -5.0f, 5.0f}, {0.0f, 0.0f, 0.0f},
        {5.0f, -5.0f, -5.0f}, {-5.0f, 5.0f, -5.0f}, {-5.0f, -5.0f, 5.0f}, {0.0f, 0.0f, 0.0f},
}, {
        {-5.0f, -5.0f, 5.0f}, {0.0f, 0.0f, 0.0f}, {5.0f, -5.0f, -5.0f}, {-5.0f, 5.0f, -5.0f},
        {-5.0f, -5.0f, 5.0f}, {0.0f, 0.0f, 0.0f}, {5.0f, -5.0f, -5.0f}, {-5.0f, 5.0f, -5.0f},
        {-5.0f, -5.0f, 5.0f}, {0.0f, 0.0f, 0.0f}, {5.0f, -5.0f, -5.0f}, {-5.0f, 5.0f, -5.0f},
        {-5.0f, -5.0f, 5.0f}, {0.0f, 0.0f, 0.0f}, {5.0f, -5.0f, -5.0f}, {-5.0f, 5.0f, -5.0f},
}};

/* Scanline Matrix */
float tv4x_crt_scanline[2][16] = {{
        1.20f, 1.20f, 1.20f, 1.20f,
        1.30f, 1.30f, 1.30f, 1.30f,
        0.80f, 0.80f, 0.80f, 0.80f,
        0.70f, 0.70f, 0.70f, 0.70f,
    }, {
        1.20f, 1.20f, 1.20f, 1.20f,
        1.30f, 1.30f, 1.30f, 1.30f,
        0.80f, 0.80f, 0.80f, 0.80f,
        0.70f, 0.70f, 0.70f, 0.70f,
}};

/* Silly Phosphor */
float tv4x_crt_silly_phosphor[2][16][3] = {{
        {0.0f, 0.0f, 0.0f}, {10.0f, -10.0f, -10.0f}, {10.0f, -10.0f, -10.0f}, {0.0f, 0.0f, 0.0f},
        {-10.0f, 10.0f, -10.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {-10.0f, 10.0f, -10.0f},
        {-10.0f, 10.0f, -10.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {-10.0f, 10.0f, -10.0f},
        {0.0f, 0.0f, 0.0f}, {-10.0f, -10.0f, 10.0f}, {-10.0f, -10.0f, 10.0f}, {0.0f, 0.0f, 0.0f},
}, {
        {0.0f, 0.0f, 0.0f}, {10.0f, -10.0f, -10.0f}, {10.0f, -10.0f, -10.0f}, {0.0f, 0.0f, 0.0f},
        {-10.0f, 10.0f, -10.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {-10.0f, 10.0f, -10.0f},
        {-10.0f, 10.0f, -10.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {-10.0f, 10.0f, -10.0f},
        {0.0f, 0.0f, 0.0f}, {-10.0f, -10.0f, 10.0f}, {-10.0f, -10.0f, 10.0f}, {0.0f, 0.0f, 0.0f},
}};

/* Silly Matrix */
float tv4x_crt_silly[2][16] = {{
        0.70f, 1.30f, 1.30f, 0.70f,
        1.30f, 0.70f, 0.70f, 1.30f,
        1.30f, 0.70f, 0.70f, 1.30f,
        0.70f, 1.30f, 1.30f, 0.70f,
    }, {
        0.70f, 1.30f, 1.30f, 0.70f,
        1.30f, 0.70f, 0.70f, 1.30f,
        1.30f, 0.70f, 0.70f, 1.30f,
        0.70f, 1.30f, 1.30f, 0.70f,
}};

/**

DOTMASK

RGBR
BRGB
RGBR
BRGB

RGGB
BRGB

GRRB
BBRG

G B R R
G G B R
**/

/* Pasthru Phosphor */
float tv4x_crt_stich_phosphor[2][16][3] = {{
        {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
}, {
        {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
}};

/* Passthru Matrix */
float tv4x_crt_stich[2][16] = {{
        1.30f, 0.80f, 0.80f, 0.80f,
        0.80f, 1.30f, 0.80f, 0.80f,
        0.80f, 0.80f, 1.30f, 0.80f,
        0.80f, 0.80f, 0.80f, 1.30f,
    }, {
        0.80f, 0.80f, 1.30f, 0.80f,
        0.80f, 1.30f, 0.80f, 0.80f,
        1.30f, 0.80f, 0.80f, 0.80f,
        0.80f, 0.80f, 0.80f, 1.30f,
}};


/* src/tv4x/tv4x.c */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#ifndef PI
    static const float PI = 3.141592653589793f;
#endif

/* Various Setups */
struct tv4x_setup tv4x_setup_coax =       {250.0f, 128.0f, 96.0f};
struct tv4x_setup tv4x_setup_composite =  {256.0f, 192.0f, 128.0f};
struct tv4x_setup tv4x_setup_svideo =     {256.0f, 224.0f, 192.0f};
struct tv4x_setup tv4x_setup_rgb =        {256.0f, 256.0f, 256.0f};

/*
* Simple function that calculates slope/intercept for the brightness/constrast
* filter.
*
* @param    float brightness    - Brightness parameter
* @param    float contrast      - Contrast parameter
* @param    float *slope        - Calculated slope placed in this
* @param    float *intercept    - Calculated intercept placed in this.
*/
static void tv4x_calc_slope_intercept(float brightness, float contrast, float *slope, float *intercept)
{
    *slope = tan((PI * (contrast / 100.0f + 1.0f) / 4.0f));
    *intercept = (brightness / 100.0f) +
                 ((100.0f - brightness) / 200.0f) *
                 (1.0f - *slope);
}

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
        int max_width) {
    
    unsigned int i;
    
    /* Accumulators */
    float y_accum = 0.0f,
          i_accum = 0.0f,
          q_accum = 0.0f;
    
    /* Accumulator Steps */
    float y_step = setup->y_events / (float)max_width;
    float i_step = setup->i_events / (float)max_width;
    float q_step = setup->q_events / (float)max_width;
    
    int x;
    float r1, g1, b1,
          r2, g2, b2;
    
    int r,
        g,
        b;
    
    uint32_t in_rgb,
             out_rgb1,
             out_rgb2;
    
    /* Brightness contrast filter */
    float slope;
    float intercept;
    
    /* Scanline slope/intercept for contrast filter */
    float scan_slope;
    float scan_intercept;
    
    /* Calculate slope/intercept */
    tv4x_calc_slope_intercept(
        brightness,
        contrast,
        &slope,
        &intercept);
    
    /* Calculate slope/intercept */
    tv4x_calc_slope_intercept(
        scan_brightness,
        scan_contrast,
        &scan_slope,
        &scan_intercept);
    
    /* Set pointers */
    k->in_fmt = in_fmt;
    k->out_fmt = out_fmt;
    k->setup = setup;
    
    /* Deluma / Dechroma */
    k->deluma = deluma;
    k->dechroma = dechroma;
    
    /* Allocate space for YIQ events */
    k->y_events = malloc(sizeof(*(k->y_events)) * max_width);
    if (!k->y_events) return 0;
    
    k->i_events = malloc(sizeof(*(k->i_events)) * max_width);
    if (!k->i_events) return 0;
    
    k->q_events = malloc(sizeof(*(k->q_events)) * max_width);
    if (!k->q_events) return 0;
    
    for (i = 0; i < max_width; i++) {
        /* Accumulators (ala Bresenham's) */
        y_accum += y_step;
        i_accum += i_step;
        q_accum += q_step;
        
        /* Set Y Events */
        if (y_accum >= 1.0f) {
            k->y_events[i] = 1;
            y_accum -= 1.0f;
        } else{
            k->y_events[i] = 0;
        }
        
        /* Set I Events */
        if (i_accum >= 1.0f) {
            k->i_events[i] = 1;
            i_accum -= 1.0f;
        } else{
            k->i_events[i] = 0;
        }
        
        /* Set Q Events */
        if (q_accum >= 1.0f) {
            k->q_events[i] = 1;
            q_accum -= 1.0f;
        } else{
            k->q_events[i] = 0;
        }
    }
    
    /*
    
    FIXME: In format now MUST be RGB15...
    
    */
    
    for (r = 0; r < 32; r++) {
    for (g = 0; g < 32; g++) {
    for (b = 0; b < 32; b++) {
        for (x = 0; x < 16; x++) {
            /* Red */
            r1 = r;
            r2 = r;
            
            /* Perform brightness/contrast on original values */
            r1 = ((slope * (1.0f / 31.0f)) *
                 r1 + intercept) * 31.0f;
                
            r2 = ((slope * (1.0f / 31.0f)) *
                 r2 + intercept) * 31.0f;
            
            /* Run through matrices */
            r1 *= crt_mask[0][x];
            r2 *= crt_mask[1][x];
            
            /* Scale */
            r1 *= (float)out_fmt->r_mask / 31.0f;
            r2 *= (float)out_fmt->r_mask / 31.0f;
            
            /* RGB Matrix */
            r1 += crt_rgb[0][x][0] * ((float)out_fmt->r_mask / 255.0f);
            r2 += crt_rgb[1][x][0] * ((float)out_fmt->r_mask / 255.0f);
            
            /* Scanline brightness/contrast */
            if (x >= 8) {
                r1 = ((scan_slope * (1.0f / (float)out_fmt->r_mask)) *
                     r1 + scan_intercept) * (float)out_fmt->r_mask;
                r2 = ((scan_slope * (1.0f / (float)out_fmt->r_mask)) *
                     r2 + scan_intercept) * (float)out_fmt->r_mask;
            }
            
            /* Clamp */
            CLAMP(r1, 0.0f, (float)out_fmt->r_mask);
            CLAMP(r2, 0.0f, (float)out_fmt->r_mask);
            
            /* Green */
            g1 = g;
            g2 = g;
            
            /* Perform brightness/contrast on original values */
            g1 = ((slope * (1.0f / 31.0f)) *
                 g1 + intercept) * 31.0f;
                
            g2 = ((slope * (1.0f / 31.0f)) *
                 g2 + intercept) * 31.0f;
            
            /* Run through matrices */
            g1 *= crt_mask[0][x];
            g2 *= crt_mask[1][x];
            
            /* Scale */
            g1 *= (float)out_fmt->g_mask / 31.0f;
            g2 *= (float)out_fmt->g_mask / 31.0f;
            
            /* RGB Matrix */
            g1 += crt_rgb[0][x][1] * ((float)out_fmt->g_mask / 255.0f);
            g2 += crt_rgb[1][x][1] * ((float)out_fmt->g_mask / 255.0f);
            
            /* Scanline brightness/contrast */
            if (x >= 8) {
                g1 = ((scan_slope * (1.0f / (float)out_fmt->g_mask)) *
                     g1 + scan_intercept) * (float)out_fmt->g_mask;
                g2 = ((scan_slope * (1.0f / (float)out_fmt->g_mask)) *
                     g2 + scan_intercept) * (float)out_fmt->g_mask;
            }
            
            /* Clamp */
            CLAMP(g1, 0.0f, (float)out_fmt->g_mask);
            CLAMP(g2, 0.0f, (float)out_fmt->g_mask);
            
            /* Blue */
            b1 = b;
            b2 = b;
            
            /* Perform brightness/contrast on original values */
            b1 = ((slope * (1.0f / 31.0f)) *
                 b1 + intercept) * 31.0f;
                
            b2 = ((slope * (1.0f / 31.0f)) *
                 b2 + intercept) * 31.0f;
            
            /* Run through matrices */
            b1 *= crt_mask[0][x];
            b2 *= crt_mask[1][x];
            
            /* Scale */
            b1 *= (float)out_fmt->b_mask / 31.0f;
            b2 *= (float)out_fmt->b_mask / 31.0f;
            
            /* RGB Matrix */
            b1 += crt_rgb[0][x][2] * ((float)out_fmt->b_mask / 255.0f);
            b2 += crt_rgb[1][x][2] * ((float)out_fmt->b_mask / 255.0f);
            
            /* Scanline brightness/contrast */
            if (x >= 8) {
                b1 = ((scan_slope * (1.0f / (float)out_fmt->b_mask)) *
                     b1 + scan_intercept) * (float)out_fmt->b_mask;
                b2 = ((scan_slope * (1.0f / (float)out_fmt->b_mask)) *
                     b2 + scan_intercept) * (float)out_fmt->b_mask;
            }
            
            /* Clamp */
            CLAMP(b1, 0.0f, (float)out_fmt->b_mask);
            CLAMP(b2, 0.0f, (float)out_fmt->b_mask);
            
            /* Get as packed */
            PACK_RGB(r, g, b, tvxx_rgb_format_rgb15, in_rgb);
            PACK_RGB((int)r1, (int)g1, (int)b1, *out_fmt, out_rgb1);
            PACK_RGB((int)r2, (int)g2, (int)b2, *out_fmt, out_rgb2);
            
            /* Copy */
            k->rgb_matrix_ev[in_rgb][x] = out_rgb1;
            k->rgb_matrix_od[in_rgb][x] = out_rgb2;
        }
    }}}
    
    return 1;
}

void tv4x_free_kernel(struct tv4x_kernel *k)
{
    free(k->y_events);
    free(k->i_events);
    free(k->q_events);
}

static TVXX_INLINE void
tv4x_process_line(
            struct tv4x_kernel *k,
            tv4x_in_type *in,
            tv4x_out_type *out,
            uint32_t (*rgb_matrix)[32768][16],
            int in_pitch,
            int out_pitch,
            int in_width,
            int out_width,
            int y) {
    
    int x;
    int i1, i2;
    uint8_t r1, g1, b1;
    
    /* Work floats */
    float tmp_yiq[3];
    float cur_yiq[3];
    float work_yiq[3];
    float next_yiq[3];
    
    /* Weighted averages (blur filter) */
    float weighted_y[2];
    float weighted_i[2];
    float weighted_q[2];
    uint32_t packed;
    
    /* Output pointers */
    tv4x_out_type *out_ln1 = out;
    tv4x_out_type *out_ln2 = out + (out_width);
    tv4x_out_type *out_ln3 = out + (out_width * 2);
    tv4x_out_type *out_ln4 = out + (out_width * 3);

    /* Indexes */
    i1 = (y * in_pitch);
    i2 = (y * out_pitch);
    
    /* Initial YIQ values */
    tvxx_rgb_to_yiq(k->in_fmt, in[i1+1], &tmp_yiq[0], &tmp_yiq[1], &tmp_yiq[2]);
    tvxx_rgb_to_yiq(k->in_fmt, in[i1], &next_yiq[0], &next_yiq[1], &next_yiq[2]);
    
    /*
    
    TODO: Multiple blur methods:
            - Blur prev + cur + next
            - Blur previous1 + previous2 + cur
            - Blur YIQ events
    
    TODO: Make blur factors controlable.
    TODO: Brightess / Contrast controls
    TODO: More shadow masks
    
    */
    
    /* The previous 2 and current YIQ values are held in these "weighed" arrays,
       for the blur function. These use a "rolling index" based on the value of x,
       so that there is no branching (ie: index = x % 2). This also has the
       benefit of automatically overwriting the least recent value, so that there
       are fewer MOV instructions being executed. */
    
    weighted_y[0] = next_yiq[0];
    weighted_y[1] = tmp_yiq[0];
    
    weighted_i[0] = next_yiq[1];
    weighted_i[1] = tmp_yiq[1];
    
    weighted_q[0] = next_yiq[2];
    weighted_q[1] = tmp_yiq[2];
    
    cur_yiq[0] = next_yiq[0];
    cur_yiq[1] = next_yiq[1];
    cur_yiq[2] = next_yiq[2];
    
    for (x = 0; x < in_width; x++) {
        /* Convert to YIQ */
        tmp_yiq[0] = next_yiq[0];
        tmp_yiq[1] = next_yiq[1];
        tmp_yiq[2] = next_yiq[2];
        
        tvxx_rgb_to_yiq(k->in_fmt, in[i1+1], &next_yiq[0], &next_yiq[1], &next_yiq[2]);
        cur_yiq[0] = tmp_yiq[0];
        
        /* I Events */
        if (k->i_events[x]) {
            cur_yiq[1] = tmp_yiq[1];
        }
        
        /* Q Events */
        if (k->q_events[x]) {
            cur_yiq[2] = tmp_yiq[2];
        }
        
        #ifdef TV4X_YIQ_BLUR_ENABLED
            work_yiq[0] = (weighted_y[0] + weighted_y[1]) * 0.4 +
                           cur_yiq[0] * 0.2;
            
            work_yiq[1] = (weighted_i[0] + weighted_i[1]) * 0.4 +
                           cur_yiq[1] * 0.2;
            
            work_yiq[2] = (weighted_q[0] + weighted_q[1]) * 0.4 +
                           cur_yiq[2] * 0.2;
            
            weighted_y[0] = cur_yiq[0];
            weighted_y[1] = next_yiq[0];
            
            weighted_i[0] = cur_yiq[1];
            weighted_i[1] = next_yiq[1];
            
            weighted_q[0] = cur_yiq[2];
            weighted_q[1] = next_yiq[2];
            
        #else
            work_yiq[0] = cur_yiq[0];
            work_yiq[1] = cur_yiq[1];
            work_yiq[2] = cur_yiq[2];
        #endif
        
        /* Deluma */
        work_yiq[0] *= k->deluma;
        
        /* Dechroma */
        work_yiq[1] *= k->dechroma;
        work_yiq[2] *= k->dechroma;
        
        /* Get RGB from YIQ */
        tvxx_yiq_to_rgb_unpacked(k->in_fmt, &r1, &g1, &b1, work_yiq[0], work_yiq[1], work_yiq[2]);
        
        #ifdef TV4X_SCALE_DOWN
            r1 *= (31.0f / (float)k->in_fmt->r_mask);
            g1 *= (31.0f / (float)k->in_fmt->g_mask);
            b1 *= (31.0f / (float)k->in_fmt->b_mask);
        #endif
        
        PACK_RGB(r1, g1, b1, tvxx_rgb_format_rgb15, packed);
        
        /* Copy */
        out_ln1[i2+0] = (*rgb_matrix)[packed][0];
        out_ln1[i2+1] = (*rgb_matrix)[packed][1];
        out_ln1[i2+2] = (*rgb_matrix)[packed][2];
        out_ln1[i2+3] = (*rgb_matrix)[packed][3];
        
        out_ln2[i2+0] = (*rgb_matrix)[packed][4];
        out_ln2[i2+1] = (*rgb_matrix)[packed][5];
        out_ln2[i2+2] = (*rgb_matrix)[packed][6];
        out_ln2[i2+3] = (*rgb_matrix)[packed][7];
        
        out_ln3[i2+0] = (*rgb_matrix)[packed][8];
        out_ln3[i2+1] = (*rgb_matrix)[packed][9];
        out_ln3[i2+2] = (*rgb_matrix)[packed][10];
        out_ln3[i2+3] = (*rgb_matrix)[packed][11];
        
        out_ln4[i2+0] = (*rgb_matrix)[packed][12];
        out_ln4[i2+1] = (*rgb_matrix)[packed][13];
        out_ln4[i2+2] = (*rgb_matrix)[packed][14];
        out_ln4[i2+3] = (*rgb_matrix)[packed][15];

        i1 += 1;
        i2 += 4;
    }
}

void tv4x_process(
            struct tv4x_kernel *k,
            tv4x_in_type *in,
            tv4x_out_type *out,
            int in_pitch,
            int out_pitch,
            int in_width,
            int in_height) {
    
    int out_width = in_width * 4;
    int y;
    
    /* Odd Fields */
    for (y = 0; y < in_height; y += 2) {
        tv4x_process_line(
            k,
            in,
            out,
            &(k->rgb_matrix_od),
            in_pitch,
            out_pitch,
            in_width,
            out_width,
            y);
    }
    
    /* Even Fields */
    for (y = 1; y < in_height; y += 2) {
        tv4x_process_line(
            k,
            in,
            out,
            &(k->rgb_matrix_ev),
            in_pitch,
            out_pitch,
            in_width,
            out_width,
            y);
    }
}
