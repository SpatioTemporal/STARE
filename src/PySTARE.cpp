/*
 * PySTARE.cpp
 *
 *  Created on: Jun 13, 2019
 *      Author: mrilee
 *
 *  Copyright (C) 2019 Rilee Systems Technologies LLC
 */


#include "PySTARE.h"

// Spatial
void from_latlon(double* lat, int len_lat, double * lon, int len_lon, int64_t* indices, int level) {            
    for (int i=0; i<len_lat; i++) {        
        indices[i] = stare.ValueFromLatLonDegrees(lat[i], lon[i], level);
    }
}

void to_latlon(int64_t* indices, int len, double* lat, double* lon) { 
    for (int i=0; i< len; i++) { 
        LatLonDegrees64 latlon = stare.LatLonDegreesFromValue(indices[i]);
		lat[i] = latlon.lat; 
        lon[i] = latlon.lon;
    }    
}

void to_latlonlevel(int64_t* indices, int len, double* lat, double* lon, int* levels) {
    for (int i=0; i<len; i++) {                
        LatLonDegrees64 latlon = stare.LatLonDegreesFromValue(indices[i]);
        lat[i] = latlon.lat; 
        lon[i] = latlon.lon;
        levels[i] = stare.ResolutionLevelFromValue(indices[i]);
    }
}

void to_level(int64_t* indices, int len, int* levels) {
    for (int i=0; i<len; i++) {                
        levels[i] = stare.ResolutionLevelFromValue(indices[i]);
    }    

}

void to_triangle(int64_t* indices, int len) {
    for (int i=0; i<len; i++) {             
        stare.TriangleFromValue(indices[i]);
        //TBD
    }
}

void to_area(int64_t* indices, int len, double* areas) {
    for (int i=0; i<len; i++) {
        areas[i] = stare.AreaFromValue(indices[i]);        
    }
    
}

/*
void intersect(int64_t* indices1, int len1, int64_t* indices2, int len2, int64_t* intersection, int leni) {
	SpatialIntervals si1(indices1, indices1+len1), si2(indices2, indices2+len2);
	SpatialRange r1(si1), r2(si2);
	SpatialRange *ri = r1 & r2;
	SpatialIntervals result = ri->toSpatialIntervals();
	leni = result->size();
	for(int i=0; i<leni; ++i) {
		intersection[i] = result[i];
	}
}
*/

// Temporal
void from_utc(int64_t *datetime, int len, int64_t *indices, int resolution) {
    int type = 2;
    for (int i=0; i<len; i++) {                       
        indices[i] = stare.ValueFromUTC(datetime[i], resolution, type);
    }
}
