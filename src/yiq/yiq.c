#include "tv4x.h"
#include "rgb.h"
#include "yiq.h"

#ifdef TV4X_USE_SSE
    #include <xmmintrin.h>
#endif

/* RGB to YIQ Matrix */
static const float yiq_in_matrix[3][3] = {
    {0.299f,     0.587f,    0.114f},
    {0.596f,    -0.274f,   -0.322f},
    {0.212f,    -0.523f,    0.311f}
};

/* YIQ to RGB Matrix */
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
void TV4X_INLINE tv4x_rgb_to_yiq(
        struct tv4x_rgb_format *fmt,
        uint32_t rgb,
        float *y,
        float *i,
        float *q) {
    
    float r, g, b;
    
    UNPACK_RGB(r, g, b, *fmt, rgb);
    
    #ifdef TV4X_YIQ_SCALE_RGB
        /* Scale up */
        r *= (float)tv4x_rgb_format_rgb24.r_mask / (float)fmt->r_mask;
        g *= (float)tv4x_rgb_format_rgb24.g_mask / (float)fmt->g_mask;
        b *= (float)tv4x_rgb_format_rgb24.b_mask / (float)fmt->b_mask;
    #endif
    
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
void TV4X_INLINE tv4x_yiq_to_rgb_unpacked(
        struct tv4x_rgb_format *fmt,
        uint8_t *ro,
        uint8_t *go,
        uint8_t *bo,
        float y,
        float i,
        float q) {

    float r, g, b;
    
    #ifdef TV4X_USE_SSE
        __m128 maxclamp;
        __m128 minclamp;
        __m128 result;
        float *rgb;
    #endif
    
    /* Get R */
    r = (y) +
        (yiq_out_matrix[0][1] * i) +
        (yiq_out_matrix[0][2] * q);
    
    /* Get G */
    g = (y) +
        (yiq_out_matrix[1][1] * i) +
        (yiq_out_matrix[1][2] * q);

    /* Get B */
    b = (y) +
        (yiq_out_matrix[2][1] * i) +
        (yiq_out_matrix[2][2] * q);
    
    /* Round */
    /*
    r += 0.5;
    g += 0.5;
    b += 0.5;
    */
    
    /* Scale down */
    #ifdef TV4X_YIQ_SCALE_RGB
        r *= ((float)fmt->r_mask / (float)tv4x_rgb_format_rgb24.r_mask);
        g *= ((float)fmt->g_mask / (float)tv4x_rgb_format_rgb24.g_mask);
        b *= ((float)fmt->b_mask / (float)tv4x_rgb_format_rgb24.b_mask);
    #endif
    
    /* Clamp */
    #ifdef TV4X_USE_SSE
        maxclamp = _mm_setr_ps(31.0f, 31.0f, 31.0f, 0.0f);
        minclamp = _mm_setr_ps( 0.0f,  0.0f,  0.0f, 0.0f);
        result   = _mm_setr_ps(r, g, b, 0.0f);
    
        result = _mm_min_ps(maxclamp, result);
        result = _mm_max_ps(minclamp, result);
    
        rgb = ((float *)&result);
    
        r = *(rgb+0);
        g = *(rgb+1);
        b = *(rgb+2);
    #else
        CLAMP(r, 0, fmt->r_mask);
        CLAMP(g, 0, fmt->g_mask);
        CLAMP(b, 0, fmt->b_mask);
    #endif
    
    *ro = (uint8_t)r;
    *go = (uint8_t)g;
    *bo = (uint8_t)b;
}
