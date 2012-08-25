
# YIQ Input Matrix (from RGB)
in_matrix = (
        (0.299,     0.587,  0.114),
        (0.596,    -0.274, -0.322),
        (0.212,    -0.523,  0.311))

# YIQ Output Matrix (to RGB)
out_matrix = (
        (1.0,   0.956,  0.621),
        (1.0,  -0.272, -0.647),
        (1.0,  -1.105,  1.702))

def from_rgb(rgb):
    """ Take a list of [R, G, B] and convert it to [Y, I Q] """
    
    yiq = [0.0, 0.0, 0.0]
    for a in range(0, len(in_matrix)):
        for b in range(0, len(in_matrix[a])):
            yiq[a] += rgb[b] * in_matrix[a][b]
        
    return yiq
    
def to_rgb(yiq):
    """ Take a list of [Y, I, Q] and convert it to [R, G B] """

    rgb = [0.0, 0.0, 0.0]
    for a in range(0, len(out_matrix)):
        for b in range(0, len(out_matrix[a])):
            rgb[a] += yiq[b] * out_matrix[a][b]
        
        if rgb[a] > 255:
            rgb[a] = 255
        elif rgb[a] < 0:
            rgb[a] = 0
    
    return rgb

"""

6.2 Mhz Band

Y: 4.2 Mhz
I: 1.5 Mhz
Q: 0.5 Mhz

Y = 0.6774193548387097
I = 0.24193548387096772
Q = 0.08870967741935484

"""

def simulate_composite_line(
            line,
            chroma_i=192.0,
            chroma_q=96.0,
            blur=0.2,
            deluma=7.0,
            dechroma=3.0):
    
    work = []
    left = [0, 0, 0]
    right = line[1]
    
    # Calculate blur amounts
    blur_a = blur / 2.0
    blur_b = float(1.0 - blur)
    
    # Calculate Chroma I and Q steps. This is used to simulate loss of
    # information over NTSC lines, such as Composite.
    chroma_i_step = chroma_i / float(len(line))
    chroma_q_step = chroma_q / float(len(line))
    chroma_i = 0.0
    chroma_q = 0.0
    
    # Since the number of I and Q events are limited over NTSC lines,
    # we use averages to compensate for loss of color information
    # during simulated transmission.
    avg_i = []
    avg_q = []
    
    cur_i = None
    cur_q = None
    
    # Calculate initial I and Q, which are averages
    for i in range(0, len(line)):
        # Increment Chroma I and Q steps
        chroma_i += chroma_i_step
        chroma_q += chroma_q_step
    
        if chroma_i >= 1.0:
            cur_i = float(sum(avg_i)) / float(len(avg_i))
        else:
            avg_i.append(line[i][1])
            
        if chroma_q >= 1.0:
            cur_q = float(sum(avg_q)) / float(len(avg_q))
        else:
            avg_q.append(line[i][2])
        
        if cur_i is not None and cur_q is not None:
            break
    
    # Reset these
    avg_i = [line[i][1]]
    avg_q = [line[i][2]]
    chroma_i = 0.0
    chroma_q = 0.0
    
    # Iterate over line, processing each Y, I, and Q
    for i in range(0, len(line)):
        # Increment Chroma I and Q steps
        chroma_i += chroma_i_step
        chroma_q += chroma_q_step
        
        # When chroma_i is >= to 1.0, cur_i is updated, using the average of
        # the previous pixels, and everything else is reset.
        if chroma_i >= 1.0:
            avg_i.append(line[i][1])
            cur_i = float(sum(avg_i)) / float(len(avg_i))
            chroma_i = 1.0 - chroma_i
            avg_i = [line[i][1]]
        else:
            avg_i.append(line[i][1])
            
        if chroma_q >= 1.0:
            avg_q.append(line[i][2])
            cur_q = float(sum(avg_q)) / float(len(avg_q))
            chroma_q = 1.0 - chroma_q
            avg_q = [line[i][2]]
        else:
            avg_q.append(line[i][2])
        
        cur_l = line[i][0]
        
        """
        luma = (left[0] * 0.15 + right[0] * 0.15 + line[i][0] * 0.7)
        #luma = line[i][0]
        work.append([luma, float(cur_i), float(cur_q)])
        """
        
        # Blur the Luma and Chroma chanels individually.
        # This uses a horizontal blur method, where i-1, i, and i+1 are
        # blurred together.
        a = (left[0] * blur_a + right[0] * blur_a + cur_l  * blur_b)
        b = (left[1] * blur_a + right[1] * blur_a + cur_i       * blur_b)
        c = (left[2] * blur_a + right[2] * blur_a + cur_q       * blur_b)
        
        # De-Luma
        if a < 0:
            a += deluma
        else:
            a -= deluma
        
        # De-Chroma
        if b < 0:
            b += dechroma
        else:
            b -= dechroma
        
        # De-Chroma
        if c < 0:
            b += dechroma
        else:
            b -= dechroma
        
        work.append([a, b, c])
        
        # Set left/right pixel
        left = line[i]
        if i+2 >= len(line):
            right = [0, 0, 0]
        else:
            right = line[i+2]
    
    return work
