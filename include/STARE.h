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
#include <map>
#include <vector>

// #include "TemporalIndex.h"

// TODO Consider index vs. array index vs. collections of the same & set logic.

/// A lightweight type for the STARE spatial index
typedef uint64 STARE_ArrayIndexSpatialValue;
typedef std::vector<STARE_ArrayIndexSpatialValue> STARE_SpatialIntervals;
typedef std::vector<STARE_ArrayIndexSpatialValue> STARE_ArrayIndexSpatialValues;
// struct LatLon   {float64 lat, lon;};
// typedef std::vector<SpatialVector> Vertices;
// struct Triangle {SpatialVector centroid;  Vertices vertices;};


/// TODO The following is a stopgap on the way to completely removing this hardcoded bit.
#define STARE_HARDWIRED_RESOLUTION_LEVEL_MAX 27
#define STARE_HARDWIRED_BUILD_LEVEL_MAX       5


class STARE {
public:
	STARE();
	STARE( int search_level, int build_level, SpatialRotation rotate_root_octahedron );
	virtual ~STARE();

	uint32 sSearchLevel() const; /// Location precision level in the sIndex.

	// Spatial array index functions. [Maybe change the name StareId to spatialStareId in the following...]
	STARE_ArrayIndexSpatialValue ValueFromLatLonDegrees(float64 latDegrees, float64 lonDegrees, int resolutionLevel = STARE_HARDWIRED_RESOLUTION_LEVEL_MAX);
	LatLonDegrees64 LatLonDegreesFromValue(STARE_ArrayIndexSpatialValue spatialStareId);
	uint32 ResolutionLevelFromValue(STARE_ArrayIndexSpatialValue spatialStareId);

	Triangle TriangleFromValue(STARE_ArrayIndexSpatialValue spatialStareId, int resolutionLevel = -1);
	float64  AreaFromValue    (STARE_ArrayIndexSpatialValue spatialStareId, int resolutionLevel = -1);

	SpatialIndex getIndex() { return sIndex; }
	SpatialIndex getIndex(int resolutionLevel);
	uint64       getMaximumSearchLevel() { return STARE_HARDWIRED_RESOLUTION_LEVEL_MAX; } // TODO Ugh. See search_level in the privates...
	uint64       getSearchLevel() { return this->search_level; } // TODO Ugh. See search_level in the privates...
	uint64       getBuildLevel()  { return this->build_level; }
	SpatialRotation getRotation() { return this->rotate_root_octahedron; }

	bool terminatorp(STARE_ArrayIndexSpatialValue spatialStareId); /// Check if the index value is a terminator.

	STARE_SpatialIntervals CoverBoundingBoxFromLatLonDegrees(LatLonDegrees64ValueVector corners, int force_resolution_level = -1);
	STARE_SpatialIntervals CoverCircleFromLatLonRadiusDegrees(float64 latDegrees, float64 lonDegrees, float64 radius_degrees, int force_resolution_level = -1);

	STARE_ArrayIndexSpatialValues NeighborsOfValue(STARE_ArrayIndexSpatialValue spatialStareId);

	float64 lengthMeterScaleFromEdgeFromLevel(int level){
		return 6371.0e3*gPio2*pow(2.0,-level);
	}

	// uint32 tResolutionLevel() const;

// private:
	/// TODO memoize sIndex in case we need to cache multiple resolutions.
	SpatialIndex    sIndex; // The current (default) index
	std::map<int,SpatialIndex> sIndexes;

	SpatialRotation rotate_root_octahedron;
	/**
	 * Set the underlying htm index to the maximum level to keep the
	 * highest precision location information. A difference between
	 * STARE and htm is that the level corresponds to a resolution,
	 * not the level of location precision.
	 */

	/// Set the search level of the default index. Note: roundoff error corrupts calculations at level 27, e.g. area(...).
	uint64          search_level = STARE_HARDWIRED_RESOLUTION_LEVEL_MAX; /// Also known as max_level.
	uint64          build_level  = STARE_HARDWIRED_BUILD_LEVEL_MAX;  /// How deep to build the lookup table. Memory hog. Space vs. time...

	uint64 htmIDFromValue(STARE_ArrayIndexSpatialValue spatialStareId, int force_resolution_level=-1);
	STARE_ArrayIndexSpatialValue ValueFromHtmID(uint64 htmID); // * UNTESTED * //

	// TemporalIndex tIndex;
};

void STARE_test();
void STARE_Temporal_test();

#endif /* INCLUDE_STARE_H_ */
