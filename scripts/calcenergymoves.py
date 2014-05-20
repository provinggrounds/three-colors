#!/Users/ccl/Library/Enthought/Canopy_64bit/User/bin/python
# gets energy of possible moves

import sys
import csv
import numpy as np
import cmath

dict = {'+' : 0 , 'o' : 1, '-' : 2}

num_args = len(sys.argv)
if num_args == 1:
    sys.exit('need input: [configuration in +o-] [kmax]')
elif num_args != 3:
    sys.exit('incorrect args: [configuration in +o-] [kmax]')

conf = sys.argv[1]
N = len(conf)
kmax = float(sys.argv[2])

l = int(N**0.5)

def CalcEnergy(conf, kpoints, N):
    """
    E = sum_k S_0(k) + S_1(k)
    S(k) = 1/N *|rho(k)|^2 = rho_R^2 + rho_I^2
    calculates energy E of configuration [conf], where subscripts 0,1 denote two of species
    
    k points are specified as (nx,ny) for square lattice
    """
    energy = 0
    N = len(conf)
    l = int(N**0.5)
    for k in kpoints:
        kx = k[0] * 2 * np.pi / l
        ky = k[1] * 2 * np.pi / l
        rho0 = complex(0,0)
        rho1 = complex(0,0)
        for i in range(N):
            curspin = conf[i]
            if dict[curspin] == 0:
                x = i%l
                y = int(i/l)
                rho0 += cmath.exp(complex(0,kx * x + ky * y))
            elif dict[curspin] == 1:
                x = i%l
                y = int(i/l)
                rho1 += cmath.exp(complex(0,kx * x + ky * y))
        energy += (rho0.real**2 + rho0.imag**2) + (rho1.real**2 + rho1.imag**2)
    return energy/N

def GetKPoints(kmax):
    k = 0; kx = 0; ky = 0;
    kpoints = []
    both = 0
    while both < 2:
        kx += 1
        k = (kx**2 + ky**2)**0.5
        if k < kmax:
            both = 0
            kpoints.append([kx,ky])
            kpoints.append([-kx,-ky])
        else:
            ky += 1
            both += 1
            kx = -1
    return kpoints

def SwapChars(i,j,conf):
    s = list(conf)
    if i > len(conf) or j > len(conf):
        return conf
    s[i],s[j] = s[j], s[i]
    return ''.join(s)

kpoints = GetKPoints(kmax)
oldE = CalcEnergy(conf, kpoints, N)

print 'configuration: ', conf
print '(N, kmax) = ', N, kmax
print 'ORIGINAL ENERGY = ', oldE

"""
    1. go through and list possible swaps
    2. enumerate possible swaps, showing which positions are swapped, new energy, and change in energy
    3. rank swaps at end, with best and worst
    """

# generate swaps
n_s = 3
colorindices = []
for i in range(n_s):
    colorindices.append([])
for i in range(N):
    curspin = conf[i]
    colorindices[dict[curspin]].append(i)
swaps = []
for i in range(n_s):
    for j in range(i+1,n_s):
        for col0 in colorindices[i]:
            for col1 in colorindices[j]:
                swaps.append([col0,col1])

newconf = []
newE = []
delE = []
count = 1000
for i in range(len(swaps)):
    if i%1000 == 0:
        print i, 'of', len(swaps)
    s = swaps[i]
    nconf = SwapChars(s[0],s[1],conf)
    nE = CalcEnergy(nconf, kpoints, N)
    dE = nE - oldE
    if dE < 0:
        print nconf, dE
    newconf.append(nconf)
    newE.append(nE)
    delE.append(dE)

swaps = np.array(swaps)

data = np.rec.fromarrays((swaps[:,0], swaps[:,1],newE,delE,newconf), names=('i','j','newE', 'delE', 'conf'))

print 'i','j','newE', 'delE', 'conf'
print data
print 'min delE=',np.min(delE)
print 'max delE=',np.max(delE)
