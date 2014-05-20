#!/Users/ccl/Library/Enthought/Canopy_64bit/User/bin/python
# converts coordinates on NxN grid to conf

import shutil
import ast
import sys
import os
import csv

from subprocess import Popen, PIPE
import numpy as np
from scipy.misc import toimage


dict = {0 : '+' , 1 : 'o', 2 : '-'}

num_args = len(sys.argv)
if num_args == 1:
    sys.exit('need input')
elif num_args == 2:
    fin = sys.argv[1]
else: sys.exit('incorrect args')

lines = list(csv.reader( open(fin, 'rb') , delimiter = ',' ))

n_s = int(lines[0][0])
N = int(lines[1][0])
gridlen = int(N**0.5)
n_c = []
grid = np.empty([gridlen,gridlen], dtype=object)

for i in range(n_s):
    n_c.append( int(lines[2+i][0]))

counter = n_s + 2
for i in range(n_s):
    counter+=1
    for j in range(n_c[i]):
        coord = lines[counter]
        x = int(coord[0])
        y = int(coord[1])
        grid[x][y] = dict[i]
        counter+=1

s = [''.join(row) for row in grid]

print ''.join(s)