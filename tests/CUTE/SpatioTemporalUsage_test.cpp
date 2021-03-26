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

// #define TAG(X) cout << dec << X << hex << endl << flush;
// #define TAGNC(X) cout << dec << X << hex << flush;
#define ENDL(out) if(globalPrintFlag) { out << endl << flush; }
// #define ENDL(out) { out << endl << flush; }
#define TAG(x) {}
#define TAGNC(x) {}
// #define ENDL(out) {}
#define INDEX_OUT0(lbl,var) if(globalPrintFlag) { cout << dec << lbl << " nat,trad,scidb: " << var.stringInNativeDate()	<< flush << ", " << var.toStringJulianTAI() << flush << endl; }
#define INDEX_OUTN(lbl,var) if(globalPrintFlag) { cout << dec << lbl << " nat,trad,scidb: " << var.stringInNativeDate()	<< flush << endl; }
#define INDEX_OUTN_NC(lbl,var) if(globalPrintFlag) { cout << dec << lbl << " nat,trad,scidb: " << var.stringInNativeDate()	<< flush; }
#define INDEX_OUT(lbl,var)  if(globalPrintFlag) { cout << dec << lbl << " nat,trad,scidb: " << var.stringInNativeDate()	<< flush << ", " << var.toStringJulianTAI() << flush << ", 0x" << setw(16) << setfill('0') << hex << var.scidbTemporalIndex() << dec << ", " << setfill(' ') << setw(22) << var.scidbTemporalIndex() << endl << flush; }
#define INDEX_OUTNC(lbl,var)  if(globalPrintFlag) { cout << dec << lbl << " nat,trad,scidb: " << var.stringInNativeDate()	<< flush << ", " << var.toStringJulianTAI() << flush << ", 0x" << setw(16) << setfill('0') << hex << var.scidbTemporalIndex() << dec << ", " << setfill(' ') << setw(22) << var.scidbTemporalIndex(); }

void SpatioTemporalUsage_test() {

  bool globalPrintFlag = true; // false
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
    cout << " tI0 == tI1 : " << ( tI0 == tI1 ) << endl << flush;
    
    ENDL(cout);

    double d1,d2;

    dt.toJulianTAI(d1,d2);
    cout << "dt -> d1,d2:    " << d1 << " " << d2 << endl << flush;
    cout << "dt -> int64 ms: " << dt.toInt64Milliseconds() << endl << flush;
    
  }
  
  FAIL();
  
}
