/*
 * STARE_test.cpp
 *
 *  Created on: Jan 31, 2019
 *      Author: mrilee
 *
 *  Copyright (C) 2019 Rilee Systems Technologies LLC
 */

#include <iostream>
#include "Test.h"

#include "SpatialInterface.h"

#ifndef DIAG
#define DIAG1(expr)
#define DIAGOUT2(out,expr)
#define DIAGOUTDELTA(out,a,b)
#else
#define DIAG1(expr) expr;
#define DIAGOUT2(out,expr) out << expr;
#define DIAGOUTDELTA(out,a,b) {SpatialVector delta_ = a-b; cout << delta_.length() << " ";}
#endif

LatLonDegrees64ValueVector makeCornerVector(void){
	LatLonDegrees64ValueVector cornerVector;
	cornerVector.push_back(LatLonDegrees64(35.65007233330923, 61.21081709172574));
	cornerVector.push_back(LatLonDegrees64(35.27066396742229, 62.23065148300589));
	cornerVector.push_back(LatLonDegrees64(37.66116404881207, 65.74563073106683));
	cornerVector.push_back(LatLonDegrees64(38.25890534113216, 71.34813113799026));
	cornerVector.push_back(LatLonDegrees64(36.72000702569632, 72.92002485544447));
	cornerVector.push_back(LatLonDegrees64(31.90141225842444, 69.31776411324256));
	cornerVector.push_back(LatLonDegrees64(31.62018911389207, 68.92667687365767));
	cornerVector.push_back(LatLonDegrees64(29.88794342703618, 66.34647260932442));
	cornerVector.push_back(LatLonDegrees64(29.56003062592809, 65.04686201361611));
	cornerVector.push_back(LatLonDegrees64(29.34081920014597, 64.14800215033125));
	cornerVector.push_back(LatLonDegrees64(29.46833079682617, 63.55026085801117));
	cornerVector.push_back(LatLonDegrees64(31.37950613049267, 61.69931440618083));
	cornerVector.push_back(LatLonDegrees64(34.40410187431986, 60.80319339380745));

	cornerVector.push_back(LatLonDegrees64(33.52883230237626, 60.96370039250601 ));

	cornerVector.push_back(LatLonDegrees64(35.65007233330923, 61.21081709172574));
	return cornerVector;
}

void STARE_test() {

	SpatialVector
		xhat(1.0,0.0,0.0),
		yhat(0.0,1.0,0.0),
		zhat(0.0,0.0,1.0);

	bool globalPrintFlag = false;

	STARE index;
	STARE index1(index.getSearchLevel(),index.getBuildLevel(),index.getRotation());
	STARE index2(index.getSearchLevel(),index.getBuildLevel());

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
	STARE_ArrayIndexSpatialValue aIndex2 = index2.ValueFromLatLonDegrees(latlon0.lat,latlon0.lon,8);
	/*
	cout << "aIndex1: " << hex << aIndex1 << dec << endl;
	cout << "Resolution level tr0: " << index.ResolutionLevelFromValue(aIndex) << endl;
	*/
	ASSERT_EQUALM("Compare default and specified constructors",aIndex,aIndex1);

	LatLonDegrees64 latlon1 = index.LatLonDegreesFromValue(aIndex1);
	LatLonDegrees64 latlon2 = index.LatLonDegreesFromValue(aIndex2);
	// cout << "latlon1: " << latlon1.lat << " " << latlon1.lon << endl;

	// float64 tol = 1.0e-14; /// What's our floating point error?
	float64 tol = 1.0e-6; /// What's our floating point error? Grumble.
	/// 1 degree ~ 111km, so 1.0e-6 * 1 degree ~ 0.1 m.
	// cout << setprecision(17);
	// cout << "delta-lat: " << latlon1.lat - latlon0.lat << endl;
	// cout << "delta-lon: " << latlon1.lon - latlon0.lon << endl;
	ASSERT_EQUAL_DELTAM("Round trip lat <-> sIndex",latlon0.lat,latlon1.lat,tol);
	ASSERT_EQUAL_DELTAM("Round trip lon <-> sIndex",latlon0.lon,latlon1.lon,tol);
	ASSERT_EQUAL_DELTAM("Round trip lat <-> sIndex",latlon0.lat,latlon2.lat,tol);
	ASSERT_EQUAL_DELTAM("Round trip lon <-> sIndex",latlon0.lon,latlon2.lon,tol);

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

	uint64 neighbors_htmid_expected[12]	= { 12680, 12681, 12682, 12685, 12687, 12684, 12725, 12727, 12724, 13188, 13191, 13190  };
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

		// cout << ".." << endl << flush;

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

		/*
		uint64 neighbors_[12];
		for(int i=0; i<9; ++i) {
			neighbors_[i] = neighbors_v[i];
		}
		for(int i=9; i<12; ++i) {
			neighbors_[i] = neighbors_e[i-9];
		}
		*/

		// cout << 90 << endl << flush;

		/* Diagnostic output
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
		 */


		level = index.ResolutionLevelFromValue(north_pole_sid);
		Triangle ta[12];
		for(int i=0; i<12; ++i) {
			ta[i] = index.TriangleFromValue(neighbors[i],level);
		}
		// cout << ".." << endl << flush;
		// for(int i = 8; i >= 0; --i ) {
		// for(int i = 0; i < 9; ++i ) {
		// for(int i = 4; i < 8; ++i ) {
		for(int i = 0; i < 12; ++i ) {
			Triangle tr0 = ta[i];
			/*
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
			 */
		}
		// cout << ".." << endl << flush;
		stringstream ss;
		for(int i=0; i<12; ++i) {
			for(int j=0; j <= i; ++j ) {
				/*
				cout << i << "," << j << " delta = "
						<< (ta[i].centroid-ta[j].centroid) << ", length = "
						<< (ta[i].centroid-ta[j].centroid).length()
						<< endl << flush;
				 */
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

	{

//		LatLonDegrees64 latlon1(
//				3.25734748840332031e+01,-1.00050041198730469e+02
//		);

		/* Bug exploration. points on edges.
		LatLonDegrees64 latlon1(
				3.23248138427734375e+01,-1.02840789794921875e+02
		);

		// LatLonDegrees64 latlon1(32.5735,-100.05);
		// STARE index1;
		STARE index2(index.getSearchLevel(),index.getBuildLevel(),index.getRotation());
		STARE_ArrayIndexSpatialValue idx = index2.ValueFromLatLonDegrees(latlon1.lat,latlon1.lon);

		cout << "latlon1: "
				<< setprecision(17)
				<< setw(20)
				<< scientific
				<< latlon1.lat << "," << latlon1.lon << " idx = " << idx << endl << flush;

				*/
	}


	if(false) { // Try a lookup at a number of levels to verify center latlon vs. geolocation...
		LatLonDegrees64 latlon1(32.5735,-100.05);
		STARE index2;
		// STARE index2(index.getSearchLevel(),index.getBuildLevel(),index.getRotation());
		STARE_ArrayIndexSpatialValue idx = index2.ValueFromLatLonDegrees(latlon1.lat,latlon1.lon);

		cout << "latlon1: "
				<< setprecision(18)
				<< setw(20)
				<< scientific
				<< latlon1.lat << "," << latlon1.lon << " idx = " << hex << idx << dec << " rLevel = default = 27 " << endl << flush;

		LatLonDegrees64 latlon2 = index2.LatLonDegreesFromValue(idx);
		cout << "latlon2: "
				<< setprecision(18)
				<< setw(20)
				<< scientific
				<< latlon2.lat << "," << latlon2.lon-360
				<< endl << flush;

		Triangle tr = index2.TriangleFromValue(idx);
		float64 tLat,tLon;
		bool ok = tr.centroid.getLatLonDegrees(tLat, tLon);
		cout << "latlonT: "
				<< setprecision(18)
				<< setw(20)
				<< scientific
				<<  tLat << "," << tLon-360
				<< endl << flush;
		/* --- */

		int rLevel = 20;
		idx = index2.ValueFromLatLonDegrees(latlon1.lat,latlon1.lon,rLevel);

		cout << "latlon1: "
				<< setprecision(18)
				<< setw(20)
				<< scientific
				<< latlon1.lat << "," << latlon1.lon << " idx = " << hex << idx << dec << " rLevel = " << rLevel << endl << flush;

		latlon2 = index2.LatLonDegreesFromValue(idx);
		cout << "latlon2: "
				<< setprecision(18)
				<< setw(20)
				<< scientific
				<< latlon2.lat << "," << latlon2.lon-360
				<< endl << flush;

		tr = index2.TriangleFromValue(idx);
		ok = tr.centroid.getLatLonDegrees(tLat, tLon);
				cout << "latlonT: "
						<< setprecision(18)
						<< setw(20)
						<< scientific
						<<  tLat << "," << tLon-360
						<< endl << flush;

		if(false) {
			cout << ok;
		}

		/* --- */

		rLevel = 26;
		idx = index2.ValueFromLatLonDegrees(latlon1.lat,latlon1.lon,rLevel);

		cout << "latlon1: "
				<< setprecision(18)
				<< setw(20)
				<< scientific
				<< latlon1.lat << "," << latlon1.lon << " idx = " << hex << idx << dec << " rLevel = " << rLevel << endl << flush;

		latlon2 = index2.LatLonDegreesFromValue(idx);
		cout << "latlon2: "
				<< setprecision(18)
				<< setw(20)
				<< scientific
				<< latlon2.lat << "," << latlon2.lon-360
				<< endl << flush;

		tr = index2.TriangleFromValue(idx);
		ok = tr.centroid.getLatLonDegrees(tLat, tLon);
				cout << "latlonT: "
						<< setprecision(18)
						<< setw(20)
						<< scientific
						<<  tLat << "," << tLon-360
						<< endl << flush;

		/* --- */

				rLevel = 5;
				idx = index2.ValueFromLatLonDegrees(latlon1.lat,latlon1.lon,rLevel);

				cout << "latlon1: "
						<< setprecision(18)
						<< setw(20)
						<< scientific
						<< latlon1.lat << "," << latlon1.lon << " idx = " << hex << idx << dec << " rLevel = " << rLevel << endl << flush;

				latlon2 = index2.LatLonDegreesFromValue(idx);
				cout << "latlon2: "
						<< setprecision(18)
						<< setw(20)
						<< scientific
						<< latlon2.lat << "," << latlon2.lon-360
						<< endl << flush;

				tr = index2.TriangleFromValue(idx);
				ok = tr.centroid.getLatLonDegrees(tLat, tLon);
						cout << "latlonT: "
								<< setprecision(18)
								<< setw(20)
								<< scientific
								<<  tLat << "," << tLon-360
								<< endl << flush;

				/* --- */
	}

	// STARE index;

	/* */

	index.tIndex.fromFormattedJulianTAI(2001, 6, 30, 23, 59, 59, 999).set_resolution(10).set_type(2);
	if(globalPrintFlag) {
	cout
	<< "tI: " << index.tIndex.toStringJulianTAI()
	<< "  : 0x" << hex << index.getArrayIndexTemporalValue() << dec
	<< ", " << index.getArrayIndexTemporalValue()
	<< endl << flush;
	}
	ASSERT_EQUAL("+000002001-06-1-5 23:59:59.999 (10) (2)",index.tIndex.stringInNativeDate());
	ASSERT_EQUAL("1 000002001-06-30 23:59:59.999 (10) (2)",index.tIndex.toStringJulianTAI());
	ASSERT_EQUAL(0x7d166dfbefe72a,index.getArrayIndexTemporalValue());

	index.tIndex.fromFormattedJulianTAI(2001, 6, 30, 23, 59, 59, 999).set_resolution(20).set_type(2);
	if(globalPrintFlag) {
	cout
	<< "tI: " << index.tIndex.toStringJulianTAI()
	<< "  : 0x" << hex << index.getArrayIndexTemporalValue() << dec
	<< ", " << index.getArrayIndexTemporalValue()
	<< endl << flush;
	}
	ASSERT_EQUAL("+000002001-06-1-5 23:59:59.999 (20) (2)",index.tIndex.stringInNativeDate());
	ASSERT_EQUAL("1 000002001-06-30 23:59:59.999 (20) (2)",index.tIndex.toStringJulianTAI());
	ASSERT_EQUAL(0x7d166dfbefe752,index.getArrayIndexTemporalValue());

	index1.tIndex.fromFormattedJulianTAI(2001, 7, 1, 0, 0, 0, 0).set_resolution(20).set_type(2);
	if(globalPrintFlag) {
	cout
	<< "tI: " << index1.tIndex.toStringJulianTAI()
	<< "  : 0x" << hex << index1.getArrayIndexTemporalValue() << dec
	<< ", " << index1.getArrayIndexTemporalValue()
	<< endl << flush;
	}
	ASSERT_EQUAL("+000002001-06-1-6 00:00:00.000 (20) (2)",index1.tIndex.stringInNativeDate());
	ASSERT_EQUAL("1 000002001-07-01 00:00:00.000 (20) (2)",index1.tIndex.toStringJulianTAI());
	ASSERT_EQUAL(0x7d167000000052,index1.getArrayIndexTemporalValue());

	{

		bool printFlag = false;
		double delta = diff_JulianTAIDays(index.tIndex,index1.tIndex);

		if(printFlag) {
			cout
			<< "cmp: " << (index.tIndex == index1.tIndex)
			<< endl << flush
			<< "delta: " << delta << ", ms: " << delta*86400.0e3 << endl << flush;
			cout << "+++" << endl << flush;
		}
		ASSERT_NOT_EQUAL_TO(index.tIndex,index1.tIndex);

		index.tIndex.fromFormattedJulianTAI(2001, 5, 30, 12, 59, 59, 999).set_resolution(22).set_type(2);
		index1.setArrayIndexTemporalValue(index.getArrayIndexTemporalValue());
		ASSERT_EQUAL(index.tIndex,index1.tIndex);

		if(printFlag) {
			cout
			<< "cmp: " << (index.tIndex == index1.tIndex)
			<< endl << flush;
			cout
			<< "tI0: " << index.tIndex.toStringJulianTAI()
			<< " : 0x" << hex << index.getArrayIndexTemporalValue() << dec
			<< ", " << index.getArrayIndexTemporalValue()
			<< endl << flush;

			cout
			<< "tI1: " << index1.tIndex.toStringJulianTAI()
			<< " : 0x" << hex << index1.getArrayIndexTemporalValue() << dec
			<< ", " << index1.getArrayIndexTemporalValue()
			<< endl << flush;
			cout << "+++" << endl << flush;
			cout << "cmp " << index.cmpTemporalAtResolution(index1.getArrayIndexTemporalValue()) << endl << flush;
		}
		ASSERT(index.cmpTemporalAtResolution(index1.getArrayIndexTemporalValue()));

		double jd0; int64_t id0;
		jd0 = index.toJulianDayTAI();
		id0 = index.getArrayIndexTemporalValue();
		if(printFlag) {
			cout << "+++" << endl << flush;
			cout << index.tIndex.toStringJulianTAI() << endl << flush;

			cout
			<< "tI-JD-TAI-0: "
			<< setprecision(18) << setw(20) << scientific
			<< jd0 << endl << flush;
		}
		ASSERT_EQUAL("1 000002001-05-30 12:59:59.999 (22) (2)",index.tIndex.toStringJulianTAI());

		double jd1 = jd0 + 1.0;
		index.fromJulianDayTAI(jd1);
		if(printFlag) {
			cout << index.tIndex.toStringJulianTAI() << endl << flush;
		}
		ASSERT_EQUAL("1 000002001-05-31 12:59:59.999 (22) (2)",index.tIndex.toStringJulianTAI());

		jd1 = -1;
		jd1 = index.toJulianDayTAI();
		int64_t id1 = index.getArrayIndexTemporalValue();
		if(printFlag) {
			cout
			<< "tI-JD-TAI-1: "
			<< setprecision(18) << setw(20) << scientific
			<< jd0 << endl << flush;
			cout << index.tIndex.toStringJulianTAI() << endl << flush;
		}
		ASSERT_EQUAL("1 000002001-05-31 12:59:59.999 (22) (2)",index.tIndex.toStringJulianTAI());

		if(printFlag) {
			cout << "cmp-0.5:   " << cmpTemporalAtResolution3(id0,id1,0.5) << endl << flush;
			cout << "cmp-1.0:   " << cmpTemporalAtResolution3(id0,id1,1.0) << endl << flush;
			cout << "cmp-1.0+e: " << cmpTemporalAtResolution3(id0,id1,1.001) << endl << flush;
			cout << "cmp-1.5:   " << cmpTemporalAtResolution3(id0,id1,1.5) << endl << flush;
		}
		ASSERT(!cmpTemporalAtResolution3(id0,id1,0.5));
		ASSERT(!cmpTemporalAtResolution3(id0,id1,1.0));
		ASSERT(cmpTemporalAtResolution3(id0,id1,1.001));
		ASSERT(cmpTemporalAtResolution3(id0,id1,1.5));
	}

	if(false) {
		// Analyze transformations supporting datetime interface and pystare.
		// Turns out we were passing ms into pystare, not seconds, so the
		// problem was mis-calling the datetime call in the STARE interface.

		// index.tIndex.fromFormattedJulianTAI(2001, 6, 30, 23, 59, 59, 999).set_resolution(10).set_type(2);
		cout << endl << "1970-01-01 Direct set." << endl << flush;
		index.tIndex.fromUTC(1970, 1, 1, 0, 0, 0, 0);
		// if(globalPrintFlag) {
		cout << "tI: jd:    " << index.tIndex.toStringJulianTAI() << endl << flush;
		cout << "tI: nd:    " << index.tIndex.stringInNativeDate() << endl << flush;
		// cout << "tI: utcjd: " << index.tIndex. ;
		// }

		int year,month,dom,hr,min,sec,msec;
		index.tIndex.toUTC(year,month,dom,hr,min,sec,msec);
		// if(globalPrintFlag) {
		cout << "tI: utc: "
				<< year << " "
				<< month << " "
				<< dom << " "
				<< hr << " "
				<< min << " "
				<< sec << " "
				<< msec
				<< endl << flush;
		cout << setprecision(16);
		cout << "tI: jdTAI: " << index.toJulianDayTAI() << endl << flush;
		cout << "tI: jdUTC: " << index.toJulianDayUTC() << endl << flush;
		// }
		time_t datetime = 0;
		int resolution = 10;
		int tValType       = 2;
		STARE_ArrayIndexTemporalValue tVal = index.ValueFromUTC(datetime, resolution, tValType);
		// if(globalPrintFlag) {
		cout << endl << "time_t datetime=0 Set through STARE." << endl << flush;
		cout << "tI: jd:    " << index.tIndex.toStringJulianTAI() << endl << flush;
		cout << "tI: nd:    " << index.tIndex.stringInNativeDate() << endl << flush;
		// cout << "tI: utcjd: " << index.tIndex. ;
		// }

		index.tIndex.toUTC(year,month,dom,hr,min,sec,msec);
		// if(globalPrintFlag) {
		cout << "tI: utc: "
				<< year << " "
				<< month << " "
				<< dom << " "
				<< hr << " "
				<< min << " "
				<< sec << " "
				<< msec
				<< endl << flush;
		cout << setprecision(16);
		cout << "tI: jdTAI: " << index.toJulianDayTAI() << endl << flush;
		cout << "tI: jdUTC: " << index.toJulianDayUTC() << endl << flush;
		// }

		{
			cout << endl << "2000-01-01 Direct set." << endl << flush;
			index.tIndex.fromUTC(2000, 1, 1, 0, 0, 0, 0);
			// if(globalPrintFlag) {
			cout << "tI: jd:    " << index.tIndex.toStringJulianTAI() << endl << flush;
			cout << "tI: nd:    " << index.tIndex.stringInNativeDate() << endl << flush;
			// cout << "tI: utcjd: " << index.tIndex. ;
			// }

			int year,month,dom,hr,min,sec,msec;
			index.tIndex.toUTC(year,month,dom,hr,min,sec,msec);
			// if(globalPrintFlag) {
			cout << "tI: utc: "
					<< year << " "
					<< month << " "
					<< dom << " "
					<< hr << " "
					<< min << " "
					<< sec << " "
					<< msec
					<< endl << flush;
			cout << setprecision(16);
			cout << "tI: jdTAI: " << index.toJulianDayTAI() << endl << flush;
			cout << "tI: jdUTC: " << index.toJulianDayUTC() << endl << flush;
			// }
			time_t datetime = (30*365+7)*86400;
			int resolution = 10;
			int tValType       = 2;
			STARE_ArrayIndexTemporalValue tVal = index.ValueFromUTC(datetime, resolution, tValType);
			// if(globalPrintFlag) {
			cout << endl << "time_t datetime=2000 Set through STARE." << endl << flush;
			cout << "tI: jd:    " << index.tIndex.toStringJulianTAI() << endl << flush;
			cout << "tI: nd:    " << index.tIndex.stringInNativeDate() << endl << flush;
			// cout << "tI: utcjd: " << index.tIndex. ;
			// }

			index.tIndex.toUTC(year,month,dom,hr,min,sec,msec);
			// if(globalPrintFlag) {
			cout << "tI: utc: "
					<< year << " "
					<< month << " "
					<< dom << " "
					<< hr << " "
					<< min << " "
					<< sec << " "
					<< msec
					<< endl << flush;
			cout << setprecision(16);
			cout << "tI: jdTAI: " << index.toJulianDayTAI() << endl << flush;
			cout << "tI: jdUTC: " << index.toJulianDayUTC() << endl << flush;
			// }
		}

		{
			cout << endl << "PySTARE debugging" << endl << flush;
			STARE stare;
			int resolution = 10;
			int type = 2;
			time_t datetime = (30*365+7)*86400;
			// int64_t datetime = (30*365+7)*86400;
			STARE_ArrayIndexTemporalValue idx = stare.ValueFromUTC(datetime, resolution, type);
			stare.setArrayIndexTemporalValue(idx);
			double jd19700101_erfa = 2440587.5;
			double jd = stare.toJulianDayUTC();
			double jdt = stare.toJulianDayTAI();
			double delta = jd-jd19700101_erfa;
			int64_t iDelta = delta*86400.0; // sec now.

			cout << setprecision(16)
							 << " jd,jdt,delta,iDelta "
							 << hex << setw(16) << setfill('0')
							 << idx << " "
							 << dec
							 << setw(20) << setfill(' ') << scientific
							 << jd << " "
							 << setw(20) << setfill(' ') << scientific
							 << jdt << " "
							 << setw(20) << setfill(' ') << scientific
							 << delta << " "
							 << setw(20) << setfill(' ') << scientific
							 << iDelta << endl << flush;
		}

		cout << endl;
		year = 1970; month = 1; dom = 1;
		double djm0,djm;
		int not_ok = eraCal2jd(year,month,dom,&djm0,&djm);
		// if(globalPrintFlag) {
		cout << "1970 erfa jd:   " << djm0+djm << endl << flush;
		// }

		year = 2000; month = 1; dom = 1;
		not_ok = eraCal2jd(year,month,dom,&djm0,&djm);
		// if(globalPrintFlag) {
		cout << "2000 erfa jd:   " << djm0+djm << endl << flush;
		// }

	}


	/*
	 * Review the question the question about the sign of the spatial index.
	 *
	 * The spatial index value is positive due to HTM's legacy skiplist expecting
	 * positive values.
	 *
	 */
	if(false)
	{
		// From Python [-45.] [135.] [720575940379279476], level = 20.
		// double lat_mn = 30.0, lon_mn=9.8, idx_mn = 1.0e+99;

		double
		lat_mn =-2.999999965649656630e+01,
		lon_mn = 9.735610352877271723e+00,
		idx_mn = 1.0e+99;

		double lat, lon;
		int level = 0;
		bool printFlag = false;
		// lat = -45.0; lon = 135.0;
		lat = -45.0; lon = 135.0;
		// for( lon = 120.0; lon <= 150.0; lon += 0.1 ) {
		// for( lat = -55.0; lat <= -35.0; lat += 0.1 ) {
		double dl    = 1.0;
		double scale = dl/10.0;
		for( lon = lon_mn-dl; lon <= lon_mn+dl; lon += scale ) {
			for( lat = lat_mn-dl; lat <= lat_mn+dl; lat += scale ) {
				//		for( lon = 0.0; lon <= 360.0; lon += 0.1 ) {
				//			for( lat = -90.0; lat <= 90.0; lat += 0.1 ) {
				//		for( lon = 0.0; lon <= 360.0; lon += 1 ) {
				//			for( lat = -90.0; lat <= 90.0; lat += 1 ) {
				//		for( lon = 0.0; lon <= 20.0; lon += 0.1 ) {
				//			for( lat = -40.0; lat <= -20.0; lat += 0.1 ) {
				LatLonDegrees64 latlon0(lat,lon);
				// LatLonDegrees64 latlon0(-45.0,135.0);
				// LatLon latlon0 = {.lat = 45.0, .lon = 45.0 };
				if (printFlag) {
					cout << " latlon0: " << latlon0.lat << " " << latlon0.lon;
				}
				// << endl << flush;
				/// TODO Need multiple indexes to handle array index values with different resolution levels.
				// STARE_ArrayIndexSpatialValue aIndex  = index.ValueFromLatLonDegrees(latlon0.lat,latlon0.lon,20);
				STARE_ArrayIndexSpatialValue aIndex  = index.ValueFromLatLonDegrees(latlon0.lat,latlon0.lon,level);
				if (printFlag) {
					cout << " aIndex:  " << hex << setw(18) << aIndex << dec << " " << setw(18) << aIndex;
					// cout << "         " << hex << 720575940379279476LL << dec << " " << 720575940379279476LL << endl << flush;
					// cout << endl << flush;
				}
				if(aIndex < idx_mn) {
					idx_mn = aIndex;
					lat_mn = lat;
					lon_mn = lon;
					if (printFlag) {
						cout << " * ";
					}
				}
				if (printFlag) {
					cout << endl << flush;
				}
			}
		}
		cout << " lt,ln,idx mn: " << lat_mn << " " << lon_mn << " "
				<< hex << setw(18)
				<< aIndex
				<< " " << dec << setw(18)
				<< aIndex
				<< flush << endl;
		cout << endl << flush;
	}

	if(false){
		/*
		 * Make some example values for Kodi (OPeNDAP).
		STARE indices for coordinates such as:  0˚, 0˚
		                                0˚, 180˚
		                                -90˚, -180˚ (or 360˚)
		 */
		double lat, lon, level;
		STARE_ArrayIndexSpatialValue id;

#define LATLONID(lat,lon,id) cout << "lat,lon,id: " \
		<< setw(4) << setfill(' ') << dec << lat << " " \
		<< setw(4) << setfill(' ') << dec << lon << " " \
		<< "0x" << setw(16) << setfill('0') << hex << id << dec \
		<< endl << flush;
		lat = 0.0; lon = -90.0; level = 8; id = index.ValueFromLatLonDegrees(lat, lon, level); LATLONID(lat,lon,id);
		lat = 0.0; lon = 360.0; level = 8; id = index.ValueFromLatLonDegrees(lat, lon, level); LATLONID(lat,lon,id);
		lat = 0.0; lon = 0.0; level = 8; id = index.ValueFromLatLonDegrees(lat, lon, level); LATLONID(lat,lon,id);
		lat = 0.0; lon = 180.0; level = 8; id = index.ValueFromLatLonDegrees(lat, lon, level); LATLONID(lat,lon,id);
		lat = -90.0; lon = -180.0; level = 8; id = index.ValueFromLatLonDegrees(lat, lon, level); LATLONID(lat,lon,id);
		lat = -90.0; lon = 360.0; level = 8; id = index.ValueFromLatLonDegrees(lat, lon, level); LATLONID(lat,lon,id);
#undef LATLONID
	}

	if(true) {
		/*
	All with level = 8.

	#starting STARE_test

	lat,lon,id:    0  -90 0x28fe71a91bda2428

	lat,lon,id:    0  360 0x3d7e69d09dbc4248

	lat,lon,id:    0    0 0x3d7e69d09dbc4248

	lat,lon,id:    0  180 0x10fe71a91bda2428

	lat,lon,id:  -90 -180 0x1fbff9ff807819e8

	lat,lon,id:  -90  360 0x1fbff9ff807819e8

	#success STARE_test OK
		 */
		double lat, lon, level;
		STARE_ArrayIndexSpatialValue id;
		stringstream ss;
#define SS_LATLONID(lat,lon,id) \
		ss.clear(); ss.str(string()); \
		ss << "lat,lon,id: " \
		<< setw(4) << setfill(' ') << dec << lat << " " \
		<< setw(4) << setfill(' ') << dec << lon << " " \
		<< "0x" << setw(16) << setfill('0') << hex << id << dec;
		lat = 0.0; lon = -90.0; level = 8; id = index.ValueFromLatLonDegrees(lat, lon, level); SS_LATLONID(lat,lon,id); ASSERT_EQUALM(ss.str().c_str(),0x28fe71a91bda2428,id);
		lat = 0.0; lon = 360.0; level = 8; id = index.ValueFromLatLonDegrees(lat, lon, level); SS_LATLONID(lat,lon,id); ASSERT_EQUALM(ss.str().c_str(),0x3d7e69d09dbc4248,id);
		lat = 0.0; lon = 0.0; level = 8; id = index.ValueFromLatLonDegrees(lat, lon, level);   SS_LATLONID(lat,lon,id); ASSERT_EQUALM(ss.str().c_str(),0x3d7e69d09dbc4248,id);
		lat = 0.0; lon = 180.0; level = 8; id = index.ValueFromLatLonDegrees(lat, lon, level); SS_LATLONID(lat,lon,id); ASSERT_EQUALM(ss.str().c_str(),0x10fe71a91bda2428,id);
		lat = -90.0; lon = -180.0; level = 8; id = index.ValueFromLatLonDegrees(lat, lon, level); SS_LATLONID(lat,lon,id); ASSERT_EQUALM(ss.str().c_str(),0x1fbff9ff807819e8,id);
		lat = -90.0; lon = 360.0; level = 8; id = index.ValueFromLatLonDegrees(lat, lon, level); SS_LATLONID(lat,lon,id); ASSERT_EQUALM(ss.str().c_str(),0x1fbff9ff807819e8,id);
#undef SS_LATLONID


	}

	if(false){
		float
		lat =  60.0,
		lon = 123.4;

		int
		level = 20;

		STARE_ArrayIndexSpatialValue id = 0;

		id = index.ValueFromLatLonDegrees( lat, lon, level );

		cout << "id= 0x" << setw(16) << setfill('0') << hex << id;

		unsigned char ch[4];

		cout << "; ";
		cout << " lat= " << dec << lat << " "; // << hex << ch[3] << ch[2] << ch[1] << ch[0] << flush;

		memcpy(ch,&lat,sizeof(float));
		cout << "0x";
		for(int i=3; i>=0; --i) {
			cout
			<< setw(2) << setfill('0')
			<< hex
			<< (int) ch[i];
			// << ".";
		}

		cout << "; ";
		cout << " lon= " << dec << lon << " "; //  << hex << ch[3] << ch[2] << ch[1] << ch[0] << flush;

		memcpy(ch,&lon,sizeof(float));
		cout << "0x";
		for(int i=3; i>=0; --i) {
			cout
			<< setw(2) << setfill('0')
			<< hex
			<< (int) ch[i];
			// << ".";
		}

		cout << "; ";
		cout << dec << " level= " << level;

		cout << endl << flush;

	}

	{
		STARE index;

		if(true) {

			ASSERT_EQUAL(
					-1,
					cmpSpatial(
							index.ValueFromLatLonDegrees( 38.0387, -76.3221, 6 ),
							index.ValueFromLatLonDegrees( 38.0,    -75.0,    4 )));

			ASSERT_EQUAL(
					1,
					cmpSpatial(
							index.ValueFromLatLonDegrees( 40.0, -75.0, 6 ),
							index.ValueFromLatLonDegrees( 40.5, -75.0, 6 )));

			ASSERT_EQUAL(
					0,
					cmpSpatial(
							index.ValueFromLatLonDegrees( 43.5, -75.0, 6 ),
							index.ValueFromLatLonDegrees( 44.0, -74.5, 6 )));

			ASSERT_EQUAL(
					0,
					cmpSpatial(
							index.ValueFromLatLonDegrees( 47.5, -75.5, 6 ),
							index.ValueFromLatLonDegrees( 48.0, -74.5, 7 )));

			ASSERT_EQUAL(
					1,
					cmpSpatial(
							index.ValueFromLatLonDegrees( 39.5, -85.5, 4 ),
							index.ValueFromLatLonDegrees( 39.5, -84.5, 6 )));

		}

		if(false) {
			cout << dec << 10 << " 5 points" << endl << flush;
			LatLonDegrees64ValueVector points;

			points.push_back(LatLonDegrees64( 0, 0));
			points.push_back(LatLonDegrees64( 0,10));
			points.push_back(LatLonDegrees64( 5, 10));
			// points.push_back(LatLonDegrees64(10,10));
			points.push_back(LatLonDegrees64(10, 0));
			// points.push_back(LatLonDegrees64( 5, 0));
			// points.push_back(LatLonDegrees64( 5, 5)); // Completes normally
			// points.push_back(LatLonDegrees64( 5, 5));
			// points.push_back(LatLonDegrees64( 5, 10));
			points.push_back(LatLonDegrees64(10,10));

			int resolution_level = 4;

			cout << dec << 20 << dec << "res level: " << resolution_level << endl << flush;

			try {
				cout << dec << 100 << endl << flush;
				STARE_SpatialIntervals cover_hull = index.ConvexHull(points,resolution_level);
				cout << dec << 110 << endl << flush;
			} catch (const SpatialException & e) {
				cout << dec << 200 << endl << flush;
				cout << "Exception " << e.what() << endl << flush;
			}
			cout << dec << 300 << endl << flush;
		}

		if(false) {
			cout << dec << 10 << endl << flush;
			LatLonDegrees64ValueVector points;

			// points.push_back(LatLonDegrees64(10,10));
			// points.push_back(LatLonDegrees64(20,20));
			// points.push_back(LatLonDegrees64(0,10));

			float64 delta = 10.0;
			//			float64 delta = 1.0;
			//			for(float64 lat = 20; lat < 45; lat += delta ) {
			//				for(float64 lon = -120; lon < -80; lon += delta ) {
			int k=11; // iFuse fail (12 points)
			// int k=10; // iFuse success
			for(float64 lat = 20; lat < 45 && k >= 0; lat += delta ) {
				for(float64 lon = -120; lon < -80 && k-- >= 0 ; lon += delta ) {
					points.push_back(LatLonDegrees64(lat,lon));
				}
			}

			int resolution_level = 4;

			cout << dec << 20 << dec << "res level: " << resolution_level << endl << flush;

			try {
				cout << dec << 100 << endl << flush;
				STARE_SpatialIntervals cover_hull = index.ConvexHull(points,resolution_level);
				cout << dec << 110 << endl << flush;
			} catch (const SpatialException & e) {
				cout << dec << 200 << endl << flush;
				cout << "Exception " << e.what() << endl << flush;
			}
			cout << dec << 300 << endl << flush;
		}

	}

	//	{
	//		STARE_ArrayIndexSpatialValue spatialStareId = 0;
	//		LatLonDegrees64 latlon0 = index.LatLonDegreesFromValue(spatialStareId);
	//		cout << " idx=0, latlon0: " << latlon0.lat << " " << latlon0.lon << endl << flush;
	//	}


	if(false) {
		LatLonDegrees64 latlon(32.5735,-100.05);
		STARE index2;
		int resolution = 1;
		STARE_ArrayIndexSpatialValue idx = index2.ValueFromLatLonDegrees(latlon.lat,latlon.lon,resolution);

		idx = shiftSpatialIdAtLevel( idx, resolution, 1 );

		cout << endl << "base..." << endl << flush;
		cout
		<< setprecision(18)
		<< setw(23)
		<< "  idx = 0x" << hex << idx << dec
		<< scientific
		<< " latlon = "
		<< latlon.lat << "," << latlon.lon
		<< dec << " rLevel = " << resolution
		<< endl << flush;


		cout << endl << "increasing resolution" << endl << flush;
		for(int resolution_ = 0; resolution_ < 12; ++ resolution_) {
			STARE_ArrayIndexSpatialValue idx1 = shiftSpatialIdAtLevel( idx, resolution_, 1 );
			LatLonDegrees64 latlon1 = index2.LatLonDegreesFromValue(idx1);
			int resolution1 = idx1 & spatialLevelMask();

			cout
			<< setprecision(18)
			<< setw(23)
			<< "  idx = 0x" << hex << idx1 << dec
			<< scientific
			<< " latlon = "
			<< latlon1.lat << "," << latlon1.lon
			<< dec << " rLevel = " << resolution1
			<< " resLevel = " << resolution_
			<< endl << flush;
		}

		cout << endl << "decreasing resolution" << endl << flush;
		for(int resolution_ = 0; resolution_ < 12; ++ resolution_) {
			STARE_ArrayIndexSpatialValue idx1 = shiftSpatialIdAtLevel( idx, resolution_, -1 );
			LatLonDegrees64 latlon1 = index2.LatLonDegreesFromValue(idx1);
			int resolution1 = idx1 & spatialLevelMask();

			cout
			<< setprecision(18)
			<< setw(23)
			<< "  idx = 0x" << hex << idx1 << dec
			<< scientific
			<< " latlon = "
			<< latlon1.lat << "," << latlon1.lon
			<< dec << " rLevel = " << resolution1
			<< " resLevel = " << resolution_
			<< endl << flush;
		}

		{
			cout << endl << "incrementing at resolution" << endl << flush;
			uint64 resolution_ = 0;
			STARE_ArrayIndexSpatialValue idx1 = 0x0000000000000001;
			stringstream ss;

			for( resolution_ = 0; resolution_ < 8; ++resolution_) {
				for(uint64 itmp = 0; itmp < 7; ++itmp) {

					// cout << resolution_ << "," << itmp << " res,i" << endl << flush;

					try {
						idx1 = shiftSpatialIdAtLevel( 0x0000000000000000+resolution_, resolution_, itmp );
						// idx1 = shiftSpatialIdAtLevel( 0ul+resolution_, resolution_, itmp );
					} catch (const SpatialException & e ) {
						cout << "Exception: " << dec << e.what() << endl << flush;
						FAIL();
					}

					LatLonDegrees64 latlon1 = index2.LatLonDegreesFromValue(idx1);

					int resolution1 = idx1 & spatialLevelMask();

					ss.clear(); ss.str(string());
					ss << "Increment by " << itmp << " at resolution level " << resolution_;

					cout
					<< "  idx = 0x" << setw(16) << hex << idx1 << dec
					<< "  cmp = 0x" << setw(16) << hex << (itmp << 59) << dec
					<< scientific << setprecision(18) << setw(23)
					<< " latlon = "
					<< setfill(' ')
					<< latlon1.lat << "," << latlon1.lon
					<< dec << " rLevel = " << resolution1
					<< " resLevel = " << resolution_
					<< endl << flush;

					ASSERT_EQUALM(ss.str().c_str(),((itmp << (59 - 2*resolution_)) || resolution_),idx1);

				}
			}
		}
	}

	// #define DIAG
#ifndef DIAG
#define DIAGOUT2(p,m)
#define SIVOUT(m,siv)
#define SIVSOUT(p,m,v)
#else
#define DIAGOUT2(p,m) p << m;
#define SIVOUT(m,siv) cout << m << " " << setw(16) << setfill('0') << hex << siv << dec << endl << flush;
#define SIVSOUT(p,m,v) { p << m << " "; for(int l=0; l<v.size(); ++l) { p << "0x" << setw(16) << setfill('0') << hex << v[l] << " ";}; p << dec << endl << flush; }
#endif
	if(true) {
		DIAGOUT2(cout,"expandIntervals 10" << endl << flush;);
		EmbeddedLevelNameEncoding leftJustified;
		uint64 one_mask_to_level, one_at_level;
		leftJustified.SciDBincrement_LevelToMaskDelta(8,one_mask_to_level, one_at_level);

		// cout << "expandIntervals 20" << endl << flush;

		// STARE Spatial index value and interval arrays.
		// STARE_ArrayIndexSpatialValue siv0 = 0x3300000000000008;
		STARE_ArrayIndexSpatialValue siv0 = 0x3213213213213208;
		SIVOUT("siv0",siv0)

		STARE_SpatialIntervals intervals;
		intervals.push_back(siv0);

		// cout << "expandIntervals 30" << endl << flush;

		STARE_ArrayIndexSpatialValues expanded_values = expandIntervals(intervals,8);
		SIVSOUT(cout,"0 intervals ",intervals);
		SIVSOUT(cout,"0 expanded_values ",expanded_values);
		ASSERT_EQUAL((siv0 & ~one_mask_to_level) | 8,expanded_values[0]);

		// cout << "expandIntervals 40" << endl << flush;

		intervals.push_back(0x2000000000000008);
		expanded_values = expandIntervals(intervals,8);
		SIVSOUT(cout,"1 intervals ",intervals);
		SIVSOUT(cout,"1 expanded_values ",expanded_values);
		ASSERT_EQUAL(intervals[1],expanded_values[0]);

		siv0 = 0x00000000000000008;
		intervals.push_back(siv0);
		siv0+=6*one_at_level;
		SIVOUT("siv0+6@8",siv0);
		leftJustified.setIdFromSciDBLeftJustifiedFormat(siv0);
		intervals.push_back(leftJustified.getSciDBTerminatorLeftJustifiedFormat());
		SIVOUT("siv0+6@8",leftJustified.getSciDBTerminatorLeftJustifiedFormat());
		STARE_ArrayIndexSpatialValue expected6_term = leftJustified.getSciDBTerminatorLeftJustifiedFormat();

		expanded_values = expandIntervals(intervals,8);
		SIVSOUT(cout,"2 intervals ",intervals);
		SIVSOUT(cout,"2 expanded_values ",expanded_values);
		ASSERT_EQUAL(0x0000300000000008,expanded_values[6]);

		leftJustified.setIdFromSciDBLeftJustifiedFormat(expanded_values[6]);
		SIVOUT("6's     ",leftJustified.getSciDBLeftJustifiedFormat());
		SIVOUT("6's term",leftJustified.getSciDBTerminatorLeftJustifiedFormat());
		ASSERT_EQUAL(expected6_term,leftJustified.getSciDBTerminatorLeftJustifiedFormat());

		expanded_values = expandIntervals(intervals,7);

#ifdef DIAG
		cout << "intervals "<< dec << endl <<flush;
		for( int i=0; i<intervals.size(); ++i ) {
			SIVOUT(i,intervals[i]);
		}

		cout << "expanded "<< dec << endl <<flush;
		for( int i=0; i<expanded_values.size(); ++i ) {
			SIVOUT(i,expanded_values[i]);
		}
#endif

		intervals.clear();
		intervals.push_back(0x0000000000000008);
		expanded_values = expandIntervals(intervals,6);
                ASSERT_EQUAL(0x0000000000000006,expanded_values[0]);

		intervals.clear();
		intervals.push_back(0x0000000000000008);
		expanded_values = expandIntervals(intervals,9);
                ASSERT_EQUAL(0x0000000000000009,expanded_values[0]);
                ASSERT_EQUAL(0x0000020000000009,expanded_values[1]);
                ASSERT_EQUAL(0x0000040000000009,expanded_values[2]);
                ASSERT_EQUAL(0x0000060000000009,expanded_values[3]);

                SIVSOUT(cout,"intervals ",intervals);
                SIVSOUT(cout,"expanded  ",expanded_values)

		// #undef DIAGOUT2
		// #undef SIVOUT
		// #define DIAGOUT2(out,expr)
		// #undef DIAG
	}

	if(true) {
		EmbeddedLevelNameEncoding lj;
		STARE index;
		SpatialVector x(1,1,0); x.normalize();
		STARE_ArrayIndexSpatialValue siv = index.ValueFromSpatialVector(x);
		SpatialVector v = index.SpatialVectorFromValue(siv); v.normalize();
		// cout << "v: " << v << endl << flush;
		ASSERT_EQUALDM("x-to-siv",x,v,1.0e-8);

		siv = 0x0001000000000008;
		v = index.SpatialVectorFromValue(siv); v.normalize(); // Note, wipes out resolution info.
		STARE_ArrayIndexSpatialValue siv1 = index.ValueFromSpatialVector(v);
		// cout << "iv: " << hex << siv << " " << hex << siv1 << endl << flush;
		ASSERT_EQUALM("iv-to-v-roundtrip",siv,(siv1 & ~lj.levelMaskSciDB) | 8);
	}

	if(true) {
		STARE index;
		SpatialVector v(1,1,1); v.normalize();
		STARE_ArrayIndexSpatialValue sid = index.ValueFromSpatialVector(v);
		SpatialVector vr = index.SpatialVectorFromValue(sid);
		STARE_ArrayIndexSpatialValue sidr = index.ValueFromSpatialVector(vr);
#ifdef DIAG
		cout << "sid  0x" << setw(16) << setfill('0') << hex << sid << dec << endl << flush;
		cout << "sidr 0x" << setw(16) << setfill('0') << hex << sidr << dec << endl << flush;
		cout << "v  " << v << endl << flush;
		cout << "vr " << vr << endl << flush;
		cout << endl << flush;
#endif
		ASSERT_EQUAL(sid,sidr);
	}

	if(true) {
		/*
		i,id  :  3 ['0x4c0000000000003']
		test id:    ['0x4c0000000000003']
		test ll :  [(14.255438319990454, 64.21872912284311), (-22.58410062366997, 70.08873514750778), (0.8929126045078797, 67.65325299601919)]
		test im :  [[0, 1, 2]]
		test lat:  [ 14.25543832 -22.58410062   0.8929126 ]
		test lon:  [64.21872912 70.08873515 67.653253  ]
		 */
		STARE index;
		// STARE_ArrayIndexSpatialValue sid = 0x4c0000000000003;
		STARE_ArrayIndexSpatialValue sid = 0x4c000000000001b;
		STARE_ArrayIndexSpatialValue sidtmp;
		SpatialVector v_sid  = index.SpatialVectorFromValue(sid);
		LatLonDegrees64 v_ll = index.LatLonDegreesFromValue(sid);
		float64 v_lat,v_lon;
		v_sid.getLatLonDegrees(v_lat, v_lon);

#ifdef DIAG
		cout << "v_sid " << v_sid << endl << flush;
		cout << "v_sid ll " << v_lat << " " << v_lon << endl << flush;
		cout << "v idx ll " << v_ll.lat << " " << v_ll.lon << endl << flush;
		cout << "sid 0x" << setw(16) << setfill('0') << hex << sid << dec << endl << flush;
		cout << endl << flush;
#endif

		ASSERT_EQUAL(v_lat,v_ll.lat);
		ASSERT_EQUAL(v_lon,v_ll.lon);

		Triangle tr = index.TriangleFromValue(sid);

#ifdef DIAG
		cout << "Triangle " << endl << flush;
		for( int i=0; i<3; ++i) {
			float64 lt,ln;
			tr.vertices[i].getLatLonDegrees(lt,ln);
			cout << setprecision(16);
			cout << i << " i,v  " << tr.vertices[i]  << " -- " << lt << "," << ln << endl << flush;
		}
		cout << endl << flush;

		cout << "index.ValueFromSpatialVector triangle" << endl << flush;
		for( int i=0; i<3; ++i) {
			sidtmp = index.ValueFromSpatialVector(tr.vertices[i],27);
			SpatialVector vtmp = index.SpatialVectorFromValue(sidtmp);
			float64 lt,ln;
			vtmp.getLatLonDegrees(lt,ln);
			cout << i << " i,vt " << vtmp  << " -- " << lt << "," << ln << endl << flush;
		}
		cout << endl << flush;
#endif

		SpatialVector vecs[3] = {
				SpatialVector( 0.4619397639595428, 0.8446232009168338, 0.2705980468259219 ),
				SpatialVector( 0.4619397725326853, 0.8446231986774957, 0.2705980391803436 ),
				SpatialVector( 0.4619397591445266, 0.8446232068078616, 0.2705980366578833 )
		};

#ifdef DIAG
		cout << "index... from Explicit vecs" << endl << flush;
		for( int i=0; i<3; ++i) {
			sidtmp = index.ValueFromSpatialVector(vecs[i]);
			SpatialVector vtmp = index.SpatialVectorFromValue(sidtmp);
			float64 lt,ln;
			vtmp.getLatLonDegrees(lt,ln);
			cout << i << " i,vt " << vtmp  << " -- " << lt << "," << ln << endl << flush;
		}
		cout << endl << flush;

		for( int i=0; i<3; ++i) {
			float64 lat, lon;
			SpatialVector vtmp = tr.vertices[i];
			vtmp.getLatLonDegrees(lat,lon);
			// tr.vertices[i].getLatLonDegrees(lat,lon);
			sidtmp = index.ValueFromSpatialVector(vtmp,27);
			// sidtmp = index.ValueFromSpatialVector(vtmp);
			// sidtmp = index.ValueFromSpatialVector(tr.vertices[i]);
			LatLonDegrees64 latlontmp = index.LatLonDegreesFromValue(sidtmp);
			cout << i << " i,tr latlon " << lat << " " << lon << " -- "
					<< latlontmp.lat << " " << latlontmp.lon
					<< " -- "
					<< "0x" << setw(16) << setfill('0') << hex << sidtmp << dec
					<< endl << flush;
		}
		cout << endl << flush;
#endif


		DIAGOUT2(cout,setprecision(16) << "tr.v[1] " << tr.vertices[1] << endl << flush;);
		SpatialVector v = tr.vertices[1];
		sid = index.ValueFromSpatialVector(v);
		SpatialVector vr = index.SpatialVectorFromValue(sid);
		STARE_ArrayIndexSpatialValue sidr = index.ValueFromSpatialVector(vr);
		float64 lat,lon;
		LatLonDegrees64 ll = index.LatLonDegreesFromValue(sid);
		LatLonDegrees64 llr = index.LatLonDegreesFromValue(sidr);
#ifdef DIAG
		cout << "sid  0x" << setw(16) << setfill('0') << hex << sid << dec
				<< " - " << ll.lat << " " << ll.lon
				<< endl << flush;
		cout << "sidr 0x" << setw(16) << setfill('0') << hex << sidr << dec
				<< " - " << llr.lat << " " << llr.lon
				<< endl << flush;
		v.getLatLonDegrees(lat, lon);
		cout << setprecision(16);
		cout << "tr.v[1] " << tr.vertices[1] << endl << flush;
		cout << "v       " << v
				<< " - " << lat << " " << lon
				<< endl << flush;
		vr.getLatLonDegrees(lat, lon);
		cout << "vr      " << vr
				<< " - " << lat << " " << lon
				<< endl << flush;
		cout << endl << flush;
#endif
		ASSERT_EQUAL(sid,sidr);
		ASSERT_EQUALDM("v-to-vr",v,vr,1.0e-8);

	}

	if(true) {
		DIAGOUT2(cout,endl << "------" << endl << endl << flush;);
		STARE index;
		int resolution = 27;
		SpatialIndex sIndex = index.getIndex(resolution);
		for( int i = 0; i < 11; ++i ) {
			SpatialVector v0 ( 0.4619397639595428, 0.8446232009168338, 0.2705980468259219 );
			// SpatialVector delta = SpatialVector(0.0,0.0,(1-i*0.1)*1.0e-16);
			SpatialVector delta = SpatialVector(0.0,0.0,(1-i*0.1)*1.0e-15);
			// v0 = v0 + SpatialVector(0.0,0.0,0.9e-8);
			v0 = v0 - delta;
			v0.normalize();
			// SpatialVector v0 ( 1, 0, 0); // Works for this one.
			STARE_ArrayIndexSpatialValue siv0 = index.ValueFromSpatialVector(v0,resolution);
			BitShiftNameEncoding       rightJustified(sIndex.idByPoint(v0));
#ifdef DIAG
			cout << i << " i,delta " << delta << endl << flush;
			cout << "sI idbp 0x" << setw(16) << setfill('0') << hex << sIndex.idByPoint(v0) << dec << endl << flush;
			cout << "rj name   "<< rightJustified.getName() << endl << flush;
#endif

			EmbeddedLevelNameEncoding  leftJustified(rightJustified.leftJustifiedId());
			EmbeddedLevelNameEncoding  leftJustifiedWithResolution = leftJustified.atLevel(resolution, true); // True means keep all bits
			STARE_ArrayIndexSpatialValue siv = leftJustifiedWithResolution.getSciDBLeftJustifiedFormat();
			SpatialVector v_siv0 = index.SpatialVectorFromValue(siv0);
			SpatialVector v_siv = index.SpatialVectorFromValue(siv);
#ifdef DIAG
			cout << "siv0  " << setw(16) << setfill('0') << hex << siv0 << endl << flush;
			cout << "siv   " << setw(16) << setfill('0') << hex << siv << endl << flush;
			cout << setprecision(16);
			cout << "v0     " << v0 << endl << flush;
			cout << "v_siv0 " << v_siv0 << endl << flush;
			cout << "v_siv  " << v_siv << endl << flush;
			cout << endl << "--" << endl << endl << flush;
#endif
			ASSERT_EQUALDM("v0-to-siv0",v0,v_siv0,1.0e-8);
			ASSERT_EQUALDM("v0-to-siv",v0,v_siv,1.0e-8);
		}

		if(true) {
			// Motivated by confusing EmbeddedLevelNameEncoding internal format with the SciDB format.
			DIAG1(cout << "ExpandInterval tests" << endl << flush;)
					// STARE_ArrayIndexSpatialValues expandInterval(STARE_SpatialIntervals interval, int64 force_resolution)
					STARE_SpatialIntervals interval;
			interval.push_back(0x2324000000000005);
			interval.push_back(0x2327ffffffffffff);
			STARE_ArrayIndexSpatialValues values = expandInterval(interval);
			DIAG1(for(int i = 0; i < values.size(); ++i ) { cout << dec << i << " i,v 0x" << setw(16) << setfill('0') << hex << values[i] << endl << flush << dec; })
			ASSERT_EQUAL(0x2324000000000005,values[0]);
			ASSERT_EQUAL(0x2326000000000005,values[1]);
		}
	}

	{ /* See makeCornerVector at the start of the file. */
		STARE stare;

		// Getting index Value
		STARE_SpatialIntervals indexValues;
		LatLonDegrees64ValueVector cornerVector = makeCornerVector();
		indexValues = stare.ConvexHull(cornerVector, 12);
		// Producing outputs
		// std::cout << "Integer Value of STARE index: "<< std::endl;
		/*
		int i = 0;
		for(auto const& value: indexValues) {
			if( i % 209 == 0 ) {
				std::cout << i << " " << value << std::endl;
			}
			++i;
		}
		*/
		// A regression. Perhaps foolhardy.
		ASSERT_EQUAL(indexValues[   0], 4063372763795030021);
		ASSERT_EQUAL(indexValues[ 209], 4047214340913233929);
		ASSERT_EQUAL(indexValues[ 418], 4048304506692173834);
		ASSERT_EQUAL(indexValues[ 627], 4069240857352470538);
		ASSERT_EQUAL(indexValues[ 836], 4047805878168977419);
		ASSERT_EQUAL(indexValues[1045], 4062281498504462347);
		ASSERT_EQUAL(indexValues[1254], 4071136552837709835);
		ASSERT_EQUAL(indexValues[1463], 4037621170680365068);
		ASSERT_EQUAL(indexValues[1672], 4037929652411432972);
		ASSERT_EQUAL(indexValues[1881], 4046503437926400012);
		ASSERT_EQUAL(indexValues[2090], 4047784506411712524);
		ASSERT_EQUAL(indexValues[2299], 4048209742533754892);
		ASSERT_EQUAL(indexValues[2508], 4049064166147751948);
		ASSERT_EQUAL(indexValues[2717], 4050190581450670092);
		ASSERT_EQUAL(indexValues[2926], 4050638220122128396);
		ASSERT_EQUAL(indexValues[3135], 4051281090826993676);
		ASSERT_EQUAL(indexValues[3344], 4051436980959969292);
		ASSERT_EQUAL(indexValues[3553], 4062307233948499980);
		ASSERT_EQUAL(indexValues[3762], 4062741541041471500);
		ASSERT_EQUAL(indexValues[3971], 4063200552786329612);
		ASSERT_EQUAL(indexValues[4180], 4066811658209591308);
		ASSERT_EQUAL(indexValues[4389], 4068128048505946124);
		ASSERT_EQUAL(indexValues[4598], 4068353207871471628);
		ASSERT_EQUAL(indexValues[4807], 4070473512966422540);
		ASSERT_EQUAL(indexValues[5016], 4071135865642942476);
		ASSERT_EQUAL(indexValues[5225], 4165894663942701068);
		ASSERT_EQUAL(indexValues[5434], 4166328008962998284);

	}

	// Didn't handle level = 0 in STARE::htmIDFromValue properly.
	if(false) {
		LatLonDegrees64 latlon0(45.0,45.0);
		int level = 0;
	  STARE_ArrayIndexSpatialValue aIndex  = index.ValueFromLatLonDegrees(latlon0.lat,latlon0.lon,level);
	  cout << "aIndex:  " << hex << aIndex << dec << endl;

	  EmbeddedLevelNameEncoding lj;
	  lj.setIdFromSciDBLeftJustifiedFormat(aIndex);
	  BitShiftNameEncoding rj(lj.rightJustifiedId());
	  uint64 htmID = rj.getId();
	  cout << "a100 " << hex << htmID << dec << endl << flush;

	  Triangle tr = index.TriangleFromValue(aIndex);
	  
	}

	if(true) {
		double cm = 0.01/6378.0e3; // cm/km
		double tolerance = 5*cm;
		int level = 27;
		STARE_ArrayIndexSpatialValue aIndex0 = index.ValueFromLatLonDegrees(  0.0,  0.0,level);
		STARE_ArrayIndexSpatialValue aIndex1 = index.ValueFromLatLonDegrees( 45.0,  0.0,level);
		STARE_ArrayIndexSpatialValue aIndex2 = index.ValueFromLatLonDegrees(-45.0,  0.0,level);
		STARE_ArrayIndexSpatialValue aIndex3 = index.ValueFromLatLonDegrees(-45.0,180.0,level);

		ASSERT_LESS(abs(0.5*sqrt(2)-index.cmpSpatialDistanceCosine(aIndex0,aIndex1)),tolerance);
		ASSERT_LESS(abs(0.5*sqrt(2)-index.cmpSpatialDistanceCosine(aIndex0,aIndex2)),tolerance);
		ASSERT_LESS(abs(0.0-index.cmpSpatialDistanceCosine(aIndex1,aIndex2)),tolerance);
		ASSERT_LESS(abs(-1.0-index.cmpSpatialDistanceCosine(aIndex1,aIndex3)),tolerance);

		ASSERT_LESS(abs(acos(0.5*sqrt(2))-index.cmpSpatialDistanceRadians(aIndex0,aIndex1)),tolerance);
		ASSERT_LESS(abs(acos(0.5*sqrt(2))-index.cmpSpatialDistanceRadians(aIndex0,aIndex2)),tolerance);
		ASSERT_LESS(abs(acos(0.0)-index.cmpSpatialDistanceRadians(aIndex1,aIndex2)),tolerance);
		ASSERT_LESS(abs(acos(-1.0)-index.cmpSpatialDistanceRadians(aIndex1,aIndex3)),tolerance);

		// cout << "resest0: " << index.cmpSpatialResolutionEstimate(aIndex0,aIndex1) << endl << flush;
		// cout << "resest1: " <<index.cmpSpatialResolutionEstimate(0x2326000000000005,0x2327000000000005) << endl << flush;
		// cout << "resest2: " <<index.cmpSpatialResolutionEstimateI(0x2326000000000005,0x2327000000000005) << endl << flush;

		ASSERT_EQUAL(1,index.cmpSpatialResolutionEstimate(aIndex0,aIndex1));
		ASSERT_EQUAL(5,index.cmpSpatialResolutionEstimate(0x2326000000000005,0x2327000000000005));
	}

	if(true) {
		// cout << "length: " << index.lengthMeterScaleFromEdgeFromLevel(10) << endl << flush;
		// cout << "res:    " << index.levelFromLengthMeterScaleFromEdge(10.0e3) << endl << flush;
		ASSERT_EQUAL(9772,int(index.lengthMeterScaleFromEdgeFromLevel(10)));
		ASSERT_EQUAL(10,int(0.5+index.levelFromLengthMeterScaleFromEdge(10.0e3)));
	}

	// FAIL();
}

