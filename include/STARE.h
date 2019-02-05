/**
 * STARE.h
 *
 *  The API to the STARE index functions.
 *
 *  Created on: Jan 31, 2019
 *      Author: mrilee
 *
 *  Copyright (C) 2019 Rilee Systems Technologies LLC
 */

#ifndef INCLUDE_STARE_H_
#define INCLUDE_STARE_H_

#include "SpatialGeneral.h"
#include "SpatialIndex.h"
#include "SpatialRotation.h"
#include "EmbeddedLevelNameEncoding.h"

// #include "TemporalIndex.h"

// TODO Consider index vs. array index vs. collections of the same & set logic.

/// A lightweight type for the STARE spatial index
typedef uint64 STARE_ArrayIndexSpatialValue;
struct LatLon {float64 lat, lon;};

class STARE {
public:
	STARE();
	virtual ~STARE();

	uint32 sSearchLevel() const; /// Location precision level in the sIndex.

	// Spatial array index functions. [Maybe change the name StareId to spatialStareId in the following...]
	STARE_ArrayIndexSpatialValue ValueFromLatLonDegrees(float64 latDegrees, float64 lonDegrees, uint32 resolutionLevel = 27);
	LatLon LatLonDegreesFromValue(STARE_ArrayIndexSpatialValue StareId);
	uint32 ResolutionLevelFromValue(STARE_ArrayIndexSpatialValue StareId);

	// uint32 tResolutionLevel() const;

private:
	SpatialIndex    sIndex;
	SpatialRotation rotate_root_octahedron;
	/**
	 * Set the underlying htm index to the maximum level to keep the
	 * highest precision location information. A difference between
	 * STARE and htm is that the level corresponds to a resolution,
	 * not the level of location precision.
	 */
	uint64          search_level = 27; /// Also known as max_level.
	uint64          build_level  = 5;  /// How deep to build the lookup table. Memory hog. Space vs. time...

	// TemporalIndex tIndex;
};

void STARE_test();

#endif /* INCLUDE_STARE_H_ */
