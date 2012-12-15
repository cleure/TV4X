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
            width,
            width,
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
