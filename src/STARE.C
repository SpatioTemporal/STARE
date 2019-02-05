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
uint32 STARE::ResolutionLevelFromValue(STARE_ArrayIndexSpatialValue StareId) {
	EmbeddedLevelNameEncoding leftJustifiedWithResolution;
	leftJustifiedWithResolution.setIdFromSciDBLeftJustifiedFormat(StareId);
	return leftJustifiedWithResolution.getLevel();
}

/**
 * Extract location information from the spatial array index value.
 */
LatLon STARE::LatLonDegreesFromValue(STARE_ArrayIndexSpatialValue StareId) {

	EmbeddedLevelNameEncoding leftJustifiedWithResolution;
	leftJustifiedWithResolution.setIdFromSciDBLeftJustifiedFormat(StareId);

	/// Workaround for a "feature." Coerce the level to the search_level of our sIndex.
	EmbeddedLevelNameEncoding leftJustifiedPositionOnly = leftJustifiedWithResolution.atLevel(this->search_level,true);

	BitShiftNameEncoding rightJustified(leftJustifiedPositionOnly.rightJustifiedId());
	uint64 htmID = rightJustified.getId();

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

