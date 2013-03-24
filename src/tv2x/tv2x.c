#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <stdint.h>

#ifndef PI
    static const float PI = 3.141592653589793f;
#endif

/* Used to access kernel->brcn_ptrs array */
#define BRCN_RED        0
#define BRCN_GREEN      1
#define BRCN_BLUE       2
#define BRCN_SCAN_RED   3
#define BRCN_SCAN_GREEN 4
#define BRCN_SCAN_BLUE  5


/*

TODO:
    - Merge common code between tv4x and tv2x.

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

/**
* Build an RGB Matrix, for use in tv2x_process(), using supplied values.
*
* @param    rgb_min     - Array of 3 floats
* @param    rgb_max     - Array of 3 floats
* @param    matrix      - Pointer to array of [3][2][3]
* @return   void
**/
static void build_rgb_matrix(   float rgb_min[3],
                                float rgb_max[3],
                                float *matrix_ptr) {

    float r[3];
    float g[3];
    float b[3];
    float matrix[3][2][3];
    int i;
    
    r[0] = rgb_max[0];
    r[1] = rgb_min[1];
    r[2] = rgb_min[2];
    
    g[0] = rgb_min[0];
    g[1] = rgb_max[1];
    g[2] = rgb_min[2];
    
    b[0] = rgb_min[0];
    b[1] = rgb_min[1];
    b[2] = rgb_max[2];
    
    for (i = 0; i < 3; i++) {
        /* RG */
        matrix[0][0][i] = r[i];
        matrix[0][1][i] = g[i];
        
        /* BR */
        matrix[1][0][i] = b[i];
        matrix[1][1][i] = r[i];
        
        /* GB */
        matrix[2][0][i] = g[i];
        matrix[2][1][i] = b[i];
    }
    
    memcpy(matrix_ptr, &matrix, sizeof(matrix));
}

int tv2x_init_kernel(
            struct tv2x_kernel *kernel,
            float brightness,
            float contrast,
            float scan_brightness,
            float scan_contrast,
            float *rgb_levels,
            float *scan_rgb_levels,
            float rgb_matrix_min[3],
            float rgb_matrix_max[3],
            struct tvxx_rgb_format *in_fmt) {
    
    int i;
    double result;
    double  *brcn_filter_red,
            *brcn_filter_green,
            *brcn_filter_blue,
            *brcn_filter_scan_red,
            *brcn_filter_scan_green,
            *brcn_filter_scan_blue;
    
    uint16_t *brcn_ptr1,
             *brcn_ptr2;
    
    float rgb_levels1[3] = {
        rgb_levels[0]/100.0,
        rgb_levels[1]/100.0,
        rgb_levels[2]/100.0,
    };
    
    float rgb_levels2[3] = {
        scan_rgb_levels[0]/100.0,
        scan_rgb_levels[1]/100.0,
        scan_rgb_levels[2]/100.0,
    };
    
    memset(kernel, 0, sizeof(*kernel));
    kernel->brightness = brightness;
    kernel->contrast = contrast;
    kernel->scan_brightness = scan_brightness;
    kernel->scan_contrast = scan_contrast;
    kernel->in_format = in_fmt;
    kernel->out_format = in_fmt;
    
    build_rgb_matrix(
        rgb_matrix_min,
        rgb_matrix_max,
        (float *)&kernel->rgb_matrix);
    
    /* Setup brightness/contrast pointers...
       Not used here, but later used by tv2x_process() */
    kernel->brcn_ptrs[BRCN_RED]         = &kernel->brcn_table[0];
    kernel->brcn_ptrs[BRCN_GREEN]       = &kernel->brcn_table[256];
    kernel->brcn_ptrs[BRCN_BLUE]        = &kernel->brcn_table[512];
    kernel->brcn_ptrs[BRCN_SCAN_RED]    = &kernel->brcn_table[768];
    kernel->brcn_ptrs[BRCN_SCAN_GREEN]  = &kernel->brcn_table[1024];
    kernel->brcn_ptrs[BRCN_SCAN_BLUE]   = &kernel->brcn_table[1280];

    /* Generate brightness/contrast lookup tables */
    brcn_filter_red = brcn_get_filter(brightness, contrast, in_fmt->r_mask);
    brcn_filter_green = brcn_get_filter(brightness, contrast, in_fmt->g_mask);
    brcn_filter_blue = brcn_get_filter(brightness, contrast, in_fmt->b_mask);
    
    /* Scanline brightness/contrast */
    brcn_filter_scan_red = brcn_get_filter(scan_brightness, scan_contrast, in_fmt->r_mask);
    brcn_filter_scan_green = brcn_get_filter(scan_brightness, scan_contrast, in_fmt->g_mask);
    brcn_filter_scan_blue = brcn_get_filter(scan_brightness, scan_contrast, in_fmt->b_mask);
    
    brcn_ptr1 = &kernel->brcn_table[0];
    brcn_ptr2 = &kernel->brcn_table[768];
    for (i = 0; i <= in_fmt->r_mask; i++) {
        result = brcn_filter_process(brcn_filter_red, i*rgb_levels1[0]);
        brcn_ptr1[i] = (int)floor(result);
        
        result = brcn_filter_process(brcn_filter_scan_red, i*rgb_levels2[0]);
        brcn_ptr2[i] = (int)floor(result);
    }
    
    brcn_ptr1 = &kernel->brcn_table[256];
    brcn_ptr2 = &kernel->brcn_table[1024];
    for (i = 0; i <= in_fmt->g_mask; i++) {
        result = brcn_filter_process(brcn_filter_green, i*rgb_levels1[1]);
        brcn_ptr1[i] = (int)floor(result);
        
        result = brcn_filter_process(brcn_filter_scan_green, i*rgb_levels2[1]);
        brcn_ptr2[i] = (int)floor(result);
    }
    
    brcn_ptr1 = &kernel->brcn_table[512];
    brcn_ptr2 = &kernel->brcn_table[1280];
    for (i = 0; i <= in_fmt->b_mask; i++) {
        result = brcn_filter_process(brcn_filter_blue, i*rgb_levels1[2]);
        brcn_ptr1[i] = (int)floor(result);
        
        result = brcn_filter_process(brcn_filter_scan_blue, i*rgb_levels2[2]);
        brcn_ptr2[i] = (int)floor(result);
    }
    
    free(brcn_filter_red);
    free(brcn_filter_green);
    free(brcn_filter_blue);
    free(brcn_filter_scan_red);
    free(brcn_filter_scan_green);
    free(brcn_filter_scan_blue);

    return 1;
}

/* RG BR GB */
/*
static float rgb_matrix[3][2][3] = {
    {{1.10f, 1.00f, 1.00f}, {1.00f, 1.10f, 1.00f}},
    {{1.00f, 1.00f, 1.10f}, {1.10f, 1.00f, 1.00f}},
    {{1.00f, 1.10f, 1.00f}, {1.00f, 1.00f, 1.10f}},
};
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
    uint32_t in_r,
             in_g,
             in_b,
             out_r,
             out_g,
             out_b;
    
    tv2x_in_type    *in_ptr;
    tv2x_in_type    *out_ptr, *out_ptr2;
    
    uint32_t linear[2][3];
    uint32_t shift_mask;
    
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
    
    #if 1
        /* Apply out matrix (slower) */
        #define APPLY_OUT_MATRIX(COLUMN)\
            out_r = (float)linear[(COLUMN)][0] * kernel->rgb_matrix[x%3][(COLUMN)][0];\
            out_g = (float)linear[(COLUMN)][1] * kernel->rgb_matrix[x%3][(COLUMN)][1];\
            out_b = (float)linear[(COLUMN)][2] * kernel->rgb_matrix[x%3][(COLUMN)][2];
    #else
        /* Copy without out matrix (faster) */
        #define APPLY_OUT_MATRIX(COLUMN)\
            out_r = linear[(COLUMN)][0];\
            out_g = linear[(COLUMN)][1];\
            out_b = linear[(COLUMN)][2];
    #endif
    
    #if 1
        /* Process Scanline, using brightness/contrast filter (slower). */
        #define PROCESS_SCANLINE()\
            PACK_RGB(\
                kernel->brcn_ptrs[BRCN_SCAN_RED][out_r],\
                kernel->brcn_ptrs[BRCN_SCAN_GREEN][out_g],\
                kernel->brcn_ptrs[BRCN_SCAN_BLUE][out_b],\
                (*kernel->in_format),\
                *out_ptr2++\
            );
    #else
        /* Process Scanline, using divide by two of previous pixel (faster). */
        #define PROCESS_SCANLINE()\
            *(out_ptr2)++ = (*out_ptr & shift_mask) >> 1;
    #endif
    
    for (y = 0; y < in_height; y++) {
        in_ptr = (tv2x_in_type *) ((uint8_t *) in + y * in_pitch);
        out_ptr = (tv2x_out_type *) ((uint8_t *) out + y * 2 * out_pitch);
        out_ptr2 = (tv2x_out_type *) ((uint8_t *) out + ((y * 2) + 1) * out_pitch);
        
        /* Unpack first pixel */
        UNPACK_RGB(in_r, in_g, in_b, (*kernel->in_format), *in_ptr);
        APPLY_CONTRAST(in_r, kernel->brcn_ptrs[BRCN_RED]);
        APPLY_CONTRAST(in_g, kernel->brcn_ptrs[BRCN_GREEN]);
        APPLY_CONTRAST(in_b, kernel->brcn_ptrs[BRCN_BLUE]);
        
        for (x = 0; x < in_width-1; x++) {
            linear[0][0] = DIVIDE_TWO(in_r);
            linear[0][1] = DIVIDE_TWO(in_g);
            linear[0][2] = DIVIDE_TWO(in_b);
            
            /* Unpack next pixel */
            UNPACK_RGB(in_r, in_g, in_b, (*kernel->in_format), *(in_ptr+1));
            APPLY_CONTRAST(in_r, kernel->brcn_ptrs[BRCN_RED]);
            APPLY_CONTRAST(in_g, kernel->brcn_ptrs[BRCN_GREEN]);
            APPLY_CONTRAST(in_b, kernel->brcn_ptrs[BRCN_BLUE]);
            
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
            
            /* Process Scanline */
            PROCESS_SCANLINE();
            
            /* Increment to next column */
            out_ptr++;

            /* Run linear through out matrix to get out_r, out_g, out_b */
            APPLY_OUT_MATRIX(1);
            
            /* Clamp */
            CLAMP_OUTPUT();
            
            /* Pack to out_ptr */
            PACK_RGB(out_r, out_g, out_b, (*kernel->out_format), *out_ptr);
            
            #ifdef TV2X_NES_ASPECT
            if ((x % 3) == 0) {
                out_ptr++;
                PACK_RGB(out_r, out_g, out_b, (*kernel->out_format), *out_ptr);
                PROCESS_SCANLINE();
            }
            #endif
            
            /* Process Scanline */
            PROCESS_SCANLINE();
            
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
        
        /* Process Scanline */
        PROCESS_SCANLINE();
        out_ptr++;
        
        APPLY_OUT_MATRIX(1);
        CLAMP_OUTPUT();
        PACK_RGB(out_r, out_g, out_b, (*kernel->out_format), *out_ptr);
            
        /* Process Scanline */
        PROCESS_SCANLINE();
    }
}
