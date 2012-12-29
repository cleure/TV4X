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
