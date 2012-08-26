#include <math.h>
#include "rgb.h"
#include "yiq.h"

// RGB to YIQ Matrix
static const float yiq_in_matrix[3][3] = {
    {0.299f,     0.587f,    0.114f},
    {0.596f,    -0.274f,   -0.322f},
    {0.212f,    -0.523f,    0.311f}
};

// YIQ to RGB Matrix
static const float yiq_out_matrix[3][3] = {
    {1.0f,      0.956f,     0.621f},
    {1.0f,     -0.272f,    -0.647f},
    {1.0f,     -1.105f,     1.702f}
};

/**
* Convert RGB15/16/24 to YIQ.
*
* @param    struct rgb_format *in_fmt
* @param    uint32_t rgb
* @param    float *y
* @param    float *i
* @param    float *q
* @return   void
**/
void inline rgb_to_yiq(
        struct rgb_format *fmt,
        uint32_t rgb,
        float *y,
        float *i,
        float *q) {
    
    float r, g, b;
    
    UNPACK_RGB(r, g, b, *fmt, rgb);
    
    // Scale up
    r *= (float)rgb_format_rgb24.r_mask / (float)fmt->r_mask;
    g *= (float)rgb_format_rgb24.g_mask / (float)fmt->g_mask;
    b *= (float)rgb_format_rgb24.b_mask / (float)fmt->b_mask;
    
    *y = (yiq_in_matrix[0][0] * r) +
         (yiq_in_matrix[0][1] * g) +
         (yiq_in_matrix[0][2] * b);
    
    *i = (yiq_in_matrix[1][0] * r) +
         (yiq_in_matrix[1][1] * g) +
         (yiq_in_matrix[1][2] * b);
    
    *q = (yiq_in_matrix[2][0] * r) +
         (yiq_in_matrix[2][1] * g) +
         (yiq_in_matrix[2][2] * b);
}

/**
* Convert YIQ to RGB15/16/24, unpacked.
*
* @param    struct rgb_format *in_fmt
* @param    uint8_t *r
* @param    uint8_t *g
* @param    uint8_t *b
* @param    float y
* @param    float i
* @param    float q
* @return   void
**/
void inline yiq_to_rgb_unpacked(
        struct rgb_format *fmt,
        uint8_t *ro,
        uint8_t *go,
        uint8_t *bo,
        float y,
        float i,
        float q) {

    float r, g, b;
    
    // Get R
    r = (yiq_out_matrix[0][0] * y) +
        (yiq_out_matrix[0][1] * i) +
        (yiq_out_matrix[0][2] * q);
    
    // Get G
    g = (yiq_out_matrix[1][0] * y) +
        (yiq_out_matrix[1][1] * i) +
        (yiq_out_matrix[1][2] * q);

    // Get B
    b = (yiq_out_matrix[2][0] * y) +
        (yiq_out_matrix[2][1] * i) +
        (yiq_out_matrix[2][2] * q);
    
    // Round
    r += 0.5;
    g += 0.5;
    b += 0.5;
    
    // Scale down
    r *= ((float)fmt->r_mask / (float)rgb_format_rgb24.r_mask);
    g *= ((float)fmt->g_mask / (float)rgb_format_rgb24.g_mask);
    b *= ((float)fmt->b_mask / (float)rgb_format_rgb24.b_mask);
    
    // Clamp
    CLAMP(r, 0, fmt->r_mask);
    CLAMP(g, 0, fmt->g_mask);
    CLAMP(b, 0, fmt->b_mask);
    
    *ro = (uint8_t)r;
    *go = (uint8_t)g;
    *bo = (uint8_t)b;
}
