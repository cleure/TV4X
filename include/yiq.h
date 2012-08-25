#pragma once
#include <stdint.h>

// Convert RGB24 to YIQ
void rgb24_to_yiq(uint32_t rgb24, float *y, float *i, float *q);

// Convert YIQ to RGB24
void yiq_to_rgb24(uint32_t *rgb24, float y, float i, float q);

// Same as above, only doesn't pack them
void yiq_to_rgb24_unpacked(uint8_t *ro, uint8_t *go, uint8_t *bo, float y, float i, float q);
