/*
 * PySTARE1.cpp
 *
 *  Created on: Jun 13, 2019
 *      Author: mrilee
 *
 *  Copyright (C) 2019 Rilee Systems Technologies LLC
 */


#include "PySTARE1.h"


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


// Temporal
void from_utc(int64_t *datetime, int len, int64_t *indices, int resolution) {
    int type = 2;
    for (int i=0; i<len; i++) {                       
        indices[i] = stare.ValueFromUTC(datetime[i], resolution, type);
    }
}
