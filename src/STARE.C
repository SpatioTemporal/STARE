/**
 * STARE.cpp
 *
 *  The API to the STARE index functions.
 *
 *  Created on: Jan 31, 2019
 *      Author: mrilee
 *
 *  Copyright (C) 2019 Rilee Systems Technologies LLC
 */

#include "STARE.h"
#include "RangeConvex.h"
#include "SpatialDomain.h"
#include "SpatialInterface.h"
#include <iostream>
#include <algorithm>

// #define DIAG

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

/*	EmbeddedLevelNameEncoding leftJustifiedWithResolution;
	leftJustifiedWithResolution.setIdFromSciDBLeftJustifiedFormat(spatialStareId);

	/// Workaround for a "feature." Coerce the level to the search_level of our sIndex. "true" keeps all of the location bits.
	EmbeddedLevelNameEncoding leftJustifiedPositionOnly = leftJustifiedWithResolution.atLevel(this->search_level,true);

	BitShiftNameEncoding rightJustified(leftJustifiedPositionOnly.rightJustifiedId());
	uint64 htmID = rightJustified.getId();*/

	// cout << "sid: " << spatialStareId << endl << flush;

	uint64 htmID = htmIDFromValue(spatialStareId);
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

/*	BitShiftNameEncoding rightJustified(hid);
	index.setMaxlevel(7); // A fairly high resolution
	hid = index.idByPoint(v);
	rightJustified.setId(hid);
	EmbeddedLevelNameEncoding leftJustified(rightJustified.leftJustifiedId());*/

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
		resolutionLevel = ResolutionLevelFromValue(spatialStareId);
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

	// at = sTerminator(a), bt = sTerminator(b);

	int overlap = 0;
	if( ( a <= b ) && ( bt <= at ) ) {
		overlap = 1;
	} else if( ( b <= a ) && ( at <= bt ) ) {
		overlap = -1;
	}
//	cout << "a,at,b,bt: " << a << " " << at << " " << b << " " << bt << endl << flush;
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

	// TODO The following pattern repeats...
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
 * Unfortunately, it erroneously returns a few spatial IDs from the interior as well.
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

TemporalIndex& STARE::setTIndexTAI(int year, int month, int day, int hour, int minute, int second, int ms, int resolution,int type) {
	if( type != 2 ) {
		throw SpatialFailure("STARE::setTIndexTAI::type != 2 NOT IMPLEMENTED");
	}
	tIndex.fromFormattedJulianTAI(year, month, day, hour, minute, second, ms);
	tIndex.set_resolution(resolution);
	return tIndex;
}

TemporalIndex& STARE::setTIndexUTC(int year, int month, int day, int hour, int minute, int second, int ms, int resolution, int type) {
	if( type != 2 ) {
		throw SpatialFailure("STARE::setTIndexTAI::type != 2 NOT IMPLEMENTED");
	}
	tIndex.fromUTC(year, month, day, hour, minute, second, ms);
	tIndex.set_resolution(resolution);
	return tIndex;
}

void STARE::toTAI(int& year, int& month, int& day, int& hour, int& minute, int& second, int& ms, int& resolution, int& type) {
	tIndex.toFormattedJulianTAI(year, month, day, hour, minute, second, ms);
	resolution = tIndex.get_resolution();
	type       = tIndex.get_type();
}

void STARE::toUTC(int& year, int& month, int& day, int& hour, int& minute, int& second, int& ms, int& resolution, int& type) {
	tIndex.toUTC(year, month, day, hour, minute, second, ms);
	resolution = tIndex.get_resolution();
	type       = tIndex.get_type();
}

STARE_ArrayIndexTemporalValue STARE::ValueFromUTC(int year, int month, int day, int hour, int minute, int second, int ms, int resolution, int type) {
    setTIndexUTC(year, month, day, hour, minute, second, ms, resolution, type);
    return getArrayIndexTemporalValue();
}

STARE_ArrayIndexTemporalValue STARE::ValueFromUTC(struct tm& tm, int resolution, int type) {
    tm.tm_year += 1900;         // tm stores years since 1900 ...
    tm.tm_mon += 1;             // and months 0-based, while STARE stores months 1-based
    return ValueFromUTC(tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, 0, resolution, 2);
}    

STARE_ArrayIndexTemporalValue STARE::ValueFromUTC(time_t& datetime, int resolution, int type) {
    struct tm tm;                       // time_t as seconds since UNIX epoch
    gmtime_r(&datetime, &tm);	        // gmtime_r converts to tm struct
    return ValueFromUTC(tm, resolution, type);
}

Datetime STARE::UTCFromValue(STARE_ArrayIndexTemporalValue temporalValue) {
    tIndex.fromTemporalIndexValue(temporalValue);
    Datetime datetime;
    int resolution;
    int type;
    toUTC(datetime.year, datetime.month, datetime.day, datetime.hour, datetime.minute, datetime.second, datetime.ms, resolution, type);
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
    DIAGOUT1(cout << endl << dec << 200 << endl << flush;)
	STARE_ArrayIndexSpatialValue siv_orig = interval[0];
	STARE_ArrayIndexSpatialValue siv0 = siv_orig;
	EmbeddedLevelNameEncoding leftJustified;
	DIAGOUT1(cout << dec << 220 << endl << flush;)
        uint64 input_resolution  = siv0 & leftJustified.levelMaskSciDB;
	uint64 return_resolution = input_resolution; // To start with.
	DIAGOUT1(cout << dec << 225 << " " << setw(16) << setfill('0') << hex << siv0 << dec << endl << flush;)
	if( force_resolution > -1 ) {
		siv0 = ( siv0 & ~leftJustified.levelMaskSciDB ) | force_resolution;
		return_resolution = force_resolution;
	}
	DIAGOUT1(cout << dec << 229 << " f & resolution: " << dec << force_resolution << " " << return_resolution << endl << flush;)
	DIAGOUT1(cout << dec << 230 << " " << setw(16) << setfill('0') << hex << siv0 << dec << endl << flush;)
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
	DIAGOUT1(cout << dec << 239 << " " << setw(16) << setfill('0') << hex << siv_term << dec << endl << flush;)
	DIAGOUT1(cout << endl << dec << 240 << endl << flush;)
	uint64 one_mask_to_resolution, one_at_resolution;
	leftJustified.SciDBincrement_LevelToMaskDelta(siv0 & leftJustified.levelMaskSciDB,one_mask_to_resolution,one_at_resolution);
	// cout << dec << 242 << endl << flush;

    DIAGOUT1(uint64 delta = ((siv_term+1)-(siv0 & ~leftJustified.levelMaskSciDB)));

	DIAGOUT1(cout << endl;)
    DIAGOUT1(uint64 one = 1);
	DIAGOUT1(cout << dec << 243 << " " << setw(16) << setfill('0') << hex << (one << (63-3-2*return_resolution)) << dec << endl << flush;)
	DIAGOUT1(cout << dec << 244 << " " << setw(16) << setfill('0') << hex <<  leftJustified.getSciDBTerminatorLeftJustifiedFormat() << dec << endl << flush;)
	DIAGOUT1(cout << dec << 245 << " " << setw(16) << setfill('0') << hex << siv_term << dec << endl << flush;)
	DIAGOUT1(cout << dec << 245 << " " << setw(16) << setfill('0') << hex << siv0 << dec << endl << flush;)
	DIAGOUT1(cout << dec << 245 << " " << setw(16) << setfill('0') << hex << delta << " " << dec << (delta/one_at_resolution) << endl << flush;)

	// Give as much rope as needed.
	DIAGOUT1(cout << dec << 246 << " " << setw(16) << setfill('0') << hex << siv0 << dec << endl << flush;)
	siv0 = (siv0 & ~one_mask_to_resolution) | return_resolution;
	DIAGOUT1(cout << dec << 247 << " " << setw(16) << setfill('0') << hex << siv0 << dec << endl << endl << flush;)
	DIAGOUT1(cout << dec << 247 << " " << setw(16) << setfill('0') << hex << one_at_resolution << dec << endl << endl << flush;)
	STARE_ArrayIndexSpatialValues expanded_interval;
	while( siv0 < siv_term ) {
		DIAGOUT1(cout << dec << 249 << " " << setw(16) << setfill('0') << hex << siv0 << dec << endl << flush;)
		expanded_interval.push_back(siv0);
		siv0 += one_at_resolution;
	}
	DIAGOUT1(cout << dec << 250 << endl << endl << flush;)
	return expanded_interval;
}

/**
 * Expand intervals found in intervals into spatial ids.
 */
STARE_ArrayIndexSpatialValues expandIntervals(STARE_SpatialIntervals intervals, int64 force_resolution) {
	STARE_ArrayIndexSpatialValues expanded_values;
	EmbeddedLevelNameEncoding leftJustified;

	int i=0;
	while( i < intervals.size() ) {
		// cout << dec << 100 << endl << flush;
		STARE_ArrayIndexSpatialValue siv0, siv1;
		STARE_SpatialIntervals interval;
		siv0 = intervals[i];
		interval.push_back(siv0);
		// cout << dec << 110 << endl << flush;
		++i;
		if( i < intervals.size() ) {
			// peek
			// cout << dec << 120 << endl << flush;
			siv1 = intervals[i];
			// cout << dec << 120 << " " << i << " " << setw(16) << setfill('0') << hex << siv1 << dec << endl << flush;
			if( (siv1 & leftJustified.levelMaskSciDB) == leftJustified.levelMaskSciDB ) { // Check for a terminator.
				// cout << dec << 121 << " " << i << endl << flush;
				interval.push_back(siv1);
				++i;
			}
		}
		// cout << dec << 130 << " " << i << endl << flush;
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
	return expanded_values;
}

STARE_SpatialIntervals spatialIntervalFromHtmIDKeyPair(KeyPair kp) {
	STARE_SpatialIntervals interval;
	interval.push_back(EmbeddedLevelNameEncoding(BitShiftNameEncoding(kp.lo).leftJustifiedId()).getSciDBLeftJustifiedFormat());
	interval.push_back(EmbeddedLevelNameEncoding(BitShiftNameEncoding(kp.hi).leftJustifiedId()).getSciDBTerminatorLeftJustifiedFormat());
	return interval;
}

