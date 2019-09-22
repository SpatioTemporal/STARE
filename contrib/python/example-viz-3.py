
from math import ceil
import pystare as ps

import matplotlib as mpl
import matplotlib.pyplot as plt
import matplotlib.tri as tri
import cartopy.crs as ccrs

import numpy as np

def shiftarg_lon(lon):
    "If lon is outside +/-180, then correct back."
    if(lon>180):
        return ((lon + 180.0) % 360.0)-180.0
    else:
        return lon

def triangulate1(lats,lons):
    "Prepare data for tri.Triangulate."
    print('triangulating1...')
    intmat=[]
    npts=int(len(lats)/3)
    k=0
    for i in range(npts):
        intmat.append([k,k+1,k+2])
        k=k+3
    for i in range(len(lons)):
        lons[i] = shiftarg_lon(lons[i])
    print('triangulating1 done.')      
    return lons,lats,intmat

def plot1(lon,lat,lons,lats,triang,c0='r',c1='b',transf=None,lw=1):
    if(lon is not None):
        x=np.zeros([lon.size+1],dtype=np.double);x[:-1]=lon[:];x[-1]=lon[0]
        y=np.zeros([lat.size+1],dtype=np.double); y[:-1]=lat[:]; y[-1]=lat[0]
        ax.plot(x,y,True,transform=transf,c=c0)
    plt.triplot(triang,c1+'-',transform=transf,lw=lw,markersize=3)
    plt.scatter(lons,lats,s=10,c=c1,transform=transf)
    return

def make_hull(lat0,lon0,resolution0,ntri0):
    hull0 = ps.to_hull_range_from_latlon(lat0,lon0,resolution0,ntri0)
    lath0,lonh0,lathc0,lonhc0 = ps.to_vertices_latlon(hull0)
    lons0,lats0,intmat0 = triangulate1(lath0,lonh0)
    triang0 = tri.Triangulation(lons0,lats0,intmat0)
    return lats0,lons0,triang0,hull0

# resolution = 7
# resolution = 12
resolution = 13 # Lost some triangles! Needed to increase ntri0!
resolution0 = resolution; ntri0 = 1500
lat0 = np.array([ 0, 0, 1,1], dtype=np.double)
lon0 = np.array([ 0,1,1,0], dtype=np.double)
lats0,lons0,triang0,hull0 = make_hull(lat0,lon0,resolution0,ntri0)
print('hull0: ',len(hull0))

# Set up the projection and transformation
proj = ccrs.PlateCarree()
# proj = ccrs.Robinson()
# proj = ccrs.Geodesic()
# proj   = ccrs.Mollweide()
transf = ccrs.Geodetic()
# transf = ccrs.PlateCarree()
plt.figure()
plt.subplot(projection=proj,transform=transf)
ax = plt.axes(projection=proj,transform=transf)
ax.set_global()
ax.coastlines()

plot1(lon0,lat0,lons0,lats0,triang0,c0='r',c1='b',transf=transf)
plt.show()
