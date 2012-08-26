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
struct tv4x_setup tv4x_setup_coax =       {0.8f, 0.5f, 0.2f};
struct tv4x_setup tv4x_setup_composite =  {1.0f, 0.6f, 0.3f};
struct tv4x_setup tv4x_setup_svideo =     {1.0f, 0.7f, 0.7f};
struct tv4x_setup tv4x_setup_rgb =        {1.0f, 1.0f, 1.0f};

// Slot Mask RGB Matrix
float tv4x_crt_slotmask_rgb[2][16][3] = {{
        {10.0f, -10.0f, -10.0f}, {-10.0f, 10.0f, -10.0f}, {-10.0f, -10.0f, 10.0f}, {0.0f, 0.0f, 0.0f},
        {10.0f, -10.0f, -10.0f}, {-10.0f, 10.0f, -10.0f}, {-10.0f, -10.0f, 10.0f}, {0.0f, 0.0f, 0.0f},
        {10.0f, -10.0f, -10.0f}, {-10.0f, 10.0f, -10.0f}, {-10.0f, -10.0f, 10.0f}, {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
}, {
        {-10.0f, -10.0f, 10.0f}, {0.0f, 0.0f, 0.0f}, {10.0f, -10.0f, -10.0f}, {-10.0f, 10.0f, -10.0f},
        {-10.0f, -10.0f, 10.0f}, {0.0f, 0.0f, 0.0f}, {10.0f, -10.0f, -10.0f}, {-10.0f, 10.0f, -10.0f},
        {-10.0f, -10.0f, 10.0f}, {0.0f, 0.0f, 0.0f}, {10.0f, -10.0f, -10.0f}, {-10.0f, 10.0f, -10.0f},
        {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
}};

// Slot Mask Layout
float tv4x_crt_slotmask[2][16] = {{
        1.25f, 1.00f, 1.25f, 1.00f,
        1.30f, 1.00f, 1.30f, 1.00f,
        //1.00f, 0.90f, 1.00f, 0.90f,
        //0.90f, 1.00f, 0.90f, 1.00f
        0.85f, 0.75f, 0.85f, 0.75f,
        0.75f, 0.85f, 0.75f, 0.85f
    }, {
        1.00f, 1.25f, 1.00f, 1.25f,
        1.00f, 1.30f, 1.00f, 1.30f,
        //0.90f, 1.00f, 0.90f, 1.00f,
        //1.00f, 0.90f, 1.00f, 0.90f,
        0.75f, 0.85f, 0.75f, 0.85f,
        0.85f, 0.75f, 0.85f, 0.75f
}};

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
    
    for (i = 0; i < max_width; i++) {
        // Accumulators
        y_accum += setup->y_step;
        i_accum += setup->i_step;
        q_accum += setup->q_step;
        
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
    
    // Red
    for (i = 0; i < (in_fmt->r_mask+1); i++) {
        for (x = 0; x < 16; x++) {
            r1 = i;
            r2 = i;
            
            // Run through matrices
            r1 *= crt_mask[0][x];
            r2 *= crt_mask[1][x];
            
            // Scale
            r1 *= (float)out_fmt->r_mask / (float)in_fmt->r_mask;
            r2 *= (float)out_fmt->r_mask / (float)in_fmt->r_mask;
            
            // RGB Matrix
            r1 += crt_rgb[0][x][0] * ((float)in_fmt->r_mask / (float)out_fmt->r_mask);
            r2 += crt_rgb[1][x][0] * ((float)in_fmt->r_mask / (float)out_fmt->r_mask);
            
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
            
            // Copy
            k->r_matrix_ev[i][x] = r1;
            k->r_matrix_od[i][x] = r2;
        }
    }
    
    // Green
    for (i = 0; i < (in_fmt->g_mask+1); i++) {
        for (x = 0; x < 16; x++) {
            g1 = i;
            g2 = i;
            
            // Run through matrices
            g1 *= crt_mask[0][x];
            g2 *= crt_mask[1][x];
            
            // Scale
            g1 *= (float)out_fmt->g_mask / (float)in_fmt->g_mask;
            g2 *= (float)out_fmt->g_mask / (float)in_fmt->g_mask;
            
            // RGB Matrix
            g1 += crt_rgb[0][x][1] * ((float)in_fmt->g_mask / (float)out_fmt->g_mask);
            g2 += crt_rgb[1][x][1] * ((float)in_fmt->g_mask / (float)out_fmt->g_mask);
            
            // Scanline brightness/contrast
            if (x >= 8) {
                r1 = ((scan_slope * (1.0f / (float)out_fmt->g_mask)) *
                     r1 + scan_intersect) * (float)out_fmt->g_mask;
                r2 = ((scan_slope * (1.0f / (float)out_fmt->g_mask)) *
                     r2 + scan_intersect) * (float)out_fmt->g_mask;
            }
            
            // Clamp
            CLAMP(g1, 0.0f, (float)out_fmt->g_mask);
            CLAMP(g2, 0.0f, (float)out_fmt->g_mask);
            
            // Copy
            k->g_matrix_ev[i][x] = g1;
            k->g_matrix_od[i][x] = g2;
        }
    }
    
    // Blue
    for (i = 0; i < (in_fmt->b_mask+1); i++) {
        for (x = 0; x < 16; x++) {
            b1 = i;
            b2 = i;
            
            // Run through matrices
            b1 *= crt_mask[0][x];
            b2 *= crt_mask[1][x];
            
            // Scale
            b1 *= (float)out_fmt->b_mask / (float)in_fmt->b_mask;
            b2 *= (float)out_fmt->b_mask / (float)in_fmt->b_mask;
            
            // RGB Matrix
            b1 += crt_rgb[0][x][2] * ((float)in_fmt->b_mask / (float)out_fmt->b_mask);
            b2 += crt_rgb[1][x][2] * ((float)in_fmt->b_mask / (float)out_fmt->b_mask);
            
            // Scanline brightness/contrast
            if (x >= 8) {
                r1 = ((scan_slope * (1.0f / (float)out_fmt->b_mask)) *
                     r1 + scan_intersect) * (float)out_fmt->b_mask;
                r2 = ((scan_slope * (1.0f / (float)out_fmt->b_mask)) *
                     r2 + scan_intersect) * (float)out_fmt->b_mask;
            }
            
            // Clamp
            CLAMP(b1, 0.0f, (float)out_fmt->b_mask);
            CLAMP(b2, 0.0f, (float)out_fmt->b_mask);
            
            // Copy
            k->b_matrix_ev[i][x] = b1;
            k->b_matrix_od[i][x] = b2;
        }
    }
    
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
            float matrix_r[256][16],
            float matrix_g[256][16],
            float matrix_b[256][16],
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
    float prev_y, prev_i, prev_q;
    float next_y, next_i, next_q;
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
    rgb_to_yiq(k->in_fmt, in[i1+1], &next_y, &next_i, &next_q);
        
    //cur_y = (cur_y + next_y) / 2.0f;
    cur_i = (cur_i + next_i) / 2.0f;
    cur_q = (cur_q + next_q) / 2.0f;
    
    prev_y = cur_y;
    prev_i = cur_i;
    prev_q = cur_q;
    
    //sum_y = cur_y;
    sum_i = cur_i;
    sum_q = cur_q;
    
    for (x = 0; x < in_width; x++) {
        /*
        
void rgb_to_yiq(
        struct rgb_format *fmt,
        uint32_t rgb,
        float *y,
        float *i,
        float *q);

void yiq_to_rgb_unpacked(
        struct rgb_format *fmt,
        uint8_t *ro,
        uint8_t *go,
        uint8_t *bo,
        float y,
        float i,
        float q);
        
        */
    
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
        
        // Blur
        work_y = (prev_y * 0.2f) + (next_y * 0.2f) + (cur_y * 0.6f);
        work_i = (prev_i * 0.2f) + (next_i * 0.2f) + (cur_i * 0.56f);
        work_q = (prev_q * 0.2f) + (next_q * 0.2f) + (cur_q * 0.56f);
        
        /**
        
        TODO:  Don't use rgb24_to_yiq. Use format agnostic version instead,
               or promote formats to RGB24 automatically.
        
        **/
        
        // Deluma
        work_y *= k->deluma;
        
        // Dechroma
        work_i *= k->dechroma;
        work_q *= k->dechroma;
        
        // Set prev YIQ
        prev_y = cur_y;
        prev_i = cur_i;
        prev_q = cur_q;
        
        // Get next YIQ
        if (i1 + 2 < in_width) {
            rgb_to_yiq(k->in_fmt, in[i1+2], &next_y, &next_i, &next_q);
        } else {
            next_y = tmp_y;
            next_i = tmp_i;
            next_q = tmp_q;
        }
        
        // Get RGB from YIQ
        yiq_to_rgb_unpacked(k->in_fmt, &r1, &g1, &b1, work_y, work_i, work_q);
    
        // Pack pixels into output, using lookup matrices
        PACK_RGB(
                (int)matrix_r[r1][0],
                (int)matrix_g[g1][0],
                (int)matrix_b[b1][0], *(k->out_fmt), out_ln1[i2]);
                
        PACK_RGB(
                (int)matrix_r[r1][1],
                (int)matrix_g[g1][1],
                (int)matrix_b[b1][1], *(k->out_fmt), out_ln1[(i2) + 1]);
            
        PACK_RGB(
                (int)matrix_r[r1][2],
                (int)matrix_g[g1][2],
                (int)matrix_b[b1][2], *(k->out_fmt), out_ln1[(i2) + 2]);
            
        PACK_RGB(
                (int)matrix_r[r1][3],
                (int)matrix_g[g1][3],
                (int)matrix_b[b1][3], *(k->out_fmt), out_ln1[(i2) + 3]);
            
        PACK_RGB(
                (int)matrix_r[r1][4],
                (int)matrix_g[g1][4],
                (int)matrix_b[b1][4], *(k->out_fmt), out_ln2[(i2) + 0]);
                
        PACK_RGB(
                (int)matrix_r[r1][5],
                (int)matrix_g[g1][5],
                (int)matrix_b[b1][5], *(k->out_fmt), out_ln2[(i2) + 1]);
                
        PACK_RGB(
                (int)matrix_r[r1][6],
                (int)matrix_g[g1][6],
                (int)matrix_b[b1][6], *(k->out_fmt), out_ln2[(i2) + 2]);
            
        PACK_RGB(
                (int)matrix_r[r1][7],
                (int)matrix_g[g1][7],
                (int)matrix_b[b1][7], *(k->out_fmt), out_ln2[(i2) + 3]);
            
        PACK_RGB(
                (int)matrix_r[r1][8],
                (int)matrix_g[g1][8],
                (int)matrix_b[b1][8], *(k->out_fmt), out_ln3[(i2) + 0]);
                
        PACK_RGB(
                (int)matrix_r[r1][9],
                (int)matrix_g[g1][9],
                (int)matrix_b[b1][9], *(k->out_fmt), out_ln3[(i2) + 1]);
                
        PACK_RGB(
                (int)matrix_r[r1][10],
                (int)matrix_g[g1][10],
                (int)matrix_b[b1][10], *(k->out_fmt), out_ln3[(i2) + 2]);
                
        PACK_RGB(
                (int)matrix_r[r1][11],
                (int)matrix_g[g1][11],
                (int)matrix_b[b1][11], *(k->out_fmt), out_ln3[(i2) + 3]);
            
        PACK_RGB(
                (int)matrix_r[r1][12],
                (int)matrix_g[g1][12],
                (int)matrix_b[b1][12], *(k->out_fmt), out_ln4[(i2) + 0]);
                
        PACK_RGB(
                (int)matrix_r[r1][13],
                (int)matrix_g[g1][13],
                (int)matrix_b[b1][13], *(k->out_fmt), out_ln4[(i2) + 1]);
                
        PACK_RGB(
                (int)matrix_r[r1][14],
                (int)matrix_g[g1][14],
                (int)matrix_b[b1][14], *(k->out_fmt), out_ln4[(i2) + 2]);
            
        PACK_RGB(
                (int)matrix_r[r1][15],
                (int)matrix_g[g1][15],
                (int)matrix_b[b1][15], *(k->out_fmt), out_ln4[(i2) + 3]);

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
            k->r_matrix_od,
            k->g_matrix_od,
            k->b_matrix_od,
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
            k->r_matrix_ev,
            k->g_matrix_ev,
            k->b_matrix_ev,
            in_width,
            out_width,
            y);
    }
}
