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
            struct tv4x_rgb_format *in_fmt,
            struct tv4x_rgb_format *out_fmt) {
    
    memset(kernel, 0, sizeof(*kernel));
    kernel->brightness = brightness;
    kernel->contrast = contrast;
    kernel->scan_brightness = scan_brightness;
    kernel->scan_contrast = scan_contrast;
    kernel->in_format = in_fmt;
    kernel->out_format = out_fmt;
    
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
    {{1.05, 1.0, 1.0}, {1.0, 1.05, 1.0}},
    {{1.0, 1.05, 1.0}, {1.05, 1.0, 1.0}},
    {{1.0, 1.05, 1.0}, {1.0, 1.0, 1.05}}
};

void tv2x_process(
            struct tv2x_kernel *kernel,
            tv2x_in_type * __restrict__ in,
            tv2x_out_type * __restrict__ out,
            int in_pitch,
            int out_pitch,
            int in_width,
            int in_height) {
    
    /*
    size_t in_size = sizeof(*in);
    size_t out_size = sizeof(*out);*/
    
    int x, y;
    int i1, i2;
    int in_r, in_g, in_b;
    int out_r, out_g, out_b;
    float tmp_r, tmp_g, tmp_b;
    
    int out_width = in_width * 2,
        out_height = in_height * 2;
    
    uint32_t linear[2][3];
/*
        
        for y in xrange(0, self.height):
            for x in xrange(0, self.width):
                if x == 1:
                    linear = [[0, 0, 0], self.data[i1]]
                if (x+1) < self.width:
                    linear = [self.data[i1],
                            [(self.data[i1][0] + self.data[i1+1][0])/2,
                             (self.data[i1][1] + self.data[i1+1][1])/2,
                             (self.data[i1][2] + self.data[i1+1][2])/2]]
                else:
                    linear = [self.data[i1], [0, 0, 0]]
                
                for a in range(0, 2):
                    for b in range(0, 3):
                        v = linear[a][b] * matrix_b[(x+(y%2))%3][a][b]
                        if v > 255.0: v = 255.0
                        if v < 0.0: v = 0.0
                        output[i2+a][b] = int(v)
                
                i2 += out_width
                for a in range(0, 2):
                    for b in range(0, 3):
                        v = ((slope * (1.0 / 255.0)) * (linear[a][b] * matrix_b[(x+(y%2))%3][a][b]) + intercept) * 255.0
                        #v = ((slope * (1.0 / 255.0)) * linear[a][b] + intercept) * 255.0
                        if v > 255.0: v = 255.0
                        if v < 0.0: v = 0.0
                        output[i2+a][b] = int(v)

*/
    
    for (y = 0; y < in_height; y++) {
        for (x = 0; x < in_width; x++) {
            i1 = (y * in_width) + x;
            i2 = (y * out_width * 2) + (x * 2);
            
            UNPACK_RGB(in_r, in_g, in_b, (*kernel->in_format), in[i1]);
            
            if (x == 0) {
                linear[0][0] = 0;
                linear[0][1] = 0;
                linear[0][2] = 0;
                linear[1][0] = in_r;
                linear[1][1] = in_g;
                linear[1][2] = in_b;
            } else if (x+1 < in_width) {
                linear[0][0] = in_r;
                linear[0][1] = in_g;
                linear[0][2] = in_b;
                UNPACK_RGB(in_r, in_g, in_b, (*kernel->in_format), in[i1+1]);
                linear[1][0] = (linear[0][0]+in_r)/2;
                linear[1][1] = (linear[0][1]+in_g)/2;
                linear[1][2] = (linear[0][2]+in_b)/2;
            } else {
                linear[0][0] = in_r;
                linear[0][1] = in_g;
                linear[0][2] = in_b;
                linear[1][0] = 0;
                linear[1][1] = 0;
                linear[1][2] = 0;
            }
            
            out_r = (float)linear[0][0] * rgb_matrix[(x+(y%2))%3][0][0];
            out_g = (float)linear[0][1] * rgb_matrix[(x+(y%2))%3][0][1];
            out_b = (float)linear[0][2] * rgb_matrix[(x+(y%2))%3][0][2];
            
            CLAMP(out_r, 0, 255);
            CLAMP(out_g, 0, 255);
            CLAMP(out_b, 0, 255);
            PACK_RGB(out_r, out_g, out_b, (*kernel->out_format), out[i2]);
            out_r /= 2;
            out_g /= 2;
            out_b /= 2;
            PACK_RGB(out_r, out_g, out_b, (*kernel->out_format), out[i2+out_width]);
            
            out_r = (float)linear[1][0] * rgb_matrix[(x+(y%2))%3][1][0];
            out_g = (float)linear[1][1] * rgb_matrix[(x+(y%2))%3][1][1];
            out_b = (float)linear[1][2] * rgb_matrix[(x+(y%2))%3][1][2];
            
            CLAMP(out_r, 0, 255);
            CLAMP(out_g, 0, 255);
            CLAMP(out_b, 0, 255);
            PACK_RGB(out_r, out_g, out_b, (*kernel->out_format), out[i2+1]);
            out_r /= 2;
            out_g /= 2;
            out_b /= 2;
            PACK_RGB(out_r, out_g, out_b, (*kernel->out_format), out[i2+out_width+1]);
        }
    }
}
