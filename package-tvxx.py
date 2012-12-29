
"""
"""

import os, sys

def main():
    files = {
        '_tv2x.h': [
            'include/tvxx_internal.h',
            'include/rgb.h',
            'include/tv2x.h',
        ],
        '_tv2x.c': [
            'include/tvxx_internal.h',
            'include/rgb.h',
            'include/rgb_defines.h',
            'include/tv2x.h',
            'src/rgb/rgb.c',
            'src/tv2x/tv2x.c'
        ],
        '_tv4x.h': [
            'include/tvxx_internal.h',
            'include/rgb.h',
            'include/tv4x.h',
        ],
        '_tv4x.c': [
            'include/tvxx_internal.h',
            'include/rgb.h',
            'include/rgb_defines.h',
            'include/yiq.h',
            'include/tv4x.h',
            'src/rgb/rgb.c',
            'src/yiq/yiq.c',
            'src/tv4x/shadow_masks.c',
            'src/tv4x/tv4x.c',
        ]
    }
    
    for filename, filelist in files.items():
        with open(filename, 'w') as output:
            for i in filelist:
                with open(i, 'r') as input:
                    output.write('\n\n/* %s */\n\n' % (i))
                    output.write(input.read())
                    input.close()
            output.close()
    sys.exit(0)

if __name__ == '__main__':
    main()
