#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <png.h>

#include "pngutil.h"
#include "rgb.h"

/**
* Read PNG file to RGB24 Buffer
*
* @param    char *path
* @param    int *width
* @param    int *height
* @return   pointer or NULL
**/
uint32_t *rgb24_from_png(const char *path, int *width, int *height)
{
    int x, y;
    uint8_t r, g, b;
    uint32_t *buffer, *out;

    /* libpng stuff */
    png_structp png_ptr;
    png_infop info_ptr;
    png_byte channels;
    png_bytep *row_pointers;

    /* Open file */
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        return NULL;
    }
    
    /* Initialize */
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return NULL;
    }
    
    /* Error handling */
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        if (fp) {
            fclose(fp);
        }
        
        return NULL;
    }
    
    /* Init PNG stuff */
    png_init_io(png_ptr, fp);
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
    fclose(fp); fp = NULL;
    
    /* Get width / height, channels */
    *width = png_get_image_width(png_ptr, info_ptr);
    *height = png_get_image_height(png_ptr, info_ptr);
    channels = png_get_channels(png_ptr, info_ptr);
 
    /* Setup buffers */
    row_pointers = png_get_rows(png_ptr, info_ptr);
    
    buffer = malloc(sizeof(*buffer) * (*width) * (*height));
    if (!buffer) {
        return NULL;
    }
    
    printf("Channels: %d\n", channels);
    
    /* Copy to buffer, removing alpha */
    out = buffer;
    
    for (y = 0; y < *height; y++) {
        for (x = 0; x < *width; x++) {
            r = row_pointers[y][(x * channels) + 0],
            g = row_pointers[y][(x * channels) + 1],
            b = row_pointers[y][(x * channels) + 2];
            
            PACK_RGB(r, g, b, tv4x_rgb_format_rgb24, *out);
            out++;
        }
    }
 
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return buffer;

}

/**
* Save PNG file from RGB24 buffer
*
* @param    uint32_t *buffer
* @param    int width
* @param    int height
* @param    char *path
* @return   -1 on error
**/
int rgb24_to_png(uint32_t *buffer, int width, int height, char *path)
{
    int x, y;
    int bytes_per_row = width * 3;
    
    uint8_t r, g, b;
    uint32_t *in = buffer;
    
    /* libpng stuff */
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    png_byte **row_pointers = NULL;

    FILE *fp = fopen(path, "wb");
    if (!fp) {
        return -1;
    }
    
    /* Initialize */
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        fclose(fp);
        return -1;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        png_destroy_write_struct(&png_ptr, NULL);
        fclose(fp);
        return -1;
    }

    /* Error Handling */
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
        return -1;
    }
    
    png_set_IHDR(png_ptr, info_ptr, width, height, 8,
        PNG_COLOR_TYPE_RGB,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT);

    /* Copy RGB buffer into PNG buffer */
    row_pointers = png_malloc(png_ptr, height * sizeof(png_byte *));
    
    for (y = 0; y < height; y++) {
        /* Allocate */
        row_pointers[y] = png_malloc(png_ptr, sizeof(uint8_t) * bytes_per_row);
        
        for (x = 0; x < width; x++) {
            /* Unpack pixels */
            UNPACK_RGB(r, g, b, tv4x_rgb_format_rgb24, *in);
            /*UNPACK_RGB(r, g, b, tv4x_rgb_format_rgb15, *in);
            
            r *= (255/31);
            g *= (255/31);
            b *= (255/31);*/
            
            /* Copy */
            row_pointers[y][(x * 3) + 0] = r;
            row_pointers[y][(x * 3) + 1] = g;
            row_pointers[y][(x * 3) + 2] = b;
            
            /* Increment RGB24 pointer */
            in++;
        }
    }

    /* Write file */
    png_init_io(png_ptr, fp);
    png_set_rows(png_ptr, info_ptr, row_pointers);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    /* Free memory, cleanup */
    for (y = 0; y < height; y++) {
        png_free(png_ptr, row_pointers[y]);
    }
    
    png_free(png_ptr, row_pointers);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    return 0;
}

/**
* PNG File to RGB byte buffer (8 bits per channel).
*
* @param    char *path
* @param    int *width
* @param    int *height
* @return   pointer or NULL
**/
uint8_t *buffer_from_png(char *path, int *width, int *height)
{
    int x, y, i;
    int bytes_per_row;
    uint8_t *buffer;
    
    /* libpng stuff */
    png_structp png_ptr;
    png_infop info_ptr;
    png_byte channels;
    png_bytep *row_pointers;

    /* Open file */
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        return NULL;
    }
    
    /* Initialize */
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return NULL;
    }
    
    /* Error handling */
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        if (fp) {
            fclose(fp);
        }
        
        return NULL;
    }
    
    /* Init PNG stuff */
    png_init_io(png_ptr, fp);
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
    fclose(fp); fp = NULL;
    
    /* Get width / height, channels */
    *width = png_get_image_width(png_ptr, info_ptr);
    *height = png_get_image_height(png_ptr, info_ptr);
    channels = png_get_channels(png_ptr, info_ptr);
 
    /* Setup buffers */
    row_pointers = png_get_rows(png_ptr, info_ptr);
    buffer = malloc(((*width) * 3) * (*height));
    if (!buffer) {
        return NULL;
    }
    
    printf("Channels: %d\n", channels);
    
    /* Copy to buffer, removing alpha */
    bytes_per_row = (*width) * channels;
    i = 0;
    for (y = 0; y < (*height); ++y) {
        for (x = 0; x < bytes_per_row; ++x) {
            /* Skip over alpha channel, if present */
            if (channels < 4 || ((x + 1) % 4)) {
                buffer[i] = row_pointers[y][x];
                ++i;
            }
        }
    }
 
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return buffer;
}

/**
* RGB byte [8 bits per channel] buffer to PNG file.
*
* @param    uint8_t *buffer
* @param    int width
* @param    int height
* @param    char *path
* @return   -1 on error
**/
int buffer_to_png(uint8_t *buffer, int width, int height, char *path)
{
    int x, y, i;
    int bytes_per_row = width * 3;
    
    /* libpng stuff */
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    png_byte **row_pointers = NULL;

    FILE *fp = fopen(path, "wb");
    if (!fp) {
        return -1;
    }
    
    /* Initialize */
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        fclose(fp);
        return -1;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        png_destroy_write_struct(&png_ptr, NULL);
        fclose(fp);
        return -1;
    }

    /* Error Handling */
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
        return -1;
    }
    
    png_set_IHDR(png_ptr, info_ptr, width, height, 8,
        PNG_COLOR_TYPE_RGB,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT);

    /* Copy RGB buffer into PNG buffer */
    row_pointers = png_malloc(png_ptr, height * sizeof(png_byte *));
    i = 0;
    for (y = 0; y < height; ++y) {
        uint8_t *data = png_malloc(png_ptr, sizeof(uint8_t) * bytes_per_row);
        row_pointers[y] = (png_byte *)data;
        
        for (x = 0; x < width; ++x) {
            *data++ = buffer[i++];
            *data++ = buffer[i++];
            *data++ = buffer[i++];
        }
    }

    /* Write file */
    png_init_io(png_ptr, fp);
    png_set_rows(png_ptr, info_ptr, row_pointers);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    /* Free memory, cleanup */
    for (y = 0; y < height; y++) {
        png_free(png_ptr, row_pointers[y]);
    }
    
    png_free(png_ptr, row_pointers);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    return 0;
}
