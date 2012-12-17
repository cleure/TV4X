#ifndef TVXX_PNGUTIL_H
#define TVXX_PNGUTIL_H

#include <stdint.h>

#ifdef __cplusplus
    extern "C" {
#endif

/* Get RGB byte buffer from PNG file */
uint8_t *buffer_from_png(char *path, int *width, int *height);

/* Write RGB byte buffer to PNG file */
int buffer_to_png(uint8_t *buffer, int width, int height, char *path);

/* Convert PNG buffer to Packed RGB24 */
uint32_t *png_buf_to_rgb24(uint8_t *in, int width, int height);

/* Read PNG file to RGB24 Buffer */
uint32_t *rgb24_from_png(const char *path, int *width, int *height);

/* Write PNG file from RGB24 Buffer */
int rgb24_to_png(uint32_t *buffer, int width, int height, char *path);

#ifdef __cplusplus
    }
#endif

/* TVXX_PNGUTIL_H */ #endif
