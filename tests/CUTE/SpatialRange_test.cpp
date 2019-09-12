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
	STARE_ArrayIndexSpatialValue siv = index.ValueFromLatLonDegrees( 30, 30, 8 );
	STARE_SpatialIntervals sis; sis.push_back(siv);

	SpatialRange range(sis);
	STARE_SpatialIntervals sis_out;

	sis_out = range.toSpatialIntervals();

// #define DIAG
#ifdef DIAG
	for(int i=0; i < sis.size(); ++i ) {
		cout << i << " i,si: " << setw(16) << setfill('0') << hex << sis[i] << dec << endl << flush;
	}
	for(int i=0; i < sis_out.size(); ++i ) {
		cout << i << " i,so: " << setw(16) << setfill('0') << hex << sis_out[i] << dec << endl << flush;
	}
#endif

	ASSERT_EQUAL(1,sis_out.size());
	ASSERT_EQUAL(sis[0],sis_out[0]);

	// sids.push_back(index.ValueFromLatLonDegrees( 45, 45, 8));

	siv = index.ValueFromLatLonDegrees( 15, 15, 8 );

	EmbeddedLevelNameEncoding leftJustified;
	leftJustified.setIdFromSciDBLeftJustifiedFormat(siv);
	sis.push_back(leftJustified.getSciDBTerminatorLeftJustifiedFormat());

#ifdef DIAG
	for(int i=0; i < sis.size(); ++i ) {
		cout << i << " i,si: " << setw(16) << setfill('0') << hex << sis[i] << dec << endl << flush;
	}
//	for(int i=0; i < sis_out.size(); ++i ) {
//		cout << i << " i,so: " << setw(16) << setfill('0') << hex << sis_out[i] << dec << endl << flush;
//	}
#endif

	// cout << 50 << endl << flush;

	range.purge();
	range.addSpatialIntervals(sis);
//	cout << 100
//			<< " nr: " << range.range->range->nranges()
//			<< endl << flush;
	sis_out = range.toSpatialIntervals();

#ifdef DIAG
	cout << 110 << " sis_out.s: " << sis_out.size() << endl << flush;
	for(int i=0; i<sis_out.size(); ++i) {
		cout << i << " i,so: " << i << " "
				<< setw(16) << setfill('0') << hex
				<< sis_out[i] << endl << flush << dec;
	}
#endif

	ASSERT_EQUAL(2,sis_out.size());
	ASSERT_EQUAL(sis[0],sis_out[0]);
	ASSERT_EQUAL(sis[1],sis_out[1]);


	if(true) {
		// TODO Fix the following to use the new 0/1 variables defined above.
#define SIVOUT(m,siv)
// #define SIVOUT(m,siv) cout << m << " " << setw(16) << setfill('0') << hex << siv << dec << endl << flush;
#define SISOUT(m,sis)
// #define SISOUT(m,sis) { cout << endl << m << endl; for(int i=0; i < sis.size(); ++i ) { cout << i << " i,si: " << setw(16) << setfill('0') << hex << sis[i] << dec << endl << flush; }}
		// #define DIAGOUT
		// #define DIAGOUT(m) {cout << m << endl << flush;}
		// Note level is 8.
		uint64 one_mask_to_level, one_at_level;
		leftJustified.increment_LevelToMaskDelta(8,one_mask_to_level, one_at_level);

		STARE_SpatialIntervals sis0;
		SpatialRange range0;
		STARE_ArrayIndexSpatialValue siv0 = 0x00000000000000008;
		sis0.push_back(siv0);
		siv0+=6*one_at_level;
		leftJustified.setIdFromSciDBLeftJustifiedFormat(siv0); // TODO Rename this STARE, or provide a STARE interface for this.
		sis0.push_back(leftJustified.getSciDBTerminatorLeftJustifiedFormat());
		range0.addSpatialIntervals(sis0);
		SISOUT("sis0",sis0);

		STARE_SpatialIntervals sis1;
		SpatialRange range1;
		STARE_ArrayIndexSpatialValue siv1 = 0x00000000000000008;
		siv1+=3*one_at_level;
		sis1.push_back(siv1);
		siv1+=6*one_at_level;
		leftJustified.setIdFromSciDBLeftJustifiedFormat(siv1);
		sis1.push_back(leftJustified.getSciDBTerminatorLeftJustifiedFormat());
		range1.addSpatialIntervals(sis1);
		SISOUT("sis1",sis1);

		// cout << 100 << endl << flush;
		SpatialRange *deltaRange;
		try {
			// cout << 110 << endl << flush;
			deltaRange = range0 & range1;
			// deltaRange = sr_intersect(range0, range1);
			// cout << 120 << endl << flush;
		} catch ( SpatialException e ) {
			// cout << 130 << endl << flush;
			cout << e.what() << endl << flush;
			// cout << 140 << endl << flush;
		}

#ifdef DIAG
		cout << 199 << endl << flush;
		cout << 1991 << " dR.r            " << hex << (deltaRange->range) << dec << endl << flush;
		cout << 1991 << " dR.r->r         " << hex << (deltaRange->range->range) << dec << endl << flush;
		cout << 1991 << " dR.r->r->my_los " << hex << (deltaRange->range->range->my_los) << dec << endl << flush;
#endif
		if(!deltaRange->range->range) { cout << "Error deltaRange is null!" << endl << flush; }
		// cout << 200 << " dR nR = " << deltaRange->range->range->nranges() << endl << flush;
		STARE_SpatialIntervals deltaSis = deltaRange->toSpatialIntervals();
		// cout << 300 << endl << flush;
		SISOUT("deltaSis",deltaSis);
		// cout << 400 << endl << flush;

		ASSERT_EQUAL(sis1[0],deltaSis[0]);
		ASSERT_EQUAL(sis0[1],deltaSis[1]);

#undef SIVOUT
#undef SISOUT

	}


	if(true) {
		// TODO Fix the following to use the new 0/1 variables defined above.
// #define DIAG
#ifndef DIAG
#define SIVOUT(m,siv)
#define SISOUT(m,sis)
#else
#define SIVOUT(m,siv) cout << m << " " << setw(16) << setfill('0') << hex << siv << dec << endl << flush;
#define SISOUT(m,sis) { cout << endl << m << endl; for(int i=0; i < sis.size(); ++i ) { cout << i << " i,si: " << setw(16) << setfill('0') << hex << sis[i] << dec << endl << flush; }}
#endif
		// #define DIAGOUT
		// #define DIAGOUT(m) {cout << m << endl << flush;}
		// Note level is 8.
		uint64 one_mask_to_level, one_at_level;
		leftJustified.increment_LevelToMaskDelta(8,one_mask_to_level, one_at_level);

		STARE_SpatialIntervals sis0;
		SpatialRange range0;
		STARE_ArrayIndexSpatialValue siv0 = 0x00000000000000008;
		sis0.push_back(siv0);
		siv0+=6*one_at_level;
		leftJustified.setIdFromSciDBLeftJustifiedFormat(siv0); // TODO Rename this STARE, or provide a STARE interface for this.
		sis0.push_back(leftJustified.getSciDBTerminatorLeftJustifiedFormat());
		range0.addSpatialIntervals(sis0);
		SISOUT("sis0",sis0);

		STARE_SpatialIntervals sis1;
		SpatialRange range1;
		STARE_ArrayIndexSpatialValue siv1 = 0x00000000000000008;
		siv1+=3*one_at_level;
		siv1 = ( siv1 & ~leftJustified.levelMaskSciDB ) | 10;
		sis1.push_back(siv1);
		siv1+=6*one_at_level;
		leftJustified.setIdFromSciDBLeftJustifiedFormat(siv1);
		sis1.push_back(leftJustified.getSciDBTerminatorLeftJustifiedFormat());
		range1.addSpatialIntervals(sis1);
		SISOUT("sis1",sis1);

		// cout << 100 << endl << flush;
		SpatialRange *deltaRange;
		try {
			// cout << 110 << endl << flush;
			deltaRange = range0 & range1;
			// deltaRange = sr_intersect(range0, range1);
			// cout << 120 << endl << flush;
		} catch ( SpatialException e ) {
			// cout << 130 << endl << flush;
			cout << e.what() << endl << flush;
			// cout << 140 << endl << flush;
		}

#ifdef DIAG
		cout << 199 << endl << flush;
		cout << 1991 << " dR.r            " << hex << (deltaRange->range) << dec << endl << flush;
		cout << 1991 << " dR.r->r         " << hex << (deltaRange->range->range) << dec << endl << flush;
		cout << 1991 << " dR.r->r->my_los " << hex << (deltaRange->range->range->my_los) << dec << endl << flush;
#endif
		if(!deltaRange->range->range) { cout << "Error deltaRange is null!" << endl << flush; }
		// cout << 200 << " dR nR = " << deltaRange->range->range->nranges() << endl << flush;
		STARE_SpatialIntervals deltaSis = deltaRange->toSpatialIntervals();
		// cout << 300 << endl << flush;
		SISOUT("deltaSis",deltaSis);
		// cout << 400 << endl << flush;

		ASSERT_EQUAL(sis1[0],deltaSis[0]);
		ASSERT_EQUAL(sis0[1],deltaSis[1]);

#undef SIVOUT
#undef SISOUT
#undef DIAG
	}

// TODO Add intersection tests.

}

