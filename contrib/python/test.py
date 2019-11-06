#!/usr/bin/python3

import numpy
import pystare


print('\nSpatial tests')
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

print('\nDatetime tests')
datetime = numpy.array(['1970-01-01T00:00:00', '2000-01-01T00:00:00', '2002-02-03T13:56:03.172', '2016-01-05T17:26:00.172'], dtype=numpy.datetime64)
print(datetime)
print(datetime.astype(numpy.int64))

index = pystare.from_utc(datetime.astype(numpy.int64), 6)
print([hex(i) for i in index])

index = pystare.from_utc(datetime.astype(numpy.int64), 27)
print([hex(i) for i in index])

print('cmp_temporal: should be diagonal')
# print(' cmp \n',pystare.cmp_temporal(index,index).reshape(4,4))
for i in range(index.size):
  print(i,' cmp ',pystare.cmp_temporal(numpy.array([index[i]],dtype=numpy.int64),index))

datetime_x = pystare.to_utc_approximate(index)
datetime_r = numpy.array(pystare.to_utc_approximate(index),dtype='datetime64[ms]')
print('type datetime_r: ',type(datetime_r))
print('type datetime_r: ',datetime_r.dtype)
print(datetime_r)
print(datetime_r.astype(numpy.int64))
print('delta datetime:    ',datetime-datetime_r)

for i in range(index.size):
  print(i,' o,r ',datetime[i],datetime_r[i],' x ',datetime[i].astype(numpy.int64),datetime_x[i])

exit()
# exit()

print('\nIntersection tests')
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


print('Expand intervals')
intervals_src = [\
 0x2320000000000005,\
 0x2324000000000005,\
 0x2327ffffffffffff,\
 0x3aa0000000000005,\
 0x3aa7ffffffffffff,\
 0x3aa8000000000004,\
 0x3ab2000000000005,\
 0x3ac7ffffffffffff,\
 0x3ad0000000000005,\
 0x3ae7ffffffffffff,\
 0x3af2000000000005,\
 0x3afa000000000005,\
 0x3b40000000000004,\
 0x3b4c000000000005,\
 0x3b52000000000005,\
 0x3b5a000000000005,\
 0x3b5fffffffffffff,\
 0x3b80000000000004,\
 0x3b8a000000000005,\
 0x3b8fffffffffffff,\
 0x3b90000000000004,\
 0x3b9fffffffffffff,\
 0x3bc0000000000005,\
 0x3bc7ffffffffffff,\
 0x3bc8000000000004,\
 0x3bd0000000000005,\
 0x3bdfffffffffffff,\
 0x3be2000000000005,\
 0x3be8000000000004,\
 0x3bf4000000000005,\
 0x3bf8000000000005,\
 0x3bfc000000000005,\
 0x3bffffffffffffff,\
 0x3e40000000000005,\
 0x3e43ffffffffffff,\
 0x3e46000000000005,\
 0x3f20000000000005,\
 0x3f24000000000005,\
 0x3f27ffffffffffff,\
 0x3f32000000000005,\
 0x3f36000000000005,\
 0x3f3a000000000005,\
 0x3fa0000000000005\
]

intervals_expanded_src = [\
 0x2320000000000005 ,\
 0x2324000000000005 ,\
 0x2326000000000005 ,\
 0x3aa0000000000005 ,\
 0x3aa2000000000005 ,\
 0x3aa4000000000005 ,\
 0x3aa6000000000005 ,\
 0x3aa8000000000004 ,\
 0x3ab2000000000005 ,\
 0x3ab4000000000005 ,\
 0x3ab6000000000005 ,\
 0x3ab8000000000005 ,\
 0x3aba000000000005 ,\
 0x3abc000000000005 ,\
 0x3abe000000000005 ,\
 0x3ac0000000000005 ,\
 0x3ac2000000000005 ,\
 0x3ac4000000000005 ,\
 0x3ac6000000000005 ,\
 0x3ad0000000000005 ,\
 0x3ad2000000000005 ,\
 0x3ad4000000000005 ,\
 0x3ad6000000000005 ,\
 0x3ad8000000000005 ,\
 0x3ada000000000005 ,\
 0x3adc000000000005 ,\
 0x3ade000000000005 ,\
 0x3ae0000000000005 ,\
 0x3ae2000000000005 ,\
 0x3ae4000000000005 ,\
 0x3ae6000000000005 ,\
 0x3af2000000000005 ,\
 0x3afa000000000005 ,\
 0x3b40000000000004 ,\
 0x3b4c000000000005 ,\
 0x3b52000000000005 ,\
 0x3b5a000000000005 ,\
 0x3b5c000000000005 ,\
 0x3b5e000000000005 ,\
 0x3b80000000000004 ,\
 0x3b8a000000000005 ,\
 0x3b8c000000000005 ,\
 0x3b8e000000000005 ,\
 0x3b90000000000004 ,\
 0x3b98000000000004 ,\
 0x3bc0000000000005 ,\
 0x3bc2000000000005 ,\
 0x3bc4000000000005 ,\
 0x3bc6000000000005 ,\
 0x3bc8000000000004 ,\
 0x3bd0000000000005 ,\
 0x3bd2000000000005 ,\
 0x3bd4000000000005 ,\
 0x3bd6000000000005 ,\
 0x3bd8000000000005 ,\
 0x3bda000000000005 ,\
 0x3bdc000000000005 ,\
 0x3bde000000000005 ,\
 0x3be2000000000005 ,\
 0x3be8000000000004 ,\
 0x3bf4000000000005 ,\
 0x3bf8000000000005 ,\
 0x3bfc000000000005 ,\
 0x3bfe000000000005 ,\
 0x3e40000000000005 ,\
 0x3e42000000000005 ,\
 0x3e46000000000005 ,\
 0x3f20000000000005 ,\
 0x3f24000000000005 ,\
 0x3f26000000000005 ,\
 0x3f32000000000005 ,\
 0x3f36000000000005 ,\
 0x3f3a000000000005 ,\
 0x3fa0000000000005 \
]                  
intervals          = numpy.array(intervals_src,dtype=numpy.int64)
expected_expanded  = numpy.array(intervals_expanded_src,dtype=numpy.int64)
intervals_expanded = numpy.zeros([len(expected_expanded)],dtype=numpy.int64)
expected_len       = numpy.zeros([1],dtype=numpy.int64)

print('len(intervals)          ',len(intervals))
print('len(expected_expanded)  ',len(expected_expanded))
print('len(intervals_expanded) ',len(intervals_expanded))
print('len(expected_len)       ',len(expected_len))
intervals_len = len(intervals)
resolution = -1
pystare._expand_intervals(intervals,resolution,intervals_expanded,expected_len)
# print('intervals_expanded: ',[hex(i) for i in intervals_expanded])
print('expected_len:           ',expected_len)
error_found = False
for i in range(len(intervals_expanded)):
  if(intervals_expanded[i] != expected_expanded[i]):
    error_found = True
    print('_expanded_intervals error at i = ',i,' ',intervals_expanded[i],' != ',expected_expanded[i])
if(not error_found):
  print('_expanded_intervals ok')
  
intervals_expanded_1 = pystare.expand_intervals(intervals,resolution)
for i in range(len(intervals_expanded_1)):
  if(intervals_expanded_1[i] != expected_expanded[i]):
    error_found = True
    print('expanded_intervals error at i = ',i,' ',intervals_expanded_1[i],' != ',expected_expanded[i])
if(not error_found):
  print('expanded_intervals ok')
