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
    
    /*
    uint8_t *buffer8 = (void *)0xaabbccdd;
    uint16_t *buffer16 = (void *)0xaabbccdd;
    uint32_t *buffer32 = (void *)0xaabbccdd;
    
    buffer8 = &buffer8[32 * 1024];
    buffer16 = &((uint8_t *)buffer16)[32 * 1024];
    buffer32 = &buffer32[32 * 1024];
    
    printf("%p\n", (void *)buffer8);
    printf("%p\n", (void *)buffer16);
    printf("%p\n", (void *)buffer32);
    
    exit(0);*/
    
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
            8.0f,
            18.0f,
            20.0f,
            10.0f,
            &tvxx_rgb_format_rgb24);
    
    /*
    
int tv2x_init_kernel(
            struct tv2x_kernel *kernel,
            float brightness,
            float contrast,
            float scan_brightness,
            float scan_contrast,
            struct tvxx_rgb_format *in_fmt,
            struct tvxx_rgb_format *out_fmt);

void tv2x_process(
            struct tv2x_kernel *k,
            tv2x_in_type * TVXX_RESTRICT in,
            tv2x_out_type * TVXX_RESTRICT out,
            uint32_t in_pitch,
            uint32_t out_pitch,
            uint32_t in_width,
            uint32_t in_height);
    
    */
    
    /* Process */
    gettimeofday(&start, NULL);

    tv2x_process(
            &kern,
            in,
            out,
            width*sizeof(*in),
            width*sizeof(*out)*2,
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
