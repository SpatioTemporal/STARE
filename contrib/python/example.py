#
# contrib/python/example.py
#
# Example script for comparing the legacy lowlevel-lookup-tool with PySTARE.
#
# Usage: python example.py
#
# Note the hardwired reference to the lowlevel-lookup-tool. The PySTARE module must be available in the PYTHONPATH.
#
# Michael Rilee, mike@rilee.net, 2019 June 18.
#

# [mrilee@noggin-1 src]$ export PYTHONPATH=`pwd`:${PYTHONPATH}
# [mrilee@noggin-1 src]$ python ../../../contrib/python/example.py 
# 100 [30. 45.]
# 101 2
# input
#  lata,lona:  30.0 45.0
# output
#  lat,lon:    30.000000122871402 44.99999990571498
#  idx:        0x399d1bcabd6f9268
#  ll-tool:    0x399d1bcabd6f9268
# 
# input
#  lata,lona:  45.0 60.0
# output
#  lat,lon:    45.00000002707218 60.00000013355399
#  idx:        0x39c1ea506ef249c8
#  ll-tool:    0x39c1ea506ef249c8

import numpy as np; from PySTARE import *;

import os;

idx       = np.zeros(2,      dtype=np.int64)
lat_array = np.array([30,45],dtype=np.double)
lon_array = np.array([45,60],dtype=np.double)
level     = 8

p = newSTARE();

ValueFromLatLonDegreesLevelNP( p, idx, lat_array, lon_array, level )

lat = np.zeros(2, dtype=np.double)
lon = np.zeros(2, dtype=np.double)

LatLonDegreesFromValueNP(p,lat,lon,idx)

print(100,lat_array)
print(101,len(lat_array))

for i in range(len(lat_array)):
    print('input')
    print( ' lata,lona: ',lat_array[i],lon_array[i] )
    print('output')
    print( ' lat,lon:   ',lat[i],lon[i] )
    print( ' idx:       ',hex(idx[i]))
    print( " ll-tool:   ",os.popen("~/workspace/STARE-CMAKE/build/default/app/lowlevel-lookup-tool --quiet --STARE --latlon "+str(level)+" "+str(lat_array[i])+" "+str(lon_array[i]),'r',1).read() )

# This doesn't seem to actually delete p, so watch out for memory leaks and double-free crashes.
deleteSTARE(p);
