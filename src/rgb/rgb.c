#include <stdint.h>
#include "rgb.h"

/* RGB Format Conversion Tables */
double tv4x_rgb24_to_rgb24[3] = {1.0000000000000000, 1.0000000000000000, 1.0000000000000000  };
double tv4x_rgb24_to_rgb16[3] = {0.1215686274509804, 0.2470588235294118, 0.1215686274509804  };
double tv4x_rgb24_to_rgb15[3] = {0.1215686274509804, 0.1215686274509804, 0.1215686274509804  };
double tv4x_rgb16_to_rgb24[3] = {8.2258064516129039, 4.0476190476190474, 8.2258064516129039  };
double tv4x_rgb16_to_rgb16[3] = {1.0000000000000000, 1.0000000000000000, 1.0000000000000000  };
double tv4x_rgb16_to_rgb15[3] = {1.0000000000000000, 0.4920634920634920, 1.0000000000000000  };
double tv4x_rgb15_to_rgb24[3] = {8.2258064516129039, 8.2258064516129039, 8.2258064516129039  };
double tv4x_rgb15_to_rgb16[3] = {1.0000000000000000, 2.0322580645161290, 1.0000000000000000  };
double tv4x_rgb15_to_rgb15[3] = {1.0000000000000000, 1.0000000000000000, 1.0000000000000000  };

/* RGB15 Format Definition */
struct tv4x_rgb_format tv4x_rgb_format_rgb15 = {
    RGB15_R_MASK,           /* r_mask */
    RGB15_G_MASK,           /* g_mask */
    RGB15_B_MASK,           /* b_mask */
    
    RGB15_R_SHIFT,          /* r_shift */
    RGB15_G_SHIFT,          /* g_shift */
    RGB15_B_SHIFT,          /* b_shift */
    
    &tv4x_rgb15_to_rgb24,   /* to RGB24 conversion table */
    &tv4x_rgb15_to_rgb16,   /* to RGB16 conversion table */
    &tv4x_rgb15_to_rgb15,   /* to RGB15 conversion table */
};

/* RGB16 Format Definition */
struct tv4x_rgb_format tv4x_rgb_format_rgb16 = {
    RGB16_R_MASK,           /* r_mask */
    RGB16_G_MASK,           /* g_mask */
    RGB16_B_MASK,           /* b_mask */
    
    RGB16_R_SHIFT,          /* r_shift */
    RGB16_G_SHIFT,          /* g_shift */
    RGB16_B_SHIFT,          /* b_shift */
    
    &tv4x_rgb16_to_rgb24,   /* to RGB24 conversion table */
    &tv4x_rgb16_to_rgb16,   /* to RGB16 conversion table */
    &tv4x_rgb16_to_rgb15,   /* to RGB15 conversion table */
};

/* RGB24 Format Definition */
struct tv4x_rgb_format tv4x_rgb_format_rgb24 = {
    RGB24_R_MASK,           /* r_mask */
    RGB24_G_MASK,           /* g_mask */
    RGB24_B_MASK,           /* b_mask */
    
    RGB24_R_SHIFT,          /* r_shift */
    RGB24_G_SHIFT,          /* g_shift */
    RGB24_B_SHIFT,          /* b_shift */
    
    &tv4x_rgb24_to_rgb24,   /* to RGB24 conversion table */
    &tv4x_rgb24_to_rgb16,   /* to RGB16 conversion table */
    &tv4x_rgb24_to_rgb15,   /* to RGB15 conversion table */
};

/*

TODO: Get proper conversion table based on supplied in/out formats.

*/

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
                    struct tv4x_rgb_format *in_fmt,
                    struct tv4x_rgb_format *out_fmt,
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
