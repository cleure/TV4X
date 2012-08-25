#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "pngutil.h"
#include "rgb.h"
#include "yiq.h"

uint32_t *png_buf_to_rgb24(uint8_t *in, int width, int height)
{
    uint32_t i, e;
    uint8_t r, g, b;
    
    uint32_t *buf = malloc(sizeof(*buf) * width * height);
    uint32_t *out = buf;
    
    if (!out) {
        return NULL;
    }
    
    e = width * height;
    for (i = 0; i < e; i++) {
        r = *in++;
        g = *in++;
        b = *in++;
        
        PACK_RGB(r, g, b, rgb_format_rgb24, *out);
        out++;
    }
  
    return buf;
}
