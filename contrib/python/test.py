#!/usr/bin/python3

import numpy
import pystare


lat = numpy.array([30,45,60], dtype=numpy.double)
lon = numpy.array([45,60,10], dtype=numpy.double)


indices = pystare.from_latlon(lat, lon, 12)
print('0 indices: ',[hex(i) for i in indices])

lat, lon = pystare.to_latlon(indices)
print(lat, lon)

lat, lon, level = pystare.to_latlonlevel(indices)
print(lat, lon, level)

level = pystare.to_level(indices)
print(level)

area = pystare.to_area(indices)
print(area)

datetime = numpy.array(['2002-02-03T13:56:03.172', '2016-01-05T17:26:00.172'], dtype=numpy.datetime64)
print(datetime)

index = pystare.from_utc(datetime, 6)
print(index)

intersected = numpy.zeros([3], dtype=numpy.int64)
#? leni = 0
pystare._intersect(indices, indices, intersected)
print('1 indices:     ',[hex(i) for i in indices])
print('1 intersected: ',[hex(i) for i in intersected] )

intersected = pystare.intersect(indices, indices)
print('2 intersected: ',[hex(i) for i in intersected])

intersected = pystare.intersect(indices, indices, multilevel=True)
print('3 intersected: ',[hex(i) for i in intersected])


