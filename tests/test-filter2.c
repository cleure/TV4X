#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include "rgb.h"
#include "tv2x.h"
#include "tv4x.h"
#include "pngutil.h"

int main(int argc, char **argv)
{
    int width, height;
    int out_width, out_height;
    
    /* For benchmarking */
    struct timeval start, end;
    double start_t, end_t;
    
    /* Kernel, data pointers */
    struct tv2x_kernel kern;
    tv2x_in_type *in;
    tv2x_out_type *out;
    
    uint32_t rgb24d = 0x00f0ffff;
    uint32_t rgb15d = 0;
    uint32_t r, g, b;
    int y, z;
    
    struct tv4x_rgb_format *formats[3] = {
        &tv4x_rgb_format_rgb24,
        &tv4x_rgb_format_rgb16,
        &tv4x_rgb_format_rgb15};
    
    #define UNPACK_RGB_FMT(r, g, b, in_fmt, conv, in)\
        (r) =       ((in) >> (fmt).r_shift & (fmt).r_mask) * (conv)[0];\
        (g) =       ((in) >> (fmt).g_shift & (fmt).g_mask) * (conv)[1];\
        (b) =       ((in) >> (fmt).b_shift & (fmt).b_mask) * (conv)[2];
    
    UNPACK_RGB_FMT(r, g, b, tv4x_rgb_format_rgb24, tv4x_rgb24_to_rgb15, rgb24d);
    printf("%d %d %d\n", r, g, b);
    
/* Pack RGB */
#define PACK_RGB(r, g, b, fmt, out)\
    (out) =     (r) << (fmt).r_shift |\
                (g) << (fmt).g_shift |\
                (b) << (fmt).b_shift;
    
    /*
    r *= tv4x_rgb24_to_rgb15[0];
    g *= tv4x_rgb24_to_rgb15[1];
    b *= tv4x_rgb24_to_rgb15[2];
    printf("%d %d %d\n", r, g, b);
    
    r *= tv4x_rgb15_to_rgb16[0];
    g *= tv4x_rgb15_to_rgb16[1];
    b *= tv4x_rgb15_to_rgb16[2];
    printf("%d %d %d\n", r, g, b);
    
    r *= tv4x_rgb16_to_rgb24[0];
    g *= tv4x_rgb16_to_rgb24[1];
    b *= tv4x_rgb16_to_rgb24[2];
    printf("%d %d %d\n", r, g, b);*/
    
    /*
    for (y = 0; y < 3; y++) {
        for (z = 0; z < 3; z++) {
            printf("double tv4x_");
            if (formats[y] == &tv4x_rgb_format_rgb24) {
                printf("rgb24");
            } else if (formats[y] == &tv4x_rgb_format_rgb16) {
                printf("rgb16");
            } else if (formats[y] == &tv4x_rgb_format_rgb15) {
                printf("rgb15");
            }
            
            printf("_to_");
            if (formats[z] == &tv4x_rgb_format_rgb24) {
                printf("rgb24");
            } else if (formats[z] == &tv4x_rgb_format_rgb16) {
                printf("rgb16");
            } else if (formats[z] == &tv4x_rgb_format_rgb15) {
                printf("rgb15");
            }

            printf("[3] = {");
            printf("%2.16f, ", (double)formats[z]->r_mask / (double)formats[y]->r_mask);
            printf("%2.16f, ", (double)formats[z]->g_mask / (double)formats[y]->g_mask);
            printf("%2.16f  ", (double)formats[z]->b_mask / (double)formats[y]->b_mask);
            printf("};\n");
        }
    }*/
    
    exit(0);
    
    if (argc < 2) {
        fprintf(stderr, "Usage: test-filter <input PNG>\n");
        exit(1);
    }
    
    /* Read file */
    in = rgb24_from_png(argv[1], &width, &height);
    
    out_width = width * 2;
    out_height = height * 2;
    out = malloc(sizeof(*out) * width * height * 2 * 2);
    
    /* Initialize kernel */
    tv2x_init_kernel(
            &kern,
            0.0f,
            0.0f,
            20.0f,
            10.0f,
            &tv4x_rgb_format_rgb24,
            &tv4x_rgb_format_rgb24);
    
    /*
    
int tv2x_init_kernel(
            struct tv2x_kernel *kernel,
            float brightness,
            float contrast,
            float scan_brightness,
            float scan_contrast,
            struct tv4x_rgb_format *in_fmt,
            struct tv4x_rgb_format *out_fmt);

void tv2x_process(
            struct tv2x_kernel *k,
            tv2x_in_type *in,
            tv2x_out_type *out,
            int in_pitch,
            int out_pitch,
            int in_width,
            int in_height);
    
    */
    
    /* Process */
    gettimeofday(&start, NULL);

    tv2x_process(
            &kern,
            in,
            out,
            0,
            0,
            width,
            height);

    /*
    tv4x_process(
            &kern,
            in,
            out,
            width,
            width*4*4,
            width,
            height);*/
    
    gettimeofday(&end, NULL);
    
    start_t = start.tv_sec + (start.tv_usec / 1000000.0);
    end_t = end.tv_sec + (end.tv_usec / 1000000.0);
    printf("Time: %2.8f\n", (end_t - start_t));

    /* Save */
    rgb24_to_png(out, out_width, out_height, "out.png");
    
    /* Free */
    free(in);
    free(out);

    exit(0);
}
