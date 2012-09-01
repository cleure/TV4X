#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include "rgb.h"
#include "yiq.h"
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
    struct tv4x_kernel kern;
    tv4x_in_type *in;
    tv4x_out_type *out;
    
    if (argc < 2) {
        fprintf(stderr, "Usage: test-filter <input PNG>\n");
        exit(1);
    }
    
    #if defined(__clang__)
        printf("Clang\n");
    #endif
    
    /* Read file */
    in = rgb24_from_png(argv[1], &width, &height);
    
    /* Convert to RGB15 */
    rgb_convert(
        &tv4x_rgb_format_rgb24,
        &tv4x_rgb_format_rgb15,
        in, in,
        width * height);
    
    out_width = width * 4;
    out_height = height * 4;
    out = malloc(sizeof(*out) * width * height * 4 * 4);
    
    /* Initialize kernel */
    tv4x_init_kernel(
            &kern,
            &tv4x_rgb_format_rgb15,
            &tv4x_rgb_format_rgb24,
            &tv4x_setup_composite,
            tv4x_crt_slotmask,
            tv4x_crt_slotmask_phosphor,
            2.0f,
            5.0f,
            -12.0f,
            12.0f,
            0.96f,
            0.9f,
            width);
    
    /* Process */
    gettimeofday(&start, NULL);
    tv4x_process(
            &kern,
            in,
            out,
            width,
            width*4*4,
            width,
            height);
    
    gettimeofday(&end, NULL);
    
    start_t = start.tv_sec + (start.tv_usec / 1000000.0);
    end_t = end.tv_sec + (end.tv_usec / 1000000.0);
    printf("Time: %2.8f\n", (end_t - start_t));

    /* Save */
    rgb24_to_png(out, out_width, out_height, "out.png");
    
    /* Free */
    tv4x_free_kernel(&kern);
    free(in);
    free(out);

    exit(0);
}
