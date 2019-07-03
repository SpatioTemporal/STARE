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


void STARE_test() {

	bool globalPrintFlag = false;

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
	}

//	{
//		STARE_ArrayIndexSpatialValue spatialStareId = 0;
//		LatLonDegrees64 latlon0 = index.LatLonDegreesFromValue(spatialStareId);
//		cout << " idx=0, latlon0: " << latlon0.lat << " " << latlon0.lon << endl << flush;
//	}

//	FAIL();
}

