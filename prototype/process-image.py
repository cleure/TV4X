
import os, sys

try:
    import png
except ImportError:
    sys.stderr.write('Failed to import PNG module\n')
    sys.stderr.write('Please install the PNG module to continue\n')
    sys.exit(1)

from image import *

def main():
    if len(sys.argv) < 3:
        print('Usage: python make-image.py <input> <output>')
        sys.exit(0)
    
    im = Image.fromPNG(sys.argv[1])
    
    """
    out = im.tv4x_scale(
        blur_factor=0.2,
        mult_matrix_odd=slot_mask_scanline[0],
        mult_matrix_even=slot_mask_scanline[1],
        rgb_matrix_odd=slot_mask_rgb_matrix[0],
        rgb_matrix_even=slot_mask_rgb_matrix[1],
        noise_probability=0.10,
        noise_max=0,
        brightness=0.0,
        contrast=4.0)
    """
    
    out = im.tv2x_scale()
    out.savePNG(sys.argv[2])
    sys.exit(0)

if __name__ == '__main__':
    main()
