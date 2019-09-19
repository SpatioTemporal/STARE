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
	for(int i=0; i < (len_ri < result.size() ? len_ri : result.size()); ++i) {
		range_indices[i] = result[i];
	}
	result_size[0] = result.size();
}

void _intersect(int64_t* indices1, int len1, int64_t* indices2, int len2, int64_t* intersection, int leni) {
	STARE_SpatialIntervals si1(indices1, indices1+len1), si2(indices2, indices2+len2);
    // intersection[0] = 69;
	SpatialRange r1(si1), r2(si2);
	// SpatialRange *ri = r1 & r2;
	SpatialRange *ri = sr_intersect(r1,r2,false);
	STARE_SpatialIntervals result = ri->toSpatialIntervals();
	leni = result.size();
	for(int i=0; i<leni; ++i) {
		intersection[i] = result[i];
	}
}

void _intersect_multiresolution(int64_t* indices1, int len1, int64_t* indices2, int len2, int64_t* intersection, int leni) {
	STARE_SpatialIntervals si1(indices1, indices1+len1), si2(indices2, indices2+len2);
    // intersection[0] = 69;
	SpatialRange r1(si1), r2(si2);
	// SpatialRange *ri = r1 & r2;
	SpatialRange *ri = sr_intersect(r1,r2,true);
	STARE_SpatialIntervals result = ri->toSpatialIntervals();
	leni = result.size();
	for(int i=0; i<leni; ++i) {
		intersection[i] = result[i];
	}
}

void _cmp_spatial(int64_t* indices1, int len1, int64_t* indices2, int len2, int64_t* cmp, int len12) {
	STARE_ArrayIndexSpatialValues sivs1(indices1, indices1+len1), sivs2(indices2, indices2+len2);
	int k=0;
	for(int i=0; i<len1; ++i) {
		for(int j=0; j<len2; ++j) {
			cmp[k] = cmpSpatial(indices1[i],indices2[j]); // i.e. [i*len2+j]
			++k;
		}
	}
}

// Temporal
void from_utc(int64_t *datetime, int len, int64_t *indices, int resolution) {
    int type = 2;
    for (int i=0; i<len; i++) {                       
        indices[i] = stare.ValueFromUTC(datetime[i], resolution, type);
    }
}
