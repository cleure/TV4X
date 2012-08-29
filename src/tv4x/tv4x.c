#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#include "rgb.h"
#include "yiq.h"
#include "tv4x.h"

#ifndef PI
  #define PI 3.141592653589793f
#endif

// Various Setups
struct tv4x_setup tv4x_setup_coax =       {250.0f, 128.0f, 96.0f};
struct tv4x_setup tv4x_setup_composite =  {256.0f, 192.0f, 128.0f};
struct tv4x_setup tv4x_setup_svideo =     {256.0f, 224.0f, 192.0f};
struct tv4x_setup tv4x_setup_rgb =        {256.0f, 256.0f, 256.0f};

int tv4x_init_kernel(
        struct tv4x_kernel *k,
        struct rgb_format *in_fmt,
        struct rgb_format *out_fmt,
        struct tv4x_setup *setup,
        float crt_mask[2][16],
        float crt_rgb[2][16][3],
        float deluma,
        float dechroma,
        int max_width) {
    
    unsigned int i;
    float y_accum = 0.0f,
          i_accum = 0.0f,
          q_accum = 0.0f;
    
    // Set pointers
    k->in_fmt = in_fmt;
    k->out_fmt = out_fmt;
    k->setup = setup;
    
    // Deluma / Dechroma
    k->deluma = deluma;
    k->dechroma = dechroma;
    
    // Allocate space for YIQ events
    k->y_events = malloc(sizeof(*(k->y_events)) * max_width);
    if (!k->y_events) return 0;
    
    k->i_events = malloc(sizeof(*(k->i_events)) * max_width);
    if (!k->i_events) return 0;
    
    k->q_events = malloc(sizeof(*(k->q_events)) * max_width);
    if (!k->q_events) return 0;
    
    float y_step = setup->y_events / (float)max_width;
    float i_step = setup->i_events / (float)max_width;
    float q_step = setup->q_events / (float)max_width;
    
    for (i = 0; i < max_width; i++) {
        // Accumulators (ala Bresenham's)
        y_accum += y_step;
        i_accum += i_step;
        q_accum += q_step;
        
        // Set Y Events
        if (y_accum >= 1.0f) {
            k->y_events[i] = 1;
            y_accum -= 1.0f;
        } else{
            k->y_events[i] = 0;
        }
        
        // Set I Events
        if (i_accum >= 1.0f) {
            k->i_events[i] = 1;
            i_accum -= 1.0f;
        } else{
            k->i_events[i] = 0;
        }
        
        // Set Q Events
        if (q_accum >= 1.0f) {
            k->q_events[i] = 1;
            q_accum -= 1.0f;
        } else{
            k->q_events[i] = 0;
        }
    }
    
    int x;
    float r1, g1, b1,
          r2, g2, b2;
    
    // Scanline settings
    float scan_brightness = 0.0f;
    float scan_contrast = 12.0f;
    
    // Calculate scanline slope/intercept for contrast filter
    float scan_slope = tan((PI * (scan_contrast / 100.0f + 1.0f) / 4.0f));
    float scan_intersect = (scan_brightness / 100.0f) +
                           ((100.0f - scan_brightness) / 200.0f) *
                           (1.0f - scan_slope);
    
    int r, g, b;
    uint32_t in_rgb, out_rgb1, out_rgb2;
    
    /*
    
    FIXME: In format now MUST be RGB15...
    
    */
    
    for (r = 0; r < 32; r++) {
    for (g = 0; g < 32; g++) {
    for (b = 0; b < 32; b++) {
        for (x = 0; x < 16; x++) {
            // Red
            r1 = r;
            r2 = r;
            
            // Run through matrices
            r1 *= crt_mask[0][x];
            r2 *= crt_mask[1][x];
            
            // Scale
            r1 *= (float)out_fmt->r_mask / 31.0f;
            r2 *= (float)out_fmt->r_mask / 31.0f;
            
            // RGB Matrix
            r1 += crt_rgb[0][x][0] * ((float)out_fmt->r_mask / 255.0f);
            r2 += crt_rgb[1][x][0] * ((float)out_fmt->r_mask / 255.0f);
            
            // Scanline brightness/contrast
            if (x >= 8) {
                r1 = ((scan_slope * (1.0f / (float)out_fmt->r_mask)) *
                     r1 + scan_intersect) * (float)out_fmt->r_mask;
                r2 = ((scan_slope * (1.0f / (float)out_fmt->r_mask)) *
                     r2 + scan_intersect) * (float)out_fmt->r_mask;
            }
            
            // Clamp
            CLAMP(r1, 0.0f, (float)out_fmt->r_mask);
            CLAMP(r2, 0.0f, (float)out_fmt->r_mask);
            
            // Green
            g1 = g;
            g2 = g;
            
            // Run through matrices
            g1 *= crt_mask[0][x];
            g2 *= crt_mask[1][x];
            
            // Scale
            g1 *= (float)out_fmt->g_mask / 31.0f;
            g2 *= (float)out_fmt->g_mask / 31.0f;
            
            // RGB Matrix
            g1 += crt_rgb[0][x][1] * ((float)out_fmt->g_mask / 255.0f);
            g2 += crt_rgb[1][x][1] * ((float)out_fmt->g_mask / 255.0f);
            
            // Scanline brightness/contrast
            if (x >= 8) {
                g1 = ((scan_slope * (1.0f / (float)out_fmt->g_mask)) *
                     g1 + scan_intersect) * (float)out_fmt->g_mask;
                g2 = ((scan_slope * (1.0f / (float)out_fmt->g_mask)) *
                     g2 + scan_intersect) * (float)out_fmt->g_mask;
            }
            
            // Clamp
            CLAMP(g1, 0.0f, (float)out_fmt->g_mask);
            CLAMP(g2, 0.0f, (float)out_fmt->g_mask);
            
            // Blue
            b1 = b;
            b2 = b;
            
            // Run through matrices
            b1 *= crt_mask[0][x];
            b2 *= crt_mask[1][x];
            
            // Scale
            b1 *= (float)out_fmt->b_mask / 31.0f;
            b2 *= (float)out_fmt->b_mask / 31.0f;
            
            // RGB Matrix
            b1 += crt_rgb[0][x][2] * ((float)out_fmt->b_mask / 255.0f);
            b2 += crt_rgb[1][x][2] * ((float)out_fmt->b_mask / 255.0f);
            
            // Scanline brightness/contrast
            if (x >= 8) {
                b1 = ((scan_slope * (1.0f / (float)out_fmt->b_mask)) *
                     b1 + scan_intersect) * (float)out_fmt->b_mask;
                b2 = ((scan_slope * (1.0f / (float)out_fmt->b_mask)) *
                     b2 + scan_intersect) * (float)out_fmt->b_mask;
            }
            
            // Clamp
            CLAMP(b1, 0.0f, (float)out_fmt->b_mask);
            CLAMP(b2, 0.0f, (float)out_fmt->b_mask);
            
            // Get as packed
            PACK_RGB(r, g, b, rgb_format_rgb15, in_rgb);
            PACK_RGB((int)r1, (int)g1, (int)b1, *out_fmt, out_rgb1);
            PACK_RGB((int)r2, (int)g2, (int)b2, *out_fmt, out_rgb2);
            
            // Copy
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

static inline void tv4x_process_line(
            struct tv4x_kernel *k,
            uint32_t *in,
            uint32_t *out,
            uint32_t rgb_matrix[32768][16],
            int in_width,
            int out_width,
            int y) {
    
    int x;
    int i1, i2;
    uint8_t r1, g1, b1;
    
    // Work floats
    float tmp_y, tmp_i, tmp_q;
    float work_y, work_i, work_q;
    float cur_y, cur_i, cur_q;
    float /*sum_y,*/ sum_i, sum_q;
    
    // Output pointers
    uint32_t *out_ln1 = out;
    uint32_t *out_ln2 = out + (out_width);
    uint32_t *out_ln3 = out + (out_width * 2);
    uint32_t *out_ln4 = out + (out_width * 3);
    
    // Sums, for averages
    int /*sum_y_len = 1,*/
        sum_i_len = 1,
        sum_q_len = 1;

    // Indexes
    i1 = (y * in_width);
    i2 = (y * out_width * 4);
    
    // Initial YIQ values
    rgb_to_yiq(k->in_fmt, in[i1], &cur_y, &cur_i, &cur_q);
    
    //sum_y = cur_y;
    sum_i = cur_i;
    sum_q = cur_q;
    
    /*
    
    TODO: Multiple blur methods:
            - Blur prev + cur + next
            - Blur previous1 + previous2 + cur
            - Blur YIQ events
    
    TODO: Make blur factors controlable.
    TODO: Brightess / Contrast controls
    TODO: More shadow masks
    
    */
    
    float weights[3] = {0.2f, 0.2f, 0.2f};
    float weighted_y[3] = {cur_y, cur_y, cur_y};
    float weighted_i[3] = {cur_i, cur_i, cur_i};
    float weighted_q[3] = {cur_q, cur_q, cur_q};
    int weighted_index = 0;
    uint32_t packed;
    
    for (x = 0; x < in_width; x++) {
        // Convert to YIQ
        rgb_to_yiq(k->in_fmt, in[i1], &tmp_y, &tmp_i, &tmp_q);
        cur_y = tmp_y;
        
        // I Events
        if (k->i_events[x]) {
            cur_i = sum_i / (float)sum_i_len;
            sum_i_len = 1;
            sum_i = tmp_i;
        } else {
            sum_i += tmp_i;
            sum_i_len++;
        }
        
        // Q Events
        if (k->q_events[x]) {
            cur_q = sum_q / (float)sum_q_len;
            sum_q_len = 1;
            sum_q = tmp_q;
        } else {
            sum_q += tmp_q;
            sum_q_len++;
        }
        
        #ifdef TV4X_YIQ_BLUR_ENABLED
            weighted_index = x % 3;
            weighted_y[weighted_index] = cur_y;
            weighted_i[weighted_index] = cur_i;
            weighted_q[weighted_index] = cur_q;
            weights[weighted_index] = 0.6f;
            
            work_y = (weighted_y[0] * weights[0]) +
                     (weighted_y[1] * weights[1]) +
                     (weighted_y[2] * weights[2]);
            
            work_i = (weighted_i[0] * weights[0]) +
                     (weighted_i[1] * weights[1]) +
                     (weighted_i[2] * weights[2]);
                     
            work_q = (weighted_q[0] * weights[0]) +
                     (weighted_q[1] * weights[1]) +
                     (weighted_q[2] * weights[2]);
        
            weights[weighted_index] = 0.2f;
        #else
            work_y = cur_y;
            work_i = cur_i;
            work_q = cur_q;
        #endif
        
        // Deluma
        work_y *= k->deluma;
        
        // Dechroma
        work_i *= k->dechroma;
        work_q *= k->dechroma;
        
        // Get RGB from YIQ
        yiq_to_rgb_unpacked(k->in_fmt, &r1, &g1, &b1, work_y, work_i, work_q);
        
        #ifdef TV4X_SCALE_DOWN
            r1 *= (31.0f / (float)k->in_fmt->r_mask);
            g1 *= (31.0f / (float)k->in_fmt->g_mask);
            b1 *= (31.0f / (float)k->in_fmt->b_mask);
        #endif
        
        PACK_RGB(r1, g1, b1, rgb_format_rgb15, packed);
        
        out_ln1[i2+0] = rgb_matrix[packed][0];
        out_ln1[i2+1] = rgb_matrix[packed][1];
        out_ln1[i2+2] = rgb_matrix[packed][2];
        out_ln1[i2+3] = rgb_matrix[packed][3];
        
        out_ln2[i2+0] = rgb_matrix[packed][4];
        out_ln2[i2+1] = rgb_matrix[packed][5];
        out_ln2[i2+2] = rgb_matrix[packed][6];
        out_ln2[i2+3] = rgb_matrix[packed][7];
        
        out_ln3[i2+0] = rgb_matrix[packed][8];
        out_ln3[i2+1] = rgb_matrix[packed][9];
        out_ln3[i2+2] = rgb_matrix[packed][10];
        out_ln3[i2+3] = rgb_matrix[packed][11];
        
        out_ln4[i2+0] = rgb_matrix[packed][12];
        out_ln4[i2+1] = rgb_matrix[packed][13];
        out_ln4[i2+2] = rgb_matrix[packed][14];
        out_ln4[i2+3] = rgb_matrix[packed][15];

        i1 += 1;
        i2 += 4;
    }
}

void tv4x_process(
            struct tv4x_kernel *k,
            uint32_t *in,
            uint32_t *out,
            int in_width,
            int in_height) {
    
    int out_width = in_width * 4;
    int y;
    
    // Odd Fields
    for (y = 0; y < in_height; y += 2) {
        tv4x_process_line(
            k,
            in,
            out,
            k->rgb_matrix_od,
            in_width,
            out_width,
            y);
    }
    
    // Even Fields
    for (y = 1; y < in_height; y += 2) {
        tv4x_process_line(
            k,
            in,
            out,
            k->rgb_matrix_ev,
            in_width,
            out_width,
            y);
    }
}
