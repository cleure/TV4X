#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include "rgb.h"
#include "_tv2x.h"
#include "pngutil.h"

int main(int argc, char **argv)
{
    int width, height;
    int out_width, out_height;
    
    /* For benchmarking */
    struct timeval start, end;
    double start_t, end_t;
    
    float rgb_levels[3] = {100.0f, 100.0f, 100.0f};
    float scan_rgb_levels[3] = {100.0f, 100.0f, 100.0f};
    
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
            10.0f,
            -30.0f,
            10.0f,
            (float *)&rgb_levels,
            (float *)&scan_rgb_levels,
            &tvxx_rgb_format_rgb24);
        
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
