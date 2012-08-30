#pragma once
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
struct tv4x_rgb_format {
    uint8_t r_mask;
    uint8_t g_mask;
    uint8_t b_mask;
    
    uint8_t r_shift;
    uint8_t g_shift;
    uint8_t b_shift;
};

// Various RGB Formats
extern struct tv4x_rgb_format tv4x_rgb_format_rgb15;
extern struct tv4x_rgb_format tv4x_rgb_format_rgb16;
extern struct tv4x_rgb_format tv4x_rgb_format_rgb24;

// Convert between RGB formats
void rgb_convert(
                    struct tv4x_rgb_format *in_fmt,
                    struct tv4x_rgb_format *out_fmt,
                    uint32_t *in,
                    uint32_t *out,
                    uint32_t size);

// Pack RGB
#define PACK_RGB(r, g, b, fmt, out)\
    (out) =     (r) << (fmt).r_shift |\
                (g) << (fmt).g_shift |\
                (b) << (fmt).b_shift;

// Unpack RGB
#define UNPACK_RGB(r, g, b, fmt, in)\
    (r) =       (in) >> (fmt).r_shift & (fmt).r_mask;\
    (g) =       (in) >> (fmt).g_shift & (fmt).g_mask;\
    (b) =       (in) >> (fmt).b_shift & (fmt).b_mask;

/* Clamp value macro */
#define CLAMP(in, min, max) {\
    if ((in) > max) {\
        (in) = max;\
    } else if ((in) < min) {\
        (in) = min;\
    }\
}

/* RGB15 Format */
#define RGB15_R_SHIFT 10
#define RGB15_G_SHIFT 5
#define RGB15_B_SHIFT 0

#define RGB15_R_MASK 0x1f
#define RGB15_G_MASK 0x1f
#define RGB15_B_MASK 0x1f

#define RGB15_GET_R RGB15_R_SHIFT & RGB15_R_MASK
#define RGB15_GET_G RGB15_G_SHIFT & RGB15_G_MASK
#define RGB15_GET_B RGB15_B_SHIFT & RGB15_B_MASK

#define RGB15_STORAGE uint16_t

/* RGB16 Format */
#define RGB16_R_SHIFT 11
#define RGB16_G_SHIFT 5
#define RGB16_B_SHIFT 0

#define RGB16_R_MASK 0x1f
#define RGB16_G_MASK 0x3f
#define RGB16_B_MASK 0x1f

#define RGB16_GET_R RGB16_R_SHIFT & RGB16_R_MASK
#define RGB16_GET_G RGB16_G_SHIFT & RGB16_G_MASK
#define RGB16_GET_B RGB16_B_SHIFT & RGB16_B_MASK

#define RGB16_STORAGE uint16_t

/* RGB24 Format */
#define RGB24_R_SHIFT 16
#define RGB24_G_SHIFT 8
#define RGB24_B_SHIFT 0

#define RGB24_R_MASK 0xff
#define RGB24_G_MASK 0xff
#define RGB24_B_MASK 0xff

#define RGB24_GET_R RGB24_R_SHIFT & RGB24_R_MASK
#define RGB24_GET_G RGB24_G_SHIFT & RGB24_G_MASK
#define RGB24_GET_B RGB24_B_SHIFT & RGB24_B_MASK

#define RGB24_STORAGE uint32_t

#ifdef __cplusplus
    }
#endif
