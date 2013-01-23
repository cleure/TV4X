#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <getopt.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

#include "rgb.h"
#include "_tv2x.h"
#include "pngutil.h"

struct setup {
    float rgb_levels[3];
    float scan_rgb_levels[3];
    float bc[2];
    float scan_bc[2];
    float rgb_matrix_min[3];
    float rgb_matrix_max[3];
};

void help()
{
    printf("Usage:\n"
    "./tv2x-filter [options] <input> <output>\n"
    "Options:\n"
        "    --brightness <float>       - Brightness (-100-100)\n"
        "    --constrast <float>        - Contrast (-100-100)\n"
        "    --scan-brightness <float>  - Scan Brightness (-100-100)\n"
        "    --scan-constrast <float>   - Scan Contrast (-100-100)\n"
        "    --red <float>              - Red Level (0-100)\n"
        "    --green <float>            - Green Level (0-100)\n"
        "    --blue <float>             - Blue Level (0-100)\n"
        "    --scan-red <float>         - Scan Red Level (0-100)\n"
        "    --scan-green <float>       - Scan Green Level (0-100)\n"
        "    --scan-blue <float>        - Scan Blue Level (0-100)\n"
    );
    
    exit(0);
}

void parse_options(int argc, char **argv, struct setup *s)
{
    static struct option options[] = {
        {"brightness", required_argument,       0, 'b'},
        {"contrast", required_argument,         0, 'c'},
        {"scan-brightness", required_argument,  0, '1'},
        {"scan-contrast", required_argument,    0, '2'},
        {"scan-red", required_argument,         0, '3'},
        {"scan-green", required_argument,       0, '4'},
        {"scan-blue", required_argument,        0, '5'},
        {"red", required_argument,              0, '6'},
        {"green", required_argument,            0, '7'},
        {"blue", required_argument,             0, '8'},
        {"help", no_argument,                   0, 'h'},
        {0, 0, 0, 0}
    };
    
    int argchr,
        option_idx;
    
    do {
        argchr = getopt_long(
                    argc,
                    argv,
                    "bc12345678h:",
                    options,
                    &option_idx);
        
        switch (argchr) {
            case 'b': s->bc[0]              = atof(optarg); break;
            case 'c': s->bc[1]              = atof(optarg); break;
            case '1': s->scan_bc[0]         = atof(optarg); break;
            case '2': s->scan_bc[1]         = atof(optarg); break;
            case '6': s->rgb_levels[0]      = atof(optarg); break;
            case '7': s->rgb_levels[1]      = atof(optarg); break;
            case '8': s->rgb_levels[2]      = atof(optarg); break;
            case '3': s->scan_rgb_levels[0] = atof(optarg); break;
            case '4': s->scan_rgb_levels[1] = atof(optarg); break;
            case '5': s->scan_rgb_levels[2] = atof(optarg); break;
            case 'h': help(); break;
            default: break;
        }
    } while (argchr != -1);
}

void check_setup(struct setup *s)
{
    #define ERRCHK(cond, msg)\
        if (cond) {\
            fprintf(stderr, "%s\n", msg);\
            exit(1);\
        }


    ERRCHK( (s->bc[0] > 100.0 || s->bc[0] < -100.0),
            "Brightness out of range (should be between -100 and 100)");

    ERRCHK( (s->bc[1] > 100.0 || s->bc[1] < -100.0),
            "Contrast out of range (should be between -100 and 100)");
    
    ERRCHK( (s->scan_bc[0] > 100.0 || s->scan_bc[0] < -100.0),
            "Scanline Brightness out of range (should be between -100 and 100)");
    
    ERRCHK( (s->scan_bc[1] > 100.0 || s->scan_bc[1] < -100.0),
            "Scanline Contrast out of range (should be between -100 and 100)");
    
    ERRCHK( (s->rgb_levels[0] > 100.0 || s->rgb_levels[0] < 0.0),
            "Red Level out of range (should be between 0 and 100)");
    
    ERRCHK( (s->rgb_levels[1] > 100.0 || s->rgb_levels[1] < 0.0),
            "Green Level out of range (should be between 0 and 100)");
    
    ERRCHK( (s->rgb_levels[2] > 100.0 || s->rgb_levels[2] < 0.0),
            "Blue Level out of range (should be between 0 and 100)");
    
    ERRCHK( (s->scan_rgb_levels[0] > 100.0 || s->scan_rgb_levels[0] < 0.0),
            "Scanline Red Level out of range (should be between 0 and 100)");
    
    ERRCHK( (s->scan_rgb_levels[1] > 100.0 || s->scan_rgb_levels[1] < 0.0),
            "Scanline Green Level out of range (should be between 0 and 100)");
    
    ERRCHK( (s->scan_rgb_levels[2] > 100.0 || s->scan_rgb_levels[2] < 0.0),
            "Scanline Blue Level out of range (should be between 0 and 100)");
    
    #undef ERRCHK
}

/*

RG BR GB
static float rgb_matrix[3][2][3] = {
    {{1.10f, 1.00f, 1.00f}, {1.00f, 1.10f, 1.00f}},
    {{1.00f, 1.00f, 1.10f}, {1.10f, 1.00f, 1.00f}},
    {{1.00f, 1.10f, 1.00f}, {1.00f, 1.00f, 1.10f}},
};

*/

int main(int argc, char **argv)
{
    int width, height;
    int out_width, out_height;
    
    /* For benchmarking */
    struct timeval start, end;
    double start_t, end_t;
    
    /* Setup */
    struct setup s = {
        {100.0f, 100.0f, 100.0f},
        {100.0f, 100.0f, 100.0f},
        {8.0f, 10.0f},
        {-30.0f, 10.0f},
        {0.97f, 0.97f, 0.97f},
        {1.03f, 1.03f, 1.03f}
    };
    
    /* Kernel, data pointers */
    struct tv2x_kernel kern;
    tv2x_in_type *in;
    tv2x_out_type *out;
    
    /* I/O */
    char *infile,
         *outfile;
    
    /* Setup */
    parse_options(argc, argv, &s);
    check_setup(&s);
    
    if (argc < 3) {
        help();
    }
    
    /* Get in/out files */
    outfile = argv[argc-1];
    infile = argv[argc-2];
    
    /* Read file */
    in = rgb24_from_png(infile, &width, &height);
    
    out_width = width * 2;
    out_height = height * 2;
    out = malloc(sizeof(*out) * width * height * 2 * 2);
    
    /* Initialize kernel */
    tv2x_init_kernel(
            &kern,
            s.bc[0],
            s.bc[1],
            s.scan_bc[0],
            s.scan_bc[1],
            (float *)&s.rgb_levels,
            (float *)&s.scan_rgb_levels,
            s.rgb_matrix_min,
            s.rgb_matrix_max,
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
    rgb24_to_png(out, out_width, out_height, outfile);
    
    /* Free */
    free(in);
    free(out);

    exit(0);
}
