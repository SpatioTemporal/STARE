/*
 * SpatialRange_test.cpp
 *
 *  Created on: Sep 8, 2019
 *      Author: mrilee
 *
 *  Copyright (C) 2019 Rilee Systems Technologies LLC
 */

#include "Test.h"
#include "SpatialPolygon.h"

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

	if(true) {
		/* CCW */
		LatLonDegrees64ValueVector latlon6ccw;
		latlon6ccw.push_back(LatLonDegrees64(-4,0));
		latlon6ccw.push_back(LatLonDegrees64(-4,1));
		latlon6ccw.push_back(LatLonDegrees64(-3,1));
		latlon6ccw.push_back(LatLonDegrees64(-3.5,0.5));
		latlon6ccw.push_back(LatLonDegrees64(-3,0));
		latlon6ccw.push_back(LatLonDegrees64(-4,0));

		Vertices vs;
		for( uint i=0; i < latlon6ccw.size(); ++i ) {
			SpatialVector v;
			v.setLatLonDegrees(latlon6ccw[i].lat, latlon6ccw[i].lon);
			vs.push_back(v);
		}

		// uint64 i0 = 0;
		uint64 i0   = 0x3d2b61000000000a;
	    // uint64 i0   = 0x3d2b600000000009;
		uint64 ix   = 0x3d2b61800000000d;

		SpatialPolygon p(vs);
		Triangle tr = index.TriangleFromValue(i0, 10);
		float64 clat,clon; tr.centroid.getLatLonDegrees(clat, clon);
/*
		cout << " tr 0 " << tr.vertices[0] << " " << p.intersection(tr.vertices[0]) << endl;
		cout << " tr 1 " << tr.vertices[1] << " " << p.intersection(tr.vertices[1]) << endl;
		cout << " tr 2 " << tr.vertices[2] << " " << p.intersection(tr.vertices[2]) << endl;
		cout << " c    " << tr.centroid    << " " << p.intersection(tr.centroid) << endl;
		cout << " c ll " << clat << " " << clon << endl;
		cout << " tr count " << p.intersect_triangle(tr) << endl << flush;
		cout << " tr crossp " << p.triangle_crossp(tr) << endl << flush;
 */
		ASSERT_EQUAL(0,p.intersect_triangle(tr));
		ASSERT_EQUAL(1,p.triangle_crossp(tr));

#if 0

		// FAIL();

		STARE_SpatialIntervals cover_ccw = index.NonConvexHull(latlon6ccw,13);

		SpatialRange interior(cover_ccw);

//		cout << "i0: " << hex << i0 << endl << flush;
/*
		for(uint i=0; i<cover_ccw.size(); ++i) {
			if(!interior.contains(cover_ccw[i])) {
				cout << i << " ccw? " << hex << cover_ccw[i] << dec << endl << flush;
			}
			int cmp_ = cmpSpatial(i0,cover_ccw[i]);
			if( cmp_ ) {
				cout << i << " ccw in i0 " << hex << cover_ccw[i] << dec << endl << flush;
			}
		}
 */

		int cmp_ = cmpSpatial(i0,ix);
//		cout << " i0,ix: " << hex << i0 << " " << ix << dec << " " << cmp_ << endl << flush;
		// uint64 di_a = 0x0000010000000000;
		STARE_SpatialIntervals si;
		si.push_back(i0);
		SpatialRange test(si);
		SpatialRange *overlap = sr_intersect(interior,test);
		KeyPair kp;
		overlap->reset();
		int at_least_one = overlap->getNext(kp);
		cout << "cw kp: " << hex << kp.lo << " " << kp.hi << dec << " " << at_least_one << endl << flush;
		cout << "ccw size: " << cover_ccw.size() << endl << flush;
#endif

#if 0
		for( uint i=0; i < cover_ccw.size(); ++i ) {

			int cmp_ = cmpSpatial(i0,cover_ccw[i]);
//			if( cmp_ != 0 ) {
			if( true ) {
				cout << i << " ccw " << hex << cover_ccw[i] << dec << " " << cmp_ << endl << flush;
			}
		}
#endif

#if 0
		/*
		859 ccw 3d2b61800000000d 0
		860 ccw 3d2b61820000000d 0
		861 ccw 3d2b61840000000d 0
		862 ccw 3d2b61860000000d 0
		863 ccw 3d2b61900000000d 0
		864 ccw 3d2b61920000000d 0
		865 ccw 3d2b61940000000d 0
		866 ccw 3d2b61960000000d 0
		867 ccw 3d2b61980000000d 0
		868 ccw 3d2b619a0000000d 0
		869 ccw 3d2b619c0000000d 0
		870 ccw 3d2b619e0000000d 0
		871 ccw 3d2b61c00000000d 0
		872 ccw 3d2b61c20000000d 0
		873 ccw 3d2b61c40000000d 0
		874 ccw 3d2b61c60000000d 0
		875 ccw 3d2b61c80000000d 0
		876 ccw 3d2b61ca0000000d 0
		877 ccw 3d2b61cc0000000d 0
		878 ccw 3d2b61ce0000000d 0
		879 ccw 3d2b61d80000000d 0
		880 ccw 3d2b61da0000000d 0
		881 ccw 3d2b61dc0000000d 0
		882 ccw 3d2b61de0000000d 0
		883 ccw 3d2b61e80000000d 0
		884 ccw 3d2b61ea0000000d 0
		885 ccw 3d2b61ec0000000d 0
		886 ccw 3d2b61ee0000000d 0
		*/

		/* CW
	    lat6 = np.array([ -2-2, -2-2, -1-2, -1.5-2, -1-2], dtype=np.double)[::-1]
	    lon6 = np.array([ 0,1,1,0.5,0], dtype=np.double)[::-1]
	    */
		LatLonDegrees64ValueVector latlon6cw;
		latlon6cw.push_back(LatLonDegrees64(-3,0));
		latlon6cw.push_back(LatLonDegrees64(-3.5,0.5));
		latlon6cw.push_back(LatLonDegrees64(-3,1));
		latlon6cw.push_back(LatLonDegrees64(-4,1));
		latlon6cw.push_back(LatLonDegrees64(-4,0));
		latlon6cw.push_back(LatLonDegrees64(-3,0));
		STARE_SpatialIntervals cover_cw = index.NonConvexHull(latlon6cw,13);
#endif

		// FAIL();
	}

#undef SIVOUT
#undef SISOUT
#undef DIAG
// TODO Add intersection tests.

}

