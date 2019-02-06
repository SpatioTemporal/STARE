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

	/// TODO Need multiple indexes to handle array index values with different resolution levels.
	STARE_ArrayIndexSpatialValue aIndex = index.ValueFromLatLonDegrees(latlon0.lat,latlon0.lon,8);
	/*
	cout << "aIndex: " << hex << aIndex << dec << endl;
	cout << "Resolution level tr0: " << index.ResolutionLevelFromValue(aIndex) << endl;
	*/

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

	// int level = 27; // works-ish... fails due to rounding errors.
	// int level = 26; // works-ish...
	int level = 8; // Doesn't work
	// cout << 100 << endl;
	//Triangle tr0 = index.TriangleFromValue(aIndex,level);
	// Test default

	/*
	Triangle tr0 = index.TriangleFromValue(aIndex);
	cout << "centroid: " << tr0.centroid    << endl;
	cout << "tr0.0:    " << tr0.vertices[0] << endl;
	cout << "tr0.1:    " << tr0.vertices[1] << endl;
	cout << "tr0.2:    " << tr0.vertices[2] << endl;

	// float64 area = index.AreaFromValue(aIndex,level);
	// Test default
	float64 area = index.AreaFromValue(aIndex);

	cout << "area:      " << area            << endl;
	cout << "area(tr0): " << index.getIndex().area(tr0.vertices[0],tr0.vertices[1],tr0.vertices[2]) << endl;
	*/

	// Regression tests.
	ASSERT_EQUAL_DELTAM("Area test",2.1138539736228398e-05,index.AreaFromValue(aIndex),tol);
	ASSERT_EQUAL_DELTAM("Area test",2.1138539736228398e-05,index.AreaFromValue(aIndex,level),tol);
	ASSERT_EQUAL_DELTAM("Area test",2.1138539736228398e-05*0.25,index.AreaFromValue(aIndex,level+1),tol);
	ASSERT_EQUAL_DELTAM("Area test",2.1138539736228398e-05/0.25,index.AreaFromValue(aIndex,level-1),tol);


	// FAIL();
}

