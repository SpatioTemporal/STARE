/**
 * PySTARE.h
 *
 *  Created on: Jun 13, 2019
 *      Author: mrilee
 *
 *  Copyright (C) 2019 Rilee Systems Technologies LLC
 *
 *
 *  EXAMPLE
 *
 *  p = newSTARE(); d=np.array([1,2,3],dtype=np.double); i=np.zeros(3,dtype=np.int64); ValueFromJDTAINP(p,i,d,100);
 *
 *  lat=np.array([-45],dtype=np.double); lon=np.array([135],dtype=np.double); i=np.zeros(1,dtype=np.int64);  ValueFromLatLonDegreesLevelNP( p, i, lat, lon, 20 ); print(lat,lon,i)
 *
 *  [-45.] [135.] [720575940379279476]
 *
 *  deleteSTARE(p);
 */


#ifndef INCLUDE_PYSTARE_H_
#define INCLUDE_PYSTARE_H_


#include <stdio.h>
#include "STARE.h"
#include "SpatialRange.h"

static STARE stare;

// Spatial
void from_latlon(double* lat, int len_lat,  double * lon, int len_lon, int64_t* indices, int level);
void to_latlon(int64_t* indices, int len, double* lat, double* lon);
void to_latlonlevel(int64_t* indices, int len, double* lat, double* lon, int* levels);
void to_level(int64_t* indices, int len,  int* levels);
void to_triangle(int64_t* indices, int len);
void to_vertices(int64_t* indices, int len, int64_t* vertices0, int64_t* vertices1, int64_t* vertices2);
void to_area(int64_t* indices, int len, double* areas);

void _to_compressed_range(int64_t* indices, int len, int64_t* range_indices, int len_ri);
void from_intervals(int64_t* intervals, int len, int64_t* indices_starts, int64_t* indices_terminators );

void _intersect(int64_t* indices1, int len1, int64_t* indices2, int len2, int64_t* intersection, int leni);
void _intersect_multiresolution(int64_t* indices1, int len1, int64_t* indices2, int len2, int64_t* intersection, int leni);

// Temporal
void from_utc(int64_t *datetime, int len, int64_t *indices, int resolution);
//void to_utc(int64_t* indices, int len, double* julian_day);
//void from_tai(double* julian_day, int len, int64_t indices);
//void to_tai(int64_t* indices, int len, double* julian_day);

#endif

