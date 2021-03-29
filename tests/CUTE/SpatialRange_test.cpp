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
#include "data/sids.h"

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
#undef DIAG

#define DIAGOUT(x)
// #define DIAGOUT(x) cout << x << endl << flush;

void SpatialRange_test () {

#define FMTX(x) " 0x" << setfill('0') << setw(16) << hex << x << dec

  if(false)
  { // pystare bug#27-1

#if 0
    STARE_SpatialIntervals sids = {
      2521898143583305738, 2521997099629805578, 2521989952804225034,
      2521991052315852810, 2521893745536794634, 2521897044071677962,
      2521897593827491850, 2521898693339119626, 2521899792850747402,
      2521991602071666698, 2521889347490283530, 2521899243094933514,
      2521900892362375178, 2521901442118189066, 2521901991874002954,
      2521903091385630730, 2521887698222841866, 2521886048955400202,
      2521992701583294474, 2521902541629816842, 2521906389920514058,
      2521890447001911306, 2521993251339108362, 2521886598711214090,
      2521908588943769610, 2521892646025166858, 2521894295292608522,
      2521989403048411146 };
#endif

    STARE_SpatialIntervals sids = {
      0x22ff98a00000000b,
      0x22ff98e00000000b,
      0x22ff96a00000000b,
      0x22ff96c00000000b,
      0x22ff96e00000000b,
      0x22ffea400000000b,
      0x22ffeb000000000b,
      0x22ffeb200000000b,
      0x22ffeb400000000b,
      0x22ffef000000000b,
      0x22ff8d200000000b,
      0x22ffea200000000b,
      0x22ff99200000000b,
      0x22ff9ea00000000b,
      0x22ff91a00000000b,
      0x22ff98200000000b,
      0x22ffe8800000000b,
      0x22ffe8e00000000b,
      0x22ffe9c00000000b,
      0x22ffef200000000b,
      0x22ffef600000000b,
      0x22ff95400000000b,
      0x22ff95600000000b,
      0x22ff95800000000b,
      0x22ff8d600000000b,
      0x22ff99800000000b,
      0x22ff98400000000b,
      0x22ff94600000000b,
      0x22ff98c00000000b,
      0x22ff94c00000000b,
      0x22ff95c00000000b,
      0x22ffe8c00000000b,
      0x22ff8d400000000b,
      0x22ff97c00000000b,
      0x22ff8bc00000000b,
      0x22ff99400000000b,
      0x22ff90400000000b,
      0x22ff91400000000b,
      0x22ff8e400000000b,
      0x22ffebc00000000b,
      0x22ff97e00000000b,
      0x22ff98600000000b,
      0x22ff99600000000b,
      0x22ff94e00000000b,
      0x22ff8e600000000b,
      0x22ff8a600000000b,
      0x22ff8be00000000b,
      0x22ff95a00000000b,
      0x22ff99a00000000b,
      0x22ff94000000000b,
      0x22ff99c00000000b,
      0x22ff95e00000000b,
      0x22ff99e00000000b,
      0x22ff8a000000000b,
      0x22ffea000000000b,
      0x22ff96200000000b,
      0x22ff8e200000000b,
      0x22ffea600000000b,
      0x22ff8ac00000000b,
      0x22ff8ae00000000b,
      0x22ff8d000000000b,
      0x22ff8e000000000b,
      0x22ff8aa00000000b,
      0x22ff91000000000b,
      0x22ff91200000000b,
      0x22ff91600000000b,
      0x22ff94200000000b,
      0x22ff94a00000000b,
      0x22ff96800000000b,
      0x22ff8b800000000b,
      0x22ff97800000000b,
      0x22ff98000000000b,
      0x22ff9c800000000b,
      0x22ff9e800000000b,
      0x22ff98800000000b,
      0x22ff99000000000b,
      0x22ff9ee00000000b,
      0x22ffe8200000000b,
      0x22ff94400000000b,
      0x22ff95000000000b,
      0x22ff95200000000b,
      0x22ff94800000000b,
      0x22ffeb600000000b,
      0x22ffeb800000000b,
      0x22ffeba00000000b,
      0x22ffebe00000000b,
      0x22ff8a800000000b,
      0x22ff8a200000000b
    };
  
    SpatialRange r(sids);

    r.compress();

#if 1
      cout << "\npystare-bug#27" << flush;
      cout << "\ncompress\n" << flush;
      cout << "***r.dump()***" << endl << flush;
      r.dump();
      cout << "-----------------" << endl << flush;
#endif

      STARE_SpatialIntervals sis = r.toSpatialIntervals();
      
      {
	for(int k=0; k<sis.size(); ++k) {
	  cout << "sis "
	       << FMTX(sis[k])
	       << endl << flush;
	}
	cout << "-----------------" << endl << flush;
      }
      
      {
	STARE_SpatialIntervals expanded = expandIntervalsMultiRes(sis,-1,true);
	for(int k=0; k<expanded.size(); ++k) {
	  cout << "exp1 "
	       << FMTX(expanded[k])
	       << endl << flush;
	}
	cout << "-----------------" << endl << flush;
      }
  }

  
  { // pystare bug#27-2

    cout << "-----------------" << endl << flush;
    
    STARE_SpatialIntervals compressed = {
      0x22ff8a000000000b,
      0x22ff8a3fffffffff,
      0x22ff8a600000000b,
      0x22ff8affffffffff
    };
#if 0
    for(int k=0; k<compressed.size(); ++k) {
      cout << "exp1 "
	   << FMTX(compressed[k])
	   << endl << flush;
    }
    cout << "-----------------" << endl << flush;
#endif

    STARE_SpatialIntervals expanded = expandIntervalsMultiRes(compressed,-1,true);
#if 0    
    for(int k=0; k<expanded.size(); ++k) {
      cout << "exp2 "
	   << FMTX(expanded[k])
	   << endl << flush;
    }
    cout << "-----------------" << endl << flush;
#endif
    
    STARE_SpatialIntervals expanded_expected = { // Abuse of the type...
      0x22ff8a000000000b,
      0x22ff8a200000000b,
      0x22ff8a600000000b,
      0x22ff8a800000000a
    };
    for(int k=0; k<expanded.size(); ++k) {
      ASSERT_EQUAL(expanded_expected[k],expanded[k]);
    }
  }

  {
    /*
     resolution = 6
     resolution0 = resolution
     lat0 = numpy.array([ 10, 5, 60,70], dtype=numpy.double)
     lon0 = numpy.array([-30,-20,60,10], dtype=numpy.double)
     hull0 = pystare.to_hull_range_from_latlon(lat0, lon0, resolution0)

     resolution1 = 6
     lat1 = numpy.array([10,  20, 30, 20 ], dtype=numpy.double)
     lon1 = numpy.array([-60, 60, 60, -60], dtype=numpy.double)
     hull1 = pystare.to_hull_range_from_latlon(lat1, lon1, resolution1)
    */

    STARE index;

    {
#define FMTX(x) " 0x" << setfill('0') << setw(16) << hex << x << dec

      STARE_SpatialIntervals sis = {
	0x3bd7800000000006,  0x3bd9ffffffffffff
      };

      SpatialRange r(sis);
#if 0
      cout << "***r.dump()***" << endl << flush;
      r.dump();
      cout << "-----------------" << endl << flush;
#endif

      r.compress();
#if 0
      cout << "\ncompress\n" << flush;
      cout << "***r.dump()***" << endl << flush;
      r.dump();
      cout << "-----------------" << endl << flush;
#endif

      EmbeddedLevelNameEncoding lj;
      STARE_ArrayIndexSpatialValue siv;
      STARE_ArrayIndexSpatialValue term;

      siv = 0x3bd9800000000006;
      lj.setIdFromSciDBLeftJustifiedFormat(siv);      
      term = lj.getSciDBTerminatorLeftJustifiedFormat();
#if 0
      cout << "siv "
	   << FMTX(siv) << " "
	   << FMTX(term)
	   << endl << flush;

      cout << "-----------------" << endl << flush;
#endif

      siv = 0x3bd9800000000005;
      lj.setIdFromSciDBLeftJustifiedFormat(siv);      
      term = lj.getSciDBTerminatorLeftJustifiedFormat();

#if 0
      cout << "siv "
	   << FMTX(siv) << " "
	   << FMTX(term)
	   << endl << flush;
      
      cout << "-----------------" << endl << flush;
#endif

      {

	EmbeddedLevelNameEncoding lj;

	lj.setIdFromSciDBLeftJustifiedFormat(sis[0]); // Lower bound of interval
	uint64 bareLo  = lj.bareId();
	uint32 levelLo = lj.getLevel();
	
	lj.setIdFromSciDBLeftJustifiedFormat(sis[1]); // Terminator

	EmbeddedLevelNameEncoding lj_hi_last_value = lj.atLevel(levelLo);
	uint64 bareHi = lj_hi_last_value.bareId();
	uint64 delta = bareHi-bareLo+1; // Number of triangles at levelLo in the interval

	uint64 one_mask_to_level, one_at_level;
	lj.SciDBincrement_LevelToMaskDelta(levelLo,one_mask_to_level, one_at_level);

#if 0
	cout << "levelLo " << levelLo << endl << flush;
	cout << "delta   " << delta << endl << flush;
#endif

	uint64 si  = sis[0];
	lj.setIdFromSciDBLeftJustifiedFormat(si);
	uint64 si_ = lj.getId();
	for(int k=0; k<delta; ++k) {
	  uint64 bareId = lj.bareId();
#if 0
	  cout << k << " " << FMTX(lj.getSciDBLeftJustifiedFormat()) << " " << FMTX(bareId) << " ltn: " << lj.getLocalTriangleNumber() << " " << FMTX(lj.getId()) << " " << FMTX(lj.increment(lj.getId(),levelLo)) << endl << flush;
#endif
	  si_ = lj.increment(si_,levelLo); // Note no change in level // Question -- how to put si into a new lj?
	  lj.setId(si_);
	  si += one_at_level;
	}

	si = sis[0];
	lj.setIdFromSciDBLeftJustifiedFormat(si);
	uint64 lj_id0 = lj.getId();
	lj.setId(lj_id0);
	uint64 lj_id1 = lj.getId();
#if 0
	cout << "check 0: " << FMTX(si) << " " << FMTX(lj_id0) << " " << FMTX(lj_id1) << endl << flush;
#endif
	lj.setIdFromSciDBLeftJustifiedFormat(si+one_at_level);
	lj_id0 = lj.getId();
	lj_id1 = lj.increment(lj_id1,levelLo);
#if 0
	cout << "check 1: " << FMTX((si+one_at_level)) << " " << FMTX(lj_id0) << " " << FMTX(lj_id1) << endl << flush;
#endif
	
      }

#if 0
      {
	STARE_SpatialIntervals expanded = expandIntervalsMultiRes(sis,-1,false);
	for(int k=0; k<expanded.size(); ++k) {
	  cout << "exp0 "
	       << FMTX(expanded[k])
	       << endl << flush;
	}
	cout << "-----------------" << endl << flush;
      }
#endif

#if 0
      {
	STARE_SpatialIntervals expanded = expandIntervalsMultiRes(sis,-1,true);
	for(int k=0; k<expanded.size(); ++k) {
	  cout << "exp1 "
	       << FMTX(expanded[k])
	       << endl << flush;
	}
	cout << "-----------------" << endl << flush;
      }
#endif

#if 0
      cout << "-----------------" << endl << flush;
      cout << "-----------------" << endl << flush;
      cout << "-----------------" << endl << flush;
#endif
    }


    

#define NPTS 4
    float64 lat0[NPTS] = { 10, 5, 60, 70 };
    float64 lon0[NPTS] = { -30, -20, 60, 10 };
    float64 lat1[NPTS] = { 10, 20, 30, 20 };
    float64 lon1[NPTS] = { -60, 60, 60, -60 };

    int resolution0 = 6;
    LatLonDegrees64ValueVector points0;
    for(int i=0; i<NPTS; ++i) {
      points0.push_back(LatLonDegrees64(lat0[i], lon0[i]));
    }

    STARE_SpatialIntervals hull0 = index.ConvexHull(points0, resolution0);
#if 0
    cout << "hull0 size: " << hull0.size() << endl << flush;
#endif
    
    SpatialRange r0;
    r0.addSpatialIntervals(index.ConvexHull(points0, resolution0));
#if 0
    cout << "r0: nranges " << r0.range->range->nranges() << endl << flush;
#endif    
    int resolution1 = 6;
    LatLonDegrees64ValueVector points1;
    for(int i=0; i<NPTS; ++i) {
      points1.push_back(LatLonDegrees64(lat1[i], lon1[i]));
    }
    SpatialRange r1;
    r1.addSpatialIntervals(index.ConvexHull(points1, resolution1));
#if 0
    cout << "r1: nranges " << r1.range->range->nranges() << endl << flush;
#endif
    STARE_SpatialIntervals hull1 = index.ConvexHull(points1, resolution1);
#if 0
    cout << "hull1 size: " << hull1.size() << endl << flush;
#endif
    SpatialRange* r01_p;
    r01_p = sr_intersect(r0,r1,true);

    SpatialRange r01_;
    r01_.addSpatialRange(*r01_p);
#if 0
    cout << "***r01_.dump()***" << endl << flush;
    r01_.dump();
    cout << "-----------------" << endl << flush;
#endif
    STARE_SpatialIntervals intersection = r01_.toSpatialIntervals();

    // add_intersect is different?

#if 0
    {
      // test
      r0.compress();
      
      Key siv = 0x3aa0000000000004;
      EmbeddedLevelNameEncoding leftJustified;
      leftJustified.setIdFromSciDBLeftJustifiedFormat(siv);
      Key k = leftJustified.maskOffLevelBit();
      HtmRangeMultiLevel_NameSpace::TInsideResult tResult = r0.range->range->tinside(k);
      cout << " srange-contains-testing 0 tResult = { \n"
	   << " incl  : " << tResult.incl      << endl
	   << " level : " << tResult.level     << endl
	   << " mid   : " << FMTX(tResult.mid) << endl
	   << " GH    : " << FMTX(tResult.GH)  << endl
	   << " GL    : " << FMTX(tResult.GL)  << endl
	   << " SH    : " << FMTX(tResult.SH)  << endl
	   << " SL    : " << FMTX(tResult.SL)  << endl
	   << " }" << endl << flush
	;
    }
#endif

#if 0
    {
      for(int i=0; i<intersection.size(); ++i) {
	cout << i << " intersection " << FMTX(intersection[i]) << endl << flush;
      }
    }
#endif
    
    {
      // r0...
      r0.compress(); // Else the big compressed triangles won't be found... Yes, funny: compression makes bigger triangles.
#if 0
      cout << "intersection size: " << intersection.size() << endl << flush;
#endif      
      for(int i = 0; i < 10; ++i) {
#if 0
	cout << i << " srange-contains-testing = " << r0.contains(intersection[i])
	     << " 0x" << setw(16) << hex << intersection[i] << dec
	     << endl << flush;
#endif
	ASSERT_EQUAL(1,r0.contains(intersection[i]));
      }
    }

    
    
  }
  
  {
#define NCSIVS 7
    uint64 coalesce_sivs[NCSIVS] = {
      //       0x300a300000000009,
      0x200a30000000000a,
      0x300a30000000000a,
      0x300a30800000000a,
      0x300a31000000000a,
      0x300a31800000000a,
      0x300a42000000000b,
      0x300a33000000000a
    };

    STARE_SpatialIntervals sivs(coalesce_sivs,coalesce_sivs+NCSIVS);

#ifdef DIAG
    for( int i=0; i < sivs.size(); ++i ) {
      cout << i << " 800 " << hex << sivs[i] << dec << endl << flush;
    }
    cout << endl << flush;
#endif

    SpatialRange sr(sivs);
    
    STARE_SpatialIntervals sr_sivs = sr.toSpatialIntervals();

#ifdef DIAG
    for( int i=0; i < sr_sivs.size(); ++i ) {
      cout << i << " 900 " << hex << sr_sivs[i] << dec << endl << flush;
    }
    cout << endl << flush;
#endif
    
    DIAGOUT(910);
    sr.range->range->verify();

    DIAGOUT(9100);
    sr.range->range->defrag();
    DIAGOUT(9110);    
    sr.range->range->verify();

    DIAGOUT(911);
    sr.compress();
    
    DIAGOUT(912);
    sr.range->range->verify();

    DIAGOUT(913);
    sr_sivs = sr.toSpatialIntervals();

    DIAGOUT(914);

#ifdef DIAG
    for( int i=0; i < sr_sivs.size(); ++i ) {
      cout << i << " 950 " << hex << sr_sivs[i] << dec << endl << flush;
    }
    DIAGOUT(915);
    cout << endl << flush;
#endif

    { int i = 0;
      ASSERT_EQUAL(sr_sivs[i++],0x200a30000000000a);
      ASSERT_EQUAL(sr_sivs[i++],0x300a300000000009);
      ASSERT_EQUAL(sr_sivs[i++],0x300a33000000000a);
      ASSERT_EQUAL(sr_sivs[i++],0x300a42000000000b);
    }

    // exit(1);
    
  }

  {
    DIAGOUT("1000 ");
    int sivlen = 1000;
    // STARE_ArrayIndexSpatialValue siv[sivlen];
    STARE_SpatialIntervals sivs(sids, sids + sivlen);
    SpatialRange sr(sivs);
    DIAGOUT("1100 ");
  }

  {
    DIAGOUT("1200 ");    
    int sivlen = 10000;
    // STARE_ArrayIndexSpatialValue siv[sivlen];
    STARE_SpatialIntervals sivs(sids, sids + sivlen);
    SpatialRange sr(sivs);
    DIAGOUT("1300 ");    
  }

  {
    STARE_SpatialIntervals sivs(sids, sids + SIDSLEN);
    SpatialRange sr(sivs);
  }

  STARE index;
  STARE_ArrayIndexSpatialValue siv = index.ValueFromLatLonDegrees( 30, 30, 8 );
  STARE_SpatialIntervals sis; sis.push_back(siv);

  STARE_SpatialIntervals sis_cleared;
  EmbeddedLevelNameEncoding lj_sis;
  for(auto i0=sis.begin(); i0 != sis.end(); ++i0) {
    if(!terminatorp(*i0)) {
      lj_sis.setIdFromSciDBLeftJustifiedFormat(*i0);
      sis_cleared.push_back(lj_sis.clearDeeperThanLevel(lj_sis.getLevel()).getSciDBLeftJustifiedFormat());
    } else {
      sis_cleared.push_back(*i0);
    }
  }

  SpatialRange range(sis);
  STARE_SpatialIntervals sis_out;

  sis_out = range.toSpatialIntervals();

  // #define DIAG
#ifdef DIAG
  for(int i=0; i < sis.size(); ++i ) {
    cout << i << " i,si: " << setw(16) << setfill('0') << hex << sis[i] << dec << endl << flush;
  }
  for(int i=0; i < sis_cleared.size(); ++i ) {
    cout << i << " i,sc: " << setw(16) << setfill('0') << hex << sis_cleared[i] << dec << endl << flush;
  }  
  for(int i=0; i < sis_out.size(); ++i ) {
    cout << i << " i,so: " << setw(16) << setfill('0') << hex << sis_out[i] << dec << endl << flush;
  }
#endif
  // #undef DIAG

  ASSERT_EQUAL(1,sis_out.size());
  ASSERT_EQUAL(sis_cleared[0],sis_out[0]);

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
  DIAGOUT(50);

  range.purge();
  range.addSpatialIntervals(sis);

  DIAGOUT(100);
  
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

  {
    STARE_SpatialIntervals sis_cleared;
    EmbeddedLevelNameEncoding lj_sis;
    for(auto i0=sis.begin(); i0 != sis.end(); ++i0) {
      if(!terminatorp(*i0)) {
        lj_sis.setIdFromSciDBLeftJustifiedFormat(*i0);
        sis_cleared.push_back(lj_sis.clearDeeperThanLevel(lj_sis.getLevel()).getSciDBLeftJustifiedFormat());
      } else {
        sis_cleared.push_back(*i0);
      }
    }

#undef DIAG
    // #define DIAG
#ifdef DIAG
    for(int i=0; i < sis.size(); ++i ) {
      cout << i << " i,si: " << setw(16) << setfill('0') << hex << sis[i] << dec << endl << flush;
    }
    for(int i=0; i < sis_cleared.size(); ++i ) {
      cout << i << " i,sc: " << setw(16) << setfill('0') << hex << sis_cleared[i] << dec << endl << flush;
    }  
    for(int i=0; i < sis_out.size(); ++i ) {
      cout << i << " i,so: " << setw(16) << setfill('0') << hex << sis_out[i] << dec << endl << flush;
    }
#endif
    // #undef DIAG
  
    ASSERT_EQUAL(2,sis_out.size());
    ASSERT_EQUAL(sis_cleared[0],sis_out[0]);
    ASSERT_EQUAL(sis_cleared[1],sis_out[1]);
  }

  if(true) {
    // TODO Fix the following to use the new 0/1 variables defined above.

#undef DIAG
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
    sis1.push_back(siv1);
    siv1+=6*one_at_level;
    leftJustified.setIdFromSciDBLeftJustifiedFormat(siv1);
    sis1.push_back(leftJustified.getSciDBTerminatorLeftJustifiedFormat());
    range1.addSpatialIntervals(sis1);
    SISOUT("sis1",sis1);

    // cout << 100 << endl << flush;
    DIAGOUT(100);
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
    DIAGOUT(200);
    // cout << 200 << " dR nR = " << deltaRange->range->range->nranges() << endl << flush;
    STARE_SpatialIntervals deltaSis = deltaRange->toSpatialIntervals();
    // cout << 300 << endl << flush;
    SISOUT("deltaSis",deltaSis);
    // cout << 400 << endl << flush;
    DIAGOUT(400);
    ASSERT_EQUAL(sis1[0],deltaSis[0]);
    ASSERT_EQUAL(sis0[1],deltaSis[1]);

    delete deltaRange;

#undef SIVOUT
#undef SISOUT

  }


  if(true) {
    // TODO Fix the following to use the new 0/1 variables defined above.
    // #define DIAG
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
    siv1 = ( siv1 & ~leftJustified.levelMaskSciDB ) | 10;
    sis1.push_back(siv1);
    siv1+=6*one_at_level;
    leftJustified.setIdFromSciDBLeftJustifiedFormat(siv1);
    sis1.push_back(leftJustified.getSciDBTerminatorLeftJustifiedFormat());
    range1.addSpatialIntervals(sis1);
    SISOUT("sis1",sis1);

    DIAGOUT(500);
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
    DIAGOUT(500199);
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

  DIAGOUT(500500);  

  // TODO Write many more tests & consider edge cases.
  if(true) {
    STARE_ArrayIndexSpatialValue siv1[2] = { 0x0000000000000008, 0x000067ffffffffff };
    STARE_ArrayIndexSpatialValue siv2[2] = { 0x000030000000000a, 0x0000907fffffffff };
    DIAGOUT(500600);
    STARE_SpatialIntervals sis1(siv1,siv1+2);
    DIAGOUT(500610);
    STARE_SpatialIntervals sis2(siv2,siv2+2);
    DIAGOUT(500620);
    SpatialRange r1(sis1), r2(sis2);
    DIAGOUT(500630);
    SpatialRange *ri = r1 & r2;
    DIAGOUT(500640);
    // SpatialRange *ri = r1 & r2;
    // SpatialRange *ri = sr_intersect(r1, r2);

    STARE_SpatialIntervals result = ri->toSpatialIntervals();
    ASSERT_EQUAL(2,result.size());
    ASSERT_EQUAL(0x000030000000000a,result[0]);
    ASSERT_EQUAL(0x000067ffffffffff,result[1]);

    DIAGOUT(500700);

    // #define DIAG
#ifdef DIAG
    EmbeddedLevelNameEncoding encoding;
    cout << endl << flush;
    result = ri->toSpatialIntervals();
    for( int i=0; i < result.size(); ++i ) {
      encoding.setIdFromSciDBLeftJustifiedFormat(result[i]);
      cout << setw(4) << i   << " 500701 " << hex << result[i] << dec << endl << flush;
      cout << setw(4) << "-" << "        " << hex 
           << encoding.getSciDBTerminatorLeftJustifiedFormat()
           << dec << endl << flush;
    }
    cout << endl << flush;
#endif
#undef DIAG  

    ri->range->range->CompressionPass();
    result = ri->toSpatialIntervals();

    // #define DIAG
#ifdef DIAG
    cout << endl << flush;
    for( int i=0; i < result.size(); ++i ) {
      encoding.setIdFromSciDBLeftJustifiedFormat(result[i]);
      cout << setw(4) << i   << " 500702 " << hex << result[i] << dec << endl << flush;
      cout << setw(4) << "-" << "        " << hex 
           << encoding.getSciDBTerminatorLeftJustifiedFormat()
           << dec << endl << flush;
    }
    cout << endl << flush;
#endif
#undef DIAG
    
    ASSERT_EQUAL(4,result.size());
    ASSERT_EQUAL(0x0000300000000008,result[0]);
    ASSERT_EQUAL(0x00003fffffffffff,result[1]);
    ASSERT_EQUAL(0x0000400000000007,result[2]);
    ASSERT_EQUAL(0x0000600000000008,result[3]);
    delete ri;

    DIAGOUT(500800);        
		
    ri = sr_intersect(r1, r2, true); // Run a compression pass on the range.

    DIAGOUT(500810);        
    
    result.clear();

    DIAGOUT(500820);        
    
    result = ri->toSpatialIntervals();

    DIAGOUT(500830);        
    
    delete ri;
    ASSERT_EQUAL(4,result.size());
    ASSERT_EQUAL(0x0000300000000008,result[0]);
    ASSERT_EQUAL(0x00003fffffffffff,result[1]);
    ASSERT_EQUAL(0x0000400000000007,result[2]);
    ASSERT_EQUAL(0x0000600000000008,result[3]);
  }

  DIAGOUT(600);

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

  DIAGOUT(700);

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

  DIAGOUT(800);  

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

  DIAGOUT(900);  
  
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
#if 0
    // uint64 i0   = 0x3d2b600000000009;
    uint64 ix   = 0x3d2b61800000000d;
#endif

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

    DIAGOUT(1000);  
    

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

