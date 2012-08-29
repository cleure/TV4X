import yiq, math

try:
    import png
except ImportError:
    sys.stderr.write('Failed to import PNG module\n')
    sys.stderr.write('Please install the PNG module to continue\n')
    sys.exit(1)

from image import *

"""
c = 0
last = -1
for i in range(0, 320):
    if math.ceil(i * 0.1) > last:
        last = math.ceil(i * 0.1)
        print i
        c += 1

print c
"""

# [86.126, -14.590000000000003, 89.70499999999998]

in_matrix = (
        (0.299,     0.587,  0.114),
        (0.596,    -0.274, -0.322),
        (0.212,    -0.523,  0.311))

# YIQ Output Matrix (to RGB)
out_matrix = (
        (1.0,   0.956,  0.621),
        (1.0,  -0.272, -0.647),
        (1.0,  -1.105,  1.702))

rgb = [128, 32, 255]
_yiq = yiq.from_rgb(rgb)
print _yiq

print ((
    _yiq[0] * out_matrix[0][0],
    _yiq[1] * out_matrix[0][1],
    _yiq[2] * out_matrix[0][2],
))

sys.exit(0)

im = Image.fromPNG('input-images/sonic2_3.png')

for y in xrange(0, im.height):
    yiq_line = []
    for x in xrange(0, im.width):
        i = (im.width * y) + x
        yiq_line.append(yiq.from_rgb(im.data[i]))
    
    yiq_line = yiq.simulate_composite_line(yiq_line)
    for x in xrange(0, im.width):
        i = (im.width * y) + x
        tmp = yiq.to_rgb(yiq_line[x])
        im.data[i] = [int(tmp[0]), int(tmp[1]), int(tmp[2])]

im.savePNG('output.png')
