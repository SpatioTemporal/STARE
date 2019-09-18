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

intersected = pystare.intersect(indices, indices, multiresolution=True)
print('3 intersected: ',[hex(i) for i in intersected])

indices1 = numpy.array([indices[1]], dtype=numpy.int64)
intersected = pystare.intersect(indices, indices1, multiresolution=True)
print('4 intersected: ',[hex(i) for i in intersected])

indices1 = numpy.array([0x100000000000000c], dtype=numpy.int64)
intersected = pystare.intersect(indices, indices1, multiresolution=True)
print('5 intersected: ',[hex(i) for i in intersected])
print("")

vertices0 = numpy.zeros([3], dtype=numpy.int64)
vertices1 = numpy.zeros([3], dtype=numpy.int64)
vertices2 = numpy.zeros([3], dtype=numpy.int64)

vertices0,vertices1,vertices2 = pystare.to_vertices(indices)
print('vertices0: ',[hex(i) for i in vertices0])
print('vertices1: ',[hex(i) for i in vertices1])
print('vertices2: ',[hex(i) for i in vertices2])

vertices0_lats,vertices0_lons = pystare.to_latlon(vertices0)
vertices1_lats,vertices1_lons = pystare.to_latlon(vertices1)
vertices2_lats,vertices2_lons = pystare.to_latlon(vertices2)

for i in range(len(vertices0_lats)):
  print(i," vert0 lat,lon: ",vertices0_lats[i],vertices0_lons[i])
  print(i," vert1 lat,lon: ",vertices1_lats[i],vertices1_lons[i])
  print(i," vert2 lat,lon: ",vertices2_lats[i],vertices2_lons[i])
  print("")






