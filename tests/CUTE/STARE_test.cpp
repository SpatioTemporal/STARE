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

	LatLonDegrees64 latlon0(45.0,45.0);
	// LatLon latlon0 = {.lat = 45.0, .lon = 45.0 };
	// cout << "latlon0: " << latlon0.lat << " " << latlon0.lon << endl;

	/// TODO Need multiple indexes to handle array index values with different resolution levels.
	STARE_ArrayIndexSpatialValue aIndex = index.ValueFromLatLonDegrees(latlon0.lat,latlon0.lon,8);
	/*
	cout << "aIndex: " << hex << aIndex << dec << endl;
	cout << "Resolution level tr0: " << index.ResolutionLevelFromValue(aIndex) << endl;
	*/

	LatLonDegrees64 latlon1 = index.LatLonDegreesFromValue(aIndex);
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

	/* bbox test

	LatLonDegrees64ValueVector latlonbox;

	latlonbox.push_back(LatLonDegrees64(0,0));
	latlonbox.push_back(LatLonDegrees64(2,0));
	latlonbox.push_back(LatLonDegrees64(2,2));
	latlonbox.push_back(LatLonDegrees64(0,2));

	// STARE_Intervals intervals = index.CoverBoundingBoxFromLatLonDegrees(latlonbox,6);
	STARE_Intervals intervals = index.CoverBoundingBoxFromLatLonDegrees(latlonbox);

	// TODO Why are the first few triangles at level 7 and not 8 for the default level = 8?
	for( STARE_Intervals::iterator it = intervals.begin(); it != intervals.end(); ++it ) {
		cout << hex << "interval: 0x" << *it << dec << endl;
	}
	cout << "intervals size: " << intervals.size() << endl;

	cout << "0x" << hex << intervals[0] << dec << endl;
	Triangle tr0 = index.TriangleFromValue(intervals[0]);
	cout << "centroid: " << tr0.centroid    << endl;
	cout << "tr0.0:    " << tr0.vertices[0] << endl;
	cout << "tr0.1:    " << tr0.vertices[1] << endl;
	cout << "tr0.2:    " << tr0.vertices[2] << endl;

	*/

	/*
	 * Cover circle test
	 *

	// STARE_Intervals intervals = index.CoverCircleFromLatLonRadiusDegrees(30.0, 45.0, 1.0, 6);
	STARE_Intervals intervals = index.CoverCircleFromLatLonRadiusDegrees(30.0, 45.0, 1.0, 8);
	for( STARE_Intervals::iterator it = intervals.begin(); it != intervals.end(); ++it ) {
		cout << hex << "interval: 0x" << *it << dec << endl;
	}
	*/

	/*
	 * Python integration bug. Turned out to be a type casting problem.
	 *
	{
	LatLonDegrees64 latlon0(30.0,45.0);
	// LatLon latlon0 = {.lat = 45.0, .lon = 45.0 };
	cout << "latlon0: " << latlon0.lat << " " << latlon0.lon << endl;

	/// TODO Need multiple indexes to handle array index values with different resolution levels.
	STARE_ArrayIndexSpatialValue aIndex = index.ValueFromLatLonDegrees(latlon0.lat,latlon0.lon,8);

	cout << "aIndex: " << hex << aIndex << " " << dec << aIndex << endl;
	cout << "Resolution level tr0: " << index.ResolutionLevelFromValue(aIndex) << endl;

	LatLonDegrees64 latlon1 = index.LatLonDegreesFromValue(aIndex);
	cout << "latlon1: " << latlon1.lat << " " << latlon1.lon << endl;

	FAIL();

	}
	*/

	//
	// spatialStareId = EmbeddedLevelNameEncoding(BitShiftNameEncoding(htmID).leftJustifiedId()).getSciDBLeftJustifiedFormat();

	uint64 htmId = 12683;
	level = 5;

	// std::cout << 100 << std::endl << std::flush;
	aIndex = EmbeddedLevelNameEncoding(BitShiftNameEncoding(htmId).leftJustifiedId()).getSciDBLeftJustifiedFormat();
	// std::cout << 110 << std::endl << std::flush;
	STARE_ArrayIndexSpatialValues neighbors_found = index.NeighborsOfValue(aIndex);
	// std::cout << 120 << std::endl << std::flush;

	uint64 neighbors_htmid_expected[12]	= { 12685, 12687, 12684, 12725, 12727, 12724, 13188, 13191, 13190, 12680, 12681, 12682 };
	uint64 neighbors_expected[12];
	// A little legacy voodoo.
	for( int i = 0; i < 12; ++i ) {
		// std::cout << 120+i << std::endl << std::flush;
		uint64 htmId_i = neighbors_htmid_expected[i];
		neighbors_expected[i] = EmbeddedLevelNameEncoding(BitShiftNameEncoding(htmId_i).leftJustifiedId()).getSciDBLeftJustifiedFormat();
	}
	// std::cout << 200 << std::endl << std::flush;

	string neighbor_test_messages[12] =
	{
			"Neighbor across vertex 1,13",
			"Neighbor across vertex 1",
			"Neighbor across vertex 1,12",
			"Neighbor across vertex 2,12",
			"Neighbor across vertex 2",
			"Neighbor across vertex 2,23",
			"Neighbor across vertex 3,23",
			"Neighbor across vertex 3",
			"Neighbor across vertex 3,13",
			"Neighbor face 23",
			"Neighbor face 13",
			"Neighbor face 12"
	};

	// std::cout << 210 << std::endl << std::flush;

	for( int i = 0; i < 12; ++i ) {
		// std::cout << 220+i << std::endl << std::flush;
		ASSERT_EQUALM(neighbor_test_messages[i],neighbors_expected[i],neighbors_found[i]);
	}

	/*
	ASSERT_EQUALM("Neighbor face 23",12680,neighbors[0]);
	ASSERT_EQUALM("Neighbor face 13",12681,neighbors[1]);
	ASSERT_EQUALM("Neighbor face 12",12682,neighbors[2]);

	ASSERT_EQUALM("Neighbor across vertex 1,13",12685,neighborsV[0]);
	ASSERT_EQUALM("Neighbor across vertex 1",   12687,neighborsV[1]);
	ASSERT_EQUALM("Neighbor across vertex 1,12",12684,neighborsV[2]);

	ASSERT_EQUALM("Neighbor across vertex 2,12",12725,neighborsV[3]);
	ASSERT_EQUALM("Neighbor across vertex 2",   12727,neighborsV[4]);
	ASSERT_EQUALM("Neighbor across vertex 2,23",12724,neighborsV[5]);

	ASSERT_EQUALM("Neighbor across vertex 3,23",13188,neighborsV[6]);
	ASSERT_EQUALM("Neighbor across vertex 3",   13191,neighborsV[7]);
	ASSERT_EQUALM("Neighbor across vertex 3,13",13190,neighborsV[8]);

	index.getIndex(level).NeighborsAcrossEdgesFromHtmId( neighbors, htmId);
	index.

	htmId = 12683;
	uint64 neighborsV[9];
	index.NeighborsAcrossVerticesFromHtmId( neighborsV, htmId);

	*/

	// FAIL();
}

