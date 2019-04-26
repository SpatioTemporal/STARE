#
# contrib/python/example.py
#
# Example script for comparing the legacy lowlevel-lookup-tool with PySTARE.
#
# Michael Rilee, mike@rilee.net, 2019 April 15
#

import numpy as np; from PySTARE import SSTARE;

import os;

s=SSTARE();

lat_array = np.array([30,45])
lon_array = np.array([45,60])
level     = 8

idx=s.ValueFromLatLonDegreesNP(lat_array,lon_array,level)

lat,lon=s.LatLonDegreesFromValueNP(idx)

for i in range(len(lat_array)):
    print('input')
    print( ' lata,lona: ',lat_array[i],lon_array[i] )
    print('output')
    print( ' lat,lon:   ',lat[i],lon[i] )
    print( ' idx:       ',hex(idx[i]))
    print( " ll-tool:   ",os.popen("~/workspace/STARE-CMAKE/build/default/app/lowlevel-lookup-tool --quiet --STARE --latlon "+str(level)+" "+str(lat_array[i])+" "+str(lon_array[i]),'r',1).read() )




