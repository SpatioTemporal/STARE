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
#include <iostream>

STARE::STARE() {
	// TODO Auto-generated constructor stub
	SpatialVector axis     = 0.5*xhat + 0.5*yhat; axis.normalize();
	float64       theta    = 0.25*gPi;
	rotate_root_octahedron = SpatialRotation(axis,theta);
	sIndex                 = SpatialIndex(search_level, build_level, rotate_root_octahedron);
	sIndexes.insert(std::make_pair(search_level,sIndex));
	/*if( sIndexes.find(search_level) ) {
		// The level was found...
	}*/
}

STARE::~STARE() {
	// TODO Auto-generated destructor stub
}

uint32 STARE::sSearchLevel() const {
	return sIndex.getMaxlevel();
}

STARE_ArrayIndexSpatialValue STARE::ValueFromLatLonDegrees(
		float64 latDegrees, float64 lonDegrees, int resolutionLevel) {

	BitShiftNameEncoding       rightJustified(sIndex.idByLatLon(latDegrees,lonDegrees)); // Dip into the legacy code
	EmbeddedLevelNameEncoding  leftJustified(rightJustified.leftJustifiedId());
	EmbeddedLevelNameEncoding  leftJustifiedWithResolution = leftJustified.atLevel(resolutionLevel, true);

	// NOTE: This is returning a SciDB-formatted index.
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

	uint64 htmID = htmIDFromValue(spatialStareId);

	SpatialVector v;
	/// This returns the center of the triangle (at index.search_level). Need to extract the position information.
	sIndex.pointByHtmId(v, htmID);

	float64 lat=-999, lon=-999;
	v.getLatLonDegrees(lat, lon);

	// LatLonDegrees64 latlon = {.lat = lat, .lon = lon };
	LatLonDegrees64 latlon(lat, lon); //  = {.lat = lat, .lon = lon };

	// return latlon;
	return LatLonDegrees64(lat, lon);

/*	BitShiftNameEncoding rightJustified(hid);
	index.setMaxlevel(7); // A fairly high resolution
	hid = index.idByPoint(v);
	rightJustified.setId(hid);
	EmbeddedLevelNameEncoding leftJustified(rightJustified.leftJustifiedId());*/

}

Triangle STARE::TriangleFromValue(STARE_ArrayIndexSpatialValue spatialStareId, int resolutionLevel) {
	// Users are going to expect the default resolution level to be that embedded in the sStareId.
	uint64 htmID = -1;
	if( resolutionLevel < 0 ) {
		// Use the level embedded in the index value.
		resolutionLevel = ResolutionLevelFromValue(spatialStareId);
		htmID = htmIDFromValue(spatialStareId,resolutionLevel);
	} else {
		// Use the coerced level, which may be set to the search_level.
		htmID = htmIDFromValue(spatialStareId,resolutionLevel);
	}

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

	SpatialIndex si = getIndex(resolutionLevel);
	si.pointByHtmId(vc,htmID);
	si.nodeVertexByHtmId(v1, v2, v3, htmID);
	// std::cout << 220 << std::endl;
	Vertices vertices; vertices.push_back(v1); vertices.push_back(v2); vertices.push_back(v3);
	// std::cout << 230 << std::endl;
	return {.centroid=vc, .vertices=vertices};
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

bool STARE::terminatorp(STARE_ArrayIndexSpatialValue spatialStareId) {
	// TODO Figure out how to avoid unneeded reformatting.
	EmbeddedLevelNameEncoding leftJustifiedWithResolution;
	leftJustifiedWithResolution.setIdFromSciDBLeftJustifiedFormat(spatialStareId);
	return leftJustifiedWithResolution.terminatorp();
}

/**
 * Return a vector of index values of potentially varying sizes (resolution levels) covering the bounding box.
 *
 * Very preliminary. Needs verification.
 *
 * @param force_resolution_level - the smallest triangles to use in the covering
 *
 */
STARE_Intervals STARE::BoundingBoxFromLatLonDegrees(
	LatLonDegrees64ValueVector corners, int force_resolution_level) {
	int resolution_level; // for the match
	STARE_Intervals intervals;
	SpatialIndex index;
	if( corners.size() != 4) {
		return intervals;
	}
	if( force_resolution_level > -1 ) {
		index = getIndex(force_resolution_level);
	} else {
		index = getIndex(8); /// TODO Hardcoded...
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
	bool varlen_false = false;
	bool overlap = d.intersect(&index,&r,&varlen_false);
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

SpatialIndex STARE::getIndex(int resolutionLevel) {
	if( sIndexes.find(resolutionLevel) == sIndexes.end() ) {
		sIndexes.insert(std::make_pair(resolutionLevel,SpatialIndex(resolutionLevel, build_level, rotate_root_octahedron)));
	}
	return sIndexes[resolutionLevel];
}

/**
 * Return the htmID value from the spatialStareId.
 *
 * Note the htmID precision level needn't have a resolution interpretation, but is more purely geometric.
 * This is important when calling into the legacy htm foundation and why it's kept private.
 */
uint64 STARE::htmIDFromValue(STARE_ArrayIndexSpatialValue spatialStareId, int force_resolution_level) {
	EmbeddedLevelNameEncoding leftJustifiedWithResolution;
	leftJustifiedWithResolution.setIdFromSciDBLeftJustifiedFormat(spatialStareId);
	/// Workaround for a "feature." Coerce the level to the search_level of our sIndex. "true" keeps all of the location bits.
	int search_level = this->search_level;  /// The default search level (for this sIndex) is usually 27. It can be coerced, say, to the resolution level of the spatial STARE index.
	if( force_resolution_level > 0 ) {
		search_level = force_resolution_level;
	}
	// EmbeddedLevelNameEncoding leftJustifiedPositionOnly = leftJustifiedWithResolution.atLevel(this->search_level,true);
	EmbeddedLevelNameEncoding leftJustifiedPositionOnly = leftJustifiedWithResolution.atLevel(search_level,true);
	BitShiftNameEncoding rightJustified(leftJustifiedPositionOnly.rightJustifiedId());
	uint64 htmID = rightJustified.getId();
	return htmID;
}
