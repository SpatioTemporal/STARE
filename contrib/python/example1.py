#
# contrib/python/example1.py
#
# Example script for comparing the legacy lowlevel-lookup-tool with PySTARE1.
#
# Usage: python example1.py
#
# Note the hardwired reference to the lowlevel-lookup-tool. The PySTARE1 module must be available in the PYTHONPATH.
#
# Michael Rilee, mike@rilee.net, 2019 June 18.
#

import numpy as np; from PySTARE1 import *;

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
