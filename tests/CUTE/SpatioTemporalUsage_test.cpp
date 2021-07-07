/*
 * SpatioTemporalUsage_test.cpp
 *
 *  Created:    2021 March 24
 *      Author: Michael Lee Rilee
 *
 *  Copyright (C) 2021 Rilee Systems Technologies LLC
 *
 */

#include "Test.h"

#define TAG(X) cout << dec << X << hex << endl << flush;
// #define TAGNC(X) cout << dec << X << hex << flush;
#define ENDL(out) if(globalPrintFlag) { out << endl << flush; }
// #define ENDL(out) { out << endl << flush; }
// #define TAG(x) {}
#define TAGNC(x) {}
// #define ENDL(out) {}
#define INDEX_OUT0(lbl,var) if(globalPrintFlag) { cout << dec << lbl << " nat,trad,scidb: " << var.stringInNativeDate()	<< flush << ", " << var.toStringJulianTAI() << flush << endl; }
#define INDEX_OUTN(lbl,var) if(globalPrintFlag) { cout << dec << lbl << " nat,trad,scidb: " << var.stringInNativeDate()	<< flush << endl; }
#define INDEX_OUTN_NC(lbl,var) if(globalPrintFlag) { cout << dec << lbl << " nat,trad,scidb: " << var.stringInNativeDate()	<< flush; }
#define INDEX_OUT(lbl,var)  if(globalPrintFlag) { cout << dec << lbl << " nat,trad,scidb: " << var.stringInNativeDate()	<< flush << ", " << var.toStringJulianTAI() << flush << ", 0x" << setw(16) << setfill('0') << hex << var.scidbTemporalIndex() << dec << ", " << setfill(' ') << setw(22) << var.scidbTemporalIndex() << endl << flush; }
#define INDEX_OUTNC(lbl,var)  if(globalPrintFlag) { cout << dec << lbl << " nat,trad,scidb: " << var.stringInNativeDate()	<< flush << ", " << var.toStringJulianTAI() << flush << ", 0x" << setw(16) << setfill('0') << hex << var.scidbTemporalIndex() << dec << ", " << setfill(' ') << setw(22) << var.scidbTemporalIndex(); }

#undef FMTX
#define FMTX(x) " 0x" << setfill('0') << setw(16) << hex << x << dec

#define FMTRES(r) " resolution: " << setw(2) << r << ", days: " << setw(16) << tIndex.daysAtResolution(r) << ", sec: " << setw(12) << ( tIndex.millisecondsAtResolution(r)/1000.0 ) << dec

// << ", mask: " << hex << scidbSetBitsFinerThanResolution(tIndex.scidbResolutionAndTypeMask(),r) << dec
// << ", check: " << scidbSetBitsFinerThanResolution(0,r) << dec

void SpatioTemporalUsage_test() {

  bool globalPrintFlag = false; // true
  stringstream ss;
  string failureMessage = "'";

  // Generic use
  {
    TemporalIndex tIndex;

    double d1,d2; // Julian day d = d1 + d2. Use two variables for roundoff control.

    d1 = d2 = 0;
    
    tIndex.setZero().fromJulianTAI(d1,d2);                     INDEX_OUT("jd=0:    ",tIndex);
    tIndex.set_year(2020);                                     INDEX_OUT("y=2020:  ",tIndex);
    tIndex.setZero().set_year(1);                              INDEX_OUT("ti=-1:   ",tIndex);
    tIndex.setZero();                                          INDEX_OUT("ti=-0:   ",tIndex);
    tIndex.setZero().set_BeforeAfterStartBit(1);               INDEX_OUT("ti=+0    ",tIndex);
    tIndex.setZero().set_BeforeAfterStartBit(1).set_year(1);   INDEX_OUT("ti=+1:   ",tIndex);

    // cout << "TAI: " << tIndex.toStringJulianTAI() << endl << flush;
    
  }

  {
    TemporalIndex tI0,tI1,dt;

    ENDL(cout);
    dt.setZero().set_BeforeAfterStartBit(0).set_millisecond(1);
    INDEX_OUT("dt:          ",(dt));
    tI0.setZero();                                          INDEX_OUT("tI=-0-1ms:   ",(tI0+dt));

    tI0.setZero();                                          INDEX_OUT("tI=-0:       ",tI0);
    tI0.setZero().set_BeforeAfterStartBit(1);               INDEX_OUT("tI=-0:       ",tI0);

    dt.setZero().set_BeforeAfterStartBit(1).set_millisecond(1);
    INDEX_OUT("dt:          ",(dt));
    tI0.setZero();                                          INDEX_OUT("tI=-0+1ms:   ",(tI0+dt));
    
    ENDL(cout);
    tI0.setZero();                                          INDEX_OUT("tI=-0:   ",tI0);
    tI1.setZero().set_BeforeAfterStartBit(1);               INDEX_OUT("tI=+0    ",tI1);
#if 0
    cout << " tI0 == tI1 : " << ( tI0 == tI1 ) << endl << flush;
    ENDL(cout);
#endif
    ASSERT_EQUAL(tI0,tI1);

    double d1,d2;
    dt.toJulianTAI(d1,d2);
#if 0
    cout << "dt -> d1,d2:    " << d1 << " " << d2 << endl << flush;
    cout << "dt -> int64 ms: " << dt.toInt64Milliseconds() << endl << flush;
    ENDL(cout);
#endif
    ASSERT_EQUAL( 1ll, dt.toInt64Milliseconds() );

    TemporalIndex tIndex;
    int64_t forward_resolution = 48;
    int64_t reverse_resolution = 48;
#if 0
    cout << "max res: " << tIndex.data.maxResolutionLevel() << endl << flush;
    cout << "zero resolution    " << FMTRES(0) << endl << flush;
    ENDL(cout);
#endif
    ASSERT_EQUAL(48,tIndex.data.maxResolutionLevel());

#if 0
    for( int r=0; r<64; ++r ) {
      ss.str("");
      ss << setw(2) << r;
      cout << ss.str() << FMTRES(r) << endl << flush;
    }
    ENDL(cout);
    cout << "forward_resolution " << FMTRES(forward_resolution) << endl << flush;
    cout << "reverse_resolution " << FMTRES(reverse_resolution) << endl << flush;
    ENDL(cout); ENDL(cout);
#endif

    tIndex.set_BeforeAfterStartBit(1).set_year(2020).set_reverse_resolution(reverse_resolution).set_forward_resolution(forward_resolution);

    globalPrintFlag = true;
    
#if 0
    INDEX_OUT("y=+2020:  ",tIndex);
    cout << "res: " << forward_resolution << " " << reverse_resolution << endl << flush;
    cout << "forward_resolution " << FMTRES(forward_resolution) << endl << flush;
    cout << "reverse_resolution " << FMTRES(reverse_resolution) << endl << flush;
    ENDL(cout);
#endif
    ASSERT_EQUAL(0.001,tIndex.millisecondsAtResolution(48)/1000.0);

    {
      int64_t tm = tIndex.scidbLowerBoundJulianTAI();
      int64_t t0 = tIndex.scidbTemporalIndex();
      int64_t tp = tIndex.scidbTerminatorJulianTAI();

      ASSERT_EQUAL(0x1f8f417efbf9f0c1,tm);
      ASSERT_EQUAL(0x1f900000000030c1,t0);
      ASSERT_EQUAL(0x1f90000000007ffd,tp);
      
      tIndex.fromTemporalIndexValue(tm);

#if 0
      ENDL(cout);
      cout << "tm: " << FMTX(tm); INDEX_OUTNC(" tm",tIndex);
      ENDL(cout);
    
      tIndex.fromTemporalIndexValue(t0);

      cout << "t0: " << FMTX(t0); INDEX_OUTNC(" t0",tIndex);
      ENDL(cout);
    
      tIndex.fromTemporalIndexValue(tp);

      cout << "tp: " << FMTX(tp); INDEX_OUTNC(" tp",tIndex);
      ENDL(cout);
#endif
    }
    
#if 0
    ENDL(cout); ENDL(cout);

    
    tIndex.set_BeforeAfterStartBit(1).set_year(2020).set_reverse_resolution(reverse_resolution).set_forward_resolution(forward_resolution); // reset
    INDEX_OUT("y=+2020:  ",tIndex);
    cout << "forward_resolution " << FMTRES(forward_resolution) << endl << flush;
    cout << "reverse_resolution " << FMTRES(reverse_resolution) << endl << flush;

    {
      int64_t tm = tIndex.scidbLowerBound();
      int64_t t0 = tIndex.scidbTemporalIndex();
      int64_t tp = tIndex.scidbTerminator();

      ENDL(cout);

      tIndex.fromTemporalIndexValue(tm);
      cout << "tm: " << FMTX(tm); INDEX_OUTNC(" tm",tIndex);
      ENDL(cout);
    
      tIndex.fromTemporalIndexValue(t0);
      cout << "t0: " << FMTX(t0); INDEX_OUTNC(" t0",tIndex);
      ENDL(cout);
    
      tIndex.fromTemporalIndexValue(tp);
      cout << "tp: " << FMTX(tp); INDEX_OUTNC(" tp",tIndex);
      ENDL(cout);
    }

    ENDL(cout); ENDL(cout);
    
    forward_resolution = 48;
    reverse_resolution = 48;
    tIndex.set_BeforeAfterStartBit(1).set_year(2020).set_reverse_resolution(reverse_resolution).set_forward_resolution(forward_resolution); // reset
    INDEX_OUT("y=+2020:  ",tIndex);
    cout << "res: " << forward_resolution << " " << reverse_resolution << endl << flush;

    {
      int64_t tm = tIndex.scidbLowerBound();
      int64_t t0 = tIndex.scidbTemporalIndex();
      int64_t tp = tIndex.scidbTerminator();

      ENDL(cout);

      tIndex.fromTemporalIndexValue(tm);
      cout << "tm: " << FMTX(tm); INDEX_OUTNC(" tm",tIndex);
      ENDL(cout);
    
      tIndex.fromTemporalIndexValue(t0);
      cout << "t0: " << FMTX(t0); INDEX_OUTNC(" t0",tIndex);
      ENDL(cout);
    
      tIndex.fromTemporalIndexValue(tp);
      cout << "tp: " << FMTX(tp); INDEX_OUTNC(" tp",tIndex);
      ENDL(cout);
    }

    ENDL(cout); ENDL(cout);

    forward_resolution = 47;
    reverse_resolution = 47;
    tIndex.set_BeforeAfterStartBit(1).set_year(2020).set_reverse_resolution(reverse_resolution).set_forward_resolution(forward_resolution); // reset
    INDEX_OUT("y=+2020:  ",tIndex);
    cout << "forward_resolution " << FMTRES(forward_resolution) << endl << flush;
    cout << "reverse_resolution " << FMTRES(reverse_resolution) << endl << flush;

    {
      int64_t tm = tIndex.scidbLowerBound();
      int64_t t0 = tIndex.scidbTemporalIndex();
      int64_t tp = tIndex.scidbTerminator();

      ENDL(cout);

      tIndex.fromTemporalIndexValue(tm);
      cout << "tm: " << FMTX(tm); INDEX_OUTNC(" tm",tIndex);
      ENDL(cout);
    
      tIndex.fromTemporalIndexValue(t0);
      cout << "t0: " << FMTX(t0); INDEX_OUTNC(" t0",tIndex);
      ENDL(cout);
    
      tIndex.fromTemporalIndexValue(tp);
      cout << "tp: " << FMTX(tp); INDEX_OUTNC(" tp",tIndex);
      ENDL(cout);
    }

    ENDL(cout); ENDL(cout);

    forward_resolution = 38;
    reverse_resolution = 38;
    tIndex.set_BeforeAfterStartBit(1).set_year(2020).set_reverse_resolution(reverse_resolution).set_forward_resolution(forward_resolution); // reset
    INDEX_OUT("y=+2020:  ",tIndex);
    cout << "forward_resolution " << FMTRES(forward_resolution) << endl << flush;
    cout << "reverse_resolution " << FMTRES(reverse_resolution) << endl << flush;

    {
      int64_t tm = tIndex.scidbLowerBound();
      int64_t t0 = tIndex.scidbTemporalIndex();
      int64_t tp = tIndex.scidbTerminator();

      ENDL(cout);

      tIndex.fromTemporalIndexValue(tm);
      cout << "tm: " << FMTX(tm); INDEX_OUTNC(" tm",tIndex);
      ENDL(cout);
    
      tIndex.fromTemporalIndexValue(t0);
      cout << "t0: " << FMTX(t0); INDEX_OUTNC(" t0",tIndex);
      ENDL(cout);
    
      tIndex.fromTemporalIndexValue(tp);
      cout << "tp: " << FMTX(tp); INDEX_OUTNC(" tp",tIndex);
      ENDL(cout);
    }    

    ENDL(cout); ENDL(cout);

    forward_resolution = 21;
    reverse_resolution = 63;
    tIndex.set_BeforeAfterStartBit(1).set_year(2020).set_reverse_resolution(reverse_resolution).set_forward_resolution(forward_resolution); // reset
    INDEX_OUT("y=+2020:  ",tIndex);
    cout << "forward_resolution " << FMTRES(forward_resolution) << endl << flush;
    cout << "reverse_resolution " << FMTRES(reverse_resolution) << endl << flush;

    {
      int64_t tm = tIndex.scidbLowerBound();
      int64_t t0 = tIndex.scidbTemporalIndex();
      int64_t tp = tIndex.scidbTerminator();

      ENDL(cout);

      tIndex.fromTemporalIndexValue(tm);
      cout << "tm: " << FMTX(tm); INDEX_OUTNC(" tm",tIndex);
      ENDL(cout);
    
      tIndex.fromTemporalIndexValue(t0);
      cout << "t0: " << FMTX(t0); INDEX_OUTNC(" t0",tIndex);
      ENDL(cout);
    
      tIndex.fromTemporalIndexValue(tp);
      cout << "tp: " << FMTX(tp); INDEX_OUTNC(" tp",tIndex);
      ENDL(cout);
    }

    ENDL(cout); ENDL(cout);

    forward_resolution = 21;
    reverse_resolution = 21;
    tIndex.set_BeforeAfterStartBit(1).set_year(2020).set_reverse_resolution(reverse_resolution).set_forward_resolution(forward_resolution); // reset
    INDEX_OUT("y=+2020:  ",tIndex);
    cout << "forward_resolution " << FMTRES(forward_resolution) << endl << flush;
    cout << "reverse_resolution " << FMTRES(reverse_resolution) << endl << flush;

    {
      int64_t tm = tIndex.scidbLowerBound();
      int64_t t0 = tIndex.scidbTemporalIndex();
      int64_t tp = tIndex.scidbTerminator();

      ENDL(cout);

      tIndex.fromTemporalIndexValue(tm);
      cout << "tm: " << FMTX(tm); INDEX_OUTNC(" tm",tIndex);
      ENDL(cout);
    
      tIndex.fromTemporalIndexValue(t0);
      cout << "t0: " << FMTX(t0); INDEX_OUTNC(" t0",tIndex);
      ENDL(cout);
    
      tIndex.fromTemporalIndexValue(tp);
      cout << "tp: " << FMTX(tp); INDEX_OUTNC(" tp",tIndex);
      ENDL(cout);
    }
    
    ENDL(cout); ENDL(cout);

    forward_resolution = 63;
    reverse_resolution = 21;
    tIndex.set_BeforeAfterStartBit(1).set_year(2020).set_reverse_resolution(reverse_resolution).set_forward_resolution(forward_resolution); // reset
    INDEX_OUT("y=+2020:  ",tIndex);
    cout << "forward_resolution " << FMTRES(forward_resolution) << endl << flush;
    cout << "reverse_resolution " << FMTRES(reverse_resolution) << endl << flush;

    {
      int64_t tm = tIndex.scidbLowerBound();
      int64_t t0 = tIndex.scidbTemporalIndex();
      int64_t tp = tIndex.scidbTerminator();

      ENDL(cout);

      tIndex.fromTemporalIndexValue(tm);
      cout << "tm: " << FMTX(tm); INDEX_OUTNC(" tm",tIndex);
      ENDL(cout);
    
      tIndex.fromTemporalIndexValue(t0);
      cout << "t0: " << FMTX(t0); INDEX_OUTNC(" t0",tIndex);
      ENDL(cout);
    
      tIndex.fromTemporalIndexValue(tp);
      cout << "tp: " << FMTX(tp); INDEX_OUTNC(" tp",tIndex);
      ENDL(cout);

      ENDL(cout);
      cout << "t0' " << FMTX(scidbSetBitsFinerThanResolution(t0,21)) << endl <<flush;
      cout << "msk " << FMTX(tIndex.scidbResolutionAndTypeMask()) << endl << flush;
      cout << "t0k " << FMTX((scidbSetBitsFinerThanResolution(t0,21) & tIndex.scidbResolutionAndTypeMask())) << endl << flush;
      cout << "t0K " << FMTX((scidbSetBitsFinerThanResolution(t0,21) & ~tIndex.scidbResolutionAndTypeMask())) << endl << flush;
    }
    
#endif
    
    /*
      NOTES MLR 2021-0707-1241-1

      Most moved to TemporalIndex.h/cpp.
      - Provided lots of standalone functions

      TODOs & Tests
      - resolution(time) - See TemporalIndex: forward_resolution() and reverse_resolution()
      - coarsestResolutionFinerOrEqualMilliseconds(milliseconds)
      - clear-below-resolution 
      -- See TemporalIndex: scidbClearBitsFinerThanResolution, etc.
      - terminate-below-resolution
      // conflation of time and interval
      logic
      - overlap
      -- scidbTemporalValueUnionIfOverlap
      -- scidbTemporalValueIntersectionIfOverlap
      - contains
      -- scidbContainsInstant
      */
    
  }
  
  // FAIL();
  
}
