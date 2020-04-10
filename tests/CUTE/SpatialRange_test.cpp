/*
 * SpatialRange_test.cpp
 *
 *  Created on: Sep 8, 2019
 *      Author: mrilee
 *
 *  Copyright (C) 2019 Rilee Systems Technologies LLC
 */

#include "Test.h"

class srange {
public:
	srange(){}
	~srange(){}
	SpatialRange range;

	void add_range(const SpatialRange& sr) { range.addSpatialRange(sr); }
	void add_intervals(STARE_SpatialIntervals sis) { range.addSpatialIntervals(sis); }
	STARE_SpatialIntervals to_intervals() { return range.toSpatialIntervals(); }

	SpatialRange test() {
		SpatialRange r0;
		r0.addSpatialIntervals(range.toSpatialIntervals());
		SpatialRange r1;
		r1.addSpatialRange(r0);
		return r1;
	}

	srange intersect(const srange& other) {
		// cout << "i000" << endl << flush;
		// SpatialRange *r = range & other.range;
		SpatialRange *r = sr_intersect(range,other.range);
		// cout << "i100" << endl << flush;
		srange res;
		// cout << "i200" << endl << flush;
		// res.range.tag = "intersect";
		// cout << "i300" << endl << flush;
		res.add_range(*r);
		// cout << "i400" << endl << flush;
		return res;
	}
};

// #define DIAG

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

#undef DIAG
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
		leftJustified.SciDBincrement_LevelToMaskDelta(8,one_mask_to_level, one_at_level);

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
		SpatialRange* deltaRange;
		try {
			// cout << 110 << endl << flush;
			deltaRange = range0 & range1;
			// deltaRange = sr_intersect(range0, range1);
			// cout << 120 << endl << flush;
			// cout << 121 << endl << flush;
		} catch ( SpatialException e ) {
		  // cout << 130 << endl << flush;
		  cout << e.what() << endl << flush;
		  // cout << 140 << endl << flush;
		}
		// cout << 122 << endl << flush;

#ifdef DIAG
		// cout << 199 << endl << flush;
		// cout << 1991 << " dR.r            " << hex << (deltaRange->range) << dec << endl << flush;
		// cout << 1991 << " dR.r->r         " << hex << (deltaRange->range->range) << dec << endl << flush;
		// cout << 1991 << " dR.r->r->my_los " << hex << (deltaRange->range->range->my_los) << dec << endl << flush;
#endif
		if(!deltaRange->range->range) { cout << "Error deltaRange is null!" << endl << flush; }
		// cout << 200 << " dR nR = " << deltaRange->range->range->nranges() << endl << flush;
		STARE_SpatialIntervals deltaSis = deltaRange->toSpatialIntervals();
		// cout << 300 << endl << flush;
		SISOUT("deltaSis",deltaSis);
		// cout << 400 << endl << flush;

		ASSERT_EQUAL(sis1[0],deltaSis[0]);
		ASSERT_EQUAL(sis0[1],deltaSis[1]);

		delete deltaRange;

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
		leftJustified.SciDBincrement_LevelToMaskDelta(8,one_mask_to_level, one_at_level);

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
		SpatialRange* deltaRange;
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

		ASSERT_EQUAL(2,deltaSis.size());
		ASSERT_EQUAL(sis1[0],deltaSis[0]);
		ASSERT_EQUAL(sis0[1],deltaSis[1]);

		delete deltaRange;
	}

	// TODO Write many more tests & consider edge cases.
	if(true) {
		STARE_ArrayIndexSpatialValue siv1[2] = { 0x0000000000000008, 0x000067ffffffffff };
		STARE_ArrayIndexSpatialValue siv2[2] = { 0x000030000000000a, 0x0000907fffffffff };
		STARE_SpatialIntervals sis1(siv1,siv1+2);
		STARE_SpatialIntervals sis2(siv2,siv2+2);
		SpatialRange r1(sis1), r2(sis2);
		SpatialRange *ri = r1 & r2;
		// SpatialRange *ri = r1 & r2;
		// SpatialRange *ri = sr_intersect(r1, r2);

		STARE_SpatialIntervals result = ri->toSpatialIntervals();
		ASSERT_EQUAL(2,result.size());
		ASSERT_EQUAL(0x000030000000000a,result[0]);
		ASSERT_EQUAL(0x000067ffffffffff,result[1]);

		ri->range->range->CompressionPass();
		result = ri->toSpatialIntervals();
		ASSERT_EQUAL(4,result.size());
		ASSERT_EQUAL(0x0000300000000008,result[0]);
		ASSERT_EQUAL(0x00003fffffffffff,result[1]);
		ASSERT_EQUAL(0x0000400000000007,result[2]);
		ASSERT_EQUAL(0x0000600000000008,result[3]);
		delete ri;
		
		ri = sr_intersect(r1, r2, true); // Run a compression pass on the range.
		result.clear();
		result = ri->toSpatialIntervals();
		delete ri;
		ASSERT_EQUAL(4,result.size());
		ASSERT_EQUAL(0x0000300000000008,result[0]);
		ASSERT_EQUAL(0x00003fffffffffff,result[1]);
		ASSERT_EQUAL(0x0000400000000007,result[2]);
		ASSERT_EQUAL(0x0000600000000008,result[3]);
	}

	if(false) {
		// From PySTARE.cpp for testing some odd behavior.
		// void _intersect(int64_t* indices1, int len1, int64_t* indices2, int len2, int64_t* intersection, int leni) {
	  const int len1 = 3, len2 = 3;
	  int leni = 3;
		int64_t
		indices1[len1] = {0,0,0},
		// indices2[len2] = {0,0,0},
		intersection[leni];

		int64_t *indices2 = indices1;

		STARE_SpatialIntervals si1(indices1, indices1+len1), si2(indices2, indices2+len2);
		// intersection[0] = 69;
		SpatialRange r1(si1), r2(si2);
		SpatialRange *ri = r1 & r2;
		STARE_SpatialIntervals result = ri->toSpatialIntervals();
		leni = result.size(); // result.size() should be less than the output-length input above (leni).
		for(int i=0; i<leni; ++i) {
			intersection[i] = result[i];
		}
		// }
	}

	if(true) {
		STARE_ArrayIndexSpatialValue siv1[2] = { 0x0000000000000008, 0x000067ffffffffff };
		STARE_SpatialIntervals sis1(siv1,siv1+2);
		SpatialRange r1(sis1);

		if(false) {
			STARE_ArrayIndexSpatialValue siv = 0x0000300000000008;
			cout << "100: " << r1.contains(siv) << endl << flush;

			siv = 0x0000700000000008;
			cout << "101: " << r1.contains(siv) << endl << flush;

			siv = 0x0000000000000008;
			cout << "102: " << r1.contains(siv) << endl << flush;

			siv = 0x000067ffffffffff;
			cout << "103: " << r1.contains(siv) << endl << flush;

			siv = 0x0000000000000007;
			cout << "104: " << r1.contains(siv) << endl << flush;
		}

		ASSERT_EQUAL(1,r1.contains(0x0000300000000008));
		ASSERT_EQUAL(0,r1.contains(0x0000700000000008));
		ASSERT_EQUAL(1,r1.contains(0x0000000000000008));
		ASSERT_EQUAL(1,r1.contains(0x000067ffffffffff));
		ASSERT_EQUAL(0,r1.contains(0x0000000000000007));
	}

	if(true) {
		/*
		STARE_SpatialIntervals sis[2] = { 0x0000000000000008, 0x000067ffffffffff };
		srange sr;
		sr.add_intervals(sis);
		sr.test();
		*/

		STARE_ArrayIndexSpatialValue siv1[2] = { 0x0000000000000008, 0x000067ffffffffff };
		STARE_ArrayIndexSpatialValue siv2[2] = { 0x000030000000000a, 0x0000907fffffffff };
		STARE_SpatialIntervals sis1(siv1,siv1+2);
		STARE_SpatialIntervals sis2(siv2,siv2+2);

		srange *r1 = new srange();
		srange *r2 = new srange();
		r1->add_intervals(sis1);
		r2->add_intervals(sis2);
		srange r12 = r1->intersect(*r2);
		delete r1;
		delete r2;

	}

#undef SIVOUT
#undef SISOUT
#undef DIAG
// TODO Add intersection tests.

}

