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
// Broken or dangerous. void to_vertices(int64_t* indices, int len, int64_t* vertices0, int64_t* vertices1, int64_t* vertices2, int64_t* centroid);
void _to_vertices_latlon(int64_t* indices, int len, double* triangle_info_lats, int dmy1, double* triangle_info_lons, int dmy2 );
void to_area(int64_t* indices, int len, double* areas);

void _to_neighbors(int64_t* indices, int len, int64_t* range_indices, int len_ri);
void _to_compressed_range(int64_t* indices, int len, int64_t* range_indices, int len_ri);
void _to_hull_range   (int64_t* indices, int len, int resolution, int64_t* range_indices, int len_ri, int64_t* result_size, int len_rs);
void _expand_intervals(int64_t* indices, int len, int resolution, int64_t* range_indices, int len_ri, int64_t* result_size, int len_rs);

void _to_hull_range_from_latlon(double* lat, int len_lat, double* lon, int len_lon, int resolution, int64_t* range_indices, int len_ri, int64_t* result_size, int len_rs);
void _to_box_cover_from_latlon(double* lat, int len_lat, double* lon, int len_lon, int resolution, int64_t* range_indices, int len_ri, int64_t* result_size, int len_rs);
void _to_circular_cover(double lat, double lon, double radius, int resolution, int64_t* range_indices, int len_ri, int64_t* result_size, int len_rs);

void from_intervals(int64_t* intervals, int len, int64_t* indices_starts, int64_t* indices_terminators );

void _intersect(int64_t* indices1, int len1, int64_t* indices2, int len2, int64_t* intersection, int leni);
void _intersect_multiresolution(int64_t* indices1, int len1, int64_t* indices2, int len2, int64_t* intersection, int leni);
void _cmp_spatial(int64_t* indices1, int len1, int64_t* indices2, int len2, int64_t* cmp, int len12);
void intersects(int64_t* indices1, int len1, int64_t* indices2, int len2, int* intersects);

// Temporal
void from_utc(int64_t *datetime, int len, int64_t *indices_out, int resolution);
void to_utc_approximate(int64_t* indices, int len, int64_t* datetime_out);
void _cmp_temporal(int64_t* indices1, int len1, int64_t* indices2, int len2, int64_t* cmp, int len12);

//void to_utc(int64_t* indices, int len, double* julian_day);
//void from_tai(double* julian_day, int len, int64_t indices);
//void to_tai(int64_t* indices, int len, double* julian_day);


enum StareResultCase { SpatialIntervals, ArrayIndexSpatialValues };

class StareResult {
 public:
  StareResult() {};
  void add_intervals(STARE_SpatialIntervals sis) {
	  this->sis = sis; this->sCase = SpatialIntervals;
	  // cout << "StareResults: sis size: " << this->sis.size() << endl << flush;
  }
  void add_indexValues(STARE_ArrayIndexSpatialValues sisvs) { this->sisvs = sisvs; this->sCase = ArrayIndexSpatialValues; }
  virtual ~StareResult();
  int                           get_size();
  int                           get_size_as_values();
  int                           get_size_as_intervals();
  void                          copy             (int64_t* indices, int len);
  void                          copy_as_values   (int64_t* indices, int len);
  void                          copy_as_intervals(int64_t* indices, int len);
  void                          convert();
  bool                          converted = false;
  STARE_SpatialIntervals        sis;
  STARE_ArrayIndexSpatialValues sisvs;
  StareResultCase               sCase;
};

StareResult _to_circular_cover1(double lat, double lon, double radius, int resolution);

#endif

