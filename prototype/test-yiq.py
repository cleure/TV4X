import yiq, imagekit, math

try:
    import png
except ImportError:
    sys.stderr.write('Failed to import PNG module\n')
    sys.stderr.write('Please install the PNG module to continue\n')
    sys.exit(1)

from image import *

def ntsc_line(line, burst_i=0.7, burst_q=0.4, offset=0.3):
    line_len = len(line)
    output = [[0,0,0] for i in range(line_len)]
    offset = math.floor(offset * 100)
    
    burst_i = 0.7
    burst_q = 0.4
    
    last_i = [0, line[0][1]]
    last_q = [0, line[0][2]]
    
    accum_i = 0.0
    accum_q = 0.0
    
    step_i = burst_i
    step_q = burst_q
    
    """
    
    Linear Algorithm: Y = A + l * ((B - A) / L)
    
    """
    
    for i in range(line_len):
        if accum_i >= 1.0:
            accum_i -= 1.0
            
            L = i - last_i[0]
            for x in range(last_i[0], i):
                Li = x - last_i[0]
                output[x][1] = imagekit.get_linear(
                                    A=last_i[1],
                                    B=line[i][1],
                                    L=L,
                                    l=Li)
            
            last_i = [i, line[i][1]]
        
        if accum_q >= 1.0:
            accum_q -= 1.0
            
            L = i - last_q[0]
            for x in range(last_q[0], i):
                Li = x - last_q[0]
                output[x][2] = imagekit.get_linear(
                                    A=last_q[1],
                                    B=line[i][2],
                                    L=L,
                                    l=Li)
            
            last_q = [i, line[i][2]]
    
        accum_i += step_i
        accum_q += step_q
        
        if i + 1 == line_len:
            output[i][0] = imagekit.get_linear(
                                A=line[i][0],
                                B=line[i-1][0],
                                L=100,
                                l=offset)
        else:
            output[i][0] = imagekit.get_linear(
                                A=line[i][0],
                                B=line[i+1][0],
                                L=100,
                                l=offset)
        """
        
        output[i][0] = line[i][0]
        
        """
    
    return output

def main():
    if len(sys.argv) < 2:
        print('Usage: python make-image.py <input> <output>')
        sys.exit(0)

    im = Image.fromPNG(sys.argv[1])

    for y in xrange(0, im.height):
        yiq_line = []
        for x in xrange(0, im.width):
            i = (im.width * y) + x
            yiq_line.append(yiq.from_rgb(im.data[i]))
    
        #yiq_line = yiq.simulate_composite_line(yiq_line)
        yiq_line = ntsc_line(yiq_line)
        
        """
        for i in range(len(yiq_line)):
            yiq_line[i][0] = 255
            #yiq_line[i][1] = 0
            #yiq_line[i][2] = 0
        """
    
        for x in xrange(0, im.width):
            i = (im.width * y) + x
            tmp = yiq.to_rgb(yiq_line[x])
            if tmp[0] > 255: tmp[0] = 255
            if tmp[1] > 255: tmp[1] = 255
            if tmp[2] > 255: tmp[2] = 255
            
            im.data[i] = [int(tmp[0]), int(tmp[1]), int(tmp[2])]

    im.savePNG('output.png')

if __name__ == '__main__':
    main()
