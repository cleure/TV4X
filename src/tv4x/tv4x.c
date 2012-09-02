#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#include "rgb.h"
#include "yiq.h"
#include "tv4x.h"

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
        struct tv4x_rgb_format *in_fmt,
        struct tv4x_rgb_format *out_fmt,
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
            PACK_RGB(r, g, b, tv4x_rgb_format_rgb15, in_rgb);
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

static TV4X_INLINE void
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
    tv4x_rgb_to_yiq(k->in_fmt, in[i1+1], &tmp_yiq[0], &tmp_yiq[1], &tmp_yiq[2]);
    tv4x_rgb_to_yiq(k->in_fmt, in[i1], &next_yiq[0], &next_yiq[1], &next_yiq[2]);
    
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
        
        tv4x_rgb_to_yiq(k->in_fmt, in[i1+1], &next_yiq[0], &next_yiq[1], &next_yiq[2]);
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
        tv4x_yiq_to_rgb_unpacked(k->in_fmt, &r1, &g1, &b1, work_yiq[0], work_yiq[1], work_yiq[2]);
        
        #ifdef TV4X_SCALE_DOWN
            r1 *= (31.0f / (float)k->in_fmt->r_mask);
            g1 *= (31.0f / (float)k->in_fmt->g_mask);
            b1 *= (31.0f / (float)k->in_fmt->b_mask);
        #endif
        
        PACK_RGB(r1, g1, b1, tv4x_rgb_format_rgb15, packed);
        
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
