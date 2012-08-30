#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "rgb.h"
#include "yiq.h"
#include "tv4x.h"
#include "pngutil.h"

int main(int argc, char **argv)
{
    int width, height;
    int out_width, out_height;
    
    if (argc < 2) {
        fprintf(stderr, "Usage: test-filter <input PNG>\n");
        exit(1);
    }
    
    // For benchmarking
    clock_t start_time,
            end_time;
    
    // Kernel, data pointers
    struct tv4x_kernel kern;
    tv4x_in_type *in;
    tv4x_out_type *out;
    
    // Read file
    in = rgb24_from_png(argv[1], &width, &height);
    
    // Convert to RGB15
    rgb_convert(
        &rgb_format_rgb24,
        &rgb_format_rgb15,
        in, in,
        width * height);
    
    out_width = width * 4;
    out_height = height * 4;
    out = malloc(sizeof(*out) * width * height * 4 * 4);
    
    // Initialize kernel
    tv4x_init_kernel(
            &kern,
            &rgb_format_rgb15,
            &rgb_format_rgb24,
            &tv4x_setup_composite,
            tv4x_crt_silly,
            tv4x_crt_silly_phosphor,
            0.96f,
            0.9f,
            width);
    
    // Process
    start_time = clock();
    tv4x_process(
            &kern,
            in,
            out,
            width,
            width*4*4,
            width,
            height);
        
    end_time = clock();
    printf("Time: %f\n", (end_time - start_time) / (float)CLOCKS_PER_SEC);

    // Save
    rgb24_to_png(out, out_width, out_height, "out.png");
    
    // Free
    tv4x_free_kernel(&kern);
    free(in);
    free(out);

    exit(0);
}
