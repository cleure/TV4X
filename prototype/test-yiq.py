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
