#pragma once
#include <stdint.h>

struct tv4x_setup {
    float y_step;
    float i_step;
    float q_step;
};

extern struct tv4x_setup tv4x_setup_coax;
extern struct tv4x_setup tv4x_setup_composite;
extern struct tv4x_setup tv4x_setup_svideo;
extern struct tv4x_setup tv4x_setup_rgb;

// CRT Shadow Masks
extern float tv4x_crt_slotmask[2][16];

// RGB Layouts
extern float tv4x_crt_slotmask_rgb[2][16][3];

struct tv4x_kernel {
    // I/O Formats
    struct rgb_format *in_fmt;
    struct rgb_format *out_fmt;
    
    // Setup Type
    struct tv4x_setup *setup;
    
    // CRT Mask
    float *crt_mask[2][16];
    
    // Event Map
    uint8_t *y_events;
    uint8_t *i_events;
    uint8_t *q_events;
    
    // RGB Lookup Matrices
    float r_matrix_ev[256][16];
    float g_matrix_ev[256][16];
    float b_matrix_ev[256][16];
    float r_matrix_od[256][16];
    float g_matrix_od[256][16];
    float b_matrix_od[256][16];
};

int tv4x_init_kernel(
        struct tv4x_kernel *k,
        struct rgb_format *in_fmt,
        struct rgb_format *out_fmt,
        struct tv4x_setup *setup,
        float crt_mask[2][16],
        float crt_rgb[2][16][3],
        int max_width);

void tv4x_free_kernel(struct tv4x_kernel *k);

void tv4x_process(
            struct tv4x_kernel *k,
            uint32_t *in,
            uint32_t *out,
            int in_width,
            int in_height);
