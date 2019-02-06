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
		float64 latDegrees, float64 lonDegrees, uint32 resolutionLevel) {

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
LatLon STARE::LatLonDegreesFromValue(STARE_ArrayIndexSpatialValue spatialStareId) {

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

	LatLon latlon = {.lat = lat, .lon = lon };

	return latlon;

/*	BitShiftNameEncoding rightJustified(hid);
	index.setMaxlevel(7); // A fairly high resolution
	hid = index.idByPoint(v);
	rightJustified.setId(hid);
	EmbeddedLevelNameEncoding leftJustified(rightJustified.leftJustifiedId());*/

}

Triangle STARE::TriangleFromValue(STARE_ArrayIndexSpatialValue spatialStareId, int resolutionLevel) {
	uint64 htmID = htmIDFromValue(spatialStareId,resolutionLevel);
	SpatialVector vc,v1,v2,v3;
	if( resolutionLevel < 0 ) {
		resolutionLevel = search_level;
	}
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
	// std::cout << 220 << std::endl;
	Vertices vertices; vertices.push_back(v1); vertices.push_back(v2); vertices.push_back(v3);
	// std::cout << 230 << std::endl;
	return {.centroid=vc, .vertices=vertices};
}

float64 STARE::AreaFromValue(STARE_ArrayIndexSpatialValue spatialStareId, int resolutionLevel) {
	uint64 htmID = htmIDFromValue(spatialStareId,resolutionLevel); /// Coerce to resolutionLevel
	if( resolutionLevel <= 0 ) {
		resolutionLevel = search_level;
	}
	/// TODO Fix redundant operations in the following two calls.
	if( sIndexes.find(resolutionLevel) == sIndexes.end() ) {
		sIndexes.insert(std::make_pair(resolutionLevel,SpatialIndex(resolutionLevel, build_level, rotate_root_octahedron)));
	}
	return sIndexes[resolutionLevel].areaByHtmId(htmID);
}

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
