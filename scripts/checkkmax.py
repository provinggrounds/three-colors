import sys

kmax = float(sys.argv[1])

k = 0
kx = 0
ky = 0
kpoints = []
outx = 0
outy = 0
both = 0

while both < 2:
    kx += 1
    k = (kx**2 + ky**2)**0.5
    if k < kmax:
        both = 0
        kpoints.append([kx,ky])
    else:
        ky += 1
        both += 1
        kx = -1

for k in kpoints:
    print k