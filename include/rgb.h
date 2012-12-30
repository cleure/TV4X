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

#ifdef __cplusplus
    }
#endif

/* TVXX_RGB_H */ #endif
