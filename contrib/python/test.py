#!/usr/bin/python3

import numpy
import pystare
import intervals


print('Spatial tests')
lat = numpy.array([30,45,60], dtype=numpy.double)
lon = numpy.array([45,60,10], dtype=numpy.double)

indices = pystare.from_latlon(lat, lon, 12)
print('0 indices: ', [hex(i) for i in indices])

lat, lon = pystare.to_latlon(indices)
print(lat, lon)

lat, lon, level = pystare.to_latlonlevel(indices)
print(lat, lon, level)

level = pystare.to_level(indices)
print(level)

area = pystare.to_area(indices)
print(area)

print('Datetime tests')
datetime = numpy.array(['1970-01-01T00:00:00', 
                        '2000-01-01T00:00:00', 
                        '2002-02-03T13:56:03.172', 
                        '2016-01-05T17:26:00.172'], dtype=numpy.datetime64)
print(datetime)
print(datetime.astype(numpy.int64))

index = pystare.from_utc(datetime.astype(numpy.int64), 6)
print([hex(i) for i in index])

index = pystare.from_utc(datetime.astype(numpy.int64), 27)
print([hex(i) for i in index])

print('cmp_temporal: should be diagonal')
for i in range(index.size):
  print(i, ' cmp ', pystare.cmp_temporal(numpy.array([index[i]],dtype=numpy.int64), index))

datetime_x = pystare.to_utc_approximate(index)
datetime_r = numpy.array(pystare.to_utc_approximate(index), dtype='datetime64[ms]')
print('type datetime_r: ', type(datetime_r))
print('type datetime_r: ', datetime_r.dtype)
print(datetime_r)
print(datetime_r.astype(numpy.int64))
print('delta datetime:    ', datetime-datetime_r)

for i in range(index.size):
  print(i,' o,r ',datetime[i],datetime_r[i], ' x ', datetime[i].astype(numpy.int64), datetime_x[i])

exit()

print('Intersection tests')
intersected = numpy.zeros([3], dtype=numpy.int64)
#? leni = 0
pystare._intersect(indices, indices, intersected)
print('1 indices:     ', [hex(i) for i in indices])
print('1 intersected: ', [hex(i) for i in intersected] )

intersected = pystare.intersect(indices, indices)
print('2 intersected: ', [hex(i) for i in intersected])

intersected = pystare.intersect(indices, indices, multiresolution=True)
print('3 intersected: ', [hex(i) for i in intersected])

indices1 = numpy.array([indices[1]], dtype=numpy.int64)
intersected = pystare.intersect(indices, indices1, multiresolution=True)
print('4 intersected: ', [hex(i) for i in intersected])

indices1 = numpy.array([0x100000000000000c], dtype=numpy.int64)
intersected = pystare.intersect(indices, indices1, multiresolution=True)
print('5 intersected: ', [hex(i) for i in intersected])
print('')

indices1 = numpy.array([0, 0, 0], dtype=numpy.int64)
pystare._to_compressed_range(indices, indices1)
print('_indices1: ', [hex(i) for i in indices1])

indices1 = numpy.array([0, 0, 0], dtype=numpy.int64)
indices1 = pystare.to_compressed_range(indices)
print('indices1:  ', list(map(hex,indices1)))

indices1 = numpy.zeros([1000], dtype=numpy.int64)
result_size = numpy.zeros([1], dtype=numpy.int)
pystare._to_hull_range(indices, 8, indices1, result_size)
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
intervals          = numpy.array(intervals.src,dtype=numpy.int64)
expected_expanded  = numpy.array(intervals.expanded_src,dtype=numpy.int64)
intervals_expanded = numpy.zeros([len(expected_expanded)],dtype=numpy.int64)
expected_len       = numpy.zeros([1],dtype=numpy.int64)

print('len(intervals)          ', len(intervals))
print('len(expected_expanded)  ', len(expected_expanded))
print('len(intervals_expanded) ', len(intervals_expanded))
print('len(expected_len)       ', len(expected_len))

intervals_len = len(intervals)
resolution = -1
pystare._expand_intervals(intervals,resolution,intervals_expanded,expected_len)
# print('intervals_expanded: ',[hex(i) for i in intervals_expanded])
print('expected_len:           ', expected_len)
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
