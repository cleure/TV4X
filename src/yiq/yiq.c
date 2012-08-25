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
* Convert RGB24 to YIQ. This is more optimized than the more generic version,
* but it can't convert from other RGB formats (like RGB15).
*
* @param    uint32_t rgb24
* @param    float *y
* @param    float *i
* @param    float *q
* @return   void
**/
void inline rgb24_to_yiq(uint32_t rgb24, float *y, float *i, float *q)
{
    float r, g, b;
    
    UNPACK_RGB(r, g, b, rgb_format_rgb24, rgb24);
    
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
* Convert YIQ to RGB24.
*
* @param    uint32_t *rgb24
* @param    float y
* @param    float i
* @param    float q
* @return   void
**/
void inline yiq_to_rgb24(uint32_t *rgb24, float y, float i, float q)
{
    float r = 0.0f, g = 0.0f, b = 0.0f;
    
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
    
    // Clamp
    if (r > 255)     r = 255;
    else if (r < 0)  r = 0;
    
    if (g > 255)     g = 255;
    else if (g < 0)  g = 0;
    
    if (b > 255)     b = 255;
    else if (b < 0)  b = 0;
    
    PACK_RGB((uint8_t)r, (uint8_t)g, (uint8_t)b, rgb_format_rgb24, *rgb24);
}

/**
* Convert YIQ to RGB24, unpacked.
*
* @param    uint8_t *r
* @param    uint8_t *g
* @param    uint8_t *b
* @param    float y
* @param    float i
* @param    float q
* @return   void
**/
void inline yiq_to_rgb24_unpacked(uint8_t *ro, uint8_t *go, uint8_t *bo, float y, float i, float q)
{
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
    
    // Clamp
    if (r > 255)     r = 255;
    else if (r < 0)  r = 0;
    
    if (g > 255)     g = 255;
    else if (g < 0)  g = 0;
    
    if (b > 255)     b = 255;
    else if (b < 0)  b = 0;
    
    *ro = (uint8_t)r;
    *go = (uint8_t)g;
    *bo = (uint8_t)b;
}
