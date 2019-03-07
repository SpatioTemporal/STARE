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
	STARE index1(index.getSearchLevel(),index.getBuildLevel(),index.getRotation());

//	ASSERT_EQUAL(1,index.ValueFromLatLonDegrees(45.0, 45.0));

	LatLonDegrees64 latlon0(45.0,45.0);
	// LatLon latlon0 = {.lat = 45.0, .lon = 45.0 };
	// cout << "latlon0: " << latlon0.lat << " " << latlon0.lon << endl;

	/// TODO Need multiple indexes to handle array index values with different resolution levels.
	STARE_ArrayIndexSpatialValue aIndex  = index.ValueFromLatLonDegrees(latlon0.lat,latlon0.lon,8);
	/*
	cout << "aIndex:  " << hex << aIndex << dec << endl;
	*/
	STARE_ArrayIndexSpatialValue aIndex1 = index1.ValueFromLatLonDegrees(latlon0.lat,latlon0.lon,8);
	/*
	cout << "aIndex1: " << hex << aIndex1 << dec << endl;
	cout << "Resolution level tr0: " << index.ResolutionLevelFromValue(aIndex) << endl;
	*/
	ASSERT_EQUALM("Compare default and specified constructors",aIndex,aIndex1);

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

	/* */
	for( int i = 0; i < 12; ++i ) {
		// std::cout << 220+i << std::endl << std::flush;
		ASSERT_EQUALM(neighbor_test_messages[i],neighbors_expected[i],neighbors_found[i]);

		if(false) {
			Triangle tr0 = index.TriangleFromValue(neighbors_found[i], 5);
			cout << "--" << endl << flush;
			for(int j = 0; j < 3; ++j ) {
				cout << j
						<< " tr0.v: "
						<< setprecision(17)
						<< setw(20)
						<< scientific
						<< tr0.vertices[j] << endl << flush;
			}
			cout << "c" << " tr0.c: " << tr0.centroid << endl << flush;
		}
	}
	/* */

	/*
	for(int level=0; level<28; ++level) {
		cout << level << " l,scale(m) "<< index.lengthMeterScaleFromEdgeFromLevel(level) << endl << flush;
	}
	*/

	if( true ){
		SpatialVector axis     = 0.5*xhat + 0.5*yhat; axis.normalize();
		float64       theta    = 0.25*gPi;
		// theta = 0.0;
		// theta    = 0.125*gPi;
		// theta    = 0.5*gPi + 0.00001;
		theta    = 0.5*gPi;
		SpatialRotation rotate_root_octahedron = SpatialRotation(axis,theta);
		int search_level = 27, build_level = 5;
		STARE index = STARE(search_level, build_level, rotate_root_octahedron);

		// STARE index;

		cout << ".." << endl << flush;

		int level = 6;

		float64 lat0 = 90, lon0 = 0.0;
		STARE_ArrayIndexSpatialValue north_pole_sid = index.ValueFromLatLonDegrees(lat0,lon0,level);

		// STARE_ArrayIndexSpatialValues neighbors;
		STARE_ArrayIndexSpatialValues neighbors = index.NeighborsOfValue(north_pole_sid);

		SpatialIndex sIndex = index.getIndex(level);

		uint64 np_htmid = index.htmIDFromValue(north_pole_sid, level);

		SpatialVector workspace_ev[18];
		uint64 neighbors_e[3];
		sIndex.NeighborsAcrossEdgesFromHtmId(neighbors_e, np_htmid, workspace_ev);

		uint64 neighbors_v[9];
		sIndex.NeighborsAcrossVerticesFromEdges(neighbors_v, neighbors_e, np_htmid, workspace_ev);

		uint64 neighbors_[12];
		for(int i=0; i<9; ++i) {
			neighbors_[i] = neighbors_v[i];
		}
		for(int i=9; i<12; ++i) {
			neighbors_[i] = neighbors_e[i-9];
		}

		cout << 90 << endl << flush;

		cout << endl << flush;
		for(int i=3; i<6; ++i) {
			cout << i << " m "
					<< setprecision(17)
					<< setw(20)
					<< scientific
					<< workspace_ev[i] << endl << flush;
		}

		cout << endl << flush;

		cout << 0 << " q "
				<< setprecision(17)
				<< setw(20)
				<< scientific
				<< workspace_ev[9+3] << " -- " << workspace_ev[8] << endl << flush;


		cout << endl << flush;
		for( int i = 0; i < 12; ++i ) {
			cout << i << " neighbors_ 0x"<< hex << neighbors_[i] << dec << endl << flush;
		}

		cout << endl << flush;
		for( int i = 0; i < 12; ++i ) {
			cout << i << " neighbors 0x"<< hex << neighbors[i] << dec << endl << flush;
		}

		level = index.ResolutionLevelFromValue(north_pole_sid);
		Triangle ta[12];
		for(int i=0; i<12; ++i) {
			ta[i] = index.TriangleFromValue(neighbors[i],level);
		}
		cout << ".." << endl << flush;
		// for(int i = 8; i >= 0; --i ) {
		// for(int i = 0; i < 9; ++i ) {
		// for(int i = 4; i < 8; ++i ) {
		for(int i = 0; i < 12; ++i ) {
			Triangle tr0 = ta[i];
			cout << "-- neighbor = " << i << " --" << endl << flush;
			for(int j = 0; j < 3; ++j ) {
				cout << j
						<< " tr0.v: "
						<< setprecision(17)
						<< setw(20)
						<< scientific
						<< tr0.vertices[j]
										<< " - "
										<< hex << neighbors[i] << dec
										<< endl << flush;
			}
			cout << "c" << " tr0.c: " << tr0.centroid << endl << flush;

		}
		cout << ".." << endl << flush;
		stringstream ss;
		for(int i=0; i<12; ++i) {
			for(int j=0; j <= i; ++j ) {
				cout << i << "," << j << " delta = "
						<< (ta[i].centroid-ta[j].centroid) << ", length = "
						<< (ta[i].centroid-ta[j].centroid).length()
						<< endl << flush;
				float64 delta = (ta[i].centroid-ta[j].centroid).length();
				ss.clear();
				ss.str(string());
				if( i == j ) {
					ss << "neighbor-neighbor check i == j for " << i << "," << j ;
					ASSERT_EQUAL_DELTAM(ss.str().c_str(),0,delta,tol);
				} else {
					ss << "neighbor-neighbor check i != j for " << i << "," << j ;
					ASSERT_GREATERM(ss.str().c_str(),delta,0);
				}
			}
		}


	}


	/* */

	// FAIL();
}

