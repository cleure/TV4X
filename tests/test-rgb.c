#include <stdio.h>
#include <stdlib.h>
#include "rgb.h"

int main(int argc, char **argv)
{
    uint32_t r, g, b;
    uint16_t *in = malloc(sizeof(*in) * 12);
    uint32_t *out = malloc(sizeof(*out) * 12);
    
    in[0] = 16 << RGB15_R_SHIFT | 12 << RGB15_G_SHIFT | 31 << RGB15_B_SHIFT;
    
    //rgb_convert_15to24_init(&config);
    rgb_convert(&rgb_format_rgb15, &rgb_format_rgb24, (uint32_t *)in, out, 1);
    
    r = in[0] >> RGB15_GET_R;
    g = in[0] >> RGB15_GET_G;
    b = in[0] >> RGB15_GET_B;
    
    printf("IN\t%d\t%d\t%d\n", r, g, b);
    
    r = out[0] >> RGB24_GET_R;
    g = out[0] >> RGB24_GET_G;
    b = out[0] >> RGB24_GET_B;
    
    printf("OUT\t%d\t%d\t%d\n", r, g, b);
    
    printf("%d\n", (uint8_t)(31 * 8.225806236267f));

    free(in);
    free(out);
    exit(0);
}
