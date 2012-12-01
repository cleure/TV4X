#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <stdint.h>

#include "rgb.h"
#include "tv2x.h"

#ifndef PI
    static const float PI = 3.141592653589793f;
#endif

/*
* Simple function that calculates slope/intercept for the brightness/constrast
* filter.
*
* @param    float brightness    - Brightness parameter
* @param    float contrast      - Contrast parameter
* @param    float *slope        - Calculated slope placed in this
* @param    float *intercept    - Calculated intercept placed in this.
*/
static void calc_slope_intercept(float brightness, float contrast, float *slope, float *intercept)
{
    *slope = tan((PI * (contrast / 100.0f + 1.0f) / 4.0f));
    *intercept = (brightness / 100.0f) +
                 ((100.0f - brightness) / 200.0f) *
                 (1.0f - *slope);
}

int tv2x_init_kernel(
            struct tv2x_kernel *kernel,
            float brightness,
            float contrast,
            float scan_brightness,
            float scan_contrast,
            struct tv4x_rgb_format *in_fmt) {
    
    memset(kernel, 0, sizeof(*kernel));
    kernel->brightness = brightness;
    kernel->contrast = contrast;
    kernel->scan_brightness = scan_brightness;
    kernel->scan_contrast = scan_contrast;
    kernel->in_format = in_fmt;
    kernel->out_format = in_fmt;
    
    calc_slope_intercept(
        brightness,
        contrast,
        &(kernel->slope),
        &(kernel->intercept));
    
    calc_slope_intercept(
        scan_brightness,
        scan_contrast,
        &(kernel->scan_slope),
        &(kernel->scan_intercept));

    return 1;
}

static float rgb_matrix[3][2][3] = {
    {{1.15, 1.10, 1.10}, {1.10, 1.15, 1.10}},
    {{1.10, 1.10, 1.15}, {1.15, 1.10, 1.10}},
    {{1.10, 1.15, 1.10}, {1.10, 1.10, 1.15}}
};

/*

RG BR GB

*/

void tv2x_process(
            struct tv2x_kernel *kernel,
            tv2x_in_type * __restrict__ in,
            tv2x_out_type * __restrict__ out,
            int in_pitch,
            int out_pitch,
            int in_width,
            int in_height) {
    
    int x, y;
    int i1, i2;
    uint32_t in_r, in_g, in_b;
    uint32_t out_r, out_g, out_b;
    
    int out_width, out_height;
    
    tv2x_in_type    *in_ptr;
    tv2x_in_type    *out_ptr;
    
    uint8_t linear[2][3];
    uint32_t shift_mask;
    
    out_width = in_width * 2;
    out_height = in_height * 2;
    
    /* Bit hack that allows fast "divide by two" on packed RGB values */
    shift_mask =
            0xffffffff
                & (~(1 << kernel->in_format->r_shift))
                & (~(1 << kernel->in_format->g_shift))
                & (~(1 << kernel->in_format->b_shift));
    
    for (y = 0; y < in_height; y++) {
        i1 = (y * in_width);
        i2 = (y * out_width * 2);
        
        in_ptr = &in[i1];
        out_ptr = &out[i2];
        
        for (x = 0; x < in_width; x++) {
            if (x == 0) {
                UNPACK_RGB(in_r, in_g, in_b, (*kernel->in_format), *in_ptr);
                linear[0][0] = in_r/3;
                linear[0][1] = in_g/3;
                linear[0][2] = in_b/3;
                linear[1][0] = in_r;
                linear[1][1] = in_g;
                linear[1][2] = in_b;
                UNPACK_RGB(in_r, in_g, in_b, (*kernel->in_format), *(in_ptr+1));
            } else if (x+1 < in_width) {
                linear[0][0] = in_r;
                linear[0][1] = in_g;
                linear[0][2] = in_b;
                UNPACK_RGB(in_r, in_g, in_b, (*kernel->in_format), *(in_ptr+1));
                linear[1][0] = (linear[0][0]+in_r)/2;
                linear[1][1] = (linear[0][1]+in_g)/2;
                linear[1][2] = (linear[0][2]+in_b)/2;
            } else {
                linear[0][0] = in_r;
                linear[0][1] = in_g;
                linear[0][2] = in_b;
                linear[1][0] = in_r/3;
                linear[1][1] = in_g/3;
                linear[1][2] = in_b/3;
            }
            
            out_r = (float)linear[0][0] * rgb_matrix[(x+(y%2))%3][0][0];
            out_g = (float)linear[0][1] * rgb_matrix[(x+(y%2))%3][0][1];
            out_b = (float)linear[0][2] * rgb_matrix[(x+(y%2))%3][0][2];
            
            if (out_r > kernel->in_format->r_mask) out_r = kernel->in_format->r_mask;
            if (out_g > kernel->in_format->g_mask) out_g = kernel->in_format->g_mask;
            if (out_b > kernel->in_format->b_mask) out_b = kernel->in_format->b_mask;
            
            PACK_RGB(out_r, out_g, out_b, (*kernel->out_format), *out_ptr);
            
            /* Fast "divide by two" */
            *(out_ptr+out_width) = (*out_ptr & shift_mask) >> 1;
            
            out_ptr++;
            out_r = (float)linear[1][0] * rgb_matrix[(x+(y%2))%3][1][0];
            out_g = (float)linear[1][1] * rgb_matrix[(x+(y%2))%3][1][1];
            out_b = (float)linear[1][2] * rgb_matrix[(x+(y%2))%3][1][2];
            
            if (out_r > kernel->in_format->r_mask) out_r = kernel->in_format->r_mask;
            if (out_g > kernel->in_format->g_mask) out_g = kernel->in_format->g_mask;
            if (out_b > kernel->in_format->b_mask) out_b = kernel->in_format->b_mask;
            
            PACK_RGB(out_r, out_g, out_b, (*kernel->out_format), *out_ptr);
            
            /* Fast "divide by two" */
            *(out_ptr+out_width) = (*out_ptr & shift_mask) >> 1;
            
            in_ptr++;
            out_ptr++;
        }
    }
}
