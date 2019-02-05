/*
 * STARE_test.cpp
 *
 *  Created on: Jan 31, 2019
 *      Author: mrilee
 *
 *  Copyright (C) 2019 Rilee Systems Technologies LLC
 */

#include "Test.h"


void STARE_test() {

	STARE index;

//	ASSERT_EQUAL(1,index.ValueFromLatLonDegrees(45.0, 45.0));

	LatLon latlon0 = {.lat = 45.0, .lon = 45.0 };
	// cout << "latlon0: " << latlon0.lat << " " << latlon0.lon << endl;

	STARE_ArrayIndexSpatialValue aIndex = index.ValueFromLatLonDegrees(latlon0.lat,latlon0.lon,8);
	// cout << "aIndex: " << aIndex << endl;

	LatLon latlon1 = index.LatLonDegreesFromValue(aIndex);
	// cout << "latlon1: " << latlon1.lat << " " << latlon1.lon << endl;

	// float64 tol = 1.0e-14; /// What's our floating point error?
	float64 tol = 1.0e-6; /// What's our floating point error? Grumble.
	/// 1 degree ~ 111km, so 1.0e-6 * 1 degree ~ 0.1 m.
	// cout << setprecision(17);
	// cout << "delta-lat: " << latlon1.lat - latlon0.lat << endl;
	// cout << "delta-lon: " << latlon1.lon - latlon0.lon << endl;
	ASSERT_EQUAL_DELTAM("Round trip lat <-> sIndex",latlon0.lat,latlon1.lat,tol);
	ASSERT_EQUAL_DELTAM("Round trip lon <-> sIndex",latlon0.lon,latlon1.lon,tol);

	// FAIL();
}

