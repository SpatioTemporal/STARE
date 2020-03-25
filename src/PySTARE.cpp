/*
 * PySTARE.cpp
 *
 c*  Created on: Jun 13, 2019
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

/*
 * Broken or dangerous
 *
void to_vertices(int64_t* indices, int len, int64_t* vertices0, int64_t* vertices1, int64_t* vertices2, int64_t* centroid) {
    for (int i=0; i<len; i++) {
        Triangle tr = stare.TriangleFromValue(indices[i]);
#if DIAG
        cout << "tr: " << setw(16) << hex << indices[i] << dec << " ";
        for(int j=0; j<3; ++j) { cout << setw(16) << tr.vertices[j] << "; "; }
#endif
        vertices0[i] = stare.ValueFromSpatialVector(tr.vertices[0]);
        vertices1[i] = stare.ValueFromSpatialVector(tr.vertices[1]);
        vertices2[i] = stare.ValueFromSpatialVector(tr.vertices[2]);
        centroid[i]  = stare.ValueFromSpatialVector(tr.centroid);
#if DIAG
        cout << setw(16) << hex
        		<< vertices0[i] << " "
				<< vertices1[i] << " "
				<< vertices2[i] << dec;
        cout << endl << flush;
#endif
    }
}
*/

/*
 * len of the output arrays is 4 times the input array lenght (len).
 */
void _to_vertices_latlon(int64_t* indices, int len, double* triangle_info_lats, int dmy1, double* triangle_info_lons, int dmy2 ) {
	double lat = 0, lon = 0;
	int k=0;
    for (int i=0; i<len; i++) {
        Triangle tr = stare.TriangleFromValue(indices[i]);
#if DIAG
        cout << "tr: " << setw(16) << hex << indices[i] << dec << " ";
        for(int j=0; j<3; ++j) { cout << setw(16) << tr.vertices[j] << "; "; }
#endif
        for(int j=0; j<3; ++j) {
        	tr.vertices[j].getLatLonDegrees(lat,lon);
        	triangle_info_lats[k] = lat; triangle_info_lons[k] = lon;
        	++k;
        }
    	tr.centroid.getLatLonDegrees(lat, lon);
    	triangle_info_lats[k] = lat; triangle_info_lons[k] = lon;
    	++k;
#if DIAG
        cout << setw(16) << hex
        		<< vertices0[i] << " "
				<< vertices1[i] << " "
				<< vertices2[i] << dec;
        cout << endl << flush;
#endif
    }
}

void to_area(int64_t* indices, int len, double* areas) {
    for (int i=0; i<len; i++) {
        areas[i] = stare.AreaFromValue(indices[i]);        
    }
}

/**
 * Go from an array of [id|id..term] to a pair of arrays [id][term] to aid tests for inclusion.
 */


void from_intervals(int64_t* intervals, int len, int64_t* indices_starts, int64_t* indices_terminators ) {
	// cout << "len: " << len << endl << flush;
	// if(false) {
	EmbeddedLevelNameEncoding leftJustified;
	int i=0, iDest=0;
	do {
		indices_starts[iDest] = intervals[i];
		if(i+1 < len) {
			if( (intervals[i+1] & leftJustified.levelMaskSciDB) == leftJustified.levelMaskSciDB ) {
				indices_terminators[iDest++] = intervals[++i]; // If the next one is a terminator, use it.
			} else {
				leftJustified.setIdFromSciDBLeftJustifiedFormat(intervals[i]);
				indices_terminators[iDest++] = leftJustified.getSciDBTerminatorLeftJustifiedFormat(); // If next is not, make one.
			}
		} else {
			leftJustified.setIdFromSciDBLeftJustifiedFormat(intervals[i]);
			indices_terminators[iDest++] = leftJustified.getSciDBTerminatorLeftJustifiedFormat(); // No more left, make on.
		}
		++i; // Next
	} while(i<len);
	if(iDest<len) {
		indices_starts[iDest]=-998;
		indices_terminators[iDest]=-999;
	}
//	}
//	for(int i=0; i< len; ++i) {
//		indices_starts[i]=i*10;
//		indices_terminators[i]=i*100;
//	}
}

void _expand_intervals(int64_t* indices, int len, int resolution, int64_t* range_indices, int len_ri,  int64_t* result_size, int len_rs) {
	STARE_SpatialIntervals si(indices, indices+len);
	STARE_ArrayIndexSpatialValues result = expandIntervals(si,resolution);
	if(len_ri < result.size()) {
		cout << dec;
		cout << "_expand_intervals-warning: range_indices.size = " << len_ri << " too small." << endl << flush;
		cout << "_expand_intervals-warning: result size        = " << result.size() << "." << endl << flush;
	}
	for(int i=0; i < (len_ri < result.size() ? len_ri : result.size()); ++i) {
		range_indices[i] = result[i];
	}
	result_size[0] = result.size();
}

void _to_neighbors(int64_t* indices, int len, int64_t* range_indices, int len_ri) {
	STARE_ArrayIndexSpatialValues sivs(indices, indices+len);
	STARE_ArrayIndexSpatialValues neighbors;
	if(len_ri < 12*len) {
	  cout << dec
	       << "pystare _neighbors return array size = " << len_ri
	       << " too small, need 3*len = " << 3*len
	       << endl << flush;
	}
	for(int i = 0; i < len; ++i )  {
	  STARE_ArrayIndexSpatialValues n = stare.NeighborsOfValue(indices[i]);
	  neighbors.insert( neighbors.end(), n.begin(), n.end() );
	}
	for(int i = 0; i < len_ri; ++i ) {
	  range_indices[i] = neighbors[i];
	}
}

void _to_circular_cover(double lat, double lon, double radius, int resolution, int64_t* range_indices, int len_ri, int64_t* result_size, int len_rs) {

  STARE_SpatialIntervals result = stare.CoverCircleFromLatLonRadiusDegrees(lat,lon,radius,resolution);
  // cout << "result: size: " << result.size() << endl << flush;

	if(len_ri < result.size()) {
		cout << dec;
		cout << "_to_circular_cover-warning: range_indices.size = " << len_ri << " too small." << endl << flush;
		cout << "_to_circular_cover-warning: result size        = " << result.size() << "." << endl << flush;
	}
	for(int i=0; i < (len_ri < result.size() ? len_ri : result.size()); ++i) {
		range_indices[i] = result[i];
	}
	result_size[0] = result.size();
}

StareResult _to_circular_cover1(double lat, double lon, double radius, int resolution) {
  StareResult result; 
  result.add_intervals(stare.CoverCircleFromLatLonRadiusDegrees(lat,lon,radius,resolution));
  return result;
}

void _to_compressed_range(int64_t* indices, int len, int64_t* range_indices, int len_ri) {
	STARE_SpatialIntervals si(indices, indices+len);
	SpatialRange r(si);
	STARE_SpatialIntervals result = r.toSpatialIntervals();
	for(int i=0; i<result.size(); ++i) {
		range_indices[i] = result[i];
	}
}

void _to_hull_range(int64_t* indices, int len, int resolution, int64_t* range_indices, int len_ri, int64_t* result_size, int len_rs) {
	STARE_ArrayIndexSpatialValues sivs(indices, indices+len);
	STARE_SpatialIntervals result = stare.ConvexHull(sivs, resolution);
	if(len_ri < result.size()) {
		cout << dec;
		cout << "_to_hull_range-warning: range_indices.size = " << len_ri << " too small." << endl << flush;
		cout << "_to_hull_range-warning: result size        = " << result.size() << "." << endl << flush;
	}
	// int k=10;
	// cout << "thr ";
	for(int i=0; i < (len_ri < result.size() ? len_ri : result.size()); ++i) {
		// if(k-->0) {	cout << "0x" << setw(16) << setfill('0') << hex << result[i] << " "; }
		range_indices[i] = result[i];
	}
	// cout << dec << endl << flush;
	result_size[0] = result.size();
}

void _to_hull_range_from_latlon(double* lat, int len_lat, double* lon, int len_lon, int resolution, int64_t* range_indices, int len_ri, int64_t* result_size, int len_rs) {

	LatLonDegrees64ValueVector points;
	for(int i=0; i<len_lat; ++i) {
		points.push_back(LatLonDegrees64(lat[i], lon[i]));
	}
	
	STARE_SpatialIntervals result = stare.ConvexHull(points, resolution);
    
	if(len_ri < result.size()) {
		cout << dec;
		cout << "_to_hull_range-warning: range_indices.size = " << len_ri << " too small." << endl << flush;
		cout << "_to_hull_range-warning: result size        = " << result.size() << "." << endl << flush;
	}
#if 0
    int k=10;
#endif
	// cout << "thr ";
	for(int i=0; i < (len_ri < result.size() ? len_ri : result.size()); ++i) {
		// if(k-->0) {	cout << "0x" << setw(16) << setfill('0') << hex << result[i] << " "; }
		range_indices[i] = result[i];
	}
	// cout << dec << endl << flush;
	result_size[0] = result.size();
}

void _to_box_cover_from_latlon(double* lat, int len_lat, double* lon, int len_lon, int resolution, int64_t* range_indices, int len_ri, int64_t* result_size, int len_rs) {

	LatLonDegrees64ValueVector points;
	for(int i=0; i<len_lat; ++i) {
		points.push_back(LatLonDegrees64(lat[i], lon[i]));
	}

	// STARE_SpatialIntervals result = stare.ConvexHull(points, resolution);
	STARE_SpatialIntervals result = stare.CoverBoundingBoxFromLatLonDegrees(points, resolution);

	if(len_ri < result.size()) {
		cout << dec;
		cout << "_to_box_cover_from_latlon-warning: range_indices.size = " << len_ri << " too small." << endl << flush;
		cout << "_to_box_cover_from_latlon-warning: result size        = " << result.size() << "." << endl << flush;
	}
	int k=10;
	// cout << "thr ";
	for(int i=0; i < (len_ri < result.size() ? len_ri : result.size()); ++i) {
		// if(k-->0) {	cout << "0x" << setw(16) << setfill('0') << hex << result[i] << " "; }
		range_indices[i] = result[i];
	}
	// cout << dec << endl << flush;
	result_size[0] = result.size();
}

void _intersect(int64_t* indices1, int len1, int64_t* indices2, int len2, int64_t* intersection, int leni) {
	STARE_SpatialIntervals si1(indices1, indices1+len1);
    STARE_SpatialIntervals si2(indices2, indices2+len2);
	SpatialRange r1(si1);   
    SpatialRange r2(si2);
	SpatialRange *ri = sr_intersect(r1, r2, false);
	STARE_SpatialIntervals result_intervals = ri->toSpatialIntervals();
	delete ri;
	STARE_ArrayIndexSpatialValues result = expandIntervals(result_intervals);
	leni = result.size();
	for(int i=0; i<leni; ++i) {
		intersection[i] = result[i];
	}
}

void intersects(int64_t* indices1, int len1, int64_t* indices2, int len2, int* intersects) {        
    for(int i=0; i<len2; ++i) {        
        intersects[i] = 0;
        for(int j=0; j<len1; ++j) {
            if (cmpSpatial(indices2[i], indices1[j]) != 0) {
                intersects[i] = 1;                
                break;
            }            
        }
    }
}

void _intersect_multiresolution(int64_t* indices1, int len1, int64_t* indices2, int len2, int64_t* intersection, int leni) {
  // cout << "lens: " << len1 << " " << len2 << " " << leni << endl << flush;
	STARE_SpatialIntervals si1(indices1, indices1+len1), si2(indices2, indices2+len2);
	// cout << 100 << endl << flush;

    // intersection[0] = 69;
	SpatialRange r1(si1), r2(si2);
	// cout << 200 << endl << flush;
	// SpatialRange *ri = r1 & r2;
	// SpatialRange ri = sr_intersect(r1,r2,true);
	SpatialRange *ri = sr_intersect(r1,r2,true); // Should be true here, was false.
	// cout << 300 << endl << flush;
	STARE_SpatialIntervals result_intervals = ri->toSpatialIntervals();
	delete ri;
	STARE_ArrayIndexSpatialValues result = expandIntervals(result_intervals);
	// cout << 400 << endl << flush;
	leni = result.size();
	// cout << 500 << " result size " << result.size() << endl << flush;
	for(int i=0; i<leni; ++i) {
		intersection[i] = result[i];
	}
	// cout << 600 << endl << flush;
}

void _cmp_spatial(int64_t* indices1, int len1, int64_t* indices2, int len2, int64_t* cmp, int len12) {
	STARE_ArrayIndexSpatialValues sivs1(indices1, indices1+len1), sivs2(indices2, indices2+len2);
	int k=0;
	for(int i=0; i<len1; ++i) {
		for(int j=0; j<len2; ++j) {
			cmp[k] = cmpSpatial(indices1[i], indices2[j]); // i.e. [i*len2+j]
			++k;
		}
	}
}

// Temporal
void from_utc(int64_t *datetime, int len, int64_t *indices_out, int resolution) {
	// datetime is in ms (numpy default).
	// double jd19700101_erfa = 2440587.5;

//    cout << "from_utc resolution: " << resolution << endl << flush;
	// cout << "from_utc" << endl << flush;
    int type = 2;
    for (int i=0; i<len; i++) {
    	int64_t idt = datetime[i]/1000;
        indices_out[i] = stare.ValueFromUTC((time_t&)idt, resolution, type);
#if 0
        // These are the methods.
        STARE_ArrayIndexTemporalValue ValueFromUTC(int year, int month, int day, int hour, int minute, int second, int ms, int resolution, int type);
        STARE_ArrayIndexTemporalValue ValueFromUTC(struct tm& tm, int& resolution, int& type);
        STARE_ArrayIndexTemporalValue ValueFromUTC(time_t& datetime, int& resolution, int& type);
#endif
        idt = datetime[i]%1000;
        stare.tIndex.set_millisecond(idt);
        indices_out[i] = stare.getArrayIndexTemporalValue();
        /*
        double jd = stare.toJulianDayUTC();
        double delta = jd - 2440587.5;
        double iDelta = delta*86400.0;
        cout
		<< setprecision(16)
		<< dec << i << " dt,jd,delta,iDelta "
		<< datetime[i] << " "
		<< setw(16) << setfill('0')	<< hex
		<< indices_out[i] << " "
		 << dec
		 << setw(20) << setfill(' ') << scientific
		 << jd << " "
		 << setw(20) << setfill(' ') << scientific
	     << delta << " "
		 << setw(20) << setfill(' ') << scientific
		 << iDelta << " "
		 << stare.tIndex.toStringJulianTAI()
		 << endl << flush;
		 */
    }
//    cout << endl << flush;
}

void to_utc_approximate(int64_t *indices, int len, int64_t *datetime_out) {
	double jd19700101_erfa = 2440587.5;
//	cout << "to_utc_approximate" << endl << flush;
  for (int i=0; i<len; i++) {
    stare.setArrayIndexTemporalValue(indices[i]);
    double jd = stare.toJulianDayUTC();
    // double jdt = stare.toJulianDayTAI();
    double delta = jd-jd19700101_erfa;
    int64_t iDelta = int64_t(delta*86400000.0+0.5); // m-sec now. since that's what numpy defaults to it seems
    /*
	   cout << setprecision(16)
		 << " jd,jdt,delta,iDelta "
		 << hex << setw(16) << setfill('0')
		 << indices[i] << " "
		 << dec
		 << setw(20) << setfill(' ') << scientific
		 << jd << " "
		 << setw(20) << setfill(' ') << scientific
		 << jdt << " "
		 << setw(20) << setfill(' ') << scientific
	     << delta << " "
		 << setw(20) << setfill(' ') << scientific
		 << iDelta << " "
		 << stare.tIndex.toStringJulianTAI()
		 << endl << flush;
     */
    datetime_out[i] = iDelta;
  }
//  cout << endl << flush;
}

void _cmp_temporal(int64_t* indices1, int len1, int64_t* indices2, int len2, int64_t* cmp, int len12) {
  // STARE_ArrayIndexTemporalValues tivs1(indices1,indices1+len1), tivs2(indices2,indices2+len2);
	int k=0;
	for(int i=0; i<len1; ++i) {
		for(int j=0; j<len2; ++j) {
			cmp[k] = cmpTemporalAtResolution2(indices1[i],indices2[j]); // i.e. [i*len2+j]
			++k;
		}
	}
}

StareResult::~StareResult() {}
int  StareResult::get_size() {
  switch( sCase ) {
  case ArrayIndexSpatialValues : return get_size_as_values();
  case SpatialIntervals        : return get_size_as_intervals();
  }
  return -1; // Maybe throw something instead.
}
int  StareResult::get_size_as_values() {
  if( sCase == SpatialIntervals ) {
    convert();
  }
  return sisvs.size();
}
int  StareResult::get_size_as_intervals() {
  if( sCase == ArrayIndexSpatialValues ) {
    convert();
  }
  return sis.size();
}
void StareResult::copy             (int64_t* indices, int len) {
  switch( sCase ) {
  case ArrayIndexSpatialValues :
    copy_as_values(indices,len);
    ; break;
  case SpatialIntervals :
    copy_as_intervals(indices,len);
    ; break;
  }
}
void StareResult::copy_as_values   (int64_t* indices, int len) {
  switch( sCase ) {
  case SpatialIntervals :
    convert();
    ; break;
  }
  for(int i = 0; i < min(len,(int)sisvs.size()); ++i) {
    indices[i] = sisvs[i];
  }
}
void StareResult::copy_as_intervals(int64_t* indices, int len) {
  switch( sCase ) {
  case ArrayIndexSpatialValues :
    convert();
    ; break;
  }
  for(int i = 0; i < min(len,(int)sis.size()); ++i) {
    indices[i] = sis[i];
  }
}
void StareResult::convert() {
  if( converted ) {
    return;
  }
  switch( sCase ) {
  case ArrayIndexSpatialValues :
    {
      SpatialRange r(sisvs);
      sis = r.toSpatialIntervals();
    }
    break;
  case SpatialIntervals :
    sisvs = expandIntervals(sis);
    break;
  }
  converted = true;
}
