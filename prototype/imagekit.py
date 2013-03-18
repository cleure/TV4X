
import os, sys
import copy
try:
    import png
    HAVE_PNG = True
except:
    HAVE_PNG = False

def chunks(source, n):
    """ Split list into chunks of n size """
    for i in xrange(0, len(source), n):
        yield source[i:i+n]

class Image(object):
    def __init__(self, width, height, data=[], pitch=None):
        if pitch is None:
            pitch = width
        
        self.width = width
        self.height = height
        self.pitch = pitch
        self.data = []
        
        if len(data) == 0:
            self.data = [[0, 0, 0] for i in range(pitch * height)]
        else:
            self.data = data
    
    def get_pixel_idx(self, x, y):
        return (y * self.pitch) + x
    
    def get_pixel(self, x, y):
        return self.data[self.get_pixel_idx(x, y)]
    
    def set_pixel(self, x, y, rgb):
        self.data[self.get_pixel_idx(x, y)] = [i for i in rgb]
    
    def get_box(self, center_x, center_y, size=3):
        """
        Returns a matrix of pixels of (size)x(size), with pixels from starting point
        (center_x, center_y). If pixel is out of range, None is used in place of a
        list of RGB values.
        """
    
        median = size / 2
        start = -(median)
        end = median+1
        box = [[None for a in range(size)] for b in range(size)]
        
        max_width = self.width - 1
        max_height = self.height - 1
        
        ps = 0
        for yo in range(start, end):
            px = 0
            for xo in range(start, end):
                xt = center_x + xo
                yt = center_y + yo
                
                if (not (
                        xt < 0 or
                        xt > max_width or
                        yt < 0 or
                        yt > max_height)):
                    box[ps][px] = self.data[self.get_pixel_idx(xt, yt)]
                
                px += 1
            ps += 1
        
        return box
    
    def savePNG(self, filename):
        if not HAVE_PNG:
            raise StandardError('SavePNG() not available without PyPNG')
    
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
        if not HAVE_PNG:
            raise StandardError('SavePNG() not available without PyPNG')
    
        r = png.Reader(filename=filename)
        r.read()
        width, height, pixels, meta = r.asRGB8()
        data = []
        
        for row in pixels:
            data.extend(list(chunks(row, 3)))
        
        del r
        del pixels
        return Image(
            width=width,
            height=height,
            data=data)

class Filter(object):
    def apply(self, input):
        raise StandardError('apply() not implemented')

class Filter_CVKernel(Filter):
    """
    Convolution Kernel Filter. Can be used for either average or
    median filters.
    """

    def __init__(self,
                avg_matrix=[],
                avg_factor=1.0,
                avg_bias=0.0,
                median_size=3,
                method="average"):
        
        self.matrix = avg_matrix
        self.size = len(avg_matrix)
        self.factor = avg_factor
        self.bias = avg_bias
        
        if method == 'average':
            self.apply = self.apply_average
        elif method == 'median':
            self.apply = self.apply_median
            self.size = median_size
        else:
            raise ValueError('method must be "average" or "median"')

    def apply_median(self, input):
        output = Image(
                    width=input.width,
                    height=input.height,
                    pitch=input.pitch)
        
        for y in range(input.height):
            for x in range(input.width):
                box = input.get_box(x, y, self.size)
                flat_r = []
                flat_g = []
                flat_b = []
                
                for by in range(self.size):
                    for bx in range(self.size):
                        if box[by][bx] is None:
                            continue
                        
                        flat_r.append(box[by][bx][0])
                        flat_g.append(box[by][bx][1])
                        flat_b.append(box[by][bx][2])
                
                m = len(flat_r)/2
                flat_r.sort()
                flat_g.sort()
                flat_b.sort()
                output.set_pixel(x, y, [flat_r[m], flat_g[m], flat_b[m]])
                
        return output
    
    def apply_average(self, input):
        output = Image(
                    width=input.width,
                    height=input.height,
                    pitch=input.pitch)
        
        for y in range(input.height):
            for x in range(input.width):
                box = input.get_box(x, y, self.size)
                rgb = [0, 0, 0]
                
                for by in range(self.size):
                    for bx in range(self.size):
                        if box[by][bx] is None:
                            continue
                        
                        rgb[0] += box[by][bx][0] * self.matrix[by][bx]
                        rgb[1] += box[by][bx][1] * self.matrix[by][bx]
                        rgb[2] += box[by][bx][2] * self.matrix[by][bx]
                
                rgb[0] = self.factor * rgb[0] + self.bias
                rgb[1] = self.factor * rgb[1] + self.bias
                rgb[2] = self.factor * rgb[2] + self.bias
                
                if rgb[0] < 0.0: rgb[0] = 0
                if rgb[1] < 0.0: rgb[1] = 0
                if rgb[2] < 0.0: rgb[2] = 0
                if rgb[0] > 255: rgb[0] = 255
                if rgb[1] > 255: rgb[1] = 255
                if rgb[2] > 255: rgb[2] = 255
                
                output.set_pixel(x, y, [int(rgb[0]), int(rgb[1]), int(rgb[2])])
        return output

class BoxScale(Filter):
    """ Prototype for Box scaling algorithms """
    
    def __init__(self, input):
        self.input = input
    
    def apply(self, width, height): pass

class Scale_NearestNeighbor(BoxScale):
    """ Nearest Neighbor Scaling Algorithm """

    def apply(self, width, height):
        input = self.input
        output = Image(width=width, height=height)
        
        y2_mult = int((input.height << 16) / height + 1)
        x2_mult = int((input.width << 16) / width + 1)
        
        for y in range(height):
            y2 = (y * y2_mult) >> 16
            for x in range(width):
                x2 = (x * x2_mult) >> 16
                output.set_pixel(x, y, list(input.get_pixel(x2, y2)))
        
        return output

class Scale_Bilinear(BoxScale):
    """ Bilinear Scaling Algorithm """

    def apply(self, width, height):
        input = self.input
        output = Image(width=width, height=height)
        
        in_y_mult   = float(input.height - 1) / height
        in_x_mult   = float(input.width - 1) / width
        
        for out_y1 in range(height):
            y = int(out_y1 * in_y_mult)
            Ly = (out_y1 * in_y_mult) - y
        
            for out_x1 in range(width):
                x = int(out_x1 * in_x_mult)
                Lx = (out_x1 * in_x_mult) - x
                
                A = list(input.get_pixel(x, y))
                B = input.get_pixel(x+1, y)
                C = input.get_pixel(x, y+1)
                D = input.get_pixel(x+1, y+1)

                """
                
                Y =   A * (1 - w) * (1 - h)   +
                      B * (    w) * (1 - h)   +
                      C * (    h) * (1 - w)   +
                      D * (    w) * (    h)
                
                """
                for i in range(len(A)):
                    A[i] = (
                            (A[i] * (1 - Lx) * (1 - Ly))    +
                            (B[i] * (    Lx) * (1 - Ly))    +
                            (C[i] * (    Ly) * (1 - Lx))    +
                            (D[i] * (    Lx) * (     Ly))
                    )
                
                output.set_pixel(out_x1, out_y1, A)
        
        return output

class Scale_Cubic(BoxScale):
    """ TODO: Cubic Scaling Algorithm """

    filters = {
        'mitchell': (0.333333333, 0.333333333),
        'catmull':  (0.0, 0.5),
        'bspline':  (1.0, 0.0),
        'hermite':  (0.0, 0.0),
    }

    def __init__(self, input, filter=None, B=None, C=None):
        self.input = input
        
        if filter is not None:
            self.filter = filter
        elif B is not None and C is not None:
            self.filter = [B, C]
        else:
            self.filter = self.filters['bspline']
        self.calc_coefficients()

    def calc_coefficients(self):
        B = self.filter[0]
        C = self.filter[1]
        twoB = B + B
        
        self.coeff = (
            1.0 - (1.0 / 3.0) * B,
            -3.0 + twoB + C,
            2.0 - 1.5 * B - C,
            (4.0 / 3.0) * B + 4.0 * C,
            -8.0 * C - twoB,
            B + 5.0 * C,
            (-1.0 / 6.0) * B - C,
        )

    def apply(self, width, height): pass

def get_linear(A, B, L, l):
    """
    Calculate Linear of A/B.
    Linear Algorithm: Y = A + l * ((B - A) / L)
    """
    return A + l * ((B - A) / L)

