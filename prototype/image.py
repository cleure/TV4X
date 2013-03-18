
import os, sys
import random, math, png, copy
import yiq

"""

Image Processing Framework, for fast prototyping of CRT-TV Emulation Filters.

The tv4x_scale method on the Image class, takes a variety of arguments
and uses them to upscale, and emulate the look of a CRT-TV. The "mult_matrix_even"
and "mult_matrix_odd" parameters are mainly useful for prototyping various pixel
layouts, such as dot mask, slot mask, and aperture grille.

Currently, it uses fixed matrix multiplication to calculate the 4x4 boxes that
represent a single pixel. For instance, the following multiplication matrix
would blow up a source pixel to 4 times it's size (ala nearest neighbor):
    [1.0, 1.0, 1.0, 1.0,
     1.0, 1.0, 1.0, 1.0,
     1.0, 1.0, 1.0, 1.0,
     1.0, 1.0, 1.0, 1.0]

Whereas, this matrix would add crude scalines to the resulting image:
    [1.0, 1.0, 1.0, 1.0,
     1.0, 1.0, 1.0, 1.0,
     0.8, 0.8, 0.8, 0.8,
     0.8, 0.8, 0.8, 0.8]

The pixel layouts for common CRT sets include the following:
    Dot Mask:  
        R G B
         R G B
        R G B
    
    Slot Mask:
        RGB     RGB
        RGB RGB RGB
        RGB RGB RGB
            RGB
    
    Aperture Grille (aka Trinitron):
        R G B R G B
        R G B R G B
        R G B R G B
        R G B R G B
        R G B R G B
        R G B R G B

Representing these pixel layouts is tricky, because consumer grade LCD's do not
yet have high enough pixel density to emulate them accurately. Because of this, we
have to make compromises on the amount of detail emulated.

-----------------------------------------------------------------------------------
Caveats, Issues, and Room for Improvement
-----------------------------------------------------------------------------------

The biggest issue with using fixed-matrix multiplication to generate the resulting
pixels, is that the darkness levels are not very well preserved. As a result, some
images will tend to have inaccurate brightness/contrast levels. One solution is to
use the value of the source pixel to determine the maximum amount that it can be
increased/decreased by, which could be accomplished using contrast-like math.

-----------------------------------------------------------------------------------
Hacking Internals - Some Useful Math
-----------------------------------------------------------------------------------

If you're in a loop such as:
    for y in range(0, height):
        for x in range(0, width):
            ...

You can easily calculate what the x/y coordinates would be at a different scale,
using some basic nearest neighbor calculations:
    y2 = int(math.ceil(
        (float(y) + 0.5) * (float(CUR_HEIGHT) / float(NEW_HEIGHT))
    ))
    
    x2 = int(math.ceil(
        (float(x) + 0.5) * (float(CUR_WIDTH) / float(NEW_WIDTH))
    ))

-----------------------------------------------------------------------------------
TODO List
-----------------------------------------------------------------------------------

    - Experiment with alternatives to fixed matrix multiplication
    - NTSC Composite Emulation (w/Dot Crawl)
    - Better Scanline Bloom
    - Implement proper pixel bluring effects
    - Gamma Control
    - RGB Signal Distortion Effects

"""

# Dot Mask CRT's were very common in the early days of color Television, and
# are often found in classic arcade monitors. Many "gaming" Tv's use this type
# of pixel layout.

dot_mask_scanline = [[
        1.25, 1.25, 0.90, 0.90,
        0.90, 0.90, 1.25, 1.25,
        1.25, 1.25, 0.90, 0.90,
        0.80, 0.80, 0.75, 0.75
    ], [
        0.90, 0.90, 1.25, 1.25,
        1.25, 1.25, 0.90, 0.90,
        0.90, 0.90, 1.25, 1.25,
        0.75, 0.75, 0.80, 0.80
    ]]
    

# Aperture Grille (Trinitron) pixel layout. Used in Sony's Trinitron line of
# TV's (and arcade monitors) from the 70's, and onward. Many clones exist, as
# the patent for it ran out in the mid 90's.

aperture_grille = [[
        1.25, 1.25, 0.95, 1.00,
        1.25, 1.25, 0.95, 1.00,
        1.25, 1.25, 0.95, 1.00,
        1.25, 1.25, 0.95, 1.00,
    ], [
        1.25, 1.25, 0.95, 1.00,
        1.25, 1.25, 0.95, 1.00,
        1.25, 1.25, 0.95, 1.00,
        1.25, 1.25, 0.95, 1.00,
    ]]
    
aperture_grille_scanline = [[
        1.25, 1.25, 0.95, 1.00,
        1.25, 1.25, 0.95, 1.00,
        1.25, 1.25, 0.95, 1.00,
        0.90, 0.90, 0.95, 1.00,
    ], [
        1.25, 1.25, 0.95, 1.00,
        1.25, 1.25, 0.95, 1.00,
        1.25, 1.25, 0.95, 1.00,
        0.90, 0.90, 0.95, 1.00,
    ]]
    
# Slot Masks are very common. Probably 2/3rds of the CRT TV's I've owned
# are Slot Masks.
slot_mask_scanline = [[
        1.25, 1.00, 1.25, 1.00,
        1.30, 1.00, 1.30, 1.00,
        0.85, 0.75, 0.85, 0.75,
        0.75, 0.85, 0.75, 0.85
    ], [
        1.00, 1.25, 1.00, 1.25,
        1.00, 1.30, 1.00, 1.30,
        0.75, 0.85, 0.75, 0.85,
        0.85, 0.75, 0.85, 0.75
    ]]

"""
slot_mask_scanline = [[
        1.25, 1.00, 1.25, 1.00,
        1.30, 1.00, 1.30, 1.00,
        0.65, 0.60, 0.65, 0.60,
        0.60, 0.65, 0.60, 0.65
    ], [
        1.00, 1.25, 1.00, 1.25,
        1.00, 1.30, 1.00, 1.30,
        0.60, 0.65, 0.60, 0.65,
        0.65, 0.60, 0.65, 0.60
    ]]
"""

# Slot Mask RGB Matrix
slot_mask_rgb_matrix = [[
        [10.0, -10.0, -10.0], [-10.0, 10.0, -10.0], [-10.0, -10.0, 10.0], [0.0, 0.0, 0.0],
        [10.0, -10.0, -10.0], [-10.0, 10.0, -10.0], [-10.0, -10.0, 10.0], [0.0, 0.0, 0.0],
        [10.0, -10.0, -10.0], [-10.0, 10.0, -10.0], [-10.0, -10.0, 10.0], [0.0, 0.0, 0.0],
        [0.0, 0.0, 0.0], [0.0, 0.0, 0.0], [0.0, 0.0, 0.0], [0.0, 0.0, 0.0],
    ], [
        [-10.0, -10.0, 10.0], [0.0, 0.0, 0.0], [10.0, -10.0, -10.0], [-10.0, 10.0, -10.0],
        [-10.0, -10.0, 10.0], [0.0, 0.0, 0.0], [10.0, -10.0, -10.0], [-10.0, 10.0, -10.0],
        [-10.0, -10.0, 10.0], [0.0, 0.0, 0.0], [10.0, -10.0, -10.0], [-10.0, 10.0, -10.0],
        [0.0, 0.0, 0.0], [0.0, 0.0, 0.0], [0.0, 0.0, 0.0], [0.0, 0.0, 0.0],
    ]]

# Dot Mask RGB Matrix
dot_mask_rgb_matrix = [[
        [1.0, -10.0, -10.0], [-10.0, 1.0, -10.0], [-10.0, -10.0, 1.0], [0.0, 0.0, 0.0],
        [0.0, 0.0, 0.0], [1.0, -10.0, -10.0], [-10.0, 1.0, -10.0], [-10.0, -10.0, 1.0],
        [1.0, -10.0, -10.0], [-10.0, 1.0, -10.0], [-10.0, -10.0, 1.0], [0.0, 0.0, 0.0],
        [0.0, 0.0, 0.0], [1.0, -10.0, -10.0], [-10.0, 1.0, -10.0], [-10.0, -10.0, 1.0],
    ], [
        [1.0, -10.0, -10.0], [-10.0, 1.0, -10.0], [-10.0, -10.0, 1.0], [0.0, 0.0, 0.0],
        [0.0, 0.0, 0.0], [1.0, -10.0, -10.0], [-10.0, 1.0, -10.0], [-10.0, -10.0, 1.0],
        [1.0, -10.0, -10.0], [-10.0, 1.0, -10.0], [-10.0, -10.0, 1.0], [0.0, 0.0, 0.0],
        [0.0, 0.0, 0.0], [1.0, -10.0, -10.0], [-10.0, 1.0, -10.0], [-10.0, -10.0, 1.0],
    ]]
    
# Aperture Grille RGB Matrix
aperture_grille_rgb_matrix = [[
        [1.0, -10.0, -10.0], [-10.0, 1.0, -10.0], [-10.0, -10.0, 1.0], [0.0, 0.0, 0.0],
        [2.0, -10.0, -10.0], [-10.0, 2.0, -10.0], [-10.0, -10.0, 2.0], [0.0, 0.0, 0.0],
        [2.0, -10.0, -10.0], [-10.0, 2.0, -10.0], [-10.0, -10.0, 2.0], [0.0, 0.0, 0.0],
        [1.0, -10.0, -10.0], [-10.0, 1.0, -10.0], [-10.0, -10.0, 1.0], [0.0, 0.0, 0.0],
    ], [
        [1.0, -10.0, -10.0], [-10.0, 1.0, -10.0], [-10.0, -10.0, 1.0], [0.0, 0.0, 0.0],
        [2.0, -10.0, -10.0], [-10.0, 2.0, -10.0], [-10.0, -10.0, 2.0], [0.0, 0.0, 0.0],
        [2.0, -10.0, -10.0], [-10.0, 2.0, -10.0], [-10.0, -10.0, 2.0], [0.0, 0.0, 0.0],
        [1.0, -10.0, -10.0], [-10.0, 1.0, -10.0], [-10.0, -10.0, 1.0], [0.0, 0.0, 0.0],
    ]]

def chunks(source, n):
    """ Split list into chunks of n size """
    for i in xrange(0, len(source), n):
        yield source[i:i+n]

class Image(object):
    def __init__(self, width, height, data=None):
        self.width = width
        self.height = height
        self.data = data
    
    def getLuminosity(self, rgb=[0, 0, 0]):
        """ Get luminosity for rgb value. """
        
        lum = int(math.ceil(sum([
            0.2989 * rgb[0],
            0.5870 * rgb[1],
            0.1140 * rgb[2]
        ])))
        
        if lum > 255: lum = 255
        if lum < 0: lum = 0
        return lum
    
    def getEntropyForPixel(self, x, y, window=1, data=None, width=None, height=None):
        """
        Get entropy for pixel, and surrouding pixels. Useful for blur
        effects, scaling filters, etc.
        
        """
        
        # Data
        if data is None: data = self.data
        
        # Width / Height
        if height is None: height = self.height
        if width is None: width = self.width
        
        # x/y
        if x > width: x = width
        if y > height: y = height
        
        entropy = []
        for a in range(-(window), window + 1):
            for b in range(-(window), window + 1):
                x2 = x + b
                y2 = y + a
                
                if x2 < 0: x2 = x
                elif x2 >= width: x2 = x 
                
                if y2 < 0: y2 = y
                elif y2 >= height: y2 = y
                
                i = ((y2) * width) + (x2)
                entropy.append(data[i])
        return entropy
    
    def boxscale(self, width, height, interpolate=None):
        scaled = [[0, 0, 0] for i in range(width * height)]
        scale_x = float(width) / float(self.width)
        scale_y = float(height) / float(self.height)
        
        if interpolate is None:
            pass
        
        for y in range(0, self.height):
            y2 = int(math.ceil((float(y) * scale_y) + 0.5)) - 1
            
            for x in range(0, self.width):
                x2 = int(math.ceil((float(x) * scale_x) + 0.5)) - 1
                i = (y * self.width) + x
                
                if y >= self.height - 1:
                    if x >= self.width - 1:
                        a = self.data[i]
                        b = self.data[i]
                        c = self.data[i]
                        d = self.data[i]
                        
                        ai = (((int(math.ceil((float(y) * scale_y) + 0.5)) - 1) * width) +
                               int(math.ceil((float(x) * scale_x) + 0.5)) - 1)
                        bi = (((int(math.ceil((float(y) * scale_y) + 0.5)) - 1) * width) +
                               int(math.ceil((float(x) * scale_x) + 0.5)) - 1)
                        ci = (((int(math.ceil((float(y) * scale_y) + 0.5)) - 1) * width) +
                               int(math.ceil((float(x) * scale_x) + 0.5)) - 1)
                        di = (((int(math.ceil((float(y) * scale_y) + 0.5)) - 1) * width) +
                               int(math.ceil((float(x) * scale_x) + 0.5)) - 1)
                    else:
                        a = self.data[i]
                        b = self.data[i+1]
                        c = self.data[i]
                        d = self.data[i+1]
                        
                        ai = (((int(math.ceil((float(y) * scale_y) + 0.5)) - 1) * width) +
                               int(math.ceil((float(x) * scale_x) + 0.5)) - 1)
                        bi = (((int(math.ceil((float(y) * scale_y) + 0.5)) - 1) * width) +
                               int(math.ceil((float(x+1) * scale_x) + 0.5)) - 1)
                        ci = (((int(math.ceil((float(y) * scale_y) + 0.5)) - 1) * width) +
                               int(math.ceil((float(x) * scale_x) + 0.5)) - 1)
                        di = (((int(math.ceil((float(y) * scale_y) + 0.5)) - 1) * width) +
                               int(math.ceil((float(x+1) * scale_x) + 0.5)) - 1)
                else:
                    if x >= self.width - 1:
                        a = self.data[i]
                        b = self.data[i]
                        c = self.data[i+self.width]
                        d = self.data[i+self.width]
                        
                        ai = (((int(math.ceil((float(y) * scale_y) + 0.5)) - 1) * width) +
                               int(math.ceil((float(x) * scale_x) + 0.5)) - 1)
                        bi = (((int(math.ceil((float(y) * scale_y) + 0.5)) - 1) * width) +
                               int(math.ceil((float(x) * scale_x) + 0.5)) - 1)
                        ci = (((int(math.ceil((float(y+1) * scale_y) + 0.5)) - 1) * width) +
                               int(math.ceil((float(x) * scale_x) + 0.5)) - 1)
                        di = (((int(math.ceil((float(y+1) * scale_y) + 0.5)) - 1) * width) +
                               int(math.ceil((float(x) * scale_x) + 0.5)) - 1)
                    else:
                        a = self.data[i]
                        b = self.data[i+1]
                        c = self.data[i+self.width]
                        d = self.data[i+self.width+1]
                        
                        ai = (((int(math.ceil((float(y) * scale_y) + 0.5)) - 1) * width) +
                               int(math.ceil((float(x) * scale_x) + 0.5)) - 1)
                        bi = (((int(math.ceil((float(y) * scale_y) + 0.5)) - 1) * width) +
                               int(math.ceil((float(x+1) * scale_x) + 0.5)) - 1)
                        ci = (((int(math.ceil((float(y+1) * scale_y) + 0.5)) - 1) * width) +
                               int(math.ceil((float(x) * scale_x) + 0.5)) - 1)
                        di = (((int(math.ceil((float(y+1) * scale_y) + 0.5)) - 1) * width) +
                               int(math.ceil((float(x+1) * scale_x) + 0.5)) - 1)

                # Vertical linear scale
                L = ((ci - ai) / width) + 1
                l = 1
                
                Lx = (bi - ai) + 1
                ax = int(ai)
                
                while l <= L:
                    rgb = [0, 0, 0]
                    
                    if L == 0:
                        rgb = c
                    else:
                        rgb[0] = a[0] + l * ((c[0] - a[0]) / L)
                        rgb[1] = a[1] + l * ((c[1] - a[1]) / L)
                        rgb[2] = a[2] + l * ((c[2] - a[2]) / L)
                        if rgb[0] < 0: rgb[0] = 0
                        if rgb[1] < 0: rgb[1] = 0
                        if rgb[2] < 0: rgb[2] = 0 
                    
                    lx = 0
                    while lx <= Lx:
                        scaled[ax+lx] = rgb
                        lx += 1
                    
                    l += 1
                    ax += width
        
        return Image(width, height, scaled)
    
    def fastscale(self, width, height):
        """ Nearest Neighbor Scale """
        
        scaled = [[0, 0, 0] for i in range(width * height)]
        
        for y in range(0, height):
            y2 = int(math.ceil(
                (float(y) + 0.5) * (float(self.height) / float(height))
            ))
            for x in range(0, width):
                x2 = int(math.ceil(
                    (float(x) + 0.5) * (float(self.width) / float(width))
                ))
            
                i = (y * width) + x
                i2 = ((y2 - 1) * self.width) + (x2 - 1)
                scaled[i] = self.data[i2]
        
        return Image(width, height, scaled)
    
    def filter_noise(self, noise_max=16, noise_probability=0.75, data=None):
        """ Noise Filter """
            
        if data is None:
            data = copy.deepcopy(self.data)
        
        noise_probability = int(math.ceil((1.0 - noise_probability) * 100.0))
        if noise_probability < 2:
            noise_probability = 2
        
        # Add noise filter
        for i in range(0, self.width * self.height):
            asd = random.randrange(0, noise_probability)
            if asd == 0:
                data[i][0] = data[i][0] - random.randrange(0, noise_max)
                data[i][1] = data[i][1] - random.randrange(0, noise_max)
                data[i][2] = data[i][2] - random.randrange(0, noise_max)
                
                if data[i][0] < 0: data[i][0] = random.randrange(0, noise_max)
                if data[i][1] < 0: data[i][1] = random.randrange(0, noise_max)
                if data[i][2] < 0: data[i][2] = random.randrange(0, noise_max)
            elif asd == 1:
                data[i][0] = data[i][0] + random.randrange(0, noise_max)
                data[i][1] = data[i][1] + random.randrange(0, noise_max)
                data[i][2] = data[i][2] + random.randrange(0, noise_max)
                
                if data[i][0] > 255:
                    data[i][0] = random.randrange(255-noise_max, 255)
                if data[i][1] > 255:
                    data[i][1] = random.randrange(255-noise_max, 255)
                if data[i][2] > 255:
                    data[i][2] = random.randrange(255-noise_max, 255)
        
        return data
    
    def brcn_filter_get_slope(self, brightness, contrast, in_range=255):
        qrange = in_range
        qscale = 1.0 / qrange
        
        # Slope
        slope = math.tan((math.pi * (contrast / 100.0 + 1.0) / 4.0))
        if slope < 0.0: slope = 0.0
        slope = round(slope, 10)
        
        # Intercept
        intercept = brightness / 100.0 + ((100.0 - brightness) / 200.0) * (1.0 - slope)
        
        return {
                'range': qrange,
                'scale': qscale,
                'slope': slope,
                'intercept': intercept}
    
    def brcn_filter_process(self, params, value):
        qrange = params['range']
        qscale = params['scale']
        slope = params['slope']
        intercept = params['intercept']
    
        value = ((slope * qscale) * value + intercept) * qrange
        if value > qrange:
            value = qrange
        elif value < 0:
            value = 0
    
        return value
    
    def filter_contrast(self, brightness=0.0, contrast=0.0, data=None):
        """ Brightness / Contrast Filter """
            
        if data is None:
            data = copy.deepcopy(self.data)
        
        # Do Nothing
        if brightness == 0.0 and contrast == 0.0:
            return data
        
        for i in range(0, len(data)):
            # Brightness
            if brightness < 0.0:
                data[i][0] = data[i][0] * (1.0 + brightness)
                data[i][1] = data[i][1] * (1.0 + brightness)
                data[i][2] = data[i][2] * (1.0 + brightness)
            else:
                data[i][0] = data[i][0] + ((1 - data[i][0]) * brightness)
                data[i][1] = data[i][1] + ((1 - data[i][1]) * brightness)
                data[i][2] = data[i][2] + ((1 - data[i][2]) * brightness)
            
            # Contrast
            #(value - 0.5) * (tan ((contrast + 1) * PI/4) ) + 0.5;
            data[i][0] = ((data[i][0] - 0.5) * (math.tan((contrast + 1) * math.pi/4))) + 0.5
            data[i][1] = ((data[i][1] - 0.5) * (math.tan((contrast + 1) * math.pi/4))) + 0.5
            data[i][2] = ((data[i][2] - 0.5) * (math.tan((contrast + 1) * math.pi/4))) + 0.5
        
        return data
    
    def filter_contrast2(self, brightness=0.0, contrast=0.0, data=None):
        if data is None:
            data = copy.deepcopy(self.data)
        
        # Do nothing (besides return a copy of data)
        if brightness == 0.0 and contrast == 0.0:
            return data
        
        # Range and Scale
        qrange = 255.0
        qscale = 1.0 / qrange
        
        # Slope
        slope = math.tan((math.pi * (contrast / 100.0 + 1.0) / 4.0))
        if slope < 0.0: slope = 0.0
        slope = round(slope, 10)
        
        # Intercept
        intercept = brightness / 100.0 + ((100.0 - brightness) / 200.0) * (1.0 - slope)
        
        # Process pixels
        for i in range(0, len(data)):
            r = ((slope * qscale) * data[i][0] + intercept) * qrange
            g = ((slope * qscale) * data[i][1] + intercept) * qrange
            b = ((slope * qscale) * data[i][2] + intercept) * qrange
            
            # Clamp
            if r > qrange: r = qrange
            if g > qrange: g = qrange
            if b > qrange: b = qrange
            if r < 0: r = 0
            if g < 0: g = 0
            if b < 0: b = 0
            
            # Copy
            data[i][0] = int(math.floor(r))
            data[i][1] = int(math.floor(g))
            data[i][2] = int(math.floor(b))
        
        return data
    
    def blend_pixels(self, pixel1, pixel2, blend_factor=1024):
        value = [0, 0, 0]
    
        lum = self.getLuminosity(pixel2) - self.getLuminosity(pixel1)
        if lum > 0.001:
            c1 = math.log(lum, blend_factor)
            c2 = (0.95 - c1)
            for a in range(0, 3):
                value[a] = int(math.ceil(sum([
                    pixel2[a] * c2,
                    pixel1[a] * c1
                ])))
        else:
            return pixel1
        
        return value
    
    def tv2x_scale(self):
        out_width = self.width * 2;
        out_height = self.height * 2
        output = [[0, 0, 0] for i in xrange(0, out_width * out_height)]
        matrix = [[1.05, 1.0, 0.95], [0.95, 1.0, 1.05]]
        matrix_b = [
            [[1.05, 1.0, 1.0], [1.0, 1.05, 1.0]],
            [[1.0, 1.0, 1.05], [1.05, 1.0, 1.0]],
            [[1.0, 1.05, 1.0], [1.0, 1.0, 1.05]]
        ]
        
        scan_brightness = -20.0
        scan_contrast = 4.0
        
        slope = math.tan((math.pi * (scan_contrast / 100.0 + 1.0) / 4.0))
        if slope < 0.0:
            slope = 0.0
        slope = round(slope, 10)
        
        intercept = scan_brightness / 100.0 + ((100.0 - scan_brightness) / 200.0) * (1.0 - slope)
        
        for y in xrange(0, self.height):
            for x in xrange(0, self.width):
                """
                
                RG BR GB
                BR GB RG
                
                """
                
                i1 = (y * self.width) + x;
                i2 = (y * out_width * 2) + (x * 2)
                
                if x == 1:
                    linear = [[0, 0, 0], self.data[i1]]
                if (x+1) < self.width:
                    linear = [self.data[i1],
                            [(self.data[i1][0] + self.data[i1+1][0])/2,
                             (self.data[i1][1] + self.data[i1+1][1])/2,
                             (self.data[i1][2] + self.data[i1+1][2])/2]]
                else:
                    linear = [self.data[i1], [0, 0, 0]]
                
                for a in range(0, 2):
                    for b in range(0, 3):
                        v = linear[a][b] * matrix_b[(x+(y%2))%3][a][b]
                        if v > 255.0: v = 255.0
                        if v < 0.0: v = 0.0
                        output[i2+a][b] = int(v)
                
                i2 += out_width
                for a in range(0, 2):
                    for b in range(0, 3):
                        v = ((slope * (1.0 / 255.0)) * (linear[a][b] * matrix_b[(x+(y%2))%3][a][b]) + intercept) * 255.0
                        #v = ((slope * (1.0 / 255.0)) * linear[a][b] + intercept) * 255.0
                        if v > 255.0: v = 255.0
                        if v < 0.0: v = 0.0
                        output[i2+a][b] = int(v)
        
        return Image(out_width, out_height, output)
    
    def tv4x_scale(self,
            blur_factor=0.4,
            noise_max=16,
            noise_probability=0.75,
            mult_matrix_even=None,
            mult_matrix_odd=None,
            rgb_matrix_even=None,
            rgb_matrix_odd=None,
            brightness=0.0,
            contrast=0.0):
        
        """
        
        TV4X Scaling Filter. Parameters are:
        
        @param  blur_factor         - 3x3 Blur Factor. Float between 0.0 and 1.0.
        @param  noise_max           - Integer. Maxiumum noise differential (+/- amount).
        @param  noise_probability   - Noise probability. Float between 0.0 (never)
                                      and 1.0 (always).
        @param  mult_matrix_even    - 4x4 Pixel multiplication matrix for even fields.
        @param  mult_matrix_odd     - 4x4 Pixel multiplication matrix for odd fields.
        @param  brightness          - Brightness amount. Float between -100.0 and 100.0.
        @param  contrast            - Constrast amount. Float between -100.0 and 100.0.
        
        """
        
        #
        # Matrix:
        #
        # O = bright
        # o = normal
        # c = medium
        # X = dark
        #
        # Even Fields:
        #
        # O o O o
        # O o O o
        # c X c X
        # X c X c
        #
        # Odd Fields:
        #
        # o O o O
        # o O o O
        # X c X c
        # c X c X
        #
        # 1.25  1.0     1.25    1.0
        # 1.25  1.0     1.25    1.0
        # 0.90  0.80    0.90    0.80
        # 0.80  0.90    0.80    0.90
        #
        
        if mult_matrix_even is None:
            mult_matrix_even = slot_mask_scanline[1]
        
        if mult_matrix_odd is None:
            mult_matrix_odd = slot_mask_scanline[0]
        
        if rgb_matrix_odd is None:
            rgb_matrix_odd = slot_mask_rgb_matrix[0]
        
        if rgb_matrix_even is None:
            rgb_matrix_even = slot_mask_rgb_matrix[1]
        
        width = self.width * 4
        height = self.height * 4
        scaled = [[0, 0, 0]] * (width * height)
        
        blur_factor_ml = blur_factor / 8
        blur_factor = 1.0 - blur_factor
        
        # Make a copy of data
        data = copy.deepcopy(self.data)
        
        # Simulate YIQ Color Space, NTSC Composite Signal, etc
        for y in xrange(0, self.height):
            yiq_line = []
            for x in xrange(0, self.width):
                i = (self.width * y) + x
                yiq_line.append(yiq.from_rgb(data[i]))
    
            yiq_line = yiq.simulate_composite_line(yiq_line)
            for x in xrange(0, self.width):
                i = (self.width * y) + x
                tmp = yiq.to_rgb(yiq_line[x])
                data[i] = [tmp[0], tmp[1], tmp[2]]
        
        # Noise filter
        if not noise_max == 0:
            data = self.filter_noise(
                    noise_max=noise_max,
                    noise_probability=noise_probability,
                    data=data)
        
        # Brightness/Contrast filter
        data = self.filter_contrast2(
                    brightness=brightness,
                    contrast=contrast,
                    data=data)
        
        # Scanline brightness/contrast
        scan_brightness = 0.0
        scan_contrast = 12.0
        
        # Scanline slope/intersect
        scan_slope = math.tan((math.pi * (scan_contrast / 100.0 + 1.0) / 4.0))
        if scan_slope < 0.0: scan_slope = 0.0
        scan_slope = round(scan_slope, 10)
        scan_intersect = scan_brightness / 100.0 + ((100.0 - scan_brightness) / 200.0) * (1.0 - scan_slope)
        
        for y in range(0, self.height):
            #last_pixel = [[0, 0, 0] for a in range(0, 16)]
            
            for x in range(0, self.width):
                # Calculate buffer indexes
                i = (y * self.width) + x
                i2 = (y * width * 4) + (x * 4)
                
                # Blur filter. Similar to gaussian blur, but uses weighted factors
                # to control how much the original color is preserved.
                entropy = self.getEntropyForPixel(x, y,
                    width=self.width,
                    height=self.height,
                    data=data)
                
                """
                value = [0, 0, 0]
                for a in range(0, 3):
                    value[a] = sum([
                        entropy[0][a] * blur_factor_ml,     # -1, -1
                        entropy[1][a] * blur_factor_ml,     # -1,  0
                        entropy[2][a] * blur_factor_ml,     # -1,  1
                        entropy[3][a] * blur_factor_ml,     #  0, -1
                        entropy[4][a] * blur_factor,        #  0,  0
                        entropy[5][a] * blur_factor_ml,     #  0,  1
                        entropy[6][a] * blur_factor_ml,     #  1, -1
                        entropy[7][a] * blur_factor_ml,     #  1,  0
                        entropy[8][a] * blur_factor_ml,     #  1,  1
                    ])
                """
                
                value = data[i]
                
                # Build base matrix
                pixels = [[value[0], value[1], value[2]] for a in range(0, 16)]
                
                # Run pixels through the multiplication matrix/matrices
                for a in range(0, len(pixels)):
                    orig = [pixels[a][0], pixels[a][1], pixels[a][2]]
                
                    if y % 2:
                        # Even Matrix
                        pixels[a][0] = ((float(pixels[a][0]) * mult_matrix_even[a])
                                            + rgb_matrix_odd[a][0])
                        pixels[a][1] = ((float(pixels[a][1]) * mult_matrix_even[a])
                                            + rgb_matrix_odd[a][1])
                        pixels[a][2] = ((float(pixels[a][2]) * mult_matrix_even[a])
                                            + rgb_matrix_odd[a][2])
                    else:
                        # Odd Matrix
                        pixels[a][0] = ((float(pixels[a][0]) * mult_matrix_odd[a])
                                            + rgb_matrix_even[a][0])
                        pixels[a][1] = ((float(pixels[a][1]) * mult_matrix_odd[a])
                                            + rgb_matrix_even[a][1])
                        pixels[a][2] = ((float(pixels[a][2]) * mult_matrix_odd[a])
                                            + rgb_matrix_even[a][2])
                    
                    # Round to ceiling, and make them integers
                    pixels[a][0] = int(math.ceil(pixels[a][0]))
                    pixels[a][1] = int(math.ceil(pixels[a][1]))
                    pixels[a][2] = int(math.ceil(pixels[a][2]))
                    
                    # Underrun checking
                    if pixels[a][0] < 0: pixels[a][0] = 0
                    if pixels[a][1] < 0: pixels[a][1] = 0
                    if pixels[a][2] < 0: pixels[a][2] = 0
                    
                    # Overrun checking
                    if pixels[a][0] > 255: pixels[a][0] = 255
                    if pixels[a][1] > 255: pixels[a][1] = 255
                    if pixels[a][2] > 255: pixels[a][2] = 255
                
                # Scanline brightness/contrast
                for a in range(8, 16):
                    for b in range(0, 3):
                        pixels[a][b] = ((scan_slope * (1.0/255.0)) * pixels[a][b] + scan_intersect) * 255.0
                        if pixels[a][b] > 255: pixels[a][b] = 255
                        if pixels[a][b] < 0: pixels[a][b] = 0
                        pixels[a][b] = int(math.floor(pixels[a][b]))
                
                # Grab surrounding pixels
                # Calculate weighted differences of surrounding pixels
                # Apply values to self, and run through matrix?
                
                """
                
                pixel = [rgb]
                entropy = [[rgb], [rgb], [rgb]...]
                
                X X X
                X T X
                X X X
                
                0 1 2
                3 4 5
                6 7 8
                
                4 =
                  a b c d
                  e f g h
                  i j k l
                  m n o p
                  
                0 0 0 0 1 1 1 1 2 2 2 2 
                0 0 0 0 1 1 1 1 2 2 2 2
                0 0 0 0 1 1 1 1 2 2 2 2
                0 0 0 0 1 1 1 1 2 2 2 2
                3 3 3 3 A B C D 5 5 5 5
                3 3 3 3 E F G H 5 5 5 5
                3 3 3 3 I J K L 5 5 5 5
                3 3 3 3 M N O P 5 5 5 5
                6 6 6 6 7 7 7 7 8 8 8 8
                6 6 6 6 7 7 7 7 8 8 8 8
                6 6 6 6 7 7 7 7 8 8 8 8
                6 6 6 6 7 7 7 7 8 8 8 8
                
                The sides and corners of the current pixel will be blended with the
                surrounding virtual pixels (from the source image). The center sub
                pixels, of the current pixel will then be blended with the sides,
                which were just blended. This should give a diffuse effect, that's
                more intense depending on the contrast between the two's luminosity.
                
                Blend Table:
                    Pixel A: Blend with 0, 1, 2
                    Pixel B: Blend with 1
                    Pixel C: Blend with 1
                    Pixel D: Blend with 1, 2, 5
                    Pixel E: Blend with 3
                    Pixel I: Blend with 3
                    Pixel M: Blend with 3, 6, 7
                    Pixel N: Blend with 7
                    Pixel M: Blend with 7
                    Pixel P: Blend with 5, 7, 8
                    Pixel F: Blend with A, B, E
                    Pixel G: Blend with C, D, H
                    Pixel J: Blend with I, M, N
                    Pixel K: Blend with O, P, L
                
                """
                
                """
                entropy_tlc = [0, 0, 0]
                entropy_trc = [0, 0, 0]
                entropy_blc = [0, 0, 0]
                entropy_brc = [0, 0, 0]
                
                for a in range(0, 3):
                    entropy_tlc[a] = int(sum([
                        entropy[0][a],
                        entropy[1][a],
                        entropy[3][a]]) / 3.0)
                    
                    entropy_trc[a] = int(sum([
                        entropy[1][a],
                        entropy[2][a],
                        entropy[5][a]]) / 3.0)
                    
                    entropy_blc[a] = int(sum([
                        entropy[3][a],
                        entropy[6][a],
                        entropy[7][a]]) / 3.0)
                    
                    entropy_brc[a] = int(sum([
                        entropy[5][a],
                        entropy[7][a],
                        entropy[8][a]]) / 3.0)
                
                pixels[0] = self.blend_pixels(pixels[0], entropy_tlc)
                pixels[1] = self.blend_pixels(pixels[2], entropy[1])
                pixels[2] = self.blend_pixels(pixels[3], entropy[1])
                pixels[3] = self.blend_pixels(pixels[4], entropy_trc)
                pixels[4] = self.blend_pixels(pixels[4], entropy[3])
                pixels[7] = self.blend_pixels(pixels[7], entropy[5])
                pixels[8] = self.blend_pixels(pixels[8], entropy[3])
                pixels[11] = self.blend_pixels(pixels[11], entropy[5])
                
                pixels[12] = self.blend_pixels(pixels[12], entropy_blc)
                pixels[13] = self.blend_pixels(pixels[13], entropy[7])
                pixels[14] = self.blend_pixels(pixels[14], entropy[7])
                pixels[15] = self.blend_pixels(pixels[15], entropy_brc)
                
                pixels[9] = self.blend_pixels(pixels[9], pixels[8])
                pixels[10] = self.blend_pixels(pixels[10], pixels[11])
                pixels[5] = self.blend_pixels(pixels[5], pixels[4])
                pixels[6] = self.blend_pixels(pixels[6], pixels[7])
                """
                
                pixels[0] = self.blend_pixels(pixels[0], entropy[3])
                pixels[1] = self.blend_pixels(pixels[1], pixels[0])
                pixels[3] = self.blend_pixels(pixels[3], entropy[5])
                pixels[2] = self.blend_pixels(pixels[2], pixels[3])
                
                pixels[4] = self.blend_pixels(pixels[4], entropy[3])
                pixels[5] = self.blend_pixels(pixels[5], pixels[4])
                pixels[7] = self.blend_pixels(pixels[7], entropy[5])
                pixels[6] = self.blend_pixels(pixels[6], pixels[7])
                
                pixels[8] = self.blend_pixels(pixels[8], entropy[3])
                pixels[9] = self.blend_pixels(pixels[9], pixels[8])
                pixels[11] = self.blend_pixels(pixels[11], entropy[5])
                pixels[10] = self.blend_pixels(pixels[10], pixels[11])
                
                pixels[12] = self.blend_pixels(pixels[12], entropy[3])
                pixels[13] = self.blend_pixels(pixels[13], pixels[12])
                pixels[14] = self.blend_pixels(pixels[14], entropy[5])
                pixels[15] = self.blend_pixels(pixels[15], pixels[14])
                
                # data[i][0] = ((data[i][0] - 0.5) * (math.tan((contrast + 1) * math.pi/4))) + 0.5
                
                # Copy values
                # Row 1
                scaled[(i2)+0] = pixels[0]
                scaled[(i2)+1] = pixels[1]
                scaled[(i2)+2] = pixels[2]
                scaled[(i2)+3] = pixels[3]
                
                 # Row 2
                scaled[(i2 + width)+0] = pixels[4]
                scaled[(i2 + width)+1] = pixels[5]
                scaled[(i2 + width)+2] = pixels[6]
                scaled[(i2 + width)+3] = pixels[7]
                
                 # Row 3
                scaled[(i2 + (width * 2))+0] = pixels[8]
                scaled[(i2 + (width * 2))+1] = pixels[9]
                scaled[(i2 + (width * 2))+2] = pixels[10]
                scaled[(i2 + (width * 2))+3] = pixels[11]
                
                # Row 4
                scaled[(i2 + (width * 3))+0] = pixels[12]
                scaled[(i2 + (width * 3))+1] = pixels[13]
                scaled[(i2 + (width * 3))+2] = pixels[14]
                scaled[(i2 + (width * 3))+3] = pixels[15]
        
        return Image(width, height, scaled)
        
    def savePNG(self, filename):
        tmp = []
        for i in self.data:
            tmp.extend(i)
        
        tmp = list(chunks(tmp, self.width * 3))
        out = png.Writer(width=self.width, height=self.height)
        out.write(open(filename, 'w'), tmp)
        del out
        del tmp
    
    @staticmethod
    def fromPNG(filename):
        r = png.Reader(filename=filename)
        r.read()
        width, height, pixels, meta = r.asRGB8()
        data = []
        
        for row in pixels:
            data.extend(list(chunks(row, 3)))
        
        del r
        return Image(width, height, data)

