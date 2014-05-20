#!/Users/ccl/Library/Enthought/Canopy_64bit/User/bin/python

import ast
import sys

import numpy as np
from scipy.misc import toimage

num_args = len(sys.argv)

if num_args == 1:
    sys.exit('need input')
elif num_args == 2:
    conf = sys.argv[1]
else: sys.exit('incorrect args')

N = len(sys.argv[1])
gridsize = int(N**0.5)
print gridsize, gridsize
if gridsize==3:
    data = np.zeros( (gridsize+1,gridsize+1,3), dtype=np.uint8 )
else:
    data = np.zeros( (gridsize,gridsize,3), dtype=np.uint8 )
red_rgb = (255,0,0)
green_rgb = (0,255,0)
blue_rgb = (0,0,255)
white_rgb = (255,255,255)

for n in range(N):
    i = n/gridsize
    j = n%gridsize
    cur_color = conf[n]
    print n, i, j, cur_color,
    if cur_color == '+':
        data[i,j] = red_rgb
        print 'r'
    elif cur_color == 'o':
        data[i,j] = green_rgb
        print 'g'
    elif cur_color == '-':
        data[i,j] = blue_rgb
        print 'b'
    else:
        data[i,j] = white_rgb
        print 'w'

print data

fout = '{:d}x{:d}.bmp'.format(gridsize, gridsize)

img = toimage( data , high=255, low=0)       # Create a PIL image
img.show()                      # View in default viewer
img.save(fout)
