/**
 * STARE.cpp
 *
 *  The API to the STARE index functions.
 *
 *  Created on: Jan 31, 2019
 *      Author: Michael Lee Rilee
 *
 *  Copyright (C) 2019 Rilee Systems Technologies LLC
 */

#include "STARE.h"
#include "SpatialRange.h"
#include "RangeConvex.h"
#include "SpatialDomain.h"
#include "SpatialInterface.h"
#include "SpatialPolygon.h"
#include <iostream>
#include <algorithm>

// #define DIAG
#undef DIAG

#ifndef DIAG
#define DIAGOUT1(a)
#else
#define DIAGOUT1(a) a
#endif

/**
 * @brief Version function with C linkage to aid in finding the library with autoconf
 * @return The library's version. Points to static storage.
 */
extern "C" const char *STARE_version() {
    return (const char *)STARE_VERSION;
}

void STARE_ArrayIndexSpatialValues_insert(STARE_ArrayIndexSpatialValues& v,  STARE_ArrayIndexSpatialValue siv) {
	// cout << dec << 1000 << endl;
	STARE_ArrayIndexSpatialValues::iterator
	it = std::lower_bound( v.begin(), v.end(), siv);
	// cout << dec << 1010 << endl;
	if(it == v.end()) {
		v.insert( it, siv );
	} else if( (*it) != siv) {
		// cout << dec << 1020 << endl;
		v.insert( it, siv );
		// cout << dec << 1030 << endl;
	}
	// cout << dec << 1040 << endl;
}
    
/**
 *
 * Major change: The default octahedral rotation is now theta    = 0.25*gPi - 12.0e-9.
 * The 7.6 cm shift puts the north pole in the center of a trixel.
 *
 */
STARE::STARE() {
	defaultConfiguration();

	sIndex                 = SpatialIndex(search_level, build_level, rotate_root_octahedron);

	sIndexes.insert(std::make_pair(search_level,sIndex));
	/*if( sIndexes.find(search_level) ) {
		// The level was found...
	}*/
}

/**
 *
 */
STARE::STARE( int search_level, int build_level ) {
	defaultConfiguration();
	this->search_level = search_level;
	this->build_level  = build_level;
	sIndex                 = SpatialIndex(this->search_level, this->build_level, this->rotate_root_octahedron);
	sIndexes.insert(std::make_pair(search_level,sIndex));
}

STARE::STARE(
		int search_level, int build_level, SpatialRotation rotate_root_octahedron ) {

	this->search_level = search_level;
	this->build_level  = build_level;
	this->rotate_root_octahedron = rotate_root_octahedron;

	sIndex = SpatialIndex(this->search_level, this->build_level, this->rotate_root_octahedron);
	sIndexes.insert(std::make_pair(this->search_level,sIndex));
}

STARE::~STARE() {
	// TODO Auto-generated destructor stub
}

void STARE::defaultConfiguration() {
	SpatialVector
		xhat(1.0,0.0,0.0),
		yhat(0.0,1.0,0.0),
		zhat(0.0,0.0,1.0);

	SpatialVector axis     = 0.5*xhat + 0.5*yhat; axis.normalize();
	float64       theta    = 0.25*gPi - 12.0e-9; // bump it over a 27-level triangle
	rotate_root_octahedron = SpatialRotation(axis,theta);
	/*
	cout << endl << "dc:rot:axis:   " << rotate_root_octahedron.axis << endl << flush;
	cout         << "dc:rot:theta:  " << rotate_root_octahedron.theta << endl << flush;
	cout         << "dc:axis:       " << axis << endl << flush;
	cout         << "dc:theta:      " << theta << endl << flush;
	cout         << "dc:xhat:       " << xhat << endl << flush;
	cout         << "dc:yhat:       " << yhat << endl << flush;
	*/
}

uint32 STARE::sSearchLevel() const {
	return sIndex.getMaxlevel();
}

STARE_ArrayIndexSpatialValue STARE::ValueFromLatLonDegrees(
		float64 latDegrees, float64 lonDegrees, int resolutionLevel) {

	/*
	cout << endl << flush;
	cout << "svflld: lat,lon,rL: " << dec << latDegrees << " " << lonDegrees << " " << resolutionLevel << endl << flush;
	cout << "svflld: &sI:        " << hex << (&sIndex) << endl << flush;
	cout << "svflld: axis:       " << dec << rotate_root_octahedron.axis << endl << flush;
	cout << "svflld: theta:      " << dec << rotate_root_octahedron.theta << endl << flush;
	*/

	BitShiftNameEncoding       rightJustified(sIndex.idByLatLon(latDegrees,lonDegrees)); // Dip into the legacy code
	EmbeddedLevelNameEncoding  leftJustified(rightJustified.leftJustifiedId());
	EmbeddedLevelNameEncoding  leftJustifiedWithResolution = leftJustified.atLevel(resolutionLevel, true);

	/*
	// cout << endl << flush;
	cout << "svflld: sI.id 0x" << setw(16) << hex << sIndex.idByLatLon(latDegrees,lonDegrees) << endl << flush;
	cout << "svflld: rj:   0x" << setw(16) << hex << rightJustified.getId() << dec << endl << flush;
	cout << "svflld: lj:   0x" << setw(16) << hex << leftJustified.getId() << dec << endl << flush;
	cout << "svflld: ljwr: 0x" << setw(16) << hex << leftJustifiedWithResolution.getId() << dec << endl << flush;
	cout << "svflld: result: " << leftJustifiedWithResolution.getSciDBLeftJustifiedFormat() << endl << flush;
	*/

	// NOTE: This is returning a SciDB-formatted index.
	return leftJustifiedWithResolution.getSciDBLeftJustifiedFormat();
}

STARE_ArrayIndexSpatialValue STARE::ValueFromSpatialVector(SpatialVector v, int resolution) {
	uint64 htmID;
	SpatialVector vtry(v);
	int k = 3;
	while(k>0) {
		try {
			--k;
			htmID = sIndex.idByPoint(vtry);
		} catch( SpatialException e ) {
			cerr << e.what();
			if( k > 0 ) {
				cerr << " " << k << " Trying again... " << endl << flush;
				vtry = vtry + 1.0e-10*SpatialVector(1,0,0); vtry.normalize();
			} else {
				cerr << endl << flush;
				stringstream ss; ss << setprecision(16);
				ss << "STARE::ValueFromSpatialVector can't find vector v= " << v << endl;
				throw SpatialFailure(ss.str().c_str());
			}

		}
	}
	BitShiftNameEncoding       rightJustified(htmID);
	EmbeddedLevelNameEncoding  leftJustified(rightJustified.leftJustifiedId());
	EmbeddedLevelNameEncoding  leftJustifiedWithResolution = leftJustified.atLevel(resolution >= 0 ? resolution : leftJustified.getLevel(), true); // True means keep all bits
	return leftJustifiedWithResolution.getSciDBLeftJustifiedFormat();
}

/**
 * Extract the resolution information from the spatial array index value. Since this
 * does not use the sIndex, it doesn't really need to be a method of this class.
 */
uint32 STARE::ResolutionLevelFromValue(STARE_ArrayIndexSpatialValue spatialStareId) {
	EmbeddedLevelNameEncoding leftJustifiedWithResolution;
	leftJustifiedWithResolution.setIdFromSciDBLeftJustifiedFormat(spatialStareId);
	return leftJustifiedWithResolution.getLevel();
}

/**
 * Extract location information from the spatial array index value.
 */
LatLonDegrees64 STARE::LatLonDegreesFromValue(STARE_ArrayIndexSpatialValue spatialStareId) {

#if 0
    EmbeddedLevelNameEncoding leftJustifiedWithResolution;
	leftJustifiedWithResolution.setIdFromSciDBLeftJustifiedFormat(spatialStareId);

	/// Workaround for a "feature." Coerce the level to the search_level of our sIndex. "true" keeps all of the location bits.
	EmbeddedLevelNameEncoding leftJustifiedPositionOnly = leftJustifiedWithResolution.atLevel(this->search_level,true);

	BitShiftNameEncoding rightJustified(leftJustifiedPositionOnly.rightJustifiedId());
	uint64 htmID = rightJustified.getId();
#endif

	// cout << "sid: " << spatialStareId << endl << flush;

  uint64 htmID = -1;
  if(terminatorp(spatialStareId)) {
    // The long way...
    // cout << "ping" << endl << flush;
    uint64 geolocation_mask = ~spatialLevelMask();
    htmID = htmIDFromValue((spatialStareId & geolocation_mask) + 27);
    // htmID = htmIDFromValue(spatialStareId-4);
  } else {
    htmID = htmIDFromValue(spatialStareId);
  }
	// cout << "lldfv htmID " << setw(16) << setfill('0') << hex << htmID << dec << endl << flush;

	SpatialVector v;
	/// This returns the center of the triangle (at index.search_level). Need to extract the position information.
	sIndex.pointByHtmId(v, htmID);

	float64 lat=-999, lon=-999;
	v.getLatLonDegrees(lat, lon);

	// cout << "0 sid-latlon: " << lat << ", " << lon << endl << flush;

	// LatLonDegrees64 latlon = {.lat = lat, .lon = lon };
	LatLonDegrees64 latlon(lat, lon); //  = {.lat = lat, .lon = lon };

	// cout << "1 sid-latlon: " << latlon.lat << ", " << latlon.lon << endl << flush;

	// return latlon;
	return LatLonDegrees64(lat, lon);

#if 0
	BitShiftNameEncoding rightJustified(hid);
	index.setMaxlevel(7); // A fairly high resolution
	hid = index.idByPoint(v);
	rightJustified.setId(hid);
	EmbeddedLevelNameEncoding leftJustified(rightJustified.leftJustifiedId());
#endif
}

SpatialVector STARE::SpatialVectorFromValue(STARE_ArrayIndexSpatialValue spatialStareId) {
	// uint64 htmID = htmIDFromValue(spatialStareId,STARE_HARDWIRED_RESOLUTION_LEVEL_MAX);  // Max resolution
	uint64 htmID = htmIDFromValue(spatialStareId);  // Max resolution
	// cout << "svfv htmID " << setw(16) << setfill('0') << hex << htmID << dec << endl << flush;
	SpatialVector v;
	/// This returns the center of the triangle (at index.search_level). Need to extract the position information.
	sIndex.pointByHtmId(v, htmID);
	return v;
}

Triangle STARE::TriangleFromValue(STARE_ArrayIndexSpatialValue spatialStareId, int resolutionLevel) {
	// Users are going to expect the default resolution level to be that embedded in the sStareId.

  // EmbeddedLevelNameEncoding elne0;
  // elne0.setIdFromSciDBLeftJustifiedFormat(spatialStareId);
  // elne0 = elne0.clearDeeperThanLevel(elne0.getLevel());
  // STARE_ArrayIndexSpatialValue spatialStareId_ = elne0.getSciDBLeftJustifiedFormat();

  // std::cout << 149 << " " << std::hex << spatialStareId  << std::dec << std::endl;
  // std::cout << 150 << " " << std::hex << spatialStareId_ << std::dec << std::endl;

	uint64 htmID = -1;
	if( resolutionLevel < 0 ) {
		// Use the level embedded in the index value.
		// Prefer this... resolutionLevel = ResolutionLevelFromValue(spatialStareId);
	  // But...
	  EmbeddedLevelNameEncoding leftJustifiedWithResolution;
	  leftJustifiedWithResolution.setIdFromSciDBLeftJustifiedFormat(spatialStareId);
	  if( leftJustifiedWithResolution.terminatorp() ) {
	    resolutionLevel = 27;
	  } else {
	    resolutionLevel = leftJustifiedWithResolution.getLevel();
	  }
	  htmID = htmIDFromValue(spatialStareId,resolutionLevel);
	} else {
		// Use the coerced level, which may be set to the search_level.
		htmID = htmIDFromValue(spatialStareId,resolutionLevel);
	}
	// std::cout << 199 << " " << std::hex << htmID << std::dec << std::endl;

	SpatialVector vc,v1,v2,v3;

	/*100
	// Supersede old default behavior.
	// if( resolutionLevel < 0 ) {
	//	resolutionLevel = search_level;
	//}

	/// TODO Fix redundant operations in the following two calls.
	// std::cout << 200 << std::endl;
	SpatialIndex *si;
	if( sIndexes.find(resolutionLevel) == sIndexes.end() ) {
		// std::cout << 201 << std::endl;
		// std::cout << "rl,bl " << resolutionLevel << " " << build_level << std::endl;
		si = new SpatialIndex(resolutionLevel, build_level, rotate_root_octahedron);
		sIndexes.insert(std::make_pair(resolutionLevel,*si));
	}
	// std::cout << 205 << std::endl;
	// std::cout << "si->maxl,bl: " << si->getMaxlevel() << " " << si->getBuildLevel() << std::endl << std::flush;
	// std::cout << 206 << std::endl;
	// std::cout << "rl, maxl, bl: " << resolutionLevel  << " " << sIndexes[resolutionLevel].getMaxlevel() << " " << sIndexes[resolutionLevel].getBuildLevel() << std::endl << std::flush;
	sIndexes[resolutionLevel].pointByHtmId(vc,htmID);
	// std::cout << 210 << std::endl;
	sIndexes[resolutionLevel].nodeVertexByHtmId(v1, v2, v3, htmID);
	100*/

	// std::cout << 217 << " " << std::dec << resolutionLevel << std::endl;
	SpatialIndex si = getIndex(resolutionLevel);
	// std::cout << 218 << " " << std::hex << htmID << std::dec << std::endl;
	si.pointByHtmId(vc,htmID);
	// std::cout << 219 << std::endl;
	si.nodeVertexByHtmId(v1, v2, v3, htmID);
	// std::cout << 220 << std::endl;
	Vertices vertices; vertices.push_back(v1); vertices.push_back(v2); vertices.push_back(v3);
	// std::cout << 230 << std::endl;
	return {.centroid=vc, .vertices=vertices};
}

STARE_ArrayIndexSpatialValues STARE::toVertices(STARE_ArrayIndexSpatialValues spatialStareIds) {
	STARE_ArrayIndexSpatialValues spatialValues;
	for(int i=0; i<spatialStareIds.size(); ++i) {
		Triangle tr = TriangleFromValue(spatialStareIds[i]);
		for(int j=0; j<3; ++j) {
			spatialValues.push_back(ValueFromSpatialVector(tr.vertices[j]));
		}
	}
	return spatialValues;
}

/**
 * Adapt the resolution of the values in a vector of spatial index values to the proximity of the
 * other index values in the vector.
 */
STARE_ArrayIndexSpatialValues STARE::adaptSpatialResolutionEstimates(STARE_ArrayIndexSpatialValues spatialStareIds) {
	STARE_ArrayIndexSpatialValues spatialValues;
	EmbeddedLevelNameEncoding lj;
	int lvl_maxes[spatialStareIds.size()];
	for(int i=0; i<spatialStareIds.size(); ++i) {
		lvl_maxes[i] = 0;
	}
	for(int i=0; i<spatialStareIds.size(); ++i) {
		for(int j=i+1; j<spatialStareIds.size(); ++j) {
			int lvl = cmpSpatialResolutionEstimateI(spatialStareIds[i],spatialStareIds[j]);
			if( lvl > lvl_maxes[i] ) {
				lvl_maxes[i] = lvl;
			}
			if( lvl > lvl_maxes[j] ) {
				lvl_maxes[j] = lvl;
			}
		}
		spatialValues.push_back( (spatialStareIds[i] & ~lj.levelMaskSciDB) | lvl_maxes[i] );
	}
	return spatialValues;
}

/**
 * @brief Adapt the resolution of the spatial index values based on proximity.
 *
 * Scan the s-indices vector and set the 5 resolution bits based on the distance
 * between any two of the s-indices. For any set of s-indices, the level is the
 * maximum computed level.
 * @note The complexity of this method is O(N(N-1)) where N is the length of the
 * input vector.
 *
 * @param s_indices Reference to a STARE_ArrayIndexSpatialValues (vector of STARE indices)
 * @return The maximum level found.
 * @author James Gallagher <jgallagher@opendap.org>
 */
void STARE::adaptSpatialResolutionEstimatesInPlace(STARE_ArrayIndexSpatialValues &s_indices) {

    EmbeddedLevelNameEncoding lj;

    vector<int> lvl_maxes(s_indices.size(), 0);

    for(int i=0; i < s_indices.size(); ++i) {
        for(int j=i+1; j < s_indices.size(); ++j) {
            int lvl = cmpSpatialResolutionEstimateI(s_indices[i], s_indices[j]);
            if( lvl > lvl_maxes[i] ) {
                lvl_maxes[i] = lvl;
            }
            if( lvl > lvl_maxes[j] ) {
                lvl_maxes[j] = lvl;
            }
        }

        s_indices[i] = (s_indices[i] & ~lj.levelMaskSciDB) | lvl_maxes[i];
    }
}

/**
 * @brief Adapt the resolution of the spatial index values based on proximity.
 *
 * Copied from the STARE_ArrayIndexSpatialValues version above.
 *
 * Scan the s-indices vector and set the 5 resolution bits based on the distance
 * between any two of the s-indices. For any set of s-indices, the level is the
 * maximum computed level.
 * @note The complexity of this method is O(N(N-1)) where N is the length of the
 * input vector.
 *
 * @param s_indices Reference to a STARE_ArrayIndexSpatialValues (vector of STARE indices)
 * @return The maximum level found.
 * @author Mike Rilee <mike@rilee.net>
 */
void STARE::adaptSpatialResolutionEstimatesInPlace(STARE_ArrayIndexSpatialValue *s_indices, const int s_indices_size) {

    EmbeddedLevelNameEncoding lj;

    vector<int> lvl_maxes(s_indices_size, 0);

    for(int i=0; i < s_indices_size; ++i) {
        for(int j=i+1; j < s_indices_size; ++j) {
            int lvl = cmpSpatialResolutionEstimateI(s_indices[i], s_indices[j]);
            if( lvl > lvl_maxes[i] ) {
                lvl_maxes[i] = lvl;
            }
            if( lvl > lvl_maxes[j] ) {
                lvl_maxes[j] = lvl;
            }
        }

        s_indices[i] = (s_indices[i] & ~lj.levelMaskSciDB) | lvl_maxes[i];
    }
}


/**
 * Return the area associated with the index value based on the embedded resolution level, by default.
 * The calculation may be coerced to another resolution level, e.g. search_level.
 *
 * NOTE: The calculation suffers from floating point error at a resolution level of 27. 2019-0206 MLR
 */
float64 STARE::AreaFromValue(STARE_ArrayIndexSpatialValue spatialStareId, int resolutionLevel) {

	uint64 htmID = -1;
	if( resolutionLevel < 0 ) {
		// Use the level embedded in the index value.
		resolutionLevel = ResolutionLevelFromValue(spatialStareId);
		htmID = htmIDFromValue(spatialStareId,resolutionLevel);
	} else {
		// Use the coerced level, which may be set to the search_level.
		htmID = htmIDFromValue(spatialStareId,resolutionLevel);
	}

	// Supersede old default behavior
	// if( resolutionLevel <= 0 ) {
	// 	resolutionLevel = search_level;
	// }
	/// TODO Fix redundant operations in the following two calls.
	if( sIndexes.find(resolutionLevel) == sIndexes.end() ) {
		sIndexes.insert(std::make_pair(resolutionLevel,SpatialIndex(resolutionLevel, build_level, rotate_root_octahedron)));
	}
	return sIndexes[resolutionLevel].areaByHtmId(htmID);
}

/// TODO STARE::InfoFromValue // Get all the info from a volume, so we only translate between forms once.

STARE_ArrayIndexSpatialValue sTerminator(STARE_ArrayIndexSpatialValue spatialStareId) {
	EmbeddedLevelNameEncoding leftJustifiedWithResolution;
	leftJustifiedWithResolution.setIdFromSciDBLeftJustifiedFormat(spatialStareId);
	return leftJustifiedWithResolution.getSciDBTerminatorLeftJustifiedFormat();
}

/**
 * Compare two spatial array index values a, b.
 *
 * Suffers some overhead, but should handle the overlap. For other
 * kinds of intersection we need to handle intervals or move to a range.
 *
 * Returns
 *   1 if b is in a
 *  -1 if a is in b
 *   0 otherwise.
 *
 */
int cmpSpatial(STARE_ArrayIndexSpatialValue a_, STARE_ArrayIndexSpatialValue b_) {

	EmbeddedLevelNameEncoding a_elne0,a_elne1,b_elne0,b_elne1;

	a_elne0.setIdFromSciDBLeftJustifiedFormat(a_);
	b_elne0.setIdFromSciDBLeftJustifiedFormat(b_);

	a_elne1 = a_elne0.clearDeeperThanLevel(a_elne0.getLevel());
	b_elne1 = b_elne0.clearDeeperThanLevel(b_elne0.getLevel());

	STARE_ArrayIndexSpatialValue
	a  = a_elne1.getSciDBLeftJustifiedFormat(),
	at = a_elne1.getSciDBTerminatorLeftJustifiedFormat(),
	b  = b_elne1.getSciDBLeftJustifiedFormat(),
	bt = b_elne1.getSciDBTerminatorLeftJustifiedFormat();

	at = sTerminator(a), bt = sTerminator(b);

	int overlap = 0;
	if( ( a <= b ) && ( bt <= at ) ) {
		overlap = 1;
	} else if( ( b <= a ) && ( at <= bt ) ) {
		overlap = -1;
	}
#ifdef DIAG
	cout << "a,at,b,bt: " << a << " " << at << " " << b << " " << bt << endl << flush;
	cout << " a,at,b,bt: " << endl << hex
			<< " " << a  << endl
			<< " " << at << endl
			<< " " << b  << endl
			<< " " << bt << endl
			<< dec << endl << flush;
#endif
	return overlap;

}


/**
 * Return a vector of index values of potentially varying sizes (resolution levels) covering the bounding box.
 *
 * NOTE The calls to the underlying legacy HTM calls automatically convey resolution information in the triangles they find.
 *
 * Very preliminary. Needs verification.
 *
 * @param force_resolution_level - the smallest triangles to use in the covering
 *
 */
STARE_SpatialIntervals STARE::CoverBoundingBoxFromLatLonDegrees(
	LatLonDegrees64ValueVector corners, int force_resolution_level) {
#if 0
    int resolution_level; // for the match, not used at present (MLR 2020-0316)
#endif
	STARE_SpatialIntervals intervals;
	SpatialIndex index;
	if( corners.size() != 4) {
		cout << "ERROR: STARE::CoverBoundingBoxFromLatLonDegrees Warning Bounding Box called with 4 != corner.size == " << corners.size() << ", returning." << endl << flush;
		return intervals;
	}
	if( force_resolution_level > -1 ) {
		index = getIndex(force_resolution_level);
	} else {
		index = getIndex(8); /// TODO Hardcoded search level for the cover...
	}
	Vertices vCorners;
	for(LatLonDegrees64ValueVector::iterator cit = corners.begin(); cit != corners.end(); ++cit) {
		SpatialVector v; v.setLatLonDegrees(cit->lat, cit->lon); // TODO change setLLD from void to an SV ref.
		vCorners.push_back(v);
	};

	// EXPERIMENTAL EXPERIMENTAL EXPERIMENTAL EXPERIMENTAL EXPERIMENTAL
	// EXPERIMENTAL EXPERIMENTAL EXPERIMENTAL EXPERIMENTAL EXPERIMENTAL
	// EXPERIMENTAL EXPERIMENTAL EXPERIMENTAL EXPERIMENTAL EXPERIMENTAL

	// RangeConvex rc = RangeConvex(&v0,&v1,&v2,&v3);
	// std::cout << 100 << std::endl;
	RangeConvex rc = RangeConvex(
			&vCorners[0],
			&vCorners[1],
			&vCorners[2],
			&vCorners[3]);
	// std::cout << 200 << std::endl;
	SpatialDomain d; d.add(rc);
	// std::cout << 300 << std::endl;
	HtmRange r; r.purge();

	// The following is the effective part of the routine.

    d.intersect(&index, &r, false /* varlen */);

	// bool overlap = d.intersect(idx, htmrange, varlen, hrInterior, hrBoundary);
	r.defrag();
	r.reset(); // Move the skip-list iterator back to the beginning.

	Key lo = -999, hi = -999;
	uint64 id0, id1;
	int indexp = r.getNext(lo,hi);
	if(indexp) {
		do {
			id0 = EmbeddedLevelNameEncoding(BitShiftNameEncoding(lo).leftJustifiedId()).getSciDBLeftJustifiedFormat();
			intervals.push_back(id0);
			// If id0,id1 is a singlet then don't do the following.
			if( lo != hi ) {
				id1 = EmbeddedLevelNameEncoding(BitShiftNameEncoding(hi).leftJustifiedId()).getSciDBTerminatorLeftJustifiedFormat();
				intervals.push_back(id1);
			}



		} while( r.getNext(lo,hi) );
	}
	return intervals;
}

/**
 * Returns a cover for a spherical circle centered at lat, lon with an angular radius in degrees.
 *
 * Uses triangles down to resolution level 8, unless forced to a different resolution.
 *
 * TODO Requires testing & presentation
 */
STARE_SpatialIntervals STARE::CoverCircleFromLatLonRadiusDegrees(float64 latDegrees, float64 lonDegrees, float64 radius_degrees, int force_resolution_level) {

	SpatialIndex index;
	if( force_resolution_level > -1 ) {
		index = getIndex(force_resolution_level);
	} else {
		index = getIndex(8); /// TODO Hardcoded search level for the cover...
	}

	SpatialVector v; v.setLatLonDegrees(latDegrees,lonDegrees);

	SpatialConstraint c(v,cos(radius_degrees*piDiv180));

	RangeConvex rc; rc.add(c);
	SpatialDomain d; d.add(rc);
	HtmRange r;	r.purge(); // TODO Review this use of legacy code

	// The following is the effective part of the routine.

    d.intersect(&index, &r, true /* varlen */);

	r.reset();

	STARE_SpatialIntervals intervals;
	Key lo=-999, hi=-999;
	uint64 id0, id1;
	int indexp = r.getNext(lo,hi);
	if(indexp) {
		do {
			id0 = EmbeddedLevelNameEncoding(BitShiftNameEncoding(lo).leftJustifiedId()).getSciDBLeftJustifiedFormat();
			intervals.push_back(id0);
			// If id0,id1 is a singlet then don't do the following.
			if( lo != hi ) {
				id1 = EmbeddedLevelNameEncoding(BitShiftNameEncoding(hi).leftJustifiedId()).getSciDBTerminatorLeftJustifiedFormat();
				intervals.push_back(id1);
			}
		} while(r.getNext(lo,hi));
	}
	return intervals;
}

/**
 * Returns a list of STARE spatial IDs on the hull about @param points.
 *
 * Unfortunately, it erroneously returns a few spatial IDs from the interior as well. [2020-0521-Seems fixed.]
 *
 */
STARE_SpatialIntervals STARE::ConvexHull(LatLonDegrees64ValueVector points, int force_resolution_level) {

	STARE_SpatialIntervals cover;
	int hullSteps = points.size();
   

	htmInterface *htm;
	DIAGOUT1(cout << dec << 1000 << " hullSteps: " << hullSteps << endl << flush;)
	if( force_resolution_level > -1 ) {
		DIAGOUT1(cout << dec << 1100 << endl << flush;)
		// htm = htmInterface(&index_);
		htm = new htmInterface(
				this->getIndex(force_resolution_level).getMaxlevel(),
				this->getIndex(force_resolution_level).getBuildLevel(),
				this->getIndex(force_resolution_level).getRotation());
		DIAGOUT1(cout << dec << 1101 << endl << flush;)
	} else {
		DIAGOUT1(cout << dec << 1200 << endl << flush;)
		// htm = htmInterface(&index_);
		htm = new htmInterface(
				this->getIndex(8).getMaxlevel(),
				this->getIndex(8).getBuildLevel(),
				this->getIndex(8).getRotation());
		DIAGOUT1(cout << dec << 1201 << endl << flush;)
	}

	DIAGOUT1(cout << dec << "a2000" << endl << flush;)
    
	HTMRangeValueVector htmRangeVector = htm->convexHull(points, hullSteps, true); // Compress result
    
	DIAGOUT1(cout << dec << "a3000 hrv.size: " << htmRangeVector.size() << endl << flush;)

	for(int i=0; i < htmRangeVector.size(); ++i) {
		uint64 lo = ValueFromHtmID(htmRangeVector[i].lo); // TODO Should this be a function?
		cover.push_back(lo);
		uint64 hi;
		if( htmRangeVector[i].lo != htmRangeVector[i].hi ) {
			hi = sTerminator(ValueFromHtmID(htmRangeVector[i].hi));
			cover.push_back(hi);
		}
	}

	DIAGOUT1(cout << dec << "a4000" << endl << flush;)

	delete htm; // TODO Hopefully this will not also delete the index we passed in.
	return cover;
}

STARE_SpatialIntervals STARE::NonConvexHull(LatLonDegrees64ValueVector points, int force_resolution_level) {
	// cout << "force_resolution_level: " << force_resolution_level << endl << flush;

	// Assume points needs to be closed, i.e. last point is not redundant with first.
	EmbeddedLevelNameEncoding lj;
	// Integrate and optimize later.
	// - Get a cover.
	// STARE_SpatialIntervals cover = ConvexHull(points,force_resolution_level);

	/* - Start with Earth.
	Spatial ID 0x0000000000000000 0
	Spatial ID 0x0800000000000000 576460752303423488
	Spatial ID 0x1000000000000000 1152921504606846976
	Spatial ID 0x1800000000000000 1729382256910270464
	Spatial ID 0x2000000000000000 2305843009213693952
	Spatial ID 0x2800000000000000 2882303761517117440
	Spatial ID 0x3000000000000000 3458764513820540928
	Spatial ID 0x3800000000000000 4035225266123964416
	*/
	STARE_SpatialIntervals cover;
	cover.push_back( 0x0000000000000000 );
	cover.push_back( 0x0800000000000000 );
	cover.push_back( 0x1000000000000000 );
	cover.push_back( 0x1800000000000000 );
	cover.push_back( 0x2000000000000000 );
	cover.push_back( 0x2800000000000000 );
	cover.push_back( 0x3000000000000000 );
	cover.push_back( 0x3800000000000000 );

	// - Make the polygon
	Vertices vs; vs.resize(points.size()+1);
	for(int i=0; i<points.size(); ++i) {
		SpatialVector v;
		v.setLatLonDegrees(points[i].lat, points[i].lon);
		vs[i] = v;
	}
	SpatialVector v;
	v.setLatLonDegrees(points[0].lat, points[0].lon);
	vs[points.size()] = v;

	SpatialPolygon p(vs);
	bool ccw_orientation = p.ccw_orientation;
	if( !ccw_orientation ) {
		reverse(vs.begin(),vs.end());
		p = SpatialPolygon(vs);
	}

	// - Loop over trixels, remove those outside the polygon.
	STARE_SpatialIntervals cover1;
	int i = 0;
	while( i < cover.size() ) {
	// for( int i=0; i < cover.size(); ++i ) {
//		cout << "i:     " << i << endl << flush;
//		cout << "cs:    " << cover.size() << endl << flush;
		uint64 i0 = cover[i];
//		cout << "i0:    " << hex << i0 << dec << endl << flush;

		uint64 i1 = i < (cover.size()-1) ? cover[i+1] : cover[i];
//		uint64 i1 = i0;
//		if( i < (cover.size()-1) ) {
//			i1 = cover[i+1];
//		}
//		cout << "i1:    " << hex << i1 << dec << endl << flush;

		int level = i0 & lj.levelMaskSciDB;
		// cout << i << " i,level " << level << endl << flush;
		// if( i > 200 ) {
		// 	exit(1);
		// }
		uint64 one_mask_to_resolution, one_at_resolution;
		lj.SciDBincrement_LevelToMaskDelta(i0 & lj.levelMaskSciDB,one_mask_to_resolution,one_at_resolution);
//		cout << "level: " << level << endl << flush;

//		cout << "----" << endl << flush;
//		for( int l = 0; l < cover.size(); ++l ) {
//			cout << l << " " << hex << cover[l] << dec << endl << flush;
//		}
//		cout << "----" << endl << flush;
		if( !terminatorp(i1) ) {
			Triangle tr = this->TriangleFromValue(i0, level);
			int siv_count = p.intersect_triangle(tr);
//			cout << hex << i0 << dec << " " << siv_count << endl << flush;
			// First case -- eliminate those completely external.
			if( siv_count >= 7 ) {
//				cout << "sv >=7, done with " << hex << i0 << dec << endl << flush;
				cover1.push_back(i0);
			} else if (siv_count > 0) {
				// cout << 1000 << " level " << (i0 & lj.levelMaskSciDB) << endl << flush;
				if( (i0 & lj.levelMaskSciDB) == force_resolution_level ) {
					// Our work is done.
//					cout << "sv>0, done with " << hex << i0 << dec << endl;
					cover1.push_back(i0);
				} else {
//					cout << " recurse 1 " << endl << flush;
					for(uint64 j = 0; j < 4; ++j ) {
						uint64 one_mask_to_resolution1, one_at_resolution1;
						lj.SciDBincrement_LevelToMaskDelta((i0 & lj.levelMaskSciDB)+1,one_mask_to_resolution1,one_at_resolution1);
						cover.push_back(1+i0+j*one_at_resolution1);
					}
				}
			} else { // siv_count is zero
				bool hit = false;
				int k = 0;
				while((!hit) && (k<vs.size())) {
					// hit = this->sIndex.isInsideBarycentric(vs[k], tr.vertices[0], tr.vertices[1], tr.vertices[2], false);
					hit = this->sIndex.isInside(vs[k], tr.vertices[0], tr.vertices[1], tr.vertices[2]);
//					cout << "hit: "
//							<< k << " "
//							<< "[ " << vs[k] << " ] "
//							<< "[ " << tr.vertices[0] << " ] "
//							<< "[ " << tr.vertices[1] << " ] "
//							<< "[ " << tr.vertices[2] << " ] "
//							<< " h = " << hit
//							<< endl << flush;
					++k;
				}
				if(!hit) {
					hit = p.triangle_crossp(tr);
				}
				if(hit) {
					if( (i0 & lj.levelMaskSciDB) == force_resolution_level ) {
						// Our work is done.
//						cout << "sv=0, done with " << hex << i0 << dec << endl;
						cover1.push_back(i0);
					} else {
//						cout << " recurse 1 on " << hex << i0 << dec << endl << flush;
						for(uint64 j = 0; j < 4; ++j ) {
							uint64 one_mask_to_resolution1, one_at_resolution1;
							lj.SciDBincrement_LevelToMaskDelta((i0 & lj.levelMaskSciDB)+1,one_mask_to_resolution1,one_at_resolution1);
//							cout << "       delta  " << hex << one_at_resolution1 << dec << endl << flush;
							uint64 child = 1+i0+j*one_at_resolution1;
//							cout << " recurse   on " << hex << child << dec << endl << flush;
							cover.push_back(child);
						}
					}
				}
			}
//			else {
//				SpatialPolygon tp(tr.vertices);
//				int k = 0;
//				bool hit = false;
//				while((!hit) && (k<vs.size())) {
//					hit = tp.intersection(vs[k]);
//					++k;
//				}
//				if( hit ) {
//					// cout << 1000 << " level " << (i0 & lj.levelMaskSciDB) << endl << flush;
//					if( (i0 & lj.levelMaskSciDB) == force_resolution_level ) {
//						// Our work is done.
//						cover1.push_back(i0);
//					} else {
//						// cout << " recurse 1 " << endl << flush;
//						for(uint64 j = 0; j < 4; ++j ) {
//							uint64 one_mask_to_resolution1, one_at_resolution1;
//							lj.SciDBincrement_LevelToMaskDelta((i0 & lj.levelMaskSciDB)+1,one_mask_to_resolution1,one_at_resolution1);
//							cover.push_back(1+i0+j*one_at_resolution1);
//						}
//					}
//				}
//			}
		} else {
			throw SpatialFailure("STARE:: non-convex hull case with terminators not implemented");
			++i;
			// Undoes compression
			for( uint64 siv=i0; siv < i1; siv += one_at_resolution ) {
				Triangle tr = this->TriangleFromValue(siv, level);
				int siv_count = p.intersect_triangle(tr);
				// cout << hex << siv << dec << " " << siv_count << " *" << endl << flush;
				// First case -- eliminate those completely external.
				if( siv_count >= 7 ) {
					cover1.push_back(siv);
				} else if ( siv_count > 0 ) {
					// cout << 2000 << " level " << (siv & lj.levelMaskSciDB) << endl << flush;
					if( (siv & lj.levelMaskSciDB) == force_resolution_level ) {
						// Our work is done.
						cover1.push_back(siv);
					} else {
						// cout << " recurse 2 " << endl << flush;
						for(uint64 j = 0; j < 4; ++j ) {
							uint64 one_mask_to_resolution1, one_at_resolution1;
							lj.SciDBincrement_LevelToMaskDelta((siv & lj.levelMaskSciDB)+1,one_mask_to_resolution1,one_at_resolution1);
							cover.push_back(1+siv+j*one_at_resolution1);
						}
					}
				}
				else {
					bool hit = false;
					int k = 0;
					while((!hit) && (k<vs.size())) {
						hit = this->sIndex.isInsideBarycentric(vs[k], tr.vertices[0], tr.vertices[1], tr.vertices[2], false);
						++k;
					}

					if( hit ) {
						if( (siv & lj.levelMaskSciDB) == force_resolution_level ) {
							// Our work is done.
							cover1.push_back(siv);
						} else {
							for(uint64 j = 0; j < 4; ++j ) {
								uint64 one_mask_to_resolution1, one_at_resolution1;
								lj.SciDBincrement_LevelToMaskDelta((siv & lj.levelMaskSciDB)+1,one_mask_to_resolution1,one_at_resolution1);
								cover.push_back(1+siv+j*one_at_resolution1);
							}
						}
					}
				}
//				else {
//					SpatialPolygon tp(tr.vertices);
//					int k = 0;
//					bool hit = false;
//					while((!hit) && (k<vs.size())) {
//						hit = tp.intersection(vs[k]);
//						++k;
//					}
//					if( hit ) {
//						if( (siv & lj.levelMaskSciDB) == force_resolution_level ) {
//							// Our work is done.
//							cover1.push_back(siv);
//						} else {
//							for(uint64 j = 0; j < 4; ++j ) {
//								uint64 one_mask_to_resolution1, one_at_resolution1;
//								lj.SciDBincrement_LevelToMaskDelta((siv & lj.levelMaskSciDB)+1,one_mask_to_resolution1,one_at_resolution1);
//								cover.push_back(1+siv+j*one_at_resolution1);
//							}
//						}
//					}
//				}
			}
		}
		++i;
	}

	if( !ccw_orientation ) {
		// Construct and return the complement, since we're using a CW polygon.
		STARE_SpatialIntervals cover2, cover3;
		cover2.push_back( 0x0000000000000000 );
		cover2.push_back( 0x0800000000000000 );
		cover2.push_back( 0x1000000000000000 );
		cover2.push_back( 0x1800000000000000 );
		cover2.push_back( 0x2000000000000000 );
		cover2.push_back( 0x2800000000000000 );
		cover2.push_back( 0x3000000000000000 );
		cover2.push_back( 0x3800000000000000 );

		SpatialRange interior(cover1);

		int i = 0;
		while( i < cover2.size() ) {
			uint64 i0 = cover2[i];
#if 0
			uint64 i1 = i < (cover2.size()-1) ? cover2[i+1] : cover2[i];
			// cout << "cw i0: " << hex << i0 << dec << endl << flush;
			if( i0 != i1 ) {
				cout << "cw i0 vs. i1: " << hex << i0 << " " << i1 << endl << flush;
			}
			bool diag = (i0 == 0x3d2b600000000009) || (i0 == 0x3d2b61000000000a);
//			cout << "cw i0: " << hex << i0 << dec << flush;
			if( diag ) {
				cout << "cw i0: " << hex << i0 << dec << flush;
				cout << " *** " << flush;
				cout << endl << flush;
			}
			// cout << endl << flush;
#endif

			STARE_SpatialIntervals si;
			si.push_back(i0);
			SpatialRange test(si);
			SpatialRange *overlap = sr_intersect(interior,test);
			KeyPair kp;
			overlap->reset();
			int at_least_one = overlap->getNext(kp);
#if 0
			if( diag ) {
				cout << "cw kp: " << hex << kp.lo << " " << kp.hi << dec << " " << at_least_one << endl << flush;
			}
#endif
			if( at_least_one ) {
				int level = i0 & lj.levelMaskSciDB;
				if( level < force_resolution_level ) { // Still room to recurse
					// subdivide i0 and push into cover2
					for(uint64 j = 0; j < 4; ++j ) {
						uint64 one_mask_to_resolution1, one_at_resolution1;
						lj.SciDBincrement_LevelToMaskDelta((i0 & lj.levelMaskSciDB)+1,one_mask_to_resolution1,one_at_resolution1);
						uint64 child = 1+i0+j*one_at_resolution1; // Add them to the end of the working queue.
#if 0
						bool diag = (child == 0x3d2b600000000009) || (child == 0x3d2b61000000000a);
						if( diag ) {
							cout << "cw child ----" << endl << flush;
							cout << "cw child: " << hex << i0 << " " << child << endl << flush;
						}
#endif
						if( ! interior.contains(child) ) {
							STARE_SpatialIntervals childi; childi.push_back(child);
							// SpatialRange *childi_overlap = sr_intersect(interior,childi);
							SpatialRange *childi_overlap = sr_intersect(childi,interior);
							KeyPair ckp;
							childi_overlap->reset();
							int at_least_one1 = childi_overlap->getNext(ckp);
							if( at_least_one1 ) {
//								if( diag ) cout << "cw cover2 needs work" << endl << flush;
								cover2.push_back(child); // Needs more work
							} else {
#if 0
								if( diag ) {
									cout << "cw cover3 done" << endl << flush;
									cout << "cw at least o " << at_least_one1 << endl << flush;
									cout << "cw ckp l h "    << ckp.lo << " " << ckp.hi << endl << flush;
								}
#endif
								cover3.push_back(child); // Done here.
							}
						}
					}
				} else { // level == force_resolution_level
					// The finest resolution... we're done, keep as on boundary.
					cover3.push_back(i0);
				} // End case where i0 overlaps interior.
			} else { // No overlap, so we're outside.
				// push i0 into cover3
				cover3.push_back(i0);
			}
			++i;
		}
		return cover3;
	}

	return cover1;
}

STARE_SpatialIntervals STARE::ConvexHull(STARE_ArrayIndexSpatialValues points,int force_resolution_level) {
	LatLonDegrees64ValueVector latlon;
	for( STARE_ArrayIndexSpatialValues::iterator i=points.begin(); i != points.end(); ++i) {
		latlon.push_back(LatLonDegreesFromValue(*i));
	}
	return ConvexHull(latlon,force_resolution_level);
}

/*
 * Return a spatial index object with a given search level. If one does not already exist, construct and memoize.
 */
SpatialIndex STARE::getIndex(int resolutionLevel) {
	if( sIndexes.find(resolutionLevel) == sIndexes.end() ) {
		sIndexes.insert(std::make_pair(resolutionLevel,SpatialIndex(resolutionLevel, build_level, rotate_root_octahedron)));
	}
	return sIndexes[resolutionLevel];
}

/**
 * Return the legacy htmID value from the spatialStareId.
 *
 * NOTE THIS IGNORES EMBEDDED RESOLUTION
 *
 * Note the htmID precision level needn't have a resolution interpretation, but is more purely geometric.
 * This is important when calling into the legacy htm foundation and why it's kept private.
 */
uint64 STARE::htmIDFromValue(STARE_ArrayIndexSpatialValue spatialStareId, int force_resolution_level) {
	EmbeddedLevelNameEncoding leftJustifiedWithResolution;
	leftJustifiedWithResolution.setIdFromSciDBLeftJustifiedFormat(spatialStareId);
	/// Workaround for a "feature." Coerce the level to the search_level of our sIndex. "true" keeps all of the location bits.
	int search_level = this->search_level;  /// The default search level (for this sIndex) is usually 27. It can be coerced, say, to the resolution level of the spatial STARE index.
	if( force_resolution_level >= 0 ) { // mlr 2020-0123
		search_level = force_resolution_level;
	}
	// EmbeddedLevelNameEncoding leftJustifiedPositionOnly = leftJustifiedWithResolution.atLevel(this->search_level,true);
	EmbeddedLevelNameEncoding leftJustifiedPositionOnly = leftJustifiedWithResolution.atLevel(search_level,true);
	BitShiftNameEncoding rightJustified(leftJustifiedPositionOnly.rightJustifiedId());
	uint64 htmID = rightJustified.getId();
	return htmID;
}

/**
 * Return the spatialStareId from the legacy htmID.
 *
 * This is the inverse of htmIDFromValue and should not be so useful.
 */
STARE_ArrayIndexSpatialValue STARE::ValueFromHtmID(uint64 htmID) {
	return EmbeddedLevelNameEncoding(BitShiftNameEncoding(htmID).leftJustifiedId()).getSciDBLeftJustifiedFormat();
}

/**
 * Return a vector of spatial STARE index values containing all 12 neighboring triangles.
 *
 * Uses htmID legacy encoding under the hood.
 */
STARE_ArrayIndexSpatialValues STARE::NeighborsOfValue(
	STARE_ArrayIndexSpatialValue spatialStareId) {
	// TODO make htmID it's own type so we can lean a bit on the compiler.
	int level = ResolutionLevelFromValue(spatialStareId);
	uint64 htmID = htmIDFromValue          (spatialStareId,level); // TODO verify this line is correct. We've got to watch out for the extra precision bits during the conversion.
	SpatialIndex index = getIndex(ResolutionLevelFromValue(spatialStareId));
	uint64 neighbors[3+9];
	SpatialVector workspace[18];

	// index.NeighborsAcrossEdgesFromHtmId(&neighbors[9], htmID, workspace);
	// index.NeighborsAcrossVerticesFromEdges(neighbors, &neighbors[9], htmID, workspace);

	index.NeighborsAcrossEdgesFromHtmId(neighbors, htmID, workspace);
	index.NeighborsAcrossVerticesFromEdges(&neighbors[3], neighbors, htmID, workspace);

	for(int i=0; i < 3+9; ++i ) {
		// cout << i << " s::nov: " << hex << "0x" << neighbors[i] << dec << endl << flush;
		neighbors[i] = ValueFromHtmID(neighbors[i]);
	}
	return STARE_ArrayIndexSpatialValues(begin(neighbors),end(neighbors));
}

TemporalIndex& STARE::setTIndexTAI(int year, int month, int day, int hour, int minute, int second, int ms, int forward_resolution, int reverse_resolution, int type) {
	if( type != 1 ) {
	  stringstream ss; ss << "STARE::setTIndexTAI::type = " << dec << type << flush;  
	  throw SpatialFailure(ss.str().c_str());
	}
	tIndex.fromFormattedJulianTAI(year, month, day, hour, minute, second, ms);
	tIndex.set_forward_resolution(forward_resolution);
	tIndex.set_reverse_resolution(reverse_resolution);
	return tIndex;
}

TemporalIndex& STARE::setTIndexUTC(int year, int month, int day, int hour, int minute, int second, int ms, int forward_resolution, int reverse_resolution, int type) {
	if( type != 1 ) {
	  stringstream ss; ss << "STARE::setTIndexUTC::type = " << dec << type << flush;
	  throw SpatialFailure(ss.str().c_str());
	}
	tIndex.fromUTC(year, month, day, hour, minute, second, ms);
	tIndex.set_forward_resolution(forward_resolution);
	tIndex.set_reverse_resolution(reverse_resolution);
	return tIndex;
}

void STARE::toTAI(int& year, int& month, int& day, int& hour, int& minute, int& second, int& ms, int& forward_resolution, int& reverse_resolution, int& type) {
	tIndex.toFormattedJulianTAI(year, month, day, hour, minute, second, ms);
	forward_resolution = tIndex.get_forward_resolution();
	reverse_resolution = tIndex.get_reverse_resolution();
	type       = tIndex.get_type();
}

void STARE::toUTC(int& year, int& month, int& day, int& hour, int& minute, int& second, int& ms, int& forward_resolution, int& reverse_resolution, int& type) {
	tIndex.toUTC(year, month, day, hour, minute, second, ms);
	forward_resolution = tIndex.get_forward_resolution();
	reverse_resolution = tIndex.get_reverse_resolution();
	type       = tIndex.get_type();
}

STARE_ArrayIndexTemporalValue STARE::ValueFromUTC(int year, int month, int day, int hour, int minute, int second, int ms, int forward_resolution, int reverse_resolution, int type) {
  setTIndexUTC(year, month, day, hour, minute, second, ms, forward_resolution, reverse_resolution, type);
  return getArrayIndexTemporalValue();
}

STARE_ArrayIndexTemporalValue STARE::ValueFromUTC(struct tm& tm, int forward_resolution, int reverse_resolution, int type) {
    tm.tm_year += 1900;         // tm stores years since 1900 ...
    tm.tm_mon += 1;             // and months 0-based, while STARE stores months 1-based
    // type should be 1...
    return ValueFromUTC(tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, 0, forward_resolution, reverse_resolution, type);
}    

STARE_ArrayIndexTemporalValue STARE::ValueFromUTC(time_t& datetime, int forward_resolution, int reverse_resolution, int type) {
    struct tm tm;                       // time_t as seconds since UNIX epoch
    gmtime_r(&datetime, &tm);	        // gmtime_r converts to tm struct
    return ValueFromUTC(tm, forward_resolution, reverse_resolution, type);
}

Datetime STARE::UTCFromValue(STARE_ArrayIndexTemporalValue temporalValue) {
    tIndex.fromTemporalIndexValue(temporalValue);
    Datetime datetime;
    int forward_resolution;
    int reverse_resolution;
    int type;
    toUTC(datetime.year, datetime.month, datetime.day, datetime.hour, datetime.minute, datetime.second, datetime.ms, forward_resolution, reverse_resolution, type);
    return datetime;
}

STARE_ArrayIndexTemporalValue STARE::getArrayIndexTemporalValue() {
	return tIndex.scidbTemporalIndex();
}

TemporalIndex& STARE::setArrayIndexTemporalValue(STARE_ArrayIndexTemporalValue temporalValue) {
	return tIndex.fromTemporalIndexValue(temporalValue);
}

bool STARE::cmpTemporalAtResolution(STARE_ArrayIndexTemporalValue temporalValue) {
	TemporalIndex inputTIndex(temporalValue);
	return cmp_JulianTAIDays(this->tIndex,inputTIndex);
}

bool cmpTemporalAtResolution2(STARE_ArrayIndexTemporalValue tv1, STARE_ArrayIndexTemporalValue tv2) {
	TemporalIndex a(tv1), b(tv2);
	return cmp_JulianTAIDays(a,b);
}

bool cmpTemporalAtResolution3(STARE_ArrayIndexTemporalValue tv1, STARE_ArrayIndexTemporalValue tv2, double days) {
	TemporalIndex a(tv1), b(tv2);
	return cmp_JulianTAIDays3(a,b,days);
}

double STARE::cmpSpatialDistanceCosine(STARE_ArrayIndexSpatialValue a, STARE_ArrayIndexSpatialValue b) {
	return SpatialVectorFromValue(a)*SpatialVectorFromValue(b);
}
double STARE::cmpSpatialDistanceRadians(STARE_ArrayIndexSpatialValue a, STARE_ArrayIndexSpatialValue b) {
	return acos(SpatialVectorFromValue(a)*SpatialVectorFromValue(b));
}
double STARE::cmpSpatialResolutionEstimate(STARE_ArrayIndexSpatialValue a, STARE_ArrayIndexSpatialValue b) {
	double deltaL_meters = 6371.0e3*cmpSpatialDistanceRadians(a,b);
	return levelFromLengthMeterScaleFromEdge(deltaL_meters);
}

// FIXME: Clang-Tidy: Casting (double + 0.5) to integer leads to incorrect rounding;
// FIXME: consider using lround (#include <cmath>) instead. jhrg 4/20/20
int STARE::cmpSpatialResolutionEstimateI(STARE_ArrayIndexSpatialValue a, STARE_ArrayIndexSpatialValue b) {
	return int(cmpSpatialResolutionEstimate(a,b)+0.5);
}

bool terminatorp(STARE_ArrayIndexSpatialValue spatialStareId) {
	// TODO Figure out how to avoid unneeded reformatting.
	EmbeddedLevelNameEncoding leftJustifiedWithResolution;
	leftJustifiedWithResolution.setIdFromSciDBLeftJustifiedFormat(spatialStareId);
	return leftJustifiedWithResolution.terminatorp();
}

STARE_ArrayIndexSpatialValue shiftSpatialIdAtLevel(
		STARE_ArrayIndexSpatialValue spatialStareId,
		int resolution,
		int shiftAmount
		) {
//	if( shiftAmount == 0 ) {
//		return spatialStareId
//	} else {
		EmbeddedLevelNameEncoding leftJustifiedWithResolution;
		leftJustifiedWithResolution.setIdFromSciDBLeftJustifiedFormat(spatialStareId);

		if( shiftAmount >= 0) {
			leftJustifiedWithResolution.setId(
					leftJustifiedWithResolution.increment(leftJustifiedWithResolution.getId(), resolution, shiftAmount));
		} else {
			leftJustifiedWithResolution.setId(
					leftJustifiedWithResolution.decrement(leftJustifiedWithResolution.getId(), resolution, -shiftAmount));
		}
		return leftJustifiedWithResolution.getSciDBLeftJustifiedFormat();
//	}
}

uint64 spatialLevelMask() {
	EmbeddedLevelNameEncoding leftJustified;
	return leftJustified.levelMaskSciDB;
}

STARE_ArrayIndexSpatialValues expandInterval(STARE_SpatialIntervals interval, int64 force_resolution) { 
  // STARE_SpatialIntervals interval should just be one interval, i.e. a value or value+terminator.
  DIAGOUT1(cout << endl << dec << 200 << endl << flush;);
  STARE_ArrayIndexSpatialValue siv_orig = interval[0];
  STARE_ArrayIndexSpatialValue siv0 = siv_orig;
  EmbeddedLevelNameEncoding leftJustified;
  DIAGOUT1(cout << dec << 220 << endl << flush;);
  uint64 input_resolution  = siv0 & leftJustified.levelMaskSciDB;
  uint64 return_resolution = input_resolution; // To start with.
  DIAGOUT1(cout << dec << 225 << " " << setw(16) << setfill('0') << hex << siv0 << dec << endl << flush;);
    if( force_resolution > -1 ) {
      siv0 = ( siv0 & ~leftJustified.levelMaskSciDB ) | force_resolution;
      return_resolution = force_resolution;
    }
    DIAGOUT1(cout << dec << 229 << " f & resolution: " << dec << force_resolution << " " << return_resolution << endl << flush;);
    DIAGOUT1(cout << dec << 230 << " " << setw(16) << setfill('0') << hex << siv0 << dec << endl << flush;);
    leftJustified.setIdFromSciDBLeftJustifiedFormat(siv0);
  // cout << dec << 235 << endl << flush;
  STARE_ArrayIndexSpatialValue siv_term;
  if( interval.size() > 1 ) {
    siv_term = interval[1];
  } else {
    if( return_resolution != input_resolution ) { // TODO Maybe clean up this logic later.
      EmbeddedLevelNameEncoding lj; lj.setIdFromSciDBLeftJustifiedFormat(siv_orig);
      siv_term = lj.getSciDBTerminatorLeftJustifiedFormat(); // From siv_orig.
    } else {
      siv_term = leftJustified.getSciDBTerminatorLeftJustifiedFormat(); // From siv0.
    }
  }
  DIAGOUT1(cout << dec << 239 << " " << setw(16) << setfill('0') << hex << siv_term << dec << endl << flush;);
  DIAGOUT1(cout << endl << dec << 240 << endl << flush;);
  uint64 one_mask_to_resolution, one_at_resolution;
  leftJustified.SciDBincrement_LevelToMaskDelta(siv0 & leftJustified.levelMaskSciDB,one_mask_to_resolution,one_at_resolution);
  // cout << dec << 242 << endl << flush;

  DIAGOUT1(uint64 delta = ((siv_term+1)-(siv0 & ~leftJustified.levelMaskSciDB)));

  DIAGOUT1(cout << endl;);
  DIAGOUT1(uint64 one = 1);
  DIAGOUT1(cout << dec << 243 << " " << setw(16) << setfill('0') << hex << (one << (63-3-2*return_resolution)) << dec << endl << flush;);
  DIAGOUT1(cout << dec << 244 << " " << setw(16) << setfill('0') << hex <<  leftJustified.getSciDBTerminatorLeftJustifiedFormat() << dec << endl << flush;);
  DIAGOUT1(cout << dec << 245 << " " << setw(16) << setfill('0') << hex << siv_term << dec << endl << flush;);
  DIAGOUT1(cout << dec << 245 << " " << setw(16) << setfill('0') << hex << siv0 << dec << endl << flush;);
  DIAGOUT1(cout << dec << 245 << " " << setw(16) << setfill('0') << hex << delta << " " << dec << (delta/one_at_resolution) << endl << flush;);

  // Give as much rope as needed.
  DIAGOUT1(cout << dec << 246 << " " << setw(16) << setfill('0') << hex << siv0 << dec << endl << flush;);
  siv0 = (siv0 & ~one_mask_to_resolution) | return_resolution;
  DIAGOUT1(cout << dec << 247 << " " << setw(16) << setfill('0') << hex << siv0 << dec << endl << endl << flush;);
  DIAGOUT1(cout << dec << 247 << " " << setw(16) << setfill('0') << hex << one_at_resolution << dec << endl << endl << flush;);
  STARE_ArrayIndexSpatialValues expanded_interval;
  while( siv0 < siv_term ) {
    DIAGOUT1(cout << dec << 249 << " " << setw(16) << setfill('0') << hex << siv0 << dec << endl << flush;);
    expanded_interval.push_back(siv0);
    siv0 += one_at_resolution;
  }
  DIAGOUT1(cout << dec << 250 << endl << endl << flush;);
  return expanded_interval;
}

/**
 * Expand intervals found in intervals into spatial ids.
 */
STARE_ArrayIndexSpatialValues expandIntervalsMultiRes(STARE_SpatialIntervals intervals, int64 force_resolution, bool multi_resolution) {
  STARE_ArrayIndexSpatialValues expanded_values;
  EmbeddedLevelNameEncoding leftJustified;

  int i=0;
  while( i < intervals.size() ) {
    // cout << dec << 100 << endl << flush;
    STARE_ArrayIndexSpatialValue siv0, siv1;
    STARE_SpatialIntervals interval;
    siv0 = intervals[i];
    interval.push_back(siv0);
    ++i;
    if( i < intervals.size() ) {
      // peek
      siv1 = intervals[i];
      if( (siv1 & leftJustified.levelMaskSciDB) == leftJustified.levelMaskSciDB ) { // Check for a terminator.
	interval.push_back(siv1);
	++i;
      }
    }
    if( interval.size() == 1 || not multi_resolution ) {
      STARE_SpatialIntervals expandOne = expandInterval(interval,force_resolution);
      for(int j=0; j < expandOne.size(); ++j) {
	STARE_ArrayIndexSpatialValues_insert( expanded_values, expandOne[j] );
      }
      interval.clear();
    } else { // multi_resolution

      EmbeddedLevelNameEncoding lj;
      lj.setIdFromSciDBLeftJustifiedFormat(interval[0]);
      int level0 = lj.getLevel();
      
      STARE_SpatialIntervals    working_interval, remaining_interval; // Should be clear...
      remaining_interval.push_back(interval[0]);
      remaining_interval.push_back(interval[1]);

#if 0
      cout << " 0100 " << FMTX(remaining_interval[0]) << endl << flush;
      cout << " 0101 " << FMTX(remaining_interval[1]) << endl << flush;
#endif
      
      while( remaining_interval[0] < remaining_interval[1] ) { // Can never be equal since [1] is a terminator
	// Some kind of loop init
	working_interval.push_back(remaining_interval[0]);
	working_interval.push_back(remaining_interval[1]);

#undef FMTX
#define FMTX(x) " 0x" << setfill('0') << setw(16) << hex << x << dec 	

	while( working_interval.size() > 0 ) {
#if 0
	  cout << "\n---\nworking on : " << FMTX(working_interval[0]) << " " << FMTX(working_interval[1]) << endl << flush;
#endif

	  // Start loop
	  lj.setIdFromSciDBLeftJustifiedFormat(working_interval[0]); // Lower bound of interval
	  int64 bareLo           = lj.bareId();
	  int64 levelLo          = lj.getLevel();
	  int64 triangleNumberLo = lj.getLocalTriangleNumber();

	  lj.setIdFromSciDBLeftJustifiedFormat(working_interval[1]); // Terminator
	  EmbeddedLevelNameEncoding lj_hi_last_value = lj.atLevel(levelLo);
	  int64 bareHi = lj_hi_last_value.bareId();
	  int64 delta = bareHi-bareLo+1; // Number of triangles at levelLo in the interval
      
	  // Find the first tNum 0.
	  int64 delta_to_first = (4 - triangleNumberLo) % 4; // Note that the delta_to_first may exceed number in interval.
	  int64 number_of_full_parents = (1+delta-delta_to_first)/4;
	  // uint64 delta_in_last = 1+delta - delta_to_first - 4*number_of_full_parents;
#if 0
	  cout << "bareLo, bareHi         " << bareLo << " " << bareHi << endl << flush;
	  cout << "bareLo, bareHi         " << hex << bareLo << " " << bareHi << dec << endl << flush;
	  cout << "triangleNumberLo       " << triangleNumberLo << endl << flush;
	  cout << "delta_to_first         " << delta_to_first << endl << flush;
	  cout << "number_of_full_parents " << number_of_full_parents << endl << flush;
#endif
	  uint64 one_mask_to_level, one_at_level;
	  STARE_SpatialIntervals expandOne;

	  if( number_of_full_parents < 1 ) {
	    expandOne = expandInterval(working_interval,force_resolution);
	    for(int j=0; j < expandOne.size(); ++j) {
	      STARE_ArrayIndexSpatialValues_insert( expanded_values, expandOne[j] );
	    }
	    lj.setIdFromSciDBLeftJustifiedFormat(working_interval[0]); // make sure we have the right one
	    lj.SciDBincrement_LevelToMaskDelta(lj.getLevel(),one_mask_to_level, one_at_level); // Get an increment 
	    while(remaining_interval[0] < working_interval[1]) { // Might also get from successor to expandOne...?
	      remaining_interval[0] += one_at_level;
	    }
	    working_interval.clear(); // Try again...
	  } else { // At least one full parent
	    lj.setIdFromSciDBLeftJustifiedFormat(working_interval[0]); // make sure we have the right one	    
	    lj.SciDBincrement_LevelToMaskDelta(lj.getLevel(),one_mask_to_level, one_at_level); // Get an increment
#if 0
	    cout << " 1900 " << FMTX(working_interval[0]) << " " << FMTX(one_at_level) << " " << lj.getLevel() << endl << flush;
#endif
	    while( delta_to_first > 0 ) {
	      STARE_ArrayIndexSpatialValues_insert( expanded_values, working_interval[0] ); // Insert the value directly
	      working_interval[0] += one_at_level; // Should be closer to triangleNumber 0 now...
	      --delta_to_first;
	    }
	    // delta to first is 0
	    // How many triangles can we greedily coalesce?
#if 0
	    cout << " 2000 " << FMTX(working_interval[0]) << endl << flush;
#endif
	    lj.setIdFromSciDBLeftJustifiedFormat(working_interval[0]); // make sure we have the right one
	    lj.SciDBincrement_LevelToMaskDelta(lj.getLevel(),one_mask_to_level, one_at_level); // Get an increment
	    uint64 nextLo = working_interval[0]+one_at_level*4*number_of_full_parents; // Increment through the parents
	    working_interval[0]--; // -1 to coarsen the level
	    lj.setIdFromSciDBLeftJustifiedFormat(nextLo-1); // -1 for the coarser level
	    uint64 pred_term_htmid = lj.predecessorToLowerBound_NoDepthBit(lj.getId(),lj.getLevel()); // *Not* a STARE index
	    lj.setId(pred_term_htmid);
	    working_interval[1] = lj.getSciDBTerminatorLeftJustifiedFormat(); // Look at the sub-interval
	    lj.setIdFromSciDBLeftJustifiedFormat(nextLo);
	    remaining_interval[0] = lj.atLevel(nextLo,level0).getSciDBLeftJustifiedFormat(); // Note this should have the original level untouched, also successor to working_interval[1]
#if 0
	    cout << " 2080 " << FMTX(nextLo) << endl << flush;
	    cout << " 2090 " << FMTX(working_interval[0]) << endl << flush;
	    cout << " 2091 " << FMTX(working_interval[1]) << endl << flush;
	    cout << " 2100 " << FMTX(remaining_interval[0]) << endl << flush;
	    cout << " 2101 " << FMTX(remaining_interval[1]) << endl << flush;
#endif
	    // Note nextLo might get pulled in over multiple iterations of this segment of the loop...
	  } // at least one full parent
	} // working interval size is > 0
      } // remaining lo < hi
    } // if multi resolution
  } // while i < intervals.size()

  /*	    
	    
	  uint64 one_mask_to_level, one_at_level;
	  lj.setIdFromSciDBLeftJustifiedFormat(interval[0]); // make sure we have the right one
	  lj.SciDBincrement_LevelToMaskDelta(lj.getLevel(),one_mask_to_level, one_at_level); // Get an increment
	  lj.setIdFromSciDBLeftJustifiedFormat(interval[0]+one_at_level*(delta_to_first+1)); // Increment to first 0 triangle
	  uint64 pred_term_htmid = lj.predecessorToLowerBound_NoDepthBit(lj.getId()); // *Not* a STARE index
	  lj.setId(pred_term_htmid);
	  sub_interval.push_back(lj.getSciDBTerminatorLeftJustifiedFormat());
	  
	}
	
      }
	
     
      
      
      SpatialRange sr(interval); sr.compress();
      sr.dump();
      STARE_SpatialIntervals sis = sr.toSpatialIntervals();
      STARE_SpatialIntervals sub_interval;
      STARE_ArrayIndexSpatialValue sis0, sis1;
      int k=0;
      while( k < sis.size() ) {
	sub_interval.clear();
	sis0=sis[k++];
	sub_interval.push_back(sis0);
	if( k < sis.size() ) {
	  sis1 = sis[k]; // peek
	  if( (sis1 & leftJustified.levelMaskSciDB) == leftJustified.levelMaskSciDB ) { // Check for a terminator.
	    sub_interval.push_back(sis1);
	    ++k;
	  }
	  STARE_SpatialIntervals expandOne = expandInterval(sub_interval,force_resolution);
	  for(int j=0; j < expandOne.size(); ++j) {
	    STARE_ArrayIndexSpatialValues_insert( expanded_values, expandOne[j] );
	  }
	}
      }
    } else {
    
      STARE_SpatialIntervals expandOne = expandInterval(interval,force_resolution);
      // cout << dec << 140 << " " << i << endl << flush;
      for(int j=0; j < expandOne.size(); ++j) {
	//			cout << dec << 142 << " " << i << " " << j
	//					<< setw(16) << setfill('0') << hex << expandOne[j] << dec
	//					<< endl << flush;
      
	STARE_ArrayIndexSpatialValues_insert( expanded_values, expandOne[j] );
      
	//			cout << dec << 143 << " " << i << " " << j << endl << flush;
      }
      //		cout << dec << 150 << " " << i << endl << flush;
    }
    //	cout << dec << 160 << " " << i << endl << flush;
  }
  */
  return expanded_values;
}

STARE_ArrayIndexSpatialValues expandIntervals(STARE_SpatialIntervals intervals, int64 force_resolution) {
  return expandIntervalsMultiRes(intervals,force_resolution,false);
}

STARE_SpatialIntervals spatialIntervalFromHtmIDKeyPair(KeyPair kp) {
	STARE_SpatialIntervals interval;
	interval.push_back(EmbeddedLevelNameEncoding(BitShiftNameEncoding(kp.lo).leftJustifiedId()).getSciDBLeftJustifiedFormat());
	interval.push_back(EmbeddedLevelNameEncoding(BitShiftNameEncoding(kp.hi).leftJustifiedId()).getSciDBTerminatorLeftJustifiedFormat());
	return interval;
}

#define CMP_MODE(x,y) ((x & y) == y)

STARE_Stash::STARE_Stash(
		const string& filename
		,int& size_of_record_
		,uint64& number_of_records_
		,ios_base::openmode mode
		,char stash_type_
		) {
	init(filename,size_of_record_,number_of_records_,mode,stash_type_);
}

void STARE_Stash::init(
		const string& filename
		,int& size_of_record_
		,uint64& number_of_records_
		,ios_base::openmode mode
		,char stash_type_
		) {
	stashFile = new fstream(filename,mode);
	if( CMP_MODE(mode,ios::out) ) {
		current_mode      = ios::out; // TODO Consider non-binary at some point.
		stash_type        = stash_type_;
		size_of_record    = size_of_record_;
		number_of_records = number_of_records_;
		stashFile->write((char*) &stash_type,        sizeof(stash_type));
		stashFile->write((char*) &size_of_record,    sizeof(size_of_record));
		stashFile->write((char*) &number_of_records, sizeof(number_of_records));
	} else if( CMP_MODE(mode,ios::in) ) {
		current_mode      = ios::in;
		stashFile->read((char*) &stash_type,        sizeof(stash_type));
		stashFile->read((char*) &size_of_record,    sizeof(size_of_record));
		stashFile->read((char*) &number_of_records, sizeof(number_of_records));
		size_of_record_    = size_of_record;
		number_of_records_ = number_of_records;
	} else {
			// TODO Come up with a STARE exception class.
		throw SpatialException("STARE_Stash ios mode not understood");
	}
}

STARE_Stash::~STARE_Stash() {
	if( stashFile->is_open() ) {
		stashFile->close();
	}
	delete stashFile;
}

/**
 * Read or write one vector of SpatialIntervals, then close the file.
 */
STARE_Stash::STARE_Stash(
	const string& filename
	,STARE_SpatialIntervals& intervals
	,ios_base::openmode mode
	) {
	int size_record = sizeof(STARE_ArrayIndexSpatialValue);
	uint64 n_records;
	if( CMP_MODE(mode,ios::out) ) {
		n_records = intervals.size();
	}
	init(filename,size_record,n_records,mode,2); // TODO enum char?
	if( CMP_MODE(mode,ios::out) ) {
		stashFile->write(reinterpret_cast<char*>(&intervals[0]),size_record*n_records);
	} else if( CMP_MODE(mode,ios::in) ) {
		intervals.resize(n_records,0);
		stashFile->read(reinterpret_cast<char*>(&intervals[0]),size_record*n_records);
	} else {
			// TODO Come up with a STARE exception class.
		throw SpatialException("STARE_Stash Intervals ios mode not understood");
	}
	stashFile->close();
}

/**
 * Read or write one vector of ArrayIndexSpatialValues, then close the file.

STARE_Stash::STARE_Stash(
	const string& filename
	,STARE_ArrayIndexSpatialValues& values
	,ios_base::openmode mode
	) {
	int n_records;
	int size_record = sizeof(STARE_ArrayIndexSpatialValue);
	if( CMP_MODE(mode,ios::out) ) {
		n_records = values.size();
	}
	init(filename,size_record,n_records,mode,2); // TODO enum char?
	if( CMP_MODE(mode,ios::out) ) {
		stashFile->write(reinterpret_cast<char*>(&values[0]),size_record*n_records);
	} else if( CMP_MODE(mode,ios::in) ) {
		values.resize(n_records,0);
		stashFile->read(reinterpret_cast<char*>(&values[0]),size_record*n_records);
	} else {
			// TODO Come up with a STARE exception class.
		throw SpatialException("STARE_Stash spatial values ios mode not understood");
	}
	stashFile.close();
}
*/
void stash_spatial(const string& filename, STARE_SpatialIntervals intervals) {
  throw SpatialException("STARE stash_spatial not implemented");
}

void fetch_spatial(const string& filename, STARE_SpatialIntervals& intervals) {
  throw SpatialException("STARE fetch_spatial not implemented");
}

#undef CMP_MODE


