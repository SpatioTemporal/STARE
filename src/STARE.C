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

/**
 * @brief Version function with C linkage to aid in finding the library with autoconf
 * @return The library's version. Points to static storage.
 */
extern "C" const char *STARE_version() {
    return (const char *)STARE_VERSION;
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

	sIndex                 = SpatialIndex(this->search_level, this->build_level, this->rotate_root_octahedron);
	sIndexes.insert(std::make_pair(this->search_level,sIndex));
}

STARE::~STARE() {
	// TODO Auto-generated destructor stub
}

void STARE::defaultConfiguration() {
	SpatialVector axis     = 0.5*xhat + 0.5*yhat; axis.normalize();
	float64       theta    = 0.25*gPi - 12.0e-9; // bump it over a 27-level triangle
	rotate_root_octahedron = SpatialRotation(axis,theta);
}


uint32 STARE::sSearchLevel() const {
	return sIndex.getMaxlevel();
}

STARE_ArrayIndexSpatialValue STARE::ValueFromLatLonDegrees(
		float64 latDegrees, float64 lonDegrees, int resolutionLevel) {

	// cout << "svflld: lat,lon,rL: " << latDegrees << " " << lonDegrees << " " << resolutionLevel << endl << flush;

	BitShiftNameEncoding       rightJustified(sIndex.idByLatLon(latDegrees,lonDegrees)); // Dip into the legacy code
	EmbeddedLevelNameEncoding  leftJustified(rightJustified.leftJustifiedId());
	EmbeddedLevelNameEncoding  leftJustifiedWithResolution = leftJustified.atLevel(resolutionLevel, true);

	// cout << "svflld: result: " << leftJustifiedWithResolution.getSciDBLeftJustifiedFormat() << endl << flush;

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

	// cout << "sid: " << spatialStareId << endl << flush;

	uint64 htmID = htmIDFromValue(spatialStareId);

	SpatialVector v;
	/// This returns the center of the triangle (at index.search_level). Need to extract the position information.
	sIndex.pointByHtmId(v, htmID);

	float64 lat=-999, lon=-999;
	v.getLatLonDegrees(lat, lon);

	// cout << "sid-latlon: " << lat << ", " << lon << endl << flush;

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

STARE_ArrayIndexSpatialValue sTerminator(STARE_ArrayIndexSpatialValue spatialStareId) {
	EmbeddedLevelNameEncoding leftJustifiedWithResolution;
	leftJustifiedWithResolution.setIdFromSciDBLeftJustifiedFormat(spatialStareId);
	return leftJustifiedWithResolution.getSciDBTerminatorLeftJustifiedFormat();
}

/**
 * Compare two spatial array index values a, b.
 *
 * Returns
 *   1 if b is in a
 *  -1 if a is in b
 *   0 otherwise.
 *
 */
int cmpSpatial(STARE_ArrayIndexTemporalValue a_, STARE_ArrayIndexTemporalValue b_) {

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

bool STARE::terminatorp(STARE_ArrayIndexSpatialValue spatialStareId) {
	// TODO Figure out how to avoid unneeded reformatting.
	EmbeddedLevelNameEncoding leftJustifiedWithResolution;
	leftJustifiedWithResolution.setIdFromSciDBLeftJustifiedFormat(spatialStareId);
	return leftJustifiedWithResolution.terminatorp();
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
	int resolution_level; // for the match
	STARE_SpatialIntervals intervals;
	SpatialIndex index;
	if( corners.size() != 4) {
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
	bool varlen_false = false;
	bool overlap = d.intersect(&index,&r,&varlen_false);
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
	// tIndex.fromFormattedJulianTAI(year, month, day, hour, minute, second, ms, type);
	tIndex.fromFormattedJulianTAI(year, month, day, hour, minute, second, ms);
	tIndex.set_resolution(resolution);
	return tIndex;
}

TemporalIndex& STARE::setTIndexUTC(int year, int month, int day, int hour, int minute, int second, int ms, int resolution, int type) {
	if( type != 2 ) {
		throw SpatialFailure("STARE::setTIndexTAI::type != 2 NOT IMPLEMENTED");
	}
	// tIndex.fromUTC(year, month,day,hour, minute, second, ms, type);
	tIndex.fromUTC(year, month,day,hour, minute, second, ms);
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

// TemporalIndex& STARE::getTIndex() { return tIndex; }

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
