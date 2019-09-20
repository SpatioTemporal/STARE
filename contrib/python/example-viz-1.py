
from math import ceil
import pystare as ps

import matplotlib as mpl
import matplotlib.pyplot as plt
import matplotlib.tri as tri
import cartopy.crs as ccrs

import numpy as np

def shiftarg_lon(lon):
    if(lon>180):
        return ((lon + 180.0) % 360.0)-180.0
    else:
        return lon

def triangulate(i0,i1,i2):
  # i0,i1,i2,ic = ps.to_vertices(indices)
  i0lat,i0lon = ps.to_latlon(i0)
  i1lat,i1lon = ps.to_latlon(i1)
  i2lat,i2lon = ps.to_latlon(i2)
  lats    = np.zeros([3*len(i0lat)],dtype=np.double)
  lons    = np.zeros([3*len(i0lat)],dtype=np.double)
  intmat  = []
  k=0
  for i in range(len(i0)):
      lats[k]   = i0lat[i]
      lons[k]   = i0lon[i]
      lats[k+1] = i1lat[i]
      lons[k+1] = i1lon[i]
      lats[k+2] = i2lat[i]
      lons[k+2] = i2lon[i]
      intmat.append([k,k+1,k+2])
      k=k+3
  for i in range(len(lons)):
      lons[i] = shiftarg_lon(lons[i])
  return lons,lats,intmat
    
# lat = np.array([0, 0,60], dtype=np.double)
# lon = np.array([0,60,30], dtype=np.double)

lat = np.array([0, 0,60], dtype=np.double)
lon = np.array([0,60,30], dtype=np.double)

s_resolution = 6
indices = ps.from_latlon(lat, lon, s_resolution)

def test1(indices):
    "Plot the outline of a triangle using first three indices."
    i0 = np.array([indices[0]], dtype=np.int64)
    i1 = np.array([indices[1]], dtype=np.int64)
    i2 = np.array([indices[2]], dtype=np.int64)
    lons,lats,intmat = triangulate(i0,i1,i2)
    # print('lats: ',lats)
    # print('lons: ',lons)
    triang = tri.Triangulation(lons,lats,intmat)
    return triang

def plot1(triang):
  ax = plt.axes(projection=ccrs.PlateCarree())
  ax.set_xlim(-180,180)
  ax.set_ylim(-90,90)
  ax.coastlines()
  # plt.contourf(xg,yg,v0g,60,transform=ccrs.PlateCarree())
  # plt.scatter(xg_flat,yg_flat,s=300,c=v_flat)
  # plt.triplot(triang,'ko-')
  plt.triplot(triang,'r-')
  # plt.show()
  return

plot1(test1(indices))

level = 3
hull = ps.to_hull_range(indices,level,100)
h0,h1,h2,hc = ps.to_vertices(hull)
lons1,lats1,intmat1 = triangulate(h0,h1,h2)
print('0 hull len:      ',len(hull))
print('0 hull lats len: ',len(lats1))
jtest=3
j=jtest
print('0 hull lats1:    ',j,lats1[j*3:(j+1)*3])
print('0 hull lons1:    ',j,lons1[j*3:(j+1)*3])
j=0
print('0 hull lats1:    ',[i for i in lats1[j:j+12]])
print('0 hull lons1:    ',[i for i in lons1[j:j+12]])
print('')

tid = np.array([0x4c0000000000003],dtype=np.int64)
t0,t1,t2,tc = ps.to_vertices(tid)
print('t0: ',hex(t0[0]))
print('t1: ',hex(t1[0]))
print('t2: ',hex(t2[0]))
print('tc: ',hex(tc[0]))
print('t0 ll : ',ps.to_latlon(t0))
print('t1 ll : ',ps.to_latlon(t1))
print('t2 ll : ',ps.to_latlon(t2))
print('tc ll : ',ps.to_latlon(tc))

print('')

if True:
    # i=9; ilen=2
    # i=10; ilen=1
    # i=jtest; ilen=4
    i=jtest; ilen=1
    id_test = np.array(hull[i:i+ilen],dtype=np.int64)
    print('i,id  : ',i,[hex(j) for j in id_test])
    i0=i*3; i1=(i+ilen)*3
    lats1   = lats1[i0:i1]
    lons1   = lons1[i0:i1]
    # intmat1 = [intmat1[i]]
    intmat1 = []
    for j in range(ilen):
        intmat1.append([3*j,3*j+1,3*j+2])
    # intmat1 = [[0,2,1]]
    i0test,i1test,i2test,ictest = ps.to_vertices(id_test)
    print('test id:   ',[hex(i) for i in id_test])
    print('test ll : ',[(lats1[i],lons1[i]) for i in range(len(lats1))])
    print('test im : ',[i for i in intmat1])
    lonstest,latstest,intmattest = triangulate(i0test,i1test,i2test)
    print('test lat: ',latstest)
    print('test lon: ',lonstest)
    triangtest = tri.Triangulation(lonstest,latstest,intmattest)
    plt.triplot(triangtest,'g-')
    plt.scatter(lonstest,latstest,s=50,c='g')

print('level   :    ',level)
# print('hull    :    ',[hex(i) for i in hull])
print('hull len:    ',len(hull))
print('hull ll len: ',len(lons1))
print('hull im len: ',len(intmat1)) 
print('hull ll : ',[(lats1[i],lons1[i]) for i in range(len(lats1))])
print('hull im : ',[i for i in intmat1])

triang1 = tri.Triangulation(lons1,lats1,intmat1)
plt.triplot(triang1,'b-')

# ax = plt.axes(projection=ccrs.PlateCarree())
# ax.set_xlim(-180,180)
# ax.set_ylim(-90,90)
# ax.coastlines()
# # plt.contourf(xg,yg,v0g,60,transform=ccrs.PlateCarree())
# # plt.scatter(xg_flat,yg_flat,s=300,c=v_flat)
# # plt.triplot(triang,'ko-')
# plt.triplot(triang1,'r-')
plt.show()




