/*
 * SpatialRange_test.cpp
 *
 *  Created on: Sep 8, 2019
 *      Author: mrilee
 *
 *  Copyright (C) 2019 Rilee Systems Technologies LLC
 */

#include "Test.h"

void SpatialRange_test () {

	STARE index;
	STARE_ArrayIndexSpatialValue sid = index.ValueFromLatLonDegrees( 30, 30, 8 );
	STARE_SpatialIntervals sids; sids.push_back(sid);

	SpatialRange range(sids);
	STARE_SpatialIntervals sids_out;

	sids_out = range.toSpatialIntervals();

// #define DIAG
#ifdef DIAG
	for(int i=0; i < sids.size(); ++i ) {
		cout << i << " i,si: " << setw(16) << setfill('0') << hex << sids[i] << dec << endl << flush;
	}
	for(int i=0; i < sids_out.size(); ++i ) {
		cout << i << " i,so: " << setw(16) << setfill('0') << hex << sids_out[i] << dec << endl << flush;
	}
#endif

	ASSERT_EQUAL(1,sids_out.size());
	ASSERT_EQUAL(sids[0],sids_out[0]);

	// sids.push_back(index.ValueFromLatLonDegrees( 45, 45, 8));

	sid = index.ValueFromLatLonDegrees( 15, 15, 8 );

	EmbeddedLevelNameEncoding leftJustified;
	leftJustified.setIdFromSciDBLeftJustifiedFormat(sid);
	sids.push_back(leftJustified.getSciDBTerminatorLeftJustifiedFormat());

#ifdef DIAG
	for(int i=0; i < sids.size(); ++i ) {
		cout << i << " i,si: " << setw(16) << setfill('0') << hex << sids[i] << dec << endl << flush;
	}
//	for(int i=0; i < sids_out.size(); ++i ) {
//		cout << i << " i,so: " << setw(16) << setfill('0') << hex << sids_out[i] << dec << endl << flush;
//	}
#endif

	// cout << 50 << endl << flush;

	range.purge();
	range.addSpatialIntervals(sids);
//	cout << 100
//			<< " nr: " << range.range->range->nranges()
//			<< endl << flush;
	sids_out = range.toSpatialIntervals();

#ifdef DIAG
	cout << 110 << " sids_out.s: " << sids_out.size() << endl << flush;
	for(int i=0; i<sids_out.size(); ++i) {
		cout << i << " i,so: " << i << " "
				<< setw(16) << setfill('0') << hex
				<< sids_out[i] << endl << flush << dec;
	}
#endif

	ASSERT_EQUAL(2,sids_out.size());
	ASSERT_EQUAL(sids[0],sids_out[0]);
	ASSERT_EQUAL(sids[1],sids_out[1]);



}

