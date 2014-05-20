#!/Users/ccl/Library/Enthought/Canopy_64bit/User/bin/python
# creates a mN x mN grid from an NxN cell
# user specifies m
# outputs the new grid in +o- format, as well as image and energy

import sys
import numpy as np
from scipy.misc import toimage

mydict = {'+' : 0 , 'o' : 1, '-' : 2}
inv_dict = dict((v,k) for k, v in mydict.iteritems())

num_args = len(sys.argv)
if num_args == 1:
    sys.exit('need input: [configuration in +o-] [m for mN x mN]')
elif num_args != 3:
    sys.exit('incorrect args: [configuration in +o-] [m for mN x mN]')

conf = sys.argv[1]
N = len(conf)
m = int(sys.argv[2])
if m <= 1:
    sys.exit('need m>1')

l = int(N**0.5)
print l

newconf = np.zeros( (l*m, l*m, 1), dtype = np.uint8 )
data = np.zeros( (l*m, l*m, 3), dtype = np.uint8 )

red_rgb = (255,0,0)
green_rgb = (0,255,0)
blue_rgb = (0,0,255)
white_rgb = (255,255,255)


for n in range(N):
    i = n/l
    j = n%l
    cur_color = conf[n]
    for mx in range(m):
        for my in range(m):
            newconf[i+l*mx,j+l*my] = mydict[cur_color]
            if cur_color == '+':
                data[i+l*mx,j+l*my] = red_rgb
            elif cur_color == 'o':
                data[i+l*mx,j+l*my] = green_rgb
            elif cur_color == '-':
                data[i+l*mx,j+l*my] = blue_rgb
            else:
                data[i+l*mx,j+l*my] = white_rgb

stringconf = ''
for i in range(m*l):
    for j in range(m*l):
        cur_char = newconf[i,j][0]
        stringconf += inv_dict[cur_char]

print stringconf

fout = '{:d}x{:d}_x{:d}.bmp'.format(l, l, m)

img = toimage( data , high=255, low=0)       # Create a PIL image
img.show()                      # View in default viewer
img.save(fout)
