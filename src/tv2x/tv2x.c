#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <stdint.h>

#ifndef PI
    static const float PI = 3.141592653589793f;
#endif

/*

TODO:
    - rgb_* functions need to be renamed to tvxx_rgb_*... Is rgb_convert() even
      needed in rgb.c? It isn't used outside of the tests.
    - Merge common code between tv4x and tv2x.
    - Tool to package up source/header files into a single source/header file,
      for easy integration into existing projects.

*/

/**
* Allocate memory, and pre-process data for brightness/contrast filter.
*
* @param    float brightness
* @param    float contrast
* @param    float in_range
*
* @return   double *
*               - Heap allocated array for 4 doubles
**/
static double *brcn_get_filter(float brightness, float contrast, float in_range)
{
    double *filter = malloc(sizeof(*filter) * 4);
    
    filter[0] = (double)in_range;
    filter[1] = 1.0 / filter[0];
    filter[2] = tan((PI * (contrast / 100.0 + 1.0) / 4.0));
    if (filter[2] < 0.0) filter[2] = 0.0;
    filter[3] = brightness / 100.0 + ((100.0 - brightness) / 200.0) * (1.0 - filter[2]);
    
    return filter;
}

/**
* Process value for brightness/contrast filter.
*
* @param    double *f       - Filter, double of 4
* @param    uint32_r value  - Value to process
* @return   double
**/
static double brcn_filter_process(double *f, uint32_t value)
{
    double result;
    
    #define RANGE(i)        i[0]
    #define SCALE(i)        i[1]
    #define SLOPE(i)        i[2]
    #define INTERCEPT(i)    i[3]
    
    result = ((SLOPE(f) * SCALE(f)) * value + INTERCEPT(f)) * RANGE(f);
    if (result > RANGE(f)) {
        result = RANGE(f);
    } else if (result < 0.0) {
        result = 0.0;
    }
    
    #undef RANGE
    #undef SCALE
    #undef SLOPE
    #undef INTERCEPT
    
    return result;
}

int tv2x_init_kernel(
            struct tv2x_kernel *kernel,
            float brightness,
            float contrast,
            float scan_brightness,
            float scan_contrast,
            struct tvxx_rgb_format *in_fmt) {
    
    int i;
    double result;
    double  *brcn_filter_red,
            *brcn_filter_green,
            *brcn_filter_blue;
    
    memset(kernel, 0, sizeof(*kernel));
    kernel->brightness = brightness;
    kernel->contrast = contrast;
    kernel->scan_brightness = scan_brightness;
    kernel->scan_contrast = scan_contrast;
    kernel->in_format = in_fmt;
    kernel->out_format = in_fmt;

    /* Generate brightness/contrast lookup tables */
    brcn_filter_red = brcn_get_filter(brightness, contrast, in_fmt->r_mask);
    brcn_filter_green = brcn_get_filter(brightness, contrast, in_fmt->g_mask);
    brcn_filter_blue = brcn_get_filter(brightness, contrast, in_fmt->b_mask);
    
    /* TODO: Controlable Red/Green/Blue levels */
    
    for (i = 0; i <= in_fmt->r_mask; i++) {
        result = brcn_filter_process(brcn_filter_red, i);
        kernel->brcn_table_r[i] = (int)floor(result);
    }
    
    for (i = 0; i <= in_fmt->g_mask; i++) {
        result = brcn_filter_process(brcn_filter_green, i);
        kernel->brcn_table_g[i] = (int)floor(result);
    }
    
    for (i = 0; i <= in_fmt->b_mask; i++) {
        result = brcn_filter_process(brcn_filter_blue, i);
        kernel->brcn_table_b[i] = (int)floor(result);
    }
    
    free(brcn_filter_red);
    free(brcn_filter_green);
    free(brcn_filter_blue);

    return 1;
}

static float rgb_matrix[3][2][3] = {
    {{1.10f, 1.00f, 1.00f}, {1.00f, 1.10f, 1.00f}},
    {{1.00f, 1.00f, 1.10f}, {1.10f, 1.00f, 1.00f}},
    {{1.00f, 1.10f, 1.00f}, {1.00f, 1.00f, 1.10f}},
    
    /*
    {{1.05f, 1.00f, 1.00f}, {1.00f, 1.05f, 1.00f}},
    {{1.00f, 1.00f, 1.05f}, {1.05f, 1.00f, 1.00f}},
    {{1.00f, 1.05f, 1.00f}, {1.00f, 1.00f, 1.05f}},*/
};

/*

RG BR GB

*/

void tv2x_process(
            struct tv2x_kernel *kernel,
            tv2x_in_type * TVXX_RESTRICT in,
            tv2x_out_type * TVXX_RESTRICT out,
            uint32_t in_pitch,
            uint32_t out_pitch,
            uint32_t in_width,
            uint32_t in_height) {
    
    int x, y;
    //int i1, i2;
    uint32_t in_r, in_g, in_b;
    uint32_t out_r, out_g, out_b;
    
    //int out_width, out_height;
    
    tv2x_in_type    *in_ptr;
    tv2x_in_type    *out_ptr, *out_ptr2;
    
    uint32_t linear[2][3];
    uint32_t shift_mask;
    
    //out_width = in_width * 2;
    //out_height = in_height * 2;
    
    /* Bit hack that allows fast "divide by two" on packed RGB values */
    PACK_RGB(1, 1, 1, (*kernel->out_format), shift_mask);
    shift_mask = ~ shift_mask;
    
    #define DIVIDE_TWO(IN) ((IN) >> 1)
    
    #define APPLY_CONTRAST(IN, TABLE)\
        (IN) = (TABLE)[(IN)];
    
    #define CLAMP_OUTPUT()\
        if (out_r > kernel->in_format->r_mask) out_r = kernel->in_format->r_mask;\
        if (out_g > kernel->in_format->g_mask) out_g = kernel->in_format->g_mask;\
        if (out_b > kernel->in_format->b_mask) out_b = kernel->in_format->b_mask;
    
    #if 0
        #define APPLY_OUT_MATRIX(COLUMN)\
            out_r = (float)linear[(COLUMN)][0] * rgb_matrix[(x+(y%2))%3][(COLUMN)][0];\
            out_g = (float)linear[(COLUMN)][1] * rgb_matrix[(x+(y%2))%3][(COLUMN)][1];\
            out_b = (float)linear[(COLUMN)][2] * rgb_matrix[(x+(y%2))%3][(COLUMN)][2];
    #else
        #define APPLY_OUT_MATRIX(COLUMN)\
            out_r = linear[(COLUMN)][0];\
            out_g = linear[(COLUMN)][1];\
            out_b = linear[(COLUMN)][2];
    #endif
    
    for (y = 0; y < in_height; y++) {
        //i1 = (y * in_pitch);
        //i2 = (y * out_pitch * 4);
        
        in_ptr = (tv2x_in_type *) ((uint8_t *) in + y * in_pitch);
        out_ptr = (tv2x_out_type *) ((uint8_t *) out + y * 2 * out_pitch);
        out_ptr2 = (tv2x_out_type *) ((uint8_t *) out + ((y * 2) + 1) * out_pitch);
        
        //in_ptr = &in[i1];
        //out_ptr = &out[i2];
        
        /* Unpack first pixel */
        UNPACK_RGB(in_r, in_g, in_b, (*kernel->in_format), *in_ptr);
        APPLY_CONTRAST(in_r, kernel->brcn_table_r);
        APPLY_CONTRAST(in_g, kernel->brcn_table_g);
        APPLY_CONTRAST(in_b, kernel->brcn_table_b);
        
        for (x = 0; x < in_width-1; x++) {
            linear[0][0] = DIVIDE_TWO(in_r);
            linear[0][1] = DIVIDE_TWO(in_g);
            linear[0][2] = DIVIDE_TWO(in_b);
            
            /* Unpack next pixel */
            UNPACK_RGB(in_r, in_g, in_b, (*kernel->in_format), *(in_ptr+1));
            APPLY_CONTRAST(in_r, kernel->brcn_table_r);
            APPLY_CONTRAST(in_g, kernel->brcn_table_g);
            APPLY_CONTRAST(in_b, kernel->brcn_table_b);
            
            /* Basicall (a+b)/2, where a is the current pixel, and b is the next pixel. */
            linear[0][0] += DIVIDE_TWO(in_r);
            linear[0][1] += DIVIDE_TWO(in_g);
            linear[0][2] += DIVIDE_TWO(in_b);
            
            /* Then get linear of current and next... Since there are only two columns
               to fill, this is just: (a+b)/2 */
            linear[1][0] = DIVIDE_TWO(linear[0][0]+in_r);
            linear[1][1] = DIVIDE_TWO(linear[0][1]+in_g);
            linear[1][2] = DIVIDE_TWO(linear[0][2]+in_b);
        
            /* Run linear through out matrix to get out_r, out_g, out_b */
            APPLY_OUT_MATRIX(0);
            
            /* Clamp */
            CLAMP_OUTPUT();
            
            /* Pack to out_ptr */
            PACK_RGB(out_r, out_g, out_b, (*kernel->out_format), *out_ptr);
            
            //*(out_ptr+out_width) = (*out_ptr & shift_mask) >> 1;
            
            /* Fast "divide by two" for scanline. Uses the value we just packed, with
               some bit hacks. */
            //*(out_ptr+(out_pitch*2)) = (*out_ptr & shift_mask) >> 1;
            *(out_ptr2)++ = (*out_ptr & shift_mask) >> 1;
            
            /* Increment to next column */
            out_ptr++;

            /* Run linear through out matrix to get out_r, out_g, out_b */
            APPLY_OUT_MATRIX(1);
            
            /* Clamp */
            CLAMP_OUTPUT();
            
            /* Pack to out_ptr */
            PACK_RGB(out_r, out_g, out_b, (*kernel->out_format), *out_ptr);
            
            //*(out_ptr+out_width) = (*out_ptr & shift_mask) >> 1;
            
            /* Fast "divide by two" */
            //*(out_ptr+(out_pitch*2)) = (*out_ptr & shift_mask) >> 1;
            *(out_ptr2)++ = (*out_ptr & shift_mask) >> 1;
            
            in_ptr++;
            out_ptr++;
        }
        
        /* Process last two out pixels */
        linear[0][0] = in_r >> 1;
        linear[0][1] = in_g >> 1;
        linear[0][2] = in_b >> 1;
        linear[1][0] = in_r >> 2;
        linear[1][1] = in_g >> 2;
        linear[1][2] = in_b >> 2;
        
        APPLY_OUT_MATRIX(0);
        CLAMP_OUTPUT();
        PACK_RGB(out_r, out_g, out_b, (*kernel->out_format), *out_ptr);
        
        *(out_ptr2)++ = (*out_ptr & shift_mask) >> 1;
        out_ptr++;
        
        APPLY_OUT_MATRIX(1);
        CLAMP_OUTPUT();
        PACK_RGB(out_r, out_g, out_b, (*kernel->out_format), *out_ptr);
            
        /* Fast "divide by two" */
        //*(out_ptr+out_width) = (*out_ptr & shift_mask) >> 1;
        *(out_ptr2)++ = (*out_ptr & shift_mask) >> 1;
    }
}
