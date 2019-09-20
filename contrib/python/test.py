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
print('')

vertices0 = numpy.zeros([3], dtype=numpy.int64)
vertices1 = numpy.zeros([3], dtype=numpy.int64)
vertices2 = numpy.zeros([3], dtype=numpy.int64)
centroid  = numpy.zeros([3], dtype=numpy.int64)

vertices0,vertices1,vertices2,centroid = pystare.to_vertices(indices)
print('vertices0: ',[hex(i) for i in vertices0])
print('vertices1: ',[hex(i) for i in vertices1])
print('vertices2: ',[hex(i) for i in vertices2])
print('centroid:  ',[hex(i) for i in centroid])
print('')

vertices0_lats,vertices0_lons = pystare.to_latlon(vertices0)
vertices1_lats,vertices1_lons = pystare.to_latlon(vertices1)
vertices2_lats,vertices2_lons = pystare.to_latlon(vertices2)
centroid_lats,centroid_lons = pystare.to_latlon(centroid)

for i in range(len(vertices0_lats)):
  print(i," vert0 lat,lon: ",vertices0_lats[i],vertices0_lons[i])
  print(i," vert1 lat,lon: ",vertices1_lats[i],vertices1_lons[i])
  print(i," vert2 lat,lon: ",vertices2_lats[i],vertices2_lons[i])
  print(i," centr lat,lon: ",centroid_lats[i],centroid_lons[i])
  print("")

indices1 = numpy.array([0,0,0], dtype=numpy.int64)
pystare._to_compressed_range(indices,indices1)
print('_indices1: ',[hex(i) for i in indices1])

indices1 = numpy.array([0,0,0], dtype=numpy.int64)
indices1 = pystare.to_compressed_range(indices)
print('indices1:  ',list(map(hex,indices1)))

indices1 = numpy.zeros([1000], dtype=numpy.int64)
result_size = numpy.zeros([1], dtype=numpy.int)
pystare._to_hull_range(indices,8,indices1,result_size)
# print('hull indices1:  ',list(map(hex,indices1[0:100])))
print('hull result_size: ',result_size)
# print('hull indices1:  ',list(map(hex,indices1[899:910])))
indices1=indices1[0:result_size[0]]
print('hull trimmed size ',indices1.size)
print('')

hull_indices = pystare.to_hull_range(indices,8,2000)
print('hull_indices size: ',hull_indices.size)
print('')

cmp = numpy.zeros([9],dtype=numpy.int64)
pystare._cmp_spatial(indices,indices,cmp)
print('cmp input: ',indices)
print('cmp:       ',cmp,' i.e. diagonal')
print('')
cmp = numpy.zeros([3*1],dtype=numpy.int64)
indices1 = numpy.zeros([1],dtype=numpy.int64)
indices1[0] = indices[1]
print('cmp input1: ',indices)
print('cmp input2: ',indices1)
pystare._cmp_spatial(indices,indices1,cmp)
print('cmp:        ',cmp)
print('')
cmp = numpy.zeros([3*2],dtype=numpy.int64)
indices1 = numpy.zeros([2],dtype=numpy.int64)
indices1[0] = indices[1]
indices1[1] = indices[1]+3
print('cmp input1: ',indices)
print('cmp input2: ',indices1)
pystare._cmp_spatial(indices,indices1,cmp)
print('cmp:        ',cmp)
pystare._cmp_spatial(indices1,indices,cmp)
print('cmp:        ',cmp)
print('')
mp = numpy.zeros([3*2],dtype=numpy.int64)
indices1 = numpy.zeros([2],dtype=numpy.int64)
indices1[0] = indices[1]-2
indices1[1] = indices[1]
print('cmp input1: ',indices)
print('cmp input2: ',indices1)
# pystare._cmp_spatial(indices,indices1,cmp)
cmp1 = pystare.cmp_spatial(indices,indices1)
print('cmp1:       ',cmp1)
print('')

tid = numpy.array([0x4c0000000000003],dtype=numpy.int64)
t0,t1,t2,tc = pystare.to_vertices(tid)
print('t0: ',hex(t0[0]))
print('t1: ',hex(t1[0]))
print('t2: ',hex(t2[0]))
print('tc: ',hex(tc[0]))
print('t0 ll : ',pystare.to_latlon(t0))
print('t1 ll : ',pystare.to_latlon(t1))
print('t2 ll : ',pystare.to_latlon(t2))
print('tc ll : ',pystare.to_latlon(tc))
print('')
