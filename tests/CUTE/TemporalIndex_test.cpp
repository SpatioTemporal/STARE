/*
 * STARE_Temporal_test.cpp
 *
 *  Created on: Apr 16, 2019
 *      Author: mrilee
 *
 *  Copyright (C) 2019 Rilee Systems Technologies LLC
 */

#include "Test.h"

// #define TAG(X) cout << dec << X << hex << endl << flush;
// #define TAGNC(X) cout << dec << X << hex << flush;
#define ENDL(out) if(globalPrintFlag) { out << endl << flush; }
// #define ENDL(out) { out << endl << flush; }
#define TAG(x) {}
#define TAGNC(x) {}
// #define ENDL(out) {}

#include <vector>
#include <string>

void TemporalIndex_test() {

  bool globalPrintFlag = false; // true

	stringstream ss;

	string failureMessage = "'";

	TemporalIndex tIndex, tIndex1;

	ASSERT_EQUAL("year",  tIndex.data.get("year")->getName());
	ASSERT_EQUAL("minute",tIndex.data.get("minute")->getName());

	ASSERT_EQUAL(0,tIndex.data.getValue("year"));
	tIndex.data.setValue("year",1);
	tIndex.data.setValue("month",2);
	tIndex.data.setValue("day",3);
	ASSERT_EQUAL(1,tIndex.data.getValue("year"));
	ASSERT_EQUAL(2,tIndex.data.getValue("month"));
	ASSERT_EQUAL(3,tIndex.data.getValue("day"));

	// tIndex.data.print();
	// cout << "month mask: " << tIndex.data.get("month")->getMask() << endl << flush;

	// cout << "day: " << tIndex.data.getValue("day") << endl << flush;
	// cout << "scidb idx: " << hex << tIndex.scidbTemporalIndex() << endl << flush;

	TAG(0)
	tIndex.setZero();
	TAG(1)

#define INDEX_OUT0(lbl,var) if(globalPrintFlag) { cout << dec << lbl << " nat,trad,scidb: " << var.stringInNativeDate()	<< flush << ", " << var.toStringJulianTAI() << flush << endl; }
#define INDEX_OUTN(lbl,var) if(globalPrintFlag) { cout << dec << lbl << " nat,trad,scidb: " << var.stringInNativeDate()	<< flush << endl; }
#define INDEX_OUTN_NC(lbl,var) if(globalPrintFlag) { cout << dec << lbl << " nat,trad,scidb: " << var.stringInNativeDate()	<< flush; }
#define INDEX_OUT(lbl,var)  if(globalPrintFlag) { cout << dec << lbl << " nat,trad,scidb: " << var.stringInNativeDate()	<< flush << ", " << var.toStringJulianTAI() << flush << ", 0x" << setw(16) << setfill('0') << hex << var.scidbTemporalIndex() << dec << ", " << setfill(' ') << setw(22) << var.scidbTemporalIndex() << endl << flush; }
#define INDEX_OUTNC(lbl,var)  if(globalPrintFlag) { cout << dec << lbl << " nat,trad,scidb: " << var.stringInNativeDate()	<< flush << ", " << var.toStringJulianTAI() << flush << ", 0x" << setw(16) << setfill('0') << hex << var.scidbTemporalIndex() << dec << ", " << setfill(' ') << setw(22) << var.scidbTemporalIndex(); }
	  

	// ASSERT_EQUAL("",tIndex.stringInNativeDate());
	// ASSERT_EQUAL("",tIndex.toStringJ());
	// ASSERT_EQUAL(,tIndex.scidbTemporalIndex());

	failureMessage = "'";
	try {
	  TAG(900)
	  tIndex = TemporalIndex(0,0,0,0,0,0,0,0,0,0,0,1); // TAG(1000); // INDEX_OUT0(1000,tIndex); TAG(1001); INDEX_OUT(1001,tIndex);
	  TAG(901)
	} catch (SpatialFailure& failure) {
		failureMessage += failure.what();
		// cout << "failureMessage: " << failureMessage << "'" << endl << flush;
		ASSERT_EQUAL("'TemporalWordFormat:UnderflowError'",failureMessage);
	};

	failureMessage = "'";
	try {
		TAG(1000)
//		tIndex = TemporalIndex(0,0,0,0,0,0,0,0,0,0,0,1); // TAG(1000); // INDEX_OUT0(1000,tIndex); TAG(1001); INDEX_OUT(1001,tIndex);
//		TAG(1001)
		ASSERT_EQUAL("-000000000-00-0-0 00:00:00.000 (00 00) (1)",tIndex.stringInNativeDate());
		TAG(1002)
		ASSERT_EQUAL("0 000000001-01-01 00:00:00.000 (00 00) (1)",tIndex.toStringJulianTAI());
		TAG(1003)
		  INDEX_OUT(1003,tIndex);
		//?? mlr 2020-1113		ASSERT_EQUAL(0xfffff20a041018fe,tIndex.scidbTemporalIndex());
		ASSERT_EQUAL(0xfffc828104063fff,tIndex.scidbTemporalIndex());
		TAG(1100)
		  tIndex = TemporalIndex(1,1,0,0,0,0,0,0,0,0,0,1); // TAG(1100); // INDEX_OUT0(1100,tIndex);
		TAG(1101); INDEX_OUT(1101,tIndex);
		ASSERT_EQUAL("+000000001-00-0-0 00:00:00.000 (00 00) (1)",tIndex.stringInNativeDate());
		ASSERT_EQUAL("1 000000001-01-01 00:00:00.000 (00 00) (1)",tIndex.toStringJulianTAI());
		ASSERT_EQUAL(0x0004000000000001,tIndex.scidbTemporalIndex());
		TAG(1200)
		// ENDL(cout);
	} catch (SpatialFailure& failure) {
		failureMessage += failure.what();
		// cout << "failureMessage: " << failureMessage << "'" << endl << flush;
		ASSERT_EQUAL("'TemporalWordFormat:UnderflowError'",failureMessage);
	};
	TAG(2)
	/*
	int64_t BeforeAfterStartBit,
	int64_t year,
	int64_t month,
	int64_t week,
	int64_t day,
	int64_t hour,
	int64_t minute,
	int64_t second,
	int64_t millisecond,
	int64_t resolution,
	int64_t type
	*/
	failureMessage = "'";
	try {
	  tIndex = TemporalIndex(0,0,0,0,0,0,0,0,0,0,0,1); tIndex.data.decrementAtName("millisecond"); INDEX_OUT0(1500,tIndex);
	} catch (SpatialFailure& failure) {
		failureMessage += failure.what();
		failureMessage += "'";
		// cout << "failureMessage: " << failureMessage << "'" << endl << flush;
		ASSERT_EQUAL("'TemporalWordFormat:UnderflowError'",failureMessage);
	};

	TAG(3)

	// remove CE from tai calls.
	tIndex.fromFormattedJulianTAI(1, 12, 31, 23, 59, 59, 998); INDEX_OUT(0,tIndex);
	tIndex1.fromStringJulianTAI(tIndex.toStringJulianTAI()); INDEX_OUT(0,tIndex1);
	ASSERT_EQUAL("+000000001-13-0-0 23:59:59.998 (00 00) (1)",tIndex.stringInNativeDate());
	ASSERT_EQUAL("1 000000001-12-31 23:59:59.998 (00 00) (1)",tIndex.toStringJulianTAI());

	tIndex.setZero().fromFormattedJulianTAI(1, 12, 31, 23, 59, 59, 999); // INDEX_OUT(1,tIndex);
	ASSERT_EQUAL("+000000001-13-0-0 23:59:59.999 (00 00) (1)",tIndex.stringInNativeDate());
	ASSERT_EQUAL("1 000000001-12-31 23:59:59.999 (00 00) (1)",tIndex.toStringJulianTAI());

	tIndex.fromFormattedJulianTAI(2, 1, 1, 0, 0, 0, 0); // INDEX_OUT(2,tIndex);
	ASSERT_EQUAL("+000000002-00-0-0 00:00:00.000 (00 00) (1)",tIndex.stringInNativeDate());
	ASSERT_EQUAL("1 000000002-01-01 00:00:00.000 (00 00) (1)",tIndex.toStringJulianTAI());

	failureMessage = "'";
	try {
		TAG(391)
		  tIndex.fromFormattedJulianTAI(1-1, 12, 31, 23, 59, 59, 999); // INDEX_OUT(3,tIndex);
		TAG(392)
	ASSERT_EQUAL("-000000000-13-0-1 23:59:59.999 (00 00) (1)",tIndex.stringInNativeDate());
		TAG(393)
	ASSERT_EQUAL("0 000000001-12-31 23:59:59.999 (00 00) (1)",tIndex.toStringJulianTAI());
	} catch (SpatialFailure& failure) {
		failureMessage += failure.what();
		cout << "failureMessage: " << failureMessage << "'" << endl << flush;
	};
	TAG(4)

	// ENDL(cout);

	tIndex.fromFormattedJulianTAI(1, 12, 31, 23, 59, 59, 999); INDEX_OUT(4,tIndex);
	ASSERT_EQUAL("+000000001-13-0-0 23:59:59.999 (00 00) (1)",tIndex.stringInNativeDate());
	ASSERT_EQUAL("1 000000001-12-31 23:59:59.999 (00 00) (1)",tIndex.toStringJulianTAI());
	// mlr 2020-1113 ASSERT_EQUAL(0x00001d05fbefe702,tIndex.scidbTemporalIndex());
	ASSERT_EQUAL(0x0007417efbf9c001,tIndex.scidbTemporalIndex());
	
	tIndex.fromFormattedJulianTAI(2, 12, 31, 23, 59, 59, 999);  INDEX_OUT(5,tIndex);
	ASSERT_EQUAL("+000000002-13-0-0 23:59:59.999 (00 00) (1)",tIndex.stringInNativeDate());
	ASSERT_EQUAL("1 000000002-12-31 23:59:59.999 (00 00) (1)",tIndex.toStringJulianTAI());
	// mlr 2020-1113 ASSERT_EQUAL(0x00002d05fbefe702,tIndex.scidbTemporalIndex());
	ASSERT_EQUAL(0x000b417efbf9c001,tIndex.scidbTemporalIndex());

	tIndex.fromFormattedJulianTAI(3, 12, 31, 23, 59, 59, 999);  INDEX_OUT(6,tIndex);
	ASSERT_EQUAL("+000000003-13-0-0 23:59:59.999 (00 00) (1)",tIndex.stringInNativeDate());
	ASSERT_EQUAL("1 000000003-12-31 23:59:59.999 (00 00) (1)",tIndex.toStringJulianTAI());
	// mlr ASSERT_EQUAL(0x00003d05fbefe702,tIndex.scidbTemporalIndex());
	ASSERT_EQUAL(0x000f417efbf9c001,tIndex.scidbTemporalIndex());
	
	tIndex.fromFormattedJulianTAI(4, 12, 31, 23, 59, 59, 999);  INDEX_OUT(7,tIndex);
	ASSERT_EQUAL("+000000004-13-0-1 23:59:59.999 (00 00) (1)",tIndex.stringInNativeDate());
	ASSERT_EQUAL("1 000000004-12-31 23:59:59.999 (00 00) (1)",tIndex.toStringJulianTAI());
	// mlr ASSERT_EQUAL(0x00004d0dfbefe702,tIndex.scidbTemporalIndex());
// <<<<<<< HEAD
// 	// INDEX_OUTNC("Check: ",tIndex);
// =======
// >>>>>>> f80cd105998be058c49e1f2441bd4d00fe06b16f
	ASSERT_EQUAL(0x0013437efbf9c001,tIndex.scidbTemporalIndex());
	
	tIndex.fromFormattedJulianTAI(5, 12, 31, 23, 59, 59, 999);  INDEX_OUT(8,tIndex);
	ASSERT_EQUAL("+000000005-13-0-0 23:59:59.999 (00 00) (1)",tIndex.stringInNativeDate());
	ASSERT_EQUAL("1 000000005-12-31 23:59:59.999 (00 00) (1)",tIndex.toStringJulianTAI());
	// mlr ASSERT_EQUAL(0x00005d05fbefe702,tIndex.scidbTemporalIndex());

	INDEX_OUTNC("Check: ",tIndex);
	ASSERT_EQUAL(0x0017417efbf9c001,tIndex.scidbTemporalIndex());
	
	// ENDL(cout);

	TAG(110)
	tIndex.setZero().fromFormattedJulianTAI(1-1,  1,  1,  0,  0,  0, 0); // ??? -> .set_BeforeAfterStartBit(0);
  INDEX_OUT(1,tIndex);
	ASSERT_EQUAL("-000000000-00-0-0 00:00:00.000 (00 00) (1)",tIndex.stringInNativeDate());
	ASSERT_EQUAL("0 000000001-01-01 00:00:00.000 (00 00) (1)",tIndex.toStringJulianTAI());

	INDEX_OUTNC("Check: ",tIndex);
	ASSERT_EQUAL(0xfffc828104063fff,tIndex.scidbTemporalIndex());

	// cout << "1 nat,trad,scidb: " << tIndex.stringInNativeDate()	<< flush << ", " << tIndex.toStringJ() << flush << ", 0x" << setw(16) << setfill('0') << hex << tIndex.scidbTemporalIndex() << dec << tIndex.scidbTemporalIndex() << endl << flush;
	// cout << "1 nat,trad,scidb: " << tIndex.stringInNativeDate()	<< flush << ", " << tIndex.toStringJ() << flush << hex << tIndex.scidbTemporalIndex() << dec << endl << flush;
	// cout << "1 nat,trad: " << tIndex.stringInNativeDate()  << ", " << tIndex.toStringJ() << endl << flush;
	// TAG(124)
	// cout << "1 scidb idx: 0x" << hex << tIndex.scidbTemporalIndex() << dec << endl << flush;

	TAG(150)
	// tIndex.setZero();
	// Months go 0..11 for hack-traditional
	tIndex.fromFormattedJulianTAI(1-2, 12, 31, 23, 59, 59, 999);
	// tIndex.setJulianFromTraditionalDate(0,  0,  1,  0,  0,  0, 0);
	tIndex.set_BeforeAfterStartBit(0); INDEX_OUT(2,tIndex);
	// cout << "2 nat,trad,scidb: " << tIndex.stringInNativeDate()	<< flush << ", " << tIndex.toStringJ() << flush << ", 0x" << setw(16) << setfill('0') << hex << tIndex.scidbTemporalIndex() << dec << tIndex.scidbTemporalIndex() << endl << flush;
	// cout << "2 nat,trad,scidb: " << tIndex.stringInNativeDate()	<< flush << ", " << tIndex.toStringJ() << flush << hex << tIndex.scidbTemporalIndex() << dec << endl << flush;
	// cout << "2 nat,trad: " << tIndex.stringInNativeDate()  << ", " << tIndex.toStringJ() << endl << flush;
	// cout << "2 scidb idx: 0x" << hex << tIndex.scidbTemporalIndex() << dec << endl << flush;

	TAG(170)
	ASSERT_EQUAL("-000000001-13-0-0 23:59:59.999 (00 00) (1)",tIndex.stringInNativeDate());
	TAG(171)
	ASSERT_EQUAL("0 000000002-12-31 23:59:59.999 (00 00) (1)",tIndex.toStringJulianTAI());

	INDEX_OUTNC("Check: ",tIndex);	
	TAG(177)
	// DomainFailure in year...
	ASSERT_EQUAL(0xfffbc3ffffffffff,tIndex.scidbTemporalIndex());
	// ASSERT_EQUAL(0xffffef0ffffffffe,tIndex.scidbTemporalIndex());

	TAG(178)

	// cout << dec << 1000 << << hex << " " << flush << (tIndex == tIndex) << endl << flush;
	ASSERT_EQUAL(tIndex,tIndex);

	TAG(180)

	  /**/

	TAG(200)
	tIndex.setZero(); tIndex.set_BeforeAfterStartBit(0); // INDEX_OUT(3,tIndex);
	tIndex.fromFormattedJulianTAI( 1,  1,  1,  0,  0,  0, 0);	// INDEX_OUT(3,tIndex);

	ENDL(cout);

	tIndex.fromFormattedJulianTAI( 2, 12, 31, 23, 59, 59, 999); INDEX_OUT(3,tIndex);
	tIndex.fromFormattedJulianTAI( 3, 12, 31, 23, 59, 59, 999); INDEX_OUT(4,tIndex);
	tIndex.fromFormattedJulianTAI( 4, 12, 31, 23, 59, 59, 999); INDEX_OUT(5,tIndex);
	tIndex.fromFormattedJulianTAI( 5, 12, 30, 23, 59, 59, 999); INDEX_OUT(6,tIndex);
	tIndex.fromFormattedJulianTAI( 5, 12, 31, 23, 59, 59, 999); INDEX_OUT(6,tIndex);
	tIndex.fromFormattedJulianTAI( 6, 12, 31, 23, 59, 59, 999); INDEX_OUT(6,tIndex);
	tIndex.fromFormattedJulianTAI( 6,  1,  2, 23, 59, 59, 999); INDEX_OUT(6,tIndex);
	tIndex.fromFormattedJulianTAI( 6,  1,  1, 23, 59, 59, 999); INDEX_OUT(6,tIndex);
	tIndex.fromFormattedJulianTAI( 7, 12, 31, 23, 59, 59, 999); INDEX_OUT(7,tIndex);

	ENDL(cout);

	tIndex.fromFormattedJulianTAI( 2, 12, 31, 23, 59, 59, 999); INDEX_OUT(3,tIndex);
	tIndex.fromFormattedJulianTAI( 3, 12, 31, 23, 59, 59, 999); INDEX_OUT(4,tIndex);
	tIndex.fromFormattedJulianTAI( 4, 12, 31, 23, 59, 59, 999); INDEX_OUT(5,tIndex);
	tIndex.fromFormattedJulianTAI( 5, 12, 30, 23, 59, 59, 999); INDEX_OUT(6,tIndex);
	tIndex.fromFormattedJulianTAI( 5, 12, 31, 23, 59, 59, 999); INDEX_OUT(6,tIndex);
	tIndex.fromFormattedJulianTAI( 6, 12, 31, 23, 59, 59, 999); INDEX_OUT(6,tIndex);
	tIndex.fromFormattedJulianTAI( 6,  1,  2, 23, 59, 59, 999); INDEX_OUT(6,tIndex);
	tIndex.fromFormattedJulianTAI( 6,  1,  1, 23, 59, 59, 999); INDEX_OUT(6,tIndex);
	tIndex.fromFormattedJulianTAI( 7, 12, 31, 23, 59, 59, 999); INDEX_OUT(7,tIndex);
	
	ENDL(cout);

	tIndex.fromFormattedJulianTAI( 6, 1, 1, 0, 0, 0, 0); INDEX_OUT(7,tIndex);
	tIndex.fromFormattedJulianTAI( 6, 1, 1, 0, 0, 0, 1); INDEX_OUT(7,tIndex);

	ENDL(cout);

	tIndex.fromFormattedJulianTAI( 4, 1, 1, 0, 0, 0, 0); INDEX_OUT(8,tIndex);
	tIndex.fromFormattedJulianTAI( 4*1024+1, 12, 31, 23, 59, 59, 999); INDEX_OUT(8,tIndex);
	tIndex.fromFormattedJulianTAI( 4*1024, 1, 1, 0, 0, 0, 0); INDEX_OUT(8,tIndex);
	tIndex.fromFormattedJulianTAI( 4*1024, 1, 1, 0, 0, 0, 1); INDEX_OUT(8,tIndex);
	tIndex.fromFormattedJulianTAI( 4*1024, 1, 1, 0, 0, 0, 0); INDEX_OUT(8,tIndex);
	tIndex.fromFormattedJulianTAI( 4*1024, 1, 1, 0, 0, 0, 1); INDEX_OUT(8,tIndex);

	ENDL(cout);
	tIndex.fromFormattedJulianTAI(  8*1024-1, 1, 1, 0, 0, 0, 1); INDEX_OUT(8,tIndex);
	// tIndex.fromFormattedJulianTAI( -8*1024, 1, 1, 0, 0, 0, 1); INDEX_OUT(8,tIndex); No negative Julian years.

	TAG(300)

	ENDL(cout);
	tIndex.fromFormattedJulianTAI( 2007, 12, 31, 0, 0, 0, 0); INDEX_OUT(9,tIndex);
	tIndex.fromFormattedJulianTAI( 2008, 12, 31, 0, 0, 0, 0); INDEX_OUT(9,tIndex);
	tIndex.fromFormattedJulianTAI( 2009,  1,  1, 0, 0, 0, 0); INDEX_OUT(9,tIndex);
	ENDL(cout);

	/**/

	tIndex.fromFormattedJulianTAI(2009,  1,  1,  0,  0,  0,   0); // INDEX_OUT(9,tIndex);
//tIndex.fromFormattedJulianTAI(   1, 12, 31, 23, 59, 59, 998); INDEX_OUT(0,tIndex);
	TemporalIndex tIndex0;

  TAG(301)
  

	try {
		tIndex0.fromStringJulianTAI(tIndex.toStringJulianTAI());
	} catch (SpatialFailure& failure) {
		failureMessage += failure.what();
		cout << "failureMessage: " << failureMessage << "'" << endl << flush;
	}

	TAG(302)

	cout << dec;

	INDEX_OUT(10,tIndex);
	INDEX_OUT(10,tIndex0);

	// ENDL(cout);

	TAG(302)
	// tIndex.data.print();
	TAG(400)
	tIndex.set_zero();
	tIndex.set_year(5);
	ASSERT_EQUAL( 0, tIndex.get_millisecond());
	ASSERT_EQUAL( 0, tIndex.get_day());
	ASSERT_EQUAL( 0, tIndex.get_BeforeAfterStartBit());
	ASSERT_EQUAL( 5, tIndex.get_year());
	// Note 1 BCE goes to 0 0.
	// cout << "4  nat,trad: " << tIndex.stringInNativeDate()  << ", " << tIndex.toStringJ() << endl << flush;
	// cout << "4  scidb idx: 0x" << hex << tIndex.scidbTemporalIndex() << dec << endl << flush;
	TAG(450)
	tIndex.set_millisecond(1);
	TAG(460)
	// tIndex.data.print();
	TAG(461)
	ASSERT_EQUAL( 1, tIndex.get_millisecond());
	ASSERT_EQUAL( 1, tIndex.set_millisecond(1).get_millisecond());
	TAG(470)
	tIndex.data.decrementAtName("millisecond",1);
	TAG(475)
	ASSERT_EQUAL( 2, tIndex.set_zero().set_millisecond(2).get_millisecond());
	TAG(476)
	// tIndex.data.print();
	TAG(477)
	tIndex.data.decrementAtName("millisecond",1);
	TAG(480)
	// tIndex.data.print();
	TAG(490)
	ASSERT_EQUAL( 1, tIndex.get_millisecond());
	TAG(500)
	// tIndex.data.print();
	TAG(600)
	tIndex.data.incrementAtName("millisecond",1);
	ASSERT_EQUAL( 2, tIndex.get_millisecond());
	TAG(610)
	tIndex.data.incrementAtName("year",1);
	TAG(620)
	// tIndex.data.print();
	ASSERT_EQUAL( 2, tIndex.get_millisecond());
	ASSERT_EQUAL( 1, tIndex.get_year());
	ASSERT_EQUAL( 0, tIndex.get_month());

	TAG(622)
	tIndex.set_year(10);
	tIndex.set_millisecond(0);
	int64_t id = tIndex.data.getFieldId("year");
	// cout << "id: " << dec << id << hex << endl << flush;
	TAG(6230)
	// tIndex.data.print();
	TAG(6231)
	tIndex.data.decrementAtId(id, 1);
	TAG(6232)
	// tIndex.data.print();
	TAG(624)
	tIndex.data.decrementAtName("year",1);
	TAG(625)
	// tIndex.data.print();
	ASSERT_EQUAL( 8, tIndex.get_year());
	TAG(630)
	tIndex.data.setValue("BeforeAfterStartBit",1);
	// tIndex.data.decrementAtName("millisecond",1);
	// tIndex.data.print();
	TAG(631)
	tIndex.data.decrementAtName("year",1);
	ASSERT_EQUAL( 7, tIndex.get_year());

//	cout << "                FEDCBA9876543210" << endl << flush;
//	for( int iRes = 0; iRes < 16; iRes += 1 ) {
//		TAG(632)
//		tIndex.data.setValue("resolution", iRes);
//		TAG(633)
//		// cout << "                FEDCBA9876543210" << endl << flush;
//		cout << "0 scidb idx:  0x" << setw(16) << setfill('0') << hex << tIndex.scidbTemporalIndex() << endl << flush;
//		TAG(640)
//		// TemporalIndex tIndex1(tIndex.scidbTemporalIndex());
//		// cout << "1 scidb idx:  0x" << setw(16) << setfill('0') << hex << tIndex1.scidbTemporalIndex() << endl << flush;
//		TAG(700)
//		int64_t iTerm = tIndex.scidbTerminator();
//		cout << "2 scidb term: 0x" << setw(16) << setfill('0') << hex << iTerm << endl << flush;
//		// tIndex.data.print();
//		TAG(710)
//		// cout << "3 scidb term: 0x" << setw(16) << setfill('0') << hex << tIndex.scidbTerminator() << endl << flush;
//	}

	TAG(719)
	// tIndex.setZero();
	int64_t one = 1;
	// ??? int64_t offsetBase = tIndex.data.get("millisecond")->getOffset();
	int64_t offsetTop = tIndex.data.get("year")->getOffset()+tIndex.data.get("year")->getWidth()-1;
	TemporalIndex tIndex2, tIndex3, tIndex4;
// <<<<<<< HEAD
	// Big year veresion tIndex2.fromFormattedJulianTAI(262144, 12, 31, 23, 59, 59, 999);
	tIndex2.fromFormattedJulianTAI(8192-1, 12, 31, 23, 59, 59, 999);
// =======
// 	tIndex2.fromFormattedJulianTAI(8191, 12, 31, 23, 59, 59, 999);
// >>>>>>> f80cd105998be058c49e1f2441bd4d00fe06b16f

	if(globalPrintFlag) {

	cout << "                  FEDCBA9876543210" << endl << flush;
	TAG(720)
	failureMessage = "";
	try {
		// for(int i=offsetBase; i < offsetBase+10; ++i) {
		// for(int i=offsetBase; i <= offsetTop; ++i) {
		for(int i=43; i <= 43; ++i) {
			int64_t iBit = one << i;
			// iBit = iBit | one << 62; // broken TODO fix overflow
			iBit = iBit | one << 61; // broken TODO fix overflow
			int64_t iResolution = offsetTop-i;
			TAG(730)
			tIndex = TemporalIndex(iBit);
			TAG(731)
			// tIndex.data.setValue("BeforeAfterStartBit",1);
			tIndex.data.setValue("forward_resolution",iResolution);
// <<<<<<< HEAD
// =======
 			tIndex.data.setValue("reverse_resolution",iResolution);
// >>>>>>> f80cd105998be058c49e1f2441bd4d00fe06b16f
			TAG(732)
			tIndex.data.setValue("type",1);
			TAG(733)
			// tIndex.data.print();
			cout << setw(2) << setfill('0') << dec << i << hex;
			cout << " scidb idx:   0x" << hex << setw(16) << setfill('0') << tIndex.scidbTemporalIndex() << ", " << tIndex.scidbTerminatorp()
				<< ", " << tIndex.stringInNativeDate() << ", " << tIndex.toStringJulianTAI()
				<< dec << endl << flush;
			TemporalIndex tIndex1 = TemporalIndex(tIndex.scidbTerminator());
			cout << "            :   0x" << hex << setw(16) << setfill('0') << tIndex.scidbTerminator()
				<< ", " << tIndex1.scidbTerminatorp()
				<< ", " << tIndex1.stringInNativeDate() << ", " << tIndex1.toStringJulianTAI()
				<< dec << endl << endl << flush;
		}
	} catch (SpatialFailure& failure) {
		failureMessage += failure.what();
		cout << "failureMessage: " << failureMessage << "'" << endl << flush;
	};

	cout << "            :   0x" << hex << setw(16) << setfill('0') << tIndex2.scidbTemporalIndex() << flush
	<< ", " << tIndex2.scidbTerminatorp() << flush
	<< ", " << tIndex2.stringInNativeDate() << flush
	<< ", " << tIndex2.toStringJulianTAI() << flush
	<< dec << endl << endl << flush;
	}

	// int not_ok = tIndex2.eraTest();
	// cout << flush;

	TAG(800)

	/*
	tIndex3.set_year(0);
	cout << "0 tI3: " << tIndex3.stringInNativeDate()  << ", " << tIndex3.toStringJ() << endl << flush;
	tIndex3.set_BeforeAfterStartBit(0);
	cout << "1 tI3: " << tIndex3.stringInNativeDate()  << ", " << tIndex3.toStringJ() << endl << flush;
	tIndex3.set_BeforeAfterStartBit(1);
	cout << "2 tI3: " << tIndex3.stringInNativeDate()  << ", " << tIndex3.toStringJ() << endl << flush;

	tIndex3.set_year(1);
	cout << "3 tI3: " << tIndex3.stringInNativeDate()  << ", " << tIndex3.toStringJ() << endl << flush;
	tIndex3.set_BeforeAfterStartBit(0);
	cout << "4 tI3: " << tIndex3.stringInNativeDate()  << ", " << tIndex3.toStringJ() << endl << flush;
	tIndex3.set_BeforeAfterStartBit(1);
	cout << "5 tI3: " << tIndex3.stringInNativeDate()  << ", " << tIndex3.toStringJ() << endl << flush;
	 */

	TAG(900)

	tIndex3.setZero();
	tIndex3.set_type(1);
	tIndex3.set_forward_resolution(62);
	tIndex3.set_reverse_resolution(62);
	tIndex3.set_BeforeAfterStartBit(0);
	tIndex3.set_year(2);
	tIndex3.set_month(3);

	tIndex4.setZero();
	tIndex4.set_type(1);
	tIndex4.set_forward_resolution(62);
	tIndex4.set_reverse_resolution(62);
	tIndex4.set_BeforeAfterStartBit(0);
	tIndex4.set_year(2);
	tIndex4.set_month(4);

	// ENDL(cout);
	// cout << "0 tI3,tI4:        " << hex << &tIndex3 << ", " << &tIndex4 << dec << endl << flush;
	// cout << "0 tI3,tI4 (data): " << hex << &(tIndex3.data) << ", " << &(tIndex4.data) << dec << endl << flush;
	// cout << "0 tI3: " << tIndex3.stringInNativeDate()  << ", " << tIndex3.toStringJ() << endl << flush;
	// cout << "0 tI4: " << tIndex4.stringInNativeDate()  << ", " << tIndex4.toStringJ() << endl << flush;
	// cout << "0 (tI3 < tI4)? " << hex << tIndex3.scidbTemporalIndex() << " " << (tIndex3.scidbTemporalIndex() < tIndex4.scidbTemporalIndex()) << " " << tIndex4.scidbTemporalIndex() << dec << endl << flush;

	ASSERT_LESSM("BC 2-3-1 < BC 2-4-1",tIndex3.scidbTemporalIndex(),tIndex4.scidbTemporalIndex());

	tIndex4.setZero();
	tIndex4.set_type(1);
	tIndex4.set_forward_resolution(62);
	tIndex4.set_reverse_resolution(62);
	tIndex4.set_BeforeAfterStartBit(0);
	tIndex4.set_year(1);
	tIndex4.set_month(3);

	ASSERT_LESSM("BC 2-3-1 < BC 1-3-1",tIndex3.scidbTemporalIndex(),tIndex4.scidbTemporalIndex());

	tIndex4.setZero();
	tIndex4.set_type(1);
	tIndex4.set_forward_resolution(63);
	tIndex4.set_reverse_resolution(63);
	tIndex4.set_BeforeAfterStartBit(0);
	tIndex4.set_year(1);
	tIndex4.set_month(3);

	// cout << "tI3: " << tIndex3.toStringJ() << endl << flush;
	// cout << "tI4: " << tIndex4.toStringJ() << endl << flush;

	ASSERT_EQUAL("0 000000003-03-26 00:00:00.000 (62 62) (1)",tIndex3.toStringJulianTAI());
	ASSERT_EQUAL("0 000000002-03-26 00:00:00.000 (63 63) (1)",tIndex4.toStringJulianTAI());
/*
	failureMessage = "'";
	try {
		// cout << hex;
		// cout << "tI4: sIdx:      0x" << tIndex4.scidbTemporalIndex() << endl << flush;
		// cout << "tI4: sIdx-term: 0x" << tIndex4.scidbTerminator() << endl << flush;
		int64_t tmp_idxTerm = tIndex4.scidbTerminator();
		// cout << dec;
	} catch (SpatialFailure& failure) {
		failureMessage += failure.what();
	}
	failureMessage += "'";
	ASSERT_EQUALM("make a terminator from a terminator => terminator out bounds","'TemporalWordFormat:setTerminatorBelowResolution:ResolutionOutOfBounds'",failureMessage);

	tIndex4.set_resolution(62);
	failureMessage = "'";
	try {
		// cout << hex;
		// cout << "tI4: sIdx:      0x" << tIndex4.scidbTemporalIndex() << endl << flush;
		// cout << "tI4: sIdx-term: 0x" << tIndex4.scidbTerminator() << endl << flush;
		int64_t tmp_idxTerm = tIndex4.scidbTerminator();
		// cout << dec;
	} catch (SpatialFailure& failure) {
		failureMessage += failure.what();
	}
	failureMessage += "'";
	ASSERT_EQUALM("terminator out bounds","'TemporalWordFormat:setTerminatorBelowResolution:ResolutionOutOfBounds'",failureMessage);
*/
	// ENDL(cout);

	tIndex3.fromFormattedJulianTAI(1-1,  1,  1,  0,  0,  0,   0);
	tIndex3.set_forward_resolution(tIndex3.data.maxResolutionLevel());
	tIndex3.set_reverse_resolution(tIndex3.data.maxResolutionLevel());
	INDEX_OUT(3000,tIndex3);
	// tIndex4.fromFormattedJulianTAI(0, 1, 12, 31, 23, 59, 59, 999);
	// cout << dec;
//	cout << "30001"
//			<< " coarsest resolution level: " << dec <<  tIndex3.data.maxResolutionLevel()
//			<< flush << endl;
	int64_t itmp4 = tIndex3.scidbTerminator();
	if(globalPrintFlag) {
	cout << dec;
	cout << "30001" << " 0x"<< setw(16) << setfill('0') << hex << tIndex3.scidbTemporalIndex() << dec << flush << endl;
	cout << "30002" << " 0x"<< setw(16) << setfill('0') << hex << itmp4 << dec << flush << endl;
	}

	failureMessage = "";
	try {
		tIndex4 = TemporalIndex(itmp4);
	} catch ( SpatialFailure& failure ) {
		failureMessage += failure.what();
		cout << "failureMessage: " << failureMessage << "'" << endl << flush;
	}
	if(globalPrintFlag) {
	cout << "30003" << " 0x" << setw(16) << setfill('0') << hex << itmp4 << " " << dec << itmp4 << " res: " << tIndex4.get_forward_resolution() << flush << endl;
	}
	INDEX_OUT(3000,tIndex4);

	tIndex3.set_forward_resolution(53);
	tIndex3.set_reverse_resolution(53);
	INDEX_OUT(3100,tIndex3);

	itmp4 = tIndex3.scidbTerminator();
	if(globalPrintFlag) {
	cout << "z1"
			<< " 0x" << setw(16) << setfill('0') << hex << itmp4 << " " << dec << itmp4
			<< endl << flush;
	}
	tIndex4 = TemporalIndex(itmp4);
	if(globalPrintFlag) {
	cout << "z2" << endl << flush;
	}
	INDEX_OUT(3101,tIndex4);

	if(globalPrintFlag) {
	cout << 3200 << endl << flush;
	}
	tIndex3.set_BeforeAfterStartBit(1).set_year(1).set_forward_resolution(52).set_reverse_resolution(52); INDEX_OUT(3200,tIndex3);
	itmp4 = tIndex3.scidbTerminator(); tIndex4 = TemporalIndex(itmp4); INDEX_OUT(3201,tIndex4);

	if(globalPrintFlag) {
	cout << dec << 3300 << endl << flush;
	cout << "tI3:  idx: 0x" << setw(16) << setfill('0') << hex << tIndex3.scidbTemporalIndex() << " " << dec << tIndex3.scidbTerminatorp() << endl << flush;
	cout << "tI4:  idx: 0x" << setw(16) << setfill('0') << hex << tIndex4.scidbTemporalIndex() << " " << dec << tIndex4.scidbTerminatorp() << endl << flush;

	ENDL(cout);
	cout << "tI3: term: 0x" << setw(16) << setfill('0') << hex << tIndex3.scidbTerminator() << " " << dec << tIndex3.scidbTerminatorp() << endl << flush;
	cout << "tI4: term: 0x" << setw(16) << setfill('0') << hex << tIndex4.scidbTerminator() << " " << dec << tIndex4.scidbTerminatorp() << endl << flush;
	}

	tIndex3.set_BeforeAfterStartBit(1).set_year(1).set_forward_resolution(51).set_reverse_resolution(51); INDEX_OUT(3202,tIndex3);
	itmp4 = tIndex3.scidbTerminator(); tIndex4 = TemporalIndex(itmp4); INDEX_OUT(3203,tIndex4);

	if(globalPrintFlag) {
	cout << dec << 3400 << endl << flush;
	cout << "tI3:  idx: 0x" << setw(16) << setfill('0') << hex << tIndex3.scidbTemporalIndex() << " " << dec << tIndex3.scidbTerminatorp() << endl << flush;
	cout << "tI4:  idx: 0x" << setw(16) << setfill('0') << hex << tIndex4.scidbTemporalIndex() << " " << dec << tIndex4.scidbTerminatorp() << endl << flush;

	ENDL(cout);
	cout << "tI3: term: 0x" << setw(16) << setfill('0') << hex << tIndex3.scidbTerminator() << " " << dec << tIndex3.scidbTerminatorp() << endl << flush;
	cout << "tI4: term: 0x" << setw(16) << setfill('0') << hex << tIndex4.scidbTerminator() << " " << dec << tIndex4.scidbTerminatorp() << endl << flush;
	}

	tIndex3.set_BeforeAfterStartBit(0);
	tIndex4.set_BeforeAfterStartBit(0);

	if(globalPrintFlag) {
	cout << dec << 3500 << endl << flush;
	cout << "tI3:  idx: 0x" << setw(16) << setfill('0') << hex << tIndex3.scidbTemporalIndex() << " " << dec << tIndex3.scidbTerminatorp() << endl << flush;
	cout << "tI4:  idx: 0x" << setw(16) << setfill('0') << hex << tIndex4.scidbTemporalIndex() << " " << dec << tIndex4.scidbTerminatorp() << endl << flush;

	ENDL(cout);
	cout << "tI3: term: 0x" << setw(16) << setfill('0') << hex << tIndex3.scidbTerminator() << " " << dec << tIndex3.scidbTerminatorp() << endl << flush;
	cout << "tI4: term: 0x" << setw(16) << setfill('0') << hex << tIndex4.scidbTerminator() << " " << dec << tIndex4.scidbTerminatorp() << endl << flush;
	}

	tIndex3.set_BeforeAfterStartBit(1).set_forward_resolution(10).set_reverse_resolution(10);
	tIndex4 = TemporalIndex(tIndex3.scidbTerminator());

	{
		TemporalIndex a,b,c; int tag = 4400;

		a.set_BeforeAfterStartBit(1).set_year(1).set_month(3);
		TAG(tag)
		int64_t a_i = a.toInt64Milliseconds();
		TAG(++tag)
		if(globalPrintFlag) ENDL(cout);
		INDEX_OUT(++tag,a);
		if(globalPrintFlag) cout << "a_i " << dec << a_i << endl << flush;
		b.fromInt64Milliseconds(a_i);
		ASSERT_EQUAL(a.stringInNativeDate(),b.stringInNativeDate());
		INDEX_OUT(++tag,b);

		tag = 4450;
		a.set_BeforeAfterStartBit(0).set_year(1).set_month(3);
		TAG(tag)
		a_i = a.toInt64Milliseconds();
		TAG(++tag)
		if(globalPrintFlag) ENDL(cout);
		INDEX_OUT(++tag,a);
		if(globalPrintFlag) cout << "a_i " << dec << a_i << endl << flush;
		b.fromInt64Milliseconds(a_i);
		ASSERT_EQUAL(a.stringInNativeDate(),b.stringInNativeDate());
		INDEX_OUT(++tag,b);

		tag = 4460;
		a.set_BeforeAfterStartBit(0).set_year(4).set_week(2);
		// a.set_BeforeAfterStartBit(1).set_year(1).set_week(0);
		TAG(tag)
		a_i = a.toInt64Milliseconds();
		TAG(++tag)
		if(globalPrintFlag) ENDL(cout);
		INDEX_OUT(++tag,a);
		if(globalPrintFlag) cout << "a_i " << dec << a_i << endl << flush;
		b.fromInt64Milliseconds(a_i);
		ASSERT_EQUAL(a.stringInNativeDate(),b.stringInNativeDate());
		INDEX_OUT(++tag,b);

		tag = 4465;
		a.set_BeforeAfterStartBit(0).setZero();
		TAG(tag)
		a_i = a.toInt64Milliseconds();
		TAG(++tag)
		if(globalPrintFlag) ENDL(cout);
		INDEX_OUT(++tag,a);
		if(globalPrintFlag) cout << "a_i " << dec << a_i << endl << flush;
		b.fromInt64Milliseconds(a_i);
		ASSERT_EQUAL(a.stringInNativeDate(),b.stringInNativeDate());
		INDEX_OUT(++tag,b);

		tag = 446500;
		a.set_BeforeAfterStartBit(0).setZero();
		TAG(tag)
		a_i = a.toInt64Milliseconds() + 31536000000;
		TAG(++tag)
		if(globalPrintFlag) ENDL(cout);
		INDEX_OUT(++tag,a);
		b.fromInt64Milliseconds(a_i);
		INDEX_OUT(++tag,b);
    c.setZero();
		c.set_BeforeAfterStartBit(1);
    c.fromInt64Milliseconds(0);
		INDEX_OUT(++tag,c);
		if(globalPrintFlag) cout << "a   " << dec << a.toInt64Milliseconds() << endl << flush;
    if(globalPrintFlag) cout << "a_i " << dec << a_i << endl << flush;
    int64 b_i = b.toInt64Milliseconds();
		if(globalPrintFlag) cout << "b_i " << dec << b_i << endl << flush;
		if(globalPrintFlag) cout << "c   " << dec << c.toInt64Milliseconds() << endl << flush;
		ASSERT_EQUAL("+000000001-00-0-0 00:00:00.000 (00 00) (1)",b.stringInNativeDate());

		if(globalPrintFlag) {
			INDEX_OUT(++tag,b);
			ENDL(cout);
			cout    << "bitOffsets " << endl
					<< " coarsest    " << tIndex.bitOffsetCoarsest() << endl
					<< " finest      " << tIndex.bitOffsetFinest() << endl
					<< " res(10)     " << tIndex.bitOffsetResolution(10) << endl
					<< " id(res(10)) " << tIndex.bitfieldIdFromResolution(10) << endl
					<< endl << flush;
		}

		ASSERT_EQUAL(tIndex.bitOffsetCoarsest(),62);
		// old type 2 ASSERT_EQUAL(tIndex.bitOffsetFinest(),8);
		ASSERT_EQUAL(tIndex.bitOffsetFinest(),14);
		ASSERT_EQUAL(tIndex.bitOffsetResolution(10),52);
		ASSERT_EQUAL(tIndex.bitfieldIdFromResolution(10),1);

		// ENDL(cout);

		if(globalPrintFlag) {
			int64_t b_ms;
			// int level = 54;
			int level = 48;
			tag = 4470;
			do {
				int64_t ms = a.millisecondsAtResolution(level);
				cout << "l,m(l): " << dec << setw(3) << level << ", "
						<< setfill(' ') << setw(17) << dec << ms
						<< " 0x" << setfill('0') << setw(17) << hex << ms << dec;
				b.setZero().fromInt64Milliseconds(ms);
				b_ms = b.toInt64Milliseconds();
				if(b.get_year() == 0) {
					b.set_year(1024);
				}
				INDEX_OUTNC("  ",b);
				cout << " " << setw(18) << ms << " " << setw(18) << b_ms
						<< setw(18) << b_ms - ms
						;
				ENDL(cout);
				--level;
			} while (level >= 0);
		}

		tag = 4500;
		TAG(tag)
		a.setZero().set_BeforeAfterStartBit(1).set_year(1);
		TAG(++tag)
		b.setZero().set_BeforeAfterStartBit(1).set_year(2);
		TAG(++tag)
		c = a + b;
		TAG(++tag)
		// ENDL(cout);
		INDEX_OUT(++tag,a);
		INDEX_OUT(++tag,b);
		INDEX_OUT(++tag,c);

		tag = 4510;
		a.setZero().set_BeforeAfterStartBit(1).set_year(1).set_minute(30);
		b.setZero().set_BeforeAfterStartBit(1).set_year(1).set_minute(30);
		c = a + b;
		// ENDL(cout);
		INDEX_OUT(tag,a);
		INDEX_OUT(++tag,b);
		INDEX_OUT(++tag,c);

		tag = 4520;
		a.setZero().set_BeforeAfterStartBit(0).set_year(0).set_minute(30);
		b.setZero().set_BeforeAfterStartBit(0).set_year(0).set_minute(30);
		c = a + b;
		// ENDL(cout);
		INDEX_OUT(tag,a);
		INDEX_OUT(++tag,b);
		INDEX_OUT(++tag,c);

		tag = 4530;
		a.setZero().set_BeforeAfterStartBit(0).set_year(0).set_month(13).set_day(0).set_minute(30);
		b.setZero().set_BeforeAfterStartBit(0).set_year(0).set_month(0 ).set_day(0).set_minute(30);
		c = a + b;
		// ENDL(cout);
		INDEX_OUT(tag,a);
		INDEX_OUT(++tag,b);
		INDEX_OUT(++tag,c);
	}

	if(globalPrintFlag) {
		ENDL(cout);
		cout << "a" << endl << flush;
		cout << "tI3:  idx: 0x" << setw(16) << setfill('0') << hex << tIndex3.scidbTemporalIndex() << dec << endl << flush;
		cout << "tI3: term: 0x" << setw(16) << setfill('0') << hex << tIndex3.scidbTerminator() << " " << dec << tIndex3.scidbTerminatorp() << endl << flush; // Not a terminator?
		cout << "tI4: term: 0x" << setw(16) << setfill('0') << hex << tIndex4.scidbTerminator() << " " << dec << tIndex4.scidbTerminatorp() << endl << flush; // A terminator?
	}

	ASSERT_EQUALM("tI3:  idx:",0x0004000000000a29,tIndex3.scidbTemporalIndex());
	ASSERT_EQUALM("tI3: term:",0x0014000000003ffd,tIndex3.scidbTerminator());
	ASSERT_EQUAL(0, tIndex3.scidbTerminatorp());
	ASSERT_EQUALM("tI4: term:",0x0014000000003ffd,tIndex4.scidbTerminator());
	ASSERT_EQUAL(1,tIndex4.scidbTerminatorp());
	
	/*
	ASSERT_EQUALM("tI3:  idx:",0x000010000000002a,tIndex3.scidbTemporalIndex());
	ASSERT_EQUALM("tI3: term:",0x00101000000000fe,tIndex3.scidbTerminator());
	ASSERT_EQUAL(0, tIndex3.scidbTerminatorp());
	ASSERT_EQUALM("tI4: term:",0x00101000000000fe,tIndex4.scidbTerminator());
	ASSERT_EQUAL(1,tIndex4.scidbTerminatorp());
	*/

	tIndex3.set_year(0);
	// tIndex3.set_year(1);
	tIndex3.set_BeforeAfterStartBit(0);
	tIndex4 = TemporalIndex(tIndex3.scidbTerminator());

	if(globalPrintFlag) {
		ENDL(cout);
		cout << "b" << endl << flush;
		INDEX_OUT(5000,tIndex3);
		INDEX_OUT(5001,tIndex4);
		cout << "tI3:  idx:  0x" << setw(16) << setfill('0') << hex << tIndex3.scidbTemporalIndex() << dec << endl << flush;
		cout << "tI3: term:  0x" << setw(16) << setfill('0') << hex << tIndex3.scidbTerminator() << " " << dec << tIndex3.scidbTerminatorp() << endl << flush;


		cout << "tI4: term:  0x" << setw(16) << setfill('0') << hex << tIndex4.scidbTerminator() << " " << dec << tIndex4.scidbTerminatorp() << endl << flush;
		cout << "tI3s   tI3t:  " <<  dec << tIndex3.scidbTemporalIndex() << " " <<  tIndex3.scidbTerminator() << endl << flush;
		cout << "tI3s < tI3t?  " <<  ( tIndex3.scidbTemporalIndex() < tIndex3.scidbTerminator() ) << endl << flush;
		cout << "tI3  < tI4 ?  " <<  ( tIndex3 < tIndex4 ) << endl << flush;
		cout << "tI3 <= tI4 ?  " <<  ( tIndex3 <= tIndex4 ) << endl << flush;
		cout << "tI3  > tI4 ?  " <<  ( tIndex3 > tIndex4 ) << endl << flush;
		cout << "tI3 >= tI4 ?  " <<  ( tIndex3 >= tIndex4 ) << endl << flush;
		cout << "tI3 == tI4 ?  " <<  ( tIndex3 == tIndex4 ) << endl << flush;
		cout << "tI3 == tI3 ?  " <<  ( tIndex3 == tIndex3 ) << endl << flush;
		cout << "tI3 >= tI3 ?  " <<  ( tIndex3 >= tIndex3 ) << endl << flush;
		cout << "tI3 <= tI3 ?  " <<  ( tIndex3 <= tIndex3 ) << endl << flush;
		cout << "tI3 != tI3 ?  " <<  ( tIndex3 != tIndex3 ) << endl << flush;
		cout << "tI3 != tI4 ?  " <<  ( tIndex3 != tIndex4 ) << endl << flush;
		cout << "tI4s < tI4t?  " <<  ( tIndex4.scidbTemporalIndex() < tIndex4.scidbTerminator() ) << endl << flush;
		cout << "tI4s <= tI4t? " <<  ( tIndex4.scidbTemporalIndex() <= tIndex4.scidbTerminator() ) << endl << flush;
		cout << "tI4s >= tI4t? " <<  ( tIndex4.scidbTemporalIndex() >= tIndex4.scidbTerminator() ) << endl << flush;
		cout << "tI4s == tI4t? " <<  ( tIndex4.scidbTemporalIndex() == tIndex4.scidbTerminator() ) << endl << flush;
		// cout << "tI4 " << tIndex4.scidbTerminatorp() << endl << flush;

		ENDL(cout);
	}

#define FMT(lbl,xval) if(globalPrintFlag) { cout << lbl << " 0x" << setw(16) << setfill('0') << hex << xval << dec << " " << xval << flush; }
	FMT("tI4s vs. tI4t ",tIndex4.scidbTemporalIndex()); // ENDL(cout);
	FMT("              ",tIndex4.scidbTerminator());    // ENDL(cout);

	ASSERT_EQUAL(tIndex3.scidbTerminatorp(),0);

	ASSERT_LESS(tIndex3.scidbTemporalIndex(),tIndex3.scidbTerminator());
	ASSERT_LESS(tIndex3,tIndex4);
	ASSERT_LESS_EQUAL(tIndex3,tIndex4);
	ASSERT_LESS_EQUAL(tIndex3,tIndex3);

	ASSERT_GREATER(tIndex4,tIndex3);
	ASSERT_GREATER_EQUAL(tIndex4,tIndex3);
	ASSERT_GREATER_EQUAL(tIndex4,tIndex4);

	ASSERT_EQUAL(tIndex3,tIndex3);
	ASSERT_EQUAL(tIndex4,tIndex4);

	ASSERT_NOT_EQUAL_TO(tIndex3,tIndex4);

	ASSERT_LESS_EQUAL( tIndex3.scidbTemporalIndex(), tIndex3.scidbTerminator() );

	ASSERT_EQUAL( tIndex4.scidbTemporalIndex(), tIndex4.scidbTerminator() );
	ASSERT_EQUAL(tIndex4.scidbTerminatorp(),1);

	int tag_id = 6000; int level = 30;

	if(globalPrintFlag) {
		ENDL(cout);

	cout << tag_id << endl << flush;
	--level;
	tIndex3.setZero().set_BeforeAfterStartBit(1).set_year(1).set_forward_resolution(level).set_reverse_resolution(level); INDEX_OUTNC(++tag_id,tIndex3);
	itmp4 = tIndex3.scidbTerminator(); tIndex4 = TemporalIndex(itmp4); INDEX_OUT(" ",tIndex4);
	do {
	  --level;
	  tIndex3.setZero().set_BeforeAfterStartBit(1).set_year(1).set_forward_resolution(level).set_reverse_resolution(level); INDEX_OUTNC(++tag_id,tIndex3);
		itmp4 = tIndex3.scidbTerminator(); tIndex4 = TemporalIndex(itmp4); INDEX_OUT(" ",tIndex4);
	} while ( level > 10 );


	--level;
	tIndex3.setZero().set_BeforeAfterStartBit(1).set_year(2).set_forward_resolution(level).set_reverse_resolution(level); INDEX_OUTNC(++tag_id,tIndex3);
	itmp4 = tIndex3.scidbTerminator(); tIndex4 = TemporalIndex(itmp4); INDEX_OUT(" ",tIndex4);

	--level;
	tIndex3.setZero().set_BeforeAfterStartBit(1).set_year(2).set_month(2).set_forward_resolution(level).set_reverse_resolution(level); INDEX_OUTNC(++tag_id,tIndex3);
	itmp4 = tIndex3.scidbTerminator(); tIndex4 = TemporalIndex(itmp4); INDEX_OUT(" ",tIndex4);

	tag_id = 7000; level = 30;
	cout << tag_id << endl << flush;
	--level;
	tIndex3.setZero().set_BeforeAfterStartBit(0).set_year(1).set_forward_resolution(level).set_reverse_resolution(level); INDEX_OUTNC(++tag_id,tIndex3);
	itmp4 = tIndex3.scidbTerminator(); tIndex4 = TemporalIndex(itmp4); INDEX_OUT(" ",tIndex4);
	do {
	  --level;
	  tIndex3.setZero().set_BeforeAfterStartBit(0).set_year(1).set_forward_resolution(level).set_reverse_resolution(level); INDEX_OUTNC(++tag_id,tIndex3);
		itmp4 = tIndex3.scidbTerminator(); tIndex4 = TemporalIndex(itmp4); INDEX_OUT(" ",tIndex4);
	} while ( level > 10 );

	--level;
	tIndex3.setZero().set_BeforeAfterStartBit(0).set_year(2).set_forward_resolution(level).set_reverse_resolution(level); INDEX_OUTNC(++tag_id,tIndex3);
	itmp4 = tIndex3.scidbTerminator(); tIndex4 = TemporalIndex(itmp4); INDEX_OUT(" ",tIndex4);

	--level;
	tIndex3.setZero().set_BeforeAfterStartBit(0).set_year(2).set_month(2).set_forward_resolution(level).set_reverse_resolution(level); INDEX_OUTNC(++tag_id,tIndex3);
	itmp4 = tIndex3.scidbTerminator(); tIndex4 = TemporalIndex(itmp4); INDEX_OUT(" ",tIndex4);

	TemporalIndex tIndex5;

	tag_id = 8000; level = 10;
	cout << tag_id << endl << flush;
	--level;
	tIndex3.setZero().set_BeforeAfterStartBit(0).set_year(1).set_forward_resolution(level).set_reverse_resolution(level); INDEX_OUT(++tag_id,tIndex3);
	--level;
	tIndex4.setZero().set_BeforeAfterStartBit(1).set_year(1).set_forward_resolution(level).set_reverse_resolution(level); INDEX_OUT(++tag_id,tIndex4);
	tIndex5 = tIndex3 + tIndex4; INDEX_OUT(++tag_id,tIndex5);

	tag_id = 8120; level = 20;
	cout << tag_id << endl << flush;
	--level;
	tIndex3.setZero().set_BeforeAfterStartBit(0).set_year(10).set_forward_resolution(level).set_reverse_resolution(level); INDEX_OUT(++tag_id,tIndex3);
	--level;
	tIndex4.setZero().set_BeforeAfterStartBit(1).set_year(5).set_forward_resolution(level).set_reverse_resolution(level); INDEX_OUT(++tag_id,tIndex4);
	tIndex5 = tIndex3 + tIndex4; INDEX_OUT(++tag_id,tIndex5);
	ENDL(cout);

	tag_id = 8130; level = 15;
	cout << tag_id << endl << flush;
	--level;
	tIndex3.setZero().set_BeforeAfterStartBit(0).set_year(0).set_month(5).set_forward_resolution(level).set_reverse_resolution(level); INDEX_OUT(++tag_id,tIndex3);
	--level;
	tIndex4.setZero().set_BeforeAfterStartBit(0).set_year(0).set_month(9).set_forward_resolution(level).set_reverse_resolution(level); INDEX_OUT(++tag_id,tIndex4);
	tIndex5 = tIndex3 + tIndex4; INDEX_OUT(++tag_id,tIndex5);
	ENDL(cout);

	tag_id = 8140; level = 15;
	cout << tag_id << endl << flush;
	int64_t i3 = 3, i4 = 8;
	--level;
	tIndex3.setZero().fromInt64Milliseconds(i3).set_forward_resolution(level).set_reverse_resolution(level); INDEX_OUT(++tag_id,tIndex3);
	--level;
	tIndex4.setZero().fromInt64Milliseconds(i4).set_forward_resolution(level).set_reverse_resolution(level); INDEX_OUT(++tag_id,tIndex4);
	tIndex5 = tIndex3 | tIndex4; INDEX_OUT(++tag_id,tIndex5);
	tIndex.setZero().set_millisecond(i3+i4).set_forward_resolution(level).set_reverse_resolution(level);
	ASSERT_EQUAL(tIndex,tIndex5);

	ENDL(cout);
	}

	// SciDB checks
	int64_t max_coordinate = (1ULL << 62) - 1;
	int64_t min_coordinate = - max_coordinate;
	if(globalPrintFlag) { cout << "min,max coordinate: " << dec << min_coordinate << ", " << max_coordinate << endl << flush; }

	// ENDL(cout);
	tag_id = 9999;
	if(globalPrintFlag) { cout << "Max SciDB coord" << endl << flush; }
	// Note: max_coordinate actually has the type bits set to 3, which is invalid for this TemporalIndex,
	// which is of type=2.
	tIndex = TemporalIndex(max_coordinate); INDEX_OUT(++tag_id,tIndex);
	// .set_year(262143)
	if(globalPrintFlag) { cout << "Native maximum (scidb)" << endl << flush; }
	// tIndex.setZero().setEOY(1,262143);INDEX_OUT(++tag_id,tIndex);
	tIndex.setZero().setEOY(1,8191);INDEX_OUT(++tag_id,tIndex);
	tIndex1 = TemporalIndex(tIndex.scidbTerminator()); INDEX_OUT(++tag_id,tIndex1);
	if(globalPrintFlag) { cout << "-" << endl << flush; }
	tIndex.setZero()
			.set_BeforeAfterStartBit(1)
			.set_year(8191)
	  //			.set_year(262144)
		; INDEX_OUT(++tag_id,tIndex);
	if(globalPrintFlag) { cout << "-" << endl << flush; }

	if(globalPrintFlag) {
	  // int levels[7] = { 54, 50, 40, 20, 10, 5, 0 };
	  int levels[7] = { 48, 45, 40, 20, 10, 5, 0 };

		for( int i = 0; i < 7; ++i ) {
			level = levels[i];
			tIndex.setZero().setEOY(1,2001).set_type(1).set_forward_resolution(level).set_reverse_resolution(level); INDEX_OUT(++tag_id,tIndex);
			tIndex1 = TemporalIndex(tIndex.scidbTerminator()); INDEX_OUT(++tag_id,tIndex1);
			tIndex1 = TemporalIndex(tIndex.scidbTerminatorJulianTAI()); INDEX_OUT(++tag_id,tIndex1);
			cout << "--" << endl << flush;
			{
				cout << "delta:     " << tIndex.millisecondsAtResolution(tIndex.get_forward_resolution()) << endl << flush;
				cout << "deltaJDms: " << tIndex.daysAtResolution(tIndex.get_forward_resolution())*86400000.0 << endl << flush;
				cout << "deltaJD:   " << tIndex.daysAtResolution(tIndex.get_forward_resolution()) << endl << flush;
				double d1,d2,delta=0.0;
				delta = tIndex.daysAtResolution(tIndex.get_forward_resolution());
				tIndex.toJulianTAI(d1, d2); tIndex.set_forward_resolution(level).set_reverse_resolution(level); INDEX_OUT(++tag_id,tIndex);
				tIndex.setZero().fromJulianTAI(d1, d2+delta); INDEX_OUT(++tag_id,tIndex);
			}
			cout << "---" << endl << flush;
		}
	}

	if(globalPrintFlag) {
		ENDL(cout);
		cout << "Min SciDB coord" << endl << flush;
		// Note min_coordinate is not a valid stare temporal index value.
		tIndex = TemporalIndex(min_coordinate);	INDEX_OUT(++tag_id,tIndex);

		ENDL(cout);
		cout << "Native minimum (scidb)" << endl << flush;
		// tIndex.setZero().set_year(262143);   	INDEX_OUT(++tag_id,tIndex);
		tIndex.setZero().set_year(8191);   	INDEX_OUT(++tag_id,tIndex);
		cout << "nat-min0 " << scidbMinimumTemporalIndex() << endl << flush;
		cout << "nat-min > sci-min: " << (tIndex.scidbTemporalIndex() > min_coordinate) << endl << flush;
		tIndex = TemporalIndex(scidbMinimumTemporalIndex()); INDEX_OUT(++tag_id,tIndex);
		ENDL(cout);

		cout << "nat/sci-max " << scidbMaximumTemporalIndex() << endl << flush; // nat-max?
		ENDL(cout);
	}

	ASSERT_EQUAL(-4612668427704320001,scidbMinimumTemporalIndex());
	ASSERT_EQUAL( 4612604656029908993,scidbMaximumTemporalIndex());
	  
	// ASSERT_EQUAL(-4611683776386295554,scidbMinimumTemporalIndex());
	// ASSERT_EQUAL(4611682745594144514,scidbMaximumTemporalIndex());

	tIndex.set_type(1);
	tIndex.setZero().setEOY(1,1024);
	if(globalPrintFlag) {
	INDEX_OUT(++tag_id,tIndex);
	ENDL(cout);
	}

	tIndex.fromFormattedJulianTAI(2000, 12, 31, 23, 59, 59, 999);
	if(globalPrintFlag) {
	INDEX_OUTNC(++tag_id,tIndex);
	cout << " " << setw(18) << setfill(' ') << tIndex.toInt64Milliseconds();
	ENDL(cout);
	}

	tIndex.setZero().set_BeforeAfterStartBit(1).set_year(2001).set_hour(23).set_minute(59).set_second(59).set_millisecond(999);
	if(globalPrintFlag) {
	INDEX_OUTNC(++tag_id,tIndex);
	cout << " " << setw(18) << setfill(' ') << tIndex.toInt64Milliseconds();
	ENDL(cout);
	ENDL(cout);
	}

	double d1,d2;
	tIndex.fromFormattedJulianTAI(1999, 12, 31, 23, 59, 59, 999);
	INDEX_OUT(++tag_id,tIndex);
	tIndex.toJulianTAI(d1, d2);
	tIndex.setZero().fromJulianTAI(d1, d2);
	if(globalPrintFlag) {
	INDEX_OUTNC(++tag_id,tIndex);
	cout << " d1,d2: " << d1 << ", " << d2 << endl << flush;
	ENDL(cout);
	}

	tIndex.setZero().fromFormattedJulianTAI(2000, 12, 31, 23, 59, 59, 999);
	INDEX_OUT(++tag_id,tIndex);
	tIndex.toJulianTAI(d1, d2);
	tIndex.setZero().fromJulianTAI(d1, d2);
	if(globalPrintFlag) {
	INDEX_OUTNC(++tag_id,tIndex);
	cout << " d1,d2: " << d1 << ", " << d2 << endl << flush;
	ENDL(cout);
	}
	//

#define FMT_JD(d1,d2) if(globalPrintFlag) { cout << " d1,d2: " << setprecision(17) << setw(20) << scientific << d1 << ", " << d2; cout.copyfmt(ios(NULL)); }

	tag_id = 11000;
	try {
	tag_id = 11500;
	// tIndex.setZero().fromFormattedJulianTAI(1, 2000, 12, 31, 23, 59, 45, 459);
	tIndex.setZero().fromFormattedJulianTAI(2000, 12, 31, 23, 59, 59, 999);	INDEX_OUT(++tag_id,tIndex);

	tIndex.toJulianTAI(d1, d2);
	tIndex.setZero().fromJulianTAI(d1, d2);  INDEX_OUTNC(++tag_id,tIndex); FMT_JD(d1,d2); // ENDL(cout);

	tIndex.toJulianTAI(d1, d2);
	tIndex.setZero().fromJulianTAI(d1, d2);  INDEX_OUTNC(++tag_id,tIndex); FMT_JD(d1,d2); // ENDL(cout);

	tIndex.toJulianTAI(d1, d2);
	tIndex.setZero().fromJulianTAI(d1, d2);  INDEX_OUTNC(++tag_id,tIndex); FMT_JD(d1,d2); // ENDL(cout);
	// ENDL(cout);

	tag_id = 11600;
	tIndex1.set_type(1); tIndex2.set_type(1); // 2 is the type for this index type
	tIndex1.setZero().fromFormattedJulianTAI(4,  1,  1,  0,  0,  0,   0);	INDEX_OUT(++tag_id,tIndex1);
	// tIndex1.setZero();	INDEX_OUT(++tag_id,tIndex1);
	tIndex2.setZero().fromFormattedJulianTAI(2,  1,  1,  0,  0,  0,   0);	INDEX_OUT(++tag_id,tIndex2);
	// tIndex2.setZero().fromJulianDoubleDay(0, 1); INDEX_OUT(++tag_id,tIndex2);
	tIndex4 = tIndex1 + tIndex2; INDEX_OUT(++tag_id,tIndex4);
	// tIndex3 = tIndex1 | tIndex2; INDEX_OUT(++tag_id,tIndex3);

	// ENDL(cout);

	tag_id = 11700;
	tIndex1.set_type(1); tIndex2.set_type(1); // 2 is the type for this index type
	tIndex1.setZero().fromFormattedJulianTAI(4,  1,  1,  0,  0,  0,   0);	INDEX_OUT(++tag_id,tIndex1);
	tIndex2.setZero().fromJulianTAI(0, 1); INDEX_OUT(++tag_id,tIndex2);
	tIndex4 = tIndex1 + tIndex2; INDEX_OUT(++tag_id,tIndex4);
	// tIndex3 = tIndex1 | tIndex2; INDEX_OUT(++tag_id,tIndex3);

	// ENDL(cout);
	} catch (SpatialFailure& failure) {
		failureMessage = "'";
		failureMessage += failure.what();
		cout << "failureMessage: " << failureMessage << "'" << endl << flush;
	};

	if(globalPrintFlag) {
		double  fd; int  hour, minute, second, ms;
		fd = 0.2503;
		fractionalDayToHMSM (fd, hour, minute, second, ms);
		cout << "fd, hmsm: " << fd
				<< " " << hour
				<< " " << minute
				<< " " << second
				<< " " << ms
				<< endl << flush
				;
		fractionalDayFromHMSM (fd, hour, minute, second, ms);
		cout << "fd, hmsm: " << fd
				<< " " << hour
				<< " " << minute
				<< " " << second
				<< " " << ms
				<< endl << flush
				;
		tag_id = 20000;
		tIndex.fromJulianTAI(fd, 0); INDEX_OUT(tag_id,tIndex);
		cout << "-" << endl << flush;
	}

	if(globalPrintFlag) {
		double d1, d2, delta;
		tag_id = 21000;

		tIndex1.setZero().fromFormattedJulianTAI(2000,  1,  1,  0,  0,  0,   0);	INDEX_OUT(++tag_id,tIndex1);
		tIndex1.toJulianTAI(d1, d2);
		tIndex2.fromJulianTAI(d1, d2); INDEX_OUT(++tag_id,tIndex2);
		cout << "d1,d2: " << d1 << ", " << d2 << endl << flush;
		cout << "d1,d2: " << (int64_t)d1 << ", " << (int64_t)d2 << endl << flush;
		cout << "---" << endl << flush;

		tIndex1.setZero().fromFormattedJulianTAI(2000,  1,  1,  0,  0,  0,   1);	INDEX_OUT(++tag_id,tIndex1);
		tIndex1.toJulianTAI(d1, d2);
		tIndex2.fromJulianTAI(d1, d2); INDEX_OUT(++tag_id,tIndex2);
		cout << "d1,d2: " << d1 << ", " << d2 << endl << flush;
		cout << "d1,d2: " << (int64_t)d1 << ", " << (int64_t)d2 << endl << flush;
		cout << "---" << endl << flush;

		delta = 1.0/86400000.0;
		tIndex1.setZero().fromFormattedJulianTAI(2001,  1,  1,  0,  0,  0,   0);	INDEX_OUT(++tag_id,tIndex1);
		tIndex1.toJulianTAI(d1, d2);
		tIndex2.fromJulianTAI(d1, d2+delta); INDEX_OUT(++tag_id,tIndex2);
		cout << "d1,d2: " << d1 << ", " << (d2+delta) << endl << flush;
		cout << "d1,d2: " << (int64_t)d1 << ", " << (int64_t)(d2+delta) << endl << flush;
		cout << "---" << endl << flush;

		tag_id = 21010;
		failureMessage = "'";
		try {
			tIndex1.setZero().fromFormattedJulianTAI(2016,  12,  31,  23,  59,  59,   999);	INDEX_OUT(++tag_id,tIndex1);
			tIndex2.setZero().fromFormattedJulianTAI(2016,  12,  31,  23,  59,  59,   999);	INDEX_OUT(++tag_id,tIndex1);
			tIndex2.setZero().fromFormattedJulianTAI(2016,  12,  31,  23,  59,  60,   999);	INDEX_OUT(++tag_id,tIndex1);
			//		cout << "d1,d2: " << d1 << ", " << (d2+delta) << endl << flush;
			//		cout << "d1,d2: " << (int64_t)d1 << ", " << (int64_t)(d2+delta) << endl << flush;
		} catch (SpatialFailure& failure) {
			failureMessage += failure.what();
			cout << "failureMessage: " << failureMessage << "'" << endl << flush;
		};
		cout << "---" << endl << flush;

//		tag_id = 21020;
//		delta = 1.0/86400000.0;
//		tIndex1.setZero().fromFormattedJulianTAI(1, 2016,  12,  31,  23,  59,  60,   999);	INDEX_OUT(++tag_id,tIndex1);
//		tIndex1.toJulianTAI(d1, d2);
//		tIndex2.fromJulianTAI(d1, d2+delta); INDEX_OUT(++tag_id,tIndex2);
//		cout << "d1,d2: " << d1 << ", " << (d2+delta) << endl << flush;
//		cout << "d1,d2: " << (int64_t)d1 << ", " << (int64_t)(d2+delta) << endl << flush;
		//		cout << "---" << endl << flush;
	}

	if(globalPrintFlag) {
		int not_ok;
		int
		_year = 0,
		_month = 1,
		_day_of_month = 1,
		_hour = 0,
		_minute = 0,
		_second = 0,
		_millisecond = 0
		;
		double d1, d2;

		cout << "UTC " << _year << endl << flush;
		not_ok = eraDtf2d( "UTC", _year, _month, _day_of_month, _hour, _minute, _second+(_millisecond*0.001), &d1, &d2 );
		cout << "date: d1,d2 "
		<< _year <<  "-" << _month << "-" << _day_of_month
		<< " : " << d1 << ", " << d2
		<< " : " << (int64_t)d1 << ", " << (int64_t)d2
		<< ", ms : " << (int64_t) ( (d1+d2) * 86400000.0 )
		<< endl << flush;

		cout << "TAI" << endl << flush;
		not_ok = eraDtf2d( "TAI", _year, _month, _day_of_month, _hour, _minute, _second+(_millisecond*0.001), &d1, &d2 );
		cout << "date: d1,d2 "
		<< _year <<  "-" << _month << "-" << _day_of_month
		<< " : " << d1 << ", " << d2
		<< " : " << (int64_t)d1 << ", " << (int64_t)d2
		<< ", ms : " << (int64_t) ( (d1+d2) * 86400000.0 )
		<< endl << flush;

		cout << "---" << endl << flush;
	}

	if(globalPrintFlag) {
		int not_ok;
		int
		_year = 1973,
		_month = 12,
		_day_of_month = 31,
		_hour = 23,
		_minute = 59,
		_second = 59,
		_millisecond = 999
		;
		double d1, d2;

		cout << "UTC " << _year << endl << flush;
		not_ok = eraDtf2d( "UTC", _year, _month, _day_of_month, _hour, _minute, _second+(_millisecond*0.001), &d1, &d2 );
		cout << "date: d1,d2 "
		<< _year <<  "-" << _month << "-" << _day_of_month
		<< " : " << d1 << ", " << d2
		<< " : " << (int64_t)d1 << ", " << (int64_t)d2
		<< ", ms : " << (int64_t) ( (d1+d2) * 86400000.0 )
		<< endl << flush;

		cout << "TAI" << endl << flush;
		not_ok = eraDtf2d( "TAI", _year, _month, _day_of_month, _hour, _minute, _second+(_millisecond*0.001), &d1, &d2 );
		cout << "date: d1,d2 "
		<< _year <<  "-" << _month << "-" << _day_of_month
		<< " : " << d1 << ", " << d2
		<< " : " << (int64_t)d1 << ", " << (int64_t)d2
		<< ", ms : " << (int64_t) ( (d1+d2) * 86400000.0 )
		<< endl << flush;

		cout << "---" << endl << flush;
	}

	tag_id = 22000;
	tIndex.setZero().fromFormattedJulianTAI(2001,  1,  1,  0,  0,  0,   0);	INDEX_OUT(++tag_id,tIndex);
	if(globalPrintFlag){
	cout << " toStringJ0: " << tIndex.toStringJulianTAI() << endl << flush;
	}
	tIndex1.fromStringJulianTAI(tIndex.toStringJulianTAI());
	if(globalPrintFlag){
	cout << " toStringJ1: " << tIndex1.toStringJulianTAI() << endl << flush;
	cout << "---" << endl << flush;
	}

	failureMessage = "";
	try {
		tIndex.setZero().fromFormattedJulianTAI(1-100,  5,  29,  6,  30,  1,   2);	INDEX_OUT(++tag_id,tIndex);
		if(globalPrintFlag){
		cout << " toStringJ0: " << tIndex.toStringJulianTAI() << endl << flush;
		}
		tIndex1.fromStringJulianTAI(tIndex.toStringJulianTAI());
		if(globalPrintFlag){
		cout << " toStringJ1: " << tIndex1.toStringJulianTAI() << endl << flush;
		cout << "---" << endl << flush;
		}
	} catch (SpatialFailure& failure) {
		failureMessage += failure.what();
		if(globalPrintFlag){
		cout << "failureMessage: " << failureMessage << "'" << endl << flush;
		}
	};

	tag_id = 22100;
	tIndex.setZero().fromFormattedJulianTAI(1972, 6, 30, 23, 59, 60, 1); INDEX_OUT(++tag_id,tIndex);
	if(globalPrintFlag){
	cout << " toStringJ0: " << tIndex.toStringJulianTAI()
			<< ", ms: " << tIndex.toInt64MillisecondsFractionOfYear()
			<< ", msj: " << tIndex.toInt64MillisecondsFractionOfYearJ()
			<< ", msp: " << millisecondsInYear(tIndex.get_BeforeAfterStartBit(),tIndex.get_year())
			<< endl << flush;
	cout << "---" << endl << flush;
	}

	tIndex.setZero().fromFormattedJulianTAI(1972, 6, 30, 23, 58, 60, 1); INDEX_OUT(++tag_id,tIndex);
	if(globalPrintFlag){
	cout << " toStringJ0: " << tIndex.toStringJulianTAI()
			<< ", ms: " << tIndex.toInt64MillisecondsFractionOfYear()
			<< ", msj: " << tIndex.toInt64MillisecondsFractionOfYearJ()
			<< ", msp: " << millisecondsInYear(tIndex.get_BeforeAfterStartBit(),tIndex.get_year())
			<< endl << flush;
	cout << "---" << endl << flush;
	}

	tIndex.setZero().fromFormattedJulianTAI(1972, 6, 29, 23, 58, 60, 1); INDEX_OUT(++tag_id,tIndex);
	if(globalPrintFlag){
	cout << " toStringJ0: " << tIndex.toStringJulianTAI()
			<< ", ms: " << tIndex.toInt64MillisecondsFractionOfYear()
			<< ", msj: " << tIndex.toInt64MillisecondsFractionOfYearJ()
			<< ", msp: " << millisecondsInYear(tIndex.get_BeforeAfterStartBit(),tIndex.get_year())
			<< endl << flush;
	cout << "---" << endl << flush;
	}

	tIndex.setZero().setEOY(1,1972); INDEX_OUT(++tag_id,tIndex);
	if(globalPrintFlag){
	cout << " toStringJ0: " << tIndex.toStringJulianTAI()
		<< ", ms: " << tIndex.toInt64MillisecondsFractionOfYear()
		<< ", msj: " << tIndex.toInt64MillisecondsFractionOfYearJ()
		<< ", msp: " << millisecondsInYear(tIndex.get_BeforeAfterStartBit(),tIndex.get_year())
		<< endl << flush;
	cout << "---" << endl << flush;
	}

	tIndex.setZero().setEOY(1,1973); INDEX_OUT(++tag_id,tIndex);
	if(globalPrintFlag){
	cout << " toStringJ0: " << tIndex.toStringJulianTAI()
			<< ", ms: " << tIndex.toInt64MillisecondsFractionOfYear()
			<< ", msj: " << tIndex.toInt64MillisecondsFractionOfYearJ()
			<< ", msp: " << millisecondsInYear(tIndex.get_BeforeAfterStartBit(),tIndex.get_year())
			<< endl << flush;
	cout << "---" << endl << flush;
	}

	tIndex.setZero().setEOY(1,1980); INDEX_OUT(++tag_id,tIndex);
	if(globalPrintFlag){
	cout << " toStringJ0: " << tIndex.toStringJulianTAI()
					<< ", ms: " << tIndex.toInt64MillisecondsFractionOfYear()
					<< ", msj: " << tIndex.toInt64MillisecondsFractionOfYearJ()
					<< ", msp: " << millisecondsInYear(tIndex.get_BeforeAfterStartBit(),tIndex.get_year())
					<< endl << flush;
	cout << "---" << endl << flush;
	}

	tIndex.setZero().setEOY(1,1992); INDEX_OUT(++tag_id,tIndex);

	if(globalPrintFlag){
	cout << " toStringJ0: " << tIndex.toStringJulianTAI()
					<< ", ms: " << tIndex.toInt64MillisecondsFractionOfYear()
					<< ", msj: " << tIndex.toInt64MillisecondsFractionOfYearJ()
					<< ", msp: " << millisecondsInYear(tIndex.get_BeforeAfterStartBit(),tIndex.get_year())
					<< endl << flush;
	cout << "---" << endl << flush;
	}


	tIndex.setZero().setEOY(1,1998); INDEX_OUT(++tag_id,tIndex);
	if(globalPrintFlag){
	cout << " toStringJ0: " << tIndex.toStringJulianTAI()
					<< ", ms: " << tIndex.toInt64MillisecondsFractionOfYear()
					<< ", msj: " << tIndex.toInt64MillisecondsFractionOfYearJ()
					<< ", msp: " << millisecondsInYear(tIndex.get_BeforeAfterStartBit(),tIndex.get_year())
					<< endl << flush;
	cout << "---" << endl << flush;
	}

	tIndex.setZero().setEOY(1,1999); INDEX_OUT(++tag_id,tIndex);
	if(globalPrintFlag){
	cout << " toStringJ0: " << tIndex.toStringJulianTAI()
					<< ", ms: " << tIndex.toInt64MillisecondsFractionOfYear()
					<< ", msj: " << tIndex.toInt64MillisecondsFractionOfYearJ()
					<< ", msp: " << millisecondsInYear(tIndex.get_BeforeAfterStartBit(),tIndex.get_year())
					<< endl << flush;
	cout << "---" << endl << flush;
	}

	tIndex.setZero().setEOY(1,2000); INDEX_OUT(++tag_id,tIndex);

	if(globalPrintFlag){
	cout << " toStringJ0: " << tIndex.toStringJulianTAI()
					<< ", ms: " << tIndex.toInt64MillisecondsFractionOfYear()
					<< ", msj: " << tIndex.toInt64MillisecondsFractionOfYearJ()
					<< ", msp: " << millisecondsInYear(tIndex.get_BeforeAfterStartBit(),tIndex.get_year())
					<< endl << flush;
	cout << "---" << endl << flush;
	}

	tIndex.setZero().setEOY(1,2001); INDEX_OUT(++tag_id,tIndex);
	if(globalPrintFlag){
	cout << " toStringJ0: " << tIndex.toStringJulianTAI()
					<< ", ms: " << tIndex.toInt64MillisecondsFractionOfYear()
					<< ", msj: " << tIndex.toInt64MillisecondsFractionOfYearJ()
					<< ", msp: " << millisecondsInYear(tIndex.get_BeforeAfterStartBit(),tIndex.get_year())
					<< endl << flush;
	cout << "---" << endl << flush;
	}

	tIndex.setZero().setEOY(1,2002); INDEX_OUT(++tag_id,tIndex);
	if(globalPrintFlag){
	cout << " toStringJ0: " << tIndex.toStringJulianTAI()
					<< ", ms: " << tIndex.toInt64MillisecondsFractionOfYear()
					<< ", msj: " << tIndex.toInt64MillisecondsFractionOfYearJ()
					<< ", msp: " << millisecondsInYear(tIndex.get_BeforeAfterStartBit(),tIndex.get_year())
					<< endl << flush;
	cout << "---" << endl << flush;
	}

	{

#define XOUT cout
// #define XOUT ss
#define FMT_DT1_NC(xout) \
		xout \
		<< setw(9) << setfill(' ') << _year << "-" \
		<< setw(2) << setfill('0') << _month << "-" \
		<< setw(2) << setfill('0') << _day_of_month << " " \
		<< setw(2) << _hour << ":" \
		<< setw(2) << _minute << ":" \
		<< setw(2) << _second << "." \
		<< setw(3) << _millisecond;

#define FMT_DT1(xout,lbl) xout << lbl; FMT_DT1_NC(xout); xout << endl << flush;

		int _year, _month, _day_of_month, _hour, _minute, _second, _millisecond;
		tIndex.toUTC(_year, _month, _day_of_month, _hour, _minute, _second, _millisecond);
		// FMT_DT1(XOUT,"")
		ss.str(""); FMT_DT1_NC(ss); ASSERT_EQUAL("     2002-12-31 23:59:27.999",ss.str().c_str());
		// cout << "---" << endl << flush;

		_year         = 1972;
		_month        =   12;
		_day_of_month =   31;
		_hour         =   23;
		_minute       =   59;
		_second       =   60;
		_millisecond  =  999;
		// FMT_DT1(XOUT,"1972 eoy leap second ")
		tIndex.setZero().fromUTC(_year, _month, _day_of_month, _hour, _minute, _second, _millisecond); INDEX_OUT(++tag_id,tIndex);
		_year         = 0;
		_month        = 0;
		_day_of_month = 0;
		_hour         = 0;
		_minute       = 0;
		_second       = 0;
		_millisecond  = 0;
		tIndex.toUTC(_year, _month, _day_of_month, _hour, _minute, _second, _millisecond);
		// FMT_DT1(XOUT,"")
		ss.str(""); FMT_DT1_NC(ss); ASSERT_EQUAL("     1972-12-31 23:59:60.999",ss.str().c_str());
		// cout << "---" << endl << flush;

		_year         = 1972;
		_month        =    6;
		_day_of_month =   30;
		_hour         =   23;
		_minute       =   59;
		_second       =   60;
		_millisecond  =  999;
		// FMT_DT1(XOUT,"1972 6/30 leap second ")
		// ASSERT_EQUAL("",ss.str.c_str());
		tIndex.setZero().fromUTC(_year, _month, _day_of_month, _hour, _minute, _second, _millisecond); INDEX_OUT(++tag_id,tIndex);
		_year         = 0;
		_month        = 0;
		_day_of_month = 0;
		_hour         = 0;
		_minute       = 0;
		_second       = 0;
		_millisecond  = 0;
		tIndex.toUTC(_year, _month, _day_of_month, _hour, _minute, _second, _millisecond);
		// FMT_DT1(XOUT,"")
		ss.str(""); FMT_DT1_NC(ss); ASSERT_EQUAL("     1972-06-30 23:59:60.999",ss.str().c_str());
		// cout << "---" << endl << flush;

		_year         = 1972;
		_month        =    6;
		_day_of_month =   30;
		_hour         =   23;
		_minute       =   59;
		_second       =   61;
		_millisecond  =  999;
		// FMT_DT1(XOUT,"1972 6/30 leap second + 1")
		tIndex.setZero().fromUTC(_year, _month, _day_of_month, _hour, _minute, _second, _millisecond); INDEX_OUT(++tag_id,tIndex);
		_year         = 0;
		_month        = 0;
		_day_of_month = 0;
		_hour         = 0;
		_minute       = 0;
		_second       = 0;
		_millisecond  = 0;
		tIndex.toUTC(_year, _month, _day_of_month, _hour, _minute, _second, _millisecond);
		// FMT_DT1(XOUT,"")
		ss.str(""); FMT_DT1_NC(ss); ASSERT_EQUAL("     1972-07-01 00:00:00.999",ss.str().c_str());
		// cout << "---" << endl << flush;

		_year         = 1986;
		_month        =   12;
		_day_of_month =   31;
		_hour         =   23;
		_minute       =   59;
		_second       =   60;
		_millisecond  =  501;
		// FMT_DT1(XOUT,"1986 eoy fake leap second")
		tIndex.setZero().fromUTC(_year, _month, _day_of_month, _hour, _minute, _second, _millisecond); INDEX_OUT(++tag_id,tIndex);
		_year         = 0;
		_month        = 0;
		_day_of_month = 0;
		_hour         = 0;
		_minute       = 0;
		_second       = 0;
		_millisecond  = 0;
		tIndex.toUTC(_year, _month, _day_of_month, _hour, _minute, _second, _millisecond);
		// FMT_DT1(XOUT,"")
		ss.str(""); FMT_DT1_NC(ss); ASSERT_EQUAL("     1987-01-01 00:00:00.501",ss.str().c_str());


		// cout << "---" << endl << flush;

		tIndex.setZero().fromJulianTAI(0, 0); INDEX_OUT(++tag_id,tIndex);

		// cout << "---" << endl << flush;
		tag_id = 23000;
		failureMessage = "'";
		try {
			if(globalPrintFlag) {
				tIndex.setZero().fromJulianTAI(-1.0, 0); INDEX_OUT(++tag_id,tIndex);
				tIndex.setZero().fromJulianTAI(-10.0, 0); INDEX_OUT(++tag_id,tIndex);
				tIndex.setZero().fromJulianTAI(-100.0, 0); INDEX_OUT(++tag_id,tIndex);
				tIndex.setZero().fromJulianTAI(-1000.0, 0); INDEX_OUT(++tag_id,tIndex);
				tIndex.setZero().fromJulianTAI(-10000.0, 0); INDEX_OUT(++tag_id,tIndex);
				tIndex.setZero().fromJulianTAI(-20000.0, 0); INDEX_OUT(++tag_id,tIndex);
				tIndex.setZero().fromJulianTAI(-40000.0, 0); INDEX_OUT(++tag_id,tIndex);
			}
			tIndex.setZero().fromJulianTAI(-40000.0, 0);
			// tIndex.setZero().fromJulianTAIDouble2(-80000.0, 0); INDEX_OUT(++tag_id,tIndex);
		} catch (SpatialFailure& failure) {
			failureMessage += failure.what();
			failureMessage += "'";
			// cout << "failureMessage: " << failureMessage << "'" << endl << flush;
			ASSERT_EQUAL("'TemporalIndex::setDateFromYearAndMilliseconds: _milliseconds < 0.'",failureMessage);
		};

		// cout << "---" << endl << flush;

		{
			int64_t CE, year, milliseconds;

			tag_id = 24000;

			if(globalPrintFlag) {
				tIndex.setZero().fromNativeCEYearAndMilliseconds(0,0,0); INDEX_OUT(++tag_id,tIndex);
				tIndex.toNativeCEYearAndMilliseconds(CE, year, milliseconds);
				cout << "CE,year,milliseconds: " << CE << " " << year << " " << milliseconds
						<< " : " << tIndex.toNativeYear()
						<< " : " << tIndex.scidbTemporalIndex()
						<< endl << flush;

				tIndex.setZero().fromNativeCEYearAndMilliseconds(0,0,365*86400000.0-1.0); INDEX_OUT(++tag_id,tIndex);
				tIndex.toNativeCEYearAndMilliseconds(CE, year, milliseconds);
				cout << "CE,year,milliseconds: " << CE << " " << year << " " << milliseconds
						<< " : " << tIndex.toNativeYear()
						<< " : " << tIndex.scidbTemporalIndex()
						<< endl << flush;
			}

		failureMessage = "'";
		try {
			tIndex.setZero().fromNativeCEYearAndMilliseconds(1,0,0); INDEX_OUT(++tag_id,tIndex);
			tIndex.toNativeCEYearAndMilliseconds(CE, year, milliseconds);
			//		cout << "CE,year,milliseconds: " << CE << " " << year << " " << milliseconds
			//				<< " : " << tIndex.toNativeYear()
			//				<< " : " << tIndex.scidbTemporalIndex()
			//				<< endl << flush;
		} catch (SpatialFailure& failure) {
			failureMessage += failure.what();
			failureMessage += "'";
			// cout << "failureMessage: " << failureMessage << "'" << endl << flush;
			ASSERT_EQUAL("'TemporalIndex::setDateFromYearAndMilliseconds: _year < 1 and CE != 0. Out of bounds. Use CE convention.'",failureMessage);
			// ASSERT_EQUAL("'TemporalIndex::setDateFromYearAndMilliseconds: _milliseconds < 0.'",failureMessage);
		};
		if(globalPrintFlag) {
			tIndex.setZero().fromNativeCEYearAndMilliseconds(0,10,0); INDEX_OUTN(++tag_id,tIndex);
			tIndex.toNativeCEYearAndMilliseconds(CE, year, milliseconds);
			cout << "CE,year,milliseconds: " << CE << " " << year << " " << milliseconds
					<< " : " << tIndex.toNativeYear()
					<< " : " << tIndex.scidbTemporalIndex()
					<< endl << flush;

			tIndex.setZero().fromNativeCEYearAndMilliseconds(0,100,120000); INDEX_OUTN(++tag_id,tIndex);
			tIndex.toNativeCEYearAndMilliseconds(CE, year, milliseconds);
			cout << "CE,year,milliseconds: " << CE << " " << year << " " << milliseconds
					<< " : " << tIndex.toNativeYear()
					<< " : " << tIndex.scidbTemporalIndex()
					<< endl << flush;

			tIndex.setZero().fromNativeCEYearAndMilliseconds(1,1000,120000); INDEX_OUTN(++tag_id,tIndex);
			tIndex.toNativeCEYearAndMilliseconds(CE, year, milliseconds);
			cout << "CE,year,milliseconds: " << CE << " " << year << " " << milliseconds
					<< " : " << tIndex.toNativeYear()
					<< " : " << tIndex.scidbTemporalIndex()
					<< endl << flush;
		}

		double year_ms = 31536000000.0; double year_ti;
		if(globalPrintFlag) {
//		if(true) {
			cout << "----ti----" << endl << flush;
			cout << setprecision(17);
		}
		tIndex.setZero().fromNativeCEYearAndMilliseconds(0,0,0.25*year_ms); INDEX_OUTN(++tag_id,tIndex);
		ASSERT_EQUAL(-0.75,tIndex.toNativeYear());
		if(globalPrintFlag) {
			tIndex.toNativeCEYearAndMilliseconds(CE, year, milliseconds);
			cout << "CE,year,milliseconds: " << CE << " " << year << " " << milliseconds
					<< " : " << tIndex.toNativeYear()
					<< " : " << tIndex.scidbTemporalIndex()
					<< endl << flush;

			year_ti = tIndex.toNativeYear(); tIndex.setZero().fromNativeYear(year_ti);  INDEX_OUTN(++tag_id,tIndex);
			cout << "year_ti: " << year_ti << endl << flush;
			cout << "----" << endl << flush;
		}

		tIndex.setZero().fromNativeCEYearAndMilliseconds(1,1,0.25*year_ms); INDEX_OUTN(++tag_id,tIndex);
		ASSERT_EQUAL(1.25,tIndex.toNativeYear());

		tIndex.toNativeCEYearAndMilliseconds(CE, year, milliseconds);
		if(globalPrintFlag) {
			cout << "CE,year,milliseconds: " << CE << " " << year << " " << milliseconds
					<< " : " << tIndex.toNativeYear()
					<< " : " << tIndex.scidbTemporalIndex()
					<< endl << flush;
		}
		year_ti = tIndex.toNativeYear(); tIndex.setZero().fromNativeYear(year_ti);  INDEX_OUTN(++tag_id,tIndex);
		if(globalPrintFlag) {
			cout << "year_ti: " << year_ti << endl << flush;
			cout << "----" << endl << flush;
			// tIndex.setZero().setDateFromYearAndMilliseconds(1,1,0.25*year_ms); INDEX_OUTN(++tag_id,tIndex);
			cout << "Note that in the current configuration, the leap-year day is not captured in toNativeYear. Use toJulianTAI instead." << endl << flush;
		}

		tIndex.setZero().fromFormattedJulianTAI(2000, 12, 31, 23, 59, 59, 999); INDEX_OUTN(++tag_id,tIndex);
		tIndex.toNativeCEYearAndMilliseconds(CE, year, milliseconds);

		if(globalPrintFlag) {
//		if(true) {
			cout << "CE,year,milliseconds: " << CE << " " << year << " " << milliseconds
					<< " : " << setprecision(17) << tIndex.toNativeYear()
					<< " : " << tIndex.scidbTemporalIndex()
					<< " , " << tIndex.scidbTerminator()
					<< " , " << tIndex.scidbTerminatorJulianTAI()
					<< endl << flush;
		}
		ASSERT_EQUAL(1,CE);
		ASSERT_EQUAL(2000,year);
		ASSERT_EQUAL(31622399999,milliseconds);
		ASSERT_EQUAL(2252718451287769089,tIndex.scidbTemporalIndex());
		ASSERT_EQUAL(6864613376924450813,tIndex.scidbTerminator()); // 0x5f44017efbf9fffd
		ASSERT_EQUAL(6861488564878311421,tIndex.scidbTerminatorJulianTAI()); // 0x5f38e77efbf9fffd
		ASSERT_EQUAL(2001.0027397259958,tIndex.toNativeYear()); // 
		
		/*
		ASSERT_EQUAL(1,CE);
		ASSERT_EQUAL(2000,year);
		ASSERT_EQUAL(31622399999,milliseconds);
		ASSERT_EQUAL(35198725801371394,tIndex.scidbTemporalIndex());
		ASSERT_EQUAL(4646888008403904510,tIndex.scidbTerminator());
		ASSERT_EQUAL(4643822982302525438,tIndex.scidbTerminatorJulianTAI());
		ASSERT_EQUAL(2001.0027397259958,tIndex.toNativeYear());
		*/

		year_ti = tIndex.toNativeYear(); tIndex.setZero().fromNativeYear(year_ti);  INDEX_OUTN(++tag_id,tIndex);

		if(globalPrintFlag) {
			cout << "year_ti: " << year_ti << endl << flush;
			cout << "----" << endl << flush;
			cout << "max resolution level: " << tIndex.data.maxResolutionLevel() << endl << flush;
			cout << "max resolution ms:    " << tIndex.millisecondsAtResolution(tIndex.data.maxResolutionLevel()) << endl << flush;
		}
		ASSERT_EQUAL(48,tIndex.data.maxResolutionLevel());
		// ASSERT_EQUAL(54,tIndex.data.maxResolutionLevel());
		ASSERT_EQUAL(1,tIndex.millisecondsAtResolution(tIndex.data.maxResolutionLevel()));

		if(globalPrintFlag){
		cout << setw(12) << "iDelta" << "  " << setw(4) << "ti" << endl << flush;
		cout	<< setw(18) << "iDelta" << "  "
			<< setw(4)  << "cR<iD" << " " 
			<< setw(18) << "ms@cR<iD" << " "
			<< setw(18) << "dy@cR<iD" << " "
			<< endl << flush;
		int64_t iDelta = 1;
		while( iDelta < 1000000000000000000 ) {
			cout
			<< setw(18) << iDelta << "  "
			<< setw(4)  << tIndex.coarsestResolutionFinerOrEqualMilliseconds(iDelta) << " "
			<< setw(18) << tIndex.millisecondsAtResolution(tIndex.coarsestResolutionFinerOrEqualMilliseconds(iDelta)) << " "
			<< setw(18) << tIndex.millisecondsAtResolution(tIndex.coarsestResolutionFinerOrEqualMilliseconds(iDelta))/86400.0e3 << " "
			<< setw(18) << tIndex.getResolutionTimescaleDays() << " " 
			<< endl << flush;
			iDelta *= 10;
		}
		/*
max resolution level: 48
max resolution ms:    1
      iDelta    ti
            iDelta  cR<iD           ms@cR<iD           dy@cR<iD 
                 1    48                  1 1.1574074074074074e-08            1495040 
                10    45                  8 9.2592592592592591e-08            1495040 
               100    42                 64 7.4074074074074073e-07            1495040 
              1000    38               1000 1.1574074074074073e-05            1495040 
             10000    35               8000 9.2592592592592588e-05            1495040 
            100000    32              60000 0.00069444444444444447            1495040 
           1000000    28             960000 0.011111111111111112            1495040 
          10000000    25            7200000 0.083333333333333329            1495040 
         100000000    21           86400000                  1            1495040 
        1000000000    18          604800000                  7            1495040 
       10000000000    14         9676800000                112            1495040 
      100000000000    11        63072000000                730            1495040 
     1000000000000     8       504576000000               5840            1495040 
    10000000000000     4      8073216000000              93440            1495040 
   100000000000000     1     64585728000000             747520            1495040 
  1000000000000000    -1                 -2 -2.3148148148148148e-08            1495040 
 10000000000000000    -1                 -2 -2.3148148148148148e-08            1495040 
100000000000000000    -1                 -2 -2.3148148148148148e-08            1495040 
		 */
		}
		if(globalPrintFlag){
		cout << "----" << endl << flush;
		}

		if(globalPrintFlag){
		cout	<< setw(18) << "iDelta" << "  "
			<< setw(4)  << "cR<iD" << " " 
			<< setw(18) << "ms@cR<iD" << " "
			<< setw(18) << "dy@cR<iD" << " "
			<< endl << flush;
		int64_t iDelta = 1;
		while( iDelta < 1000000000000000 ) {
			cout
			<< setw(18) << iDelta << "  "
			<< setw(4)  << tIndex.coarsestResolutionFinerOrEqualMilliseconds(iDelta) << " "
			<< setw(18) << tIndex.millisecondsAtResolution(tIndex.coarsestResolutionFinerOrEqualMilliseconds(iDelta)) << " "
			<< setw(18) << tIndex.millisecondsAtResolution(tIndex.coarsestResolutionFinerOrEqualMilliseconds(iDelta))/86400.0e3 << " "
			<< setw(18) << tIndex.getResolutionTimescaleDays() << " " 
			<< endl << flush;
			iDelta *= 2;
		}
		/*
            iDelta  cR<iD           ms@cR<iD           dy@cR<iD 
                 1    48                  1 1.1574074074074074e-08            1495040 
                 2    47                  2 2.3148148148148148e-08            1495040 
                 4    46                  4 4.6296296296296295e-08            1495040 
                 8    45                  8 9.2592592592592591e-08            1495040 
                16    44                 16 1.8518518518518518e-07            1495040 
                32    43                 32 3.7037037037037036e-07            1495040 
                64    42                 64 7.4074074074074073e-07            1495040 
               128    41                128 1.4814814814814815e-06            1495040 
               256    40                256 2.9629629629629629e-06            1495040 
               512    39                512 5.9259259259259258e-06            1495040 
              1024    38               1000 1.1574074074074073e-05            1495040 
              2048    37               2000 2.3148148148148147e-05            1495040 
              4096    36               4000 4.6296296296296294e-05            1495040 
              8192    35               8000 9.2592592592592588e-05            1495040 
             16384    34              16000 0.00018518518518518518            1495040 
             32768    33              32000 0.00037037037037037035            1495040 
             65536    32              60000 0.00069444444444444447            1495040 
            131072    31             120000 0.0013888888888888889            1495040 
            262144    30             240000 0.0027777777777777779            1495040 
            524288    29             480000 0.0055555555555555558            1495040 
           1048576    28             960000 0.011111111111111112            1495040 
           2097152    27            1920000 0.022222222222222223            1495040 
           4194304    26            3600000 0.041666666666666664            1495040 
           8388608    25            7200000 0.083333333333333329            1495040 
          16777216    24           14400000 0.16666666666666666            1495040 
          33554432    23           28800000 0.33333333333333331            1495040 
          67108864    22           57600000 0.66666666666666663            1495040 
         134217728    21           86400000                  1            1495040 
         268435456    20          172800000                  2            1495040 
         536870912    19          345600000                  4            1495040 
        1073741824    18          604800000                  7            1495040 
        2147483648    17         1209600000                 14            1495040 
        4294967296    16         2419200000                 28            1495040 
        8589934592    15         4838400000                 56            1495040 
       17179869184    14         9676800000                112            1495040 
       34359738368    12        31536000000                365            1495040 
       68719476736    11        63072000000                730            1495040 
      137438953472    10       126144000000               1460            1495040 
      274877906944     9       252288000000               2920            1495040 
      549755813888     8       504576000000               5840            1495040 
     1099511627776     7      1009152000000              11680            1495040 
     2199023255552     6      2018304000000              23360            1495040 
     4398046511104     5      4036608000000              46720            1495040 
     8796093022208     4      8073216000000              93440            1495040 
    17592186044416     3     16146432000000             186880            1495040 
    35184372088832     2     32292864000000             373760            1495040 
    70368744177664     1     64585728000000             747520            1495040 
   140737488355328    -1                 -2 -2.3148148148148148e-08            1495040 
   281474976710656    -1                 -2 -2.3148148148148148e-08            1495040 
   562949953421312    -1                 -2 -2.3148148148148148e-08            1495040 

		 */
		}
		if(globalPrintFlag){
		cout << "----" << endl << flush;
		}

		
		level = 10;
		tIndex1.setZero().fromFormattedJulianTAI(2000, 12, 31, 23, 59, 59, 999).set_forward_resolution(level).set_reverse_resolution(level); INDEX_OUTN_NC(++tag_id,tIndex1);

		if(globalPrintFlag){
		cout << " res_days: " << tIndex1.getResolutionTimescaleDays() << endl << flush;
		}
		// level = 54;
		level = 48;
		tIndex2.setZero().fromFormattedJulianTAI(1990, 12, 31, 23, 59, 59, 999).set_forward_resolution(level).set_reverse_resolution(level); INDEX_OUTN_NC(++tag_id,tIndex2);

		if(globalPrintFlag){
		cout << " res_days: " << tIndex1.getResolutionTimescaleDays() << endl << flush;
		cout << " dif_days: " << diff_JulianTAIDays(tIndex1,tIndex2) << endl << flush;
		cout << " cmp_days: " << cmp_JulianTAIDays(tIndex1,tIndex2) << endl << flush;
		cout << "----" << endl << flush;
		}
		// level = 40;
		level = 34;
		tIndex1.setZero().fromFormattedJulianTAI(2000, 12, 31, 23, 59, 59, 999).set_forward_resolution(level).set_reverse_resolution(level); INDEX_OUTN_NC(++tag_id,tIndex1);

		if(globalPrintFlag){
		  cout << " res_days: " << tIndex1.getResolutionTimescaleDays() << " " 
		       << tIndex1.getReverseResolutionTimescaleDays() << " " 
		       << tIndex1.getForwardResolutionTimescaleDays() << " " 
		       << endl << flush;
		}
		// level = 54;
		level = 48;
		tIndex2.setZero().fromFormattedJulianTAI(1990, 12, 31, 23, 59, 59, 999).set_forward_resolution(level).set_reverse_resolution(level); INDEX_OUTN_NC(++tag_id,tIndex2);

		if(globalPrintFlag){
		  cout << " res_days: " << tIndex2.getResolutionTimescaleDays() << " " 
		       << tIndex2.getReverseResolutionTimescaleDays() << " " 
		       << tIndex2.getForwardResolutionTimescaleDays() << " " 
		       << endl << flush;
		  cout << " dif_days: " << diff_JulianTAIDays(tIndex1,tIndex2) << endl << flush;
		  cout << " cmp_days: " << cmp_JulianTAIDays(tIndex1,tIndex2) << endl << flush;
		}
		ASSERT_EQUAL(0.000185185185185185,tIndex1.getResolutionTimescaleDays());

		try {
			failureMessage = "'";
			ASSERT_EQUAL(3653,diff_JulianTAIDays(tIndex1,tIndex2));
		} catch (SpatialFailure& failure) {
			failureMessage += failure.what(); failureMessage += "'";
			cout << "failureMessage: " << failureMessage << "'" << endl << flush;
		}

		ASSERT(!cmp_JulianTAIDays(tIndex1,tIndex2));

		if(globalPrintFlag){
		cout << "----" << endl << flush;
		}
		tIndex1.setZero().fromFormattedJulianTAI(2000, 12, 31, 23, 59, 59, 999).setResolutionFromTimescaleDays(1000);

		INDEX_OUTN_NC(++tag_id,tIndex1);
		if(globalPrintFlag){
		cout << " res_days: " << tIndex1.getResolutionTimescaleDays() << endl << flush;
		}
		// level = 54;
		level = 48;
		tIndex2.setZero().fromFormattedJulianTAI(1990, 12, 31, 23, 59, 59, 999).set_forward_resolution(level).set_reverse_resolution(level);

		INDEX_OUTN_NC(++tag_id,tIndex2);
		if(globalPrintFlag){
		cout << " res_days: " << tIndex1.getResolutionTimescaleDays() << endl << flush;
		cout << " dif_days: " << diff_JulianTAIDays(tIndex1,tIndex2) << endl << flush;
		cout << " cmp_days: " << cmp_JulianTAIDays(tIndex1,tIndex2) << endl << flush;
		}

		ASSERT_EQUAL(1460,tIndex1.getResolutionTimescaleDays());
		ASSERT_EQUAL(3653,diff_JulianTAIDays(tIndex1,tIndex2));
		ASSERT(!cmp_JulianTAIDays(tIndex1,tIndex2));
		if(globalPrintFlag){
		cout << "----" << endl << flush;
		}
		tIndex1.setZero().fromFormattedJulianTAI(2000, 12, 31, 23, 59, 59, 999).setResolutionFromTimescaleDays(3653);

		INDEX_OUTN_NC(++tag_id,tIndex1);
		if(globalPrintFlag){
		cout << " res_days: " << tIndex1.getResolutionTimescaleDays() << endl << flush;
		}
		// level = 54;
		level = 48;
		tIndex2.setZero().fromFormattedJulianTAI(1990, 12, 31, 23, 59, 59, 999).set_forward_resolution(level).set_reverse_resolution(level);
		ASSERT_EQUAL(5840,tIndex1.getResolutionTimescaleDays());
		ASSERT_EQUAL(3653,diff_JulianTAIDays(tIndex1,tIndex2));
		ASSERT(cmp_JulianTAIDays(tIndex1,tIndex2));

		INDEX_OUTN_NC(++tag_id,tIndex2);
		if(globalPrintFlag){
			cout << " res_days: " << tIndex1.getResolutionTimescaleDays() << endl << flush;
			cout << " dif_days: " << diff_JulianTAIDays(tIndex1,tIndex2) << endl << flush;
			cout << " cmp_days: " << cmp_JulianTAIDays(tIndex1,tIndex2) << endl << flush;
		}
		ASSERT_EQUAL(5840,tIndex1.getResolutionTimescaleDays());
		ASSERT_EQUAL(3653,diff_JulianTAIDays(tIndex1,tIndex2));
		ASSERT(cmp_JulianTAIDays(tIndex1,tIndex2));

		}
#undef FMT_DT1

	}

#if 0
	if(false) {
		cout << endl << "Print some resolution information." << endl << flush;
		STARE index;

		for( int i=0; i<64; ++i) {
			int resolution = i;
			double days = index.tIndex.daysAtResolution(resolution);
			cout << "temp res level,days: " << resolution << " " << days << " hours = " << days*24.0 << endl << flush;
		}
/*

///--- The new type 1. Two-sided resolution.
///---

                Print some resolution information.
                temp res level,days: 0 1495040 hours = 35880960
                temp res level,days: 1 747520 hours = 17940480
                temp res level,days: 2 373760 hours = 8970240
                temp res level,days: 3 186880 hours = 4485120
                temp res level,days: 4 93440 hours = 2242560
                temp res level,days: 5 46720 hours = 1121280
                temp res level,days: 6 23360 hours = 560640
                temp res level,days: 7 11680 hours = 280320
                temp res level,days: 8 5840 hours = 140160
                temp res level,days: 9 2920 hours = 70080
                temp res level,days: 10 1460 hours = 35040
                temp res level,days: 11 730 hours = 17520
                temp res level,days: 12 365 hours = 8760
                temp res level,days: 13 224 hours = 5376
                temp res level,days: 14 112 hours = 2688
                temp res level,days: 15 56 hours = 1344
                temp res level,days: 16 28 hours = 672
                temp res level,days: 17 14 hours = 336
                temp res level,days: 18 7 hours = 168
                temp res level,days: 19 4 hours = 96
                temp res level,days: 20 2 hours = 48
                temp res level,days: 21 1 hours = 24
                temp res level,days: 22 0.66666666666666663 hours = 16
                temp res level,days: 23 0.33333333333333331 hours = 8
                temp res level,days: 24 0.16666666666666666 hours = 4
                temp res level,days: 25 0.083333333333333329 hours = 2
                temp res level,days: 26 0.041666666666666664 hours = 1
                temp res level,days: 27 0.022222222222222223 hours = 0.53333333333333333
                temp res level,days: 28 0.011111111111111112 hours = 0.26666666666666666
                temp res level,days: 29 0.0055555555555555558 hours = 0.13333333333333333
                temp res level,days: 30 0.0027777777777777779 hours = 0.066666666666666666
                temp res level,days: 31 0.0013888888888888889 hours = 0.033333333333333333
                temp res level,days: 32 0.00069444444444444447 hours = 0.016666666666666666
                temp res level,days: 33 0.00037037037037037035 hours = 0.0088888888888888889
                temp res level,days: 34 0.00018518518518518518 hours = 0.0044444444444444444
                temp res level,days: 35 9.2592592592592588e-05 hours = 0.0022222222222222222
                temp res level,days: 36 4.6296296296296294e-05 hours = 0.0011111111111111111
                temp res level,days: 37 2.3148148148148147e-05 hours = 0.00055555555555555556
                temp res level,days: 38 1.1574074074074073e-05 hours = 0.00027777777777777778
                temp res level,days: 39 5.9259259259259258e-06 hours = 0.00014222222222222221
                temp res level,days: 40 2.9629629629629629e-06 hours = 7.1111111111111106e-05
                temp res level,days: 41 1.4814814814814815e-06 hours = 3.5555555555555553e-05
                temp res level,days: 42 7.4074074074074073e-07 hours = 1.7777777777777777e-05
                temp res level,days: 43 3.7037037037037036e-07 hours = 8.8888888888888883e-06
                temp res level,days: 44 1.8518518518518518e-07 hours = 4.4444444444444441e-06
                temp res level,days: 45 9.2592592592592591e-08 hours = 2.2222222222222221e-06
                temp res level,days: 46 4.6296296296296295e-08 hours = 1.111111111111111e-06
                temp res level,days: 47 2.3148148148148148e-08 hours = 5.5555555555555552e-07
                temp res level,days: 48 1.1574074074074074e-08 hours = 2.7777777777777776e-07
                temp res level,days: 49 0 hours = 0
                temp res level,days: 50 0 hours = 0
                temp res level,days: 51 0 hours = 0
                temp res level,days: 52 0 hours = 0
                temp res level,days: 53 0 hours = 0
                temp res level,days: 54 0 hours = 0
                temp res level,days: 55 0 hours = 0
                temp res level,days: 56 0 hours = 0
                temp res level,days: 57 0 hours = 0
                temp res level,days: 58 0 hours = 0
                temp res level,days: 59 0 hours = 0
                temp res level,days: 60 0 hours = 0
                temp res level,days: 61 0 hours = 0
                temp res level,days: 62 0 hours = 0
                temp res level,days: 63 0 hours = 0

///--- The old type 2. One-sided resolution.
///---
		Print some resolution information.
		temp res level,days: 0 9.56826e+07 hours = 2.29638e+09
		temp res level,days: 1 4.78413e+07 hours = 1.14819e+09
		temp res level,days: 2 2.39206e+07 hours = 5.74095e+08
		temp res level,days: 3 1.19603e+07 hours = 2.87048e+08
		temp res level,days: 4 5.98016e+06 hours = 1.43524e+08
		temp res level,days: 5 2.99008e+06 hours = 7.17619e+07
		temp res level,days: 6 1.49504e+06 hours = 3.5881e+07
		temp res level,days: 7 747520 hours = 1.79405e+07
		temp res level,days: 8 373760 hours = 8.97024e+06
		temp res level,days: 9 186880 hours = 4.48512e+06
		temp res level,days: 10 93440 hours = 2.24256e+06
		temp res level,days: 11 46720 hours = 1.12128e+06
		temp res level,days: 12 23360 hours = 560640
		temp res level,days: 13 11680 hours = 280320
		temp res level,days: 14 5840 hours = 140160
		temp res level,days: 15 2920 hours = 70080
		temp res level,days: 16 1460 hours = 35040
		temp res level,days: 17 730 hours = 17520
		temp res level,days: 18 365 hours = 8760
		temp res level,days: 19 224 hours = 5376
		temp res level,days: 20 112 hours = 2688
		temp res level,days: 21 56 hours = 1344
		temp res level,days: 22 28 hours = 672
		temp res level,days: 23 14 hours = 336
		temp res level,days: 24 7 hours = 168
		temp res level,days: 25 4 hours = 96
		temp res level,days: 26 2 hours = 48
		temp res level,days: 27 1 hours = 24
		temp res level,days: 28 0.666667 hours = 16
		temp res level,days: 29 0.333333 hours = 8
		temp res level,days: 30 0.166667 hours = 4
		temp res level,days: 31 0.0833333 hours = 2
		temp res level,days: 32 0.0416667 hours = 1
		temp res level,days: 33 0.0222222 hours = 0.533333
		temp res level,days: 34 0.0111111 hours = 0.266667
		temp res level,days: 35 0.00555556 hours = 0.133333
		temp res level,days: 36 0.00277778 hours = 0.0666667
		temp res level,days: 37 0.00138889 hours = 0.0333333
		temp res level,days: 38 0.000694444 hours = 0.0166667
		temp res level,days: 39 0.00037037 hours = 0.00888889
		temp res level,days: 40 0.000185185 hours = 0.00444444
		temp res level,days: 41 9.25926e-05 hours = 0.00222222
		temp res level,days: 42 4.62963e-05 hours = 0.00111111
		temp res level,days: 43 2.31481e-05 hours = 0.000555556
		temp res level,days: 44 1.15741e-05 hours = 0.000277778
		temp res level,days: 45 5.92593e-06 hours = 0.000142222
		temp res level,days: 46 2.96296e-06 hours = 7.11111e-05
		temp res level,days: 47 1.48148e-06 hours = 3.55556e-05
		temp res level,days: 48 7.40741e-07 hours = 1.77778e-05
		temp res level,days: 49 3.7037e-07 hours = 8.88889e-06
		temp res level,days: 50 1.85185e-07 hours = 4.44444e-06
		temp res level,days: 51 9.25926e-08 hours = 2.22222e-06
		temp res level,days: 52 4.62963e-08 hours = 1.11111e-06
		temp res level,days: 53 2.31481e-08 hours = 5.55556e-07
		temp res level,days: 54 1.15741e-08 hours = 2.77778e-07
		temp res level,days: 55 0 hours = 0
		temp res level,days: 56 0 hours = 0
		temp res level,days: 57 0 hours = 0
		temp res level,days: 58 0 hours = 0
		temp res level,days: 59 0 hours = 0
		temp res level,days: 60 0 hours = 0
		temp res level,days: 61 0 hours = 0
		temp res level,days: 62 0 hours = 0
		temp res level,days: 63 0 hours = 0
		*/
	}
#endif


	{
	  TemporalIndex tIndex;
	  int64_t forward_resolution = 48;
	  int64_t reverse_resolution = 48;

	  ENDL(cout);
	  // cout << "max res: " << tIndex.data.maxResolutionLevel() << endl << flush;

	  tIndex.set_BeforeAfterStartBit(1).set_year(2020).set_reverse_resolution(reverse_resolution).set_forward_resolution(forward_resolution);
	  // tIndex.setZero().setEOY(1,2020);

	  globalPrintFlag = false; // true

	  ENDL(cout);

	  int resolution = 20; // 2 days

	  int64_t tv0 = tIndex.scidbTemporalIndex();
	  int64_t tv1 = scidbClearBitsFinerThanResolution(tv0,resolution);
	  int64_t tv2 = scidbSetBitsFinerThanResolution(tv0,resolution);
	  
#if 0
	  cout << "resolution ms " << tIndex.millisecondsAtResolution(resolution) << endl << flush;
	  cout << "resolution dy " << tIndex.daysAtResolution(resolution) << endl << flush;
#endif

	  FMT("tv0 ",tv0); ENDL(cout);
	  FMT("tv1 ",tv1); ENDL(cout);
	  FMT("tv2 ",tv2); ENDL(cout);

	  ENDL(cout);

	  tIndex.fromTemporalIndexValue(tv0);
	  INDEX_OUT("tv0 ",tIndex);

	  tIndex.fromTemporalIndexValue(tv1);
	  INDEX_OUT("tv1 ",tIndex);

	  tIndex.fromTemporalIndexValue(tv2);
	  INDEX_OUT("tv2 ",tIndex);
	  
	  globalPrintFlag = false;
	}

	if(false)
	{
	  TemporalIndex tIndex;
	  int64_t forward_resolution = 48;
	  int64_t reverse_resolution = 48;

	  ENDL(cout);
	  // cout << "max res: " << tIndex.data.maxResolutionLevel() << endl << flush;

	  globalPrintFlag = false; // true

	  ENDL(cout);

	  int resolution = 21; // 1 day
	  // int resolution = 48; // 1 ms // No effect
	  // int resolution = 47; // 2 ms
	  // int resolution = 46; // 4 ms

	  reverse_resolution = resolution; forward_resolution = resolution;
	  tIndex.set_BeforeAfterStartBit(1).set_year(2020).set_reverse_resolution(reverse_resolution).set_forward_resolution(forward_resolution);
	  // tIndex.setZero().setEOY(1,2020);

	  int64_t tvl = tIndex.scidbLowerBound();
	  int64_t tv0 = tIndex.scidbTemporalIndex();
	  int64_t tv1 = scidbClearBitsFinerThanResolution(tv0,resolution);
	  int64_t tv2 = scidbSetBitsFinerThanResolution(tv0,resolution);
	  int64_t tv3 = scidbSetBitsFinerThanResolutionLimited(tv0,resolution);
	  int64_t tvt = tIndex.scidbTerminator();
#if 0
	  cout << "resolution ms " << tIndex.millisecondsAtResolution(resolution) << endl << flush;
	  cout << "resolution dy " << tIndex.daysAtResolution(resolution) << endl << flush;	  
#endif  
	  FMT("tvl ",tvl); ENDL(cout);
	  FMT("tv0 ",tv0); ENDL(cout);
	  FMT("tv1 ",tv1); ENDL(cout);
	  FMT("tv2 ",tv2); ENDL(cout);
	  FMT("tv3 ",tv3); ENDL(cout);
	  FMT("tvt ",tvt); ENDL(cout);

	  ENDL(cout);

	  tIndex.fromTemporalIndexValue(tvl);
	  INDEX_OUT("tvl ",tIndex);
	  
	  tIndex.fromTemporalIndexValue(tv0);
	  INDEX_OUT("tv0 ",tIndex);

	  tIndex.fromTemporalIndexValue(tv1);
	  INDEX_OUT("tv1 ",tIndex);

	  tIndex.fromTemporalIndexValue(tv2);
	  INDEX_OUT("tv2 ",tIndex);

	  tIndex.fromTemporalIndexValue(tv3);
	  INDEX_OUT("tv3 ",tIndex);

	  tIndex.fromTemporalIndexValue(tvt);
	  INDEX_OUT("tvt ",tIndex);
	  
	  ENDL(cout);

	  tIndex.set_BeforeAfterStartBit(1).set_year(2020).set_reverse_resolution(reverse_resolution).set_forward_resolution(forward_resolution);	  
	  int64_t tvl_tai = tIndex.scidbLowerBoundJulianTAI();
	  int64_t tvt_tai = tIndex.scidbTerminatorJulianTAI();

	  tIndex.fromTemporalIndexValue(tvl_tai);
	  INDEX_OUT("tla ",tIndex);

	  tIndex.fromTemporalIndexValue(tvt_tai);
	  INDEX_OUT("tta ",tIndex);

	  ENDL(cout);
	  
	  tIndex.checkBitFormat();

	  int64_t offsetBottom = tIndex.bitOffsetFinest();
	  int64_t offsetTop    = tIndex.bitOffsetResolution(resolution);
	  cout << "offsetBottom " << offsetBottom << endl << flush;
	  cout << "offsetTop    " << offsetTop << endl << flush;
	  
	  globalPrintFlag = false;
	}

	{
	  globalPrintFlag = false; // true

	  int tag_id = 10000;
	  TemporalIndex tIndex;
	  
	  tIndex.setZero().setEOY(1,2002); INDEX_OUT(++tag_id,tIndex);
	  if(globalPrintFlag){
	    cout
	      << " toStringJ0: " << tIndex.toStringJulianTAI()
	      << ", toStringJ0_ISO: " << tIndex.toStringJulianTAI_ISO()
	      << ", ms: " << tIndex.toInt64MillisecondsFractionOfYear()
	      << ", msj: " << tIndex.toInt64MillisecondsFractionOfYearJ()
	      << ", msp: " << millisecondsInYear(tIndex.get_BeforeAfterStartBit(),tIndex.get_year())
	      << endl << flush;
	    cout << "---" << endl << flush;
	  }
	  TemporalIndex tIndex0;
	  tIndex0.fromStringJulianTAI_ISO(tIndex.toStringJulianTAI_ISO());
	  if(globalPrintFlag){
	    cout
	      << " toStringJ0: " << tIndex0.toStringJulianTAI()
	      << ", toStringJ0_ISO: " << tIndex0.toStringJulianTAI_ISO()
	      << ", ms: " << tIndex0.toInt64MillisecondsFractionOfYear()
	      << ", msj: " << tIndex0.toInt64MillisecondsFractionOfYearJ()
	      << ", msp: " << millisecondsInYear(tIndex0.get_BeforeAfterStartBit(),tIndex0.get_year())
	      << endl << flush;
	    cout << "---" << endl << flush;
	  }
	  ASSERT_EQUAL(tIndex.toStringJulianTAI(), tIndex0.toStringJulianTAI());
	  ASSERT_EQUAL(tIndex.toStringJulianTAI_ISO(), tIndex0.toStringJulianTAI_ISO());
	  ASSERT_EQUAL(tIndex.toStringJulianTAI_ISO(),"2002-12-31T23:59:59.999 (00 00) (1)");
	  ASSERT_EQUAL(tIndex0.toStringJulianTAI_ISO(),"2002-12-31T23:59:59.999 (00 00) (1)");
		  
	  globalPrintFlag = false;
	}

	{
	  globalPrintFlag = false; // true

	  int tag_id = 11000;
	  
	  TemporalIndex tIndex0;
	  tIndex0.setZero().set_BeforeAfterStartBit(1).set_year(2002); INDEX_OUT(++tag_id,tIndex0);
	  if(globalPrintFlag){
	    cout
	      << " toStringJ0: " << tIndex0.toStringJulianTAI()
	      << ", toStringJ0_ISO: " << tIndex0.toStringJulianTAI_ISO()
	      << ", ms: " << tIndex0.toInt64MillisecondsFractionOfYear()
	      << ", msj: " << tIndex0.toInt64MillisecondsFractionOfYearJ()
	      << ", msp: " << millisecondsInYear(tIndex0.get_BeforeAfterStartBit(),tIndex0.get_year())
	      << endl << flush;
	    cout << "---" << endl << flush;
	  }

	  tIndex0.setZero().set_BeforeAfterStartBit(1).set_year(2002); INDEX_OUT(++tag_id,tIndex0);
	  int64_t t0L = tIndex0.scidbTemporalIndex();

	  tIndex0.setZero().set_BeforeAfterStartBit(1).set_year(2004); INDEX_OUT(++tag_id,tIndex0);
	  int64_t t0U = tIndex0.scidbTemporalIndex();

	  int64_t t0 = scidbNewTemporalValue(t0L,-1,t0U);

	  TemporalIndex tIndex1(t0);
	  // tIndex1.setZero().setEOY(1,2002); INDEX_OUT(++tag_id,tIndex1);
	  if(globalPrintFlag){
	    cout
	      << " toStringJ0: " << tIndex1.toStringJulianTAI()
	      << ", toStringJ0_ISO: " << tIndex1.toStringJulianTAI_ISO()
	      << ", ms: " << tIndex1.toInt64MillisecondsFractionOfYear()
	      << ", msj: " << tIndex1.toInt64MillisecondsFractionOfYearJ()
	      << ", msp: " << millisecondsInYear(tIndex1.get_BeforeAfterStartBit(),tIndex1.get_year())
	      << endl << flush;
	    cout << "---" << endl << flush;
	  }

	  TemporalIndex tIndex2;
	  tIndex2.fromStringJulianTAI_ISO("2003-01-01T00:00:00.000 (12 12) (1)");
	  if(globalPrintFlag){
	    cout
	      << " toStringJ0: " << tIndex2.toStringJulianTAI()
	      << ", toStringJ0_ISO: " << tIndex2.toStringJulianTAI_ISO()
	      << ", ms: " << tIndex2.toInt64MillisecondsFractionOfYear()
	      << ", msj: " << tIndex2.toInt64MillisecondsFractionOfYearJ()
	      << ", msp: " << millisecondsInYear(tIndex2.get_BeforeAfterStartBit(),tIndex2.get_year())
	      << endl << flush;
	    cout << "---" << endl << flush;
	  }
	  ASSERT_EQUAL(tIndex1.toStringJulianTAI_ISO(),tIndex2.toStringJulianTAI_ISO());

	  int64_t t2  = tIndex2.scidbTemporalIndex();
	  int64_t t2L = scidbLowerBoundMS(t2);
	  int64_t t2U = scidbUpperBoundMS(t2);
#if 0
	  FMT("t2L",t2L); cout << " " << TemporalIndex(t2L).toStringJulianTAI_ISO() << endl << flush;
	  FMT("t2 ", t2); cout << " " << TemporalIndex(t2).toStringJulianTAI_ISO() << endl << flush;
	  FMT("t2U",t2U); cout << " " << TemporalIndex(t2U).toStringJulianTAI_ISO() << endl << flush;
	  ENDL(cout);
#endif

	  FMT("t2 from string ",fromStringJulianTAI_ISO("2003-01-01T00:00:00.000 (12 12) (1)")); ENDL(cout);
	  // cout << "string from t2 " << toStringJulianTAI_ISO(0x1f4c000000000c31); ENDL(cout);
	  

	  // Roundtrip
	  ASSERT_EQUAL("2003-01-01T00:00:00.000 (12 12) (1)",
		       toStringJulianTAI_ISO(fromStringJulianTAI_ISO("2003-01-01T00:00:00.000 (12 12) (1)")));

	  ASSERT(scidbOverlap(fromStringJulianTAI_ISO("2003-01-01T00:00:00.000 (12 12) (1)"),
			      fromStringJulianTAI_ISO("2004-01-01T00:00:00.000 (12 12) (1)")));

	  ASSERT(!scidbOverlap(fromStringJulianTAI_ISO("2003-01-01T00:00:00.000 (12 12) (1)"),
			       fromStringJulianTAI_ISO("2010-01-01T00:00:00.000 (12 12) (1)")));

	  ASSERT(scidbOverlapTAI(fromStringJulianTAI_ISO("2003-01-01T00:00:00.000 (12 12) (1)"),
				 fromStringJulianTAI_ISO("2004-01-01T00:00:00.000 (12 12) (1)")));

	  ASSERT(!scidbOverlapTAI(fromStringJulianTAI_ISO("2003-01-01T00:00:00.000 (12 12) (1)"),
				  fromStringJulianTAI_ISO("2010-01-01T00:00:00.000 (12 12) (1)")));

	  // cout << "x1000" << endl << flush;
	  
#define FROM_SJ(str) fromStringJulianTAI_ISO(str)
#define FMTNEWTIV(label,tL,t0,tU,flag) { \
	    cout << "----" << label << "----" << endl << flush;\
	    int64_t ttmp = scidbNewTemporalValue(tL,t0,tU,flag);\
	    int64_t ttmpL=scidbLowerBoundMS(ttmp),ttmpU=scidbUpperBoundMS(ttmp);\
	    FMT("L ",ttmpL); cout << " " << TemporalIndex(ttmpL).toStringJulianTAI_ISO() << " - " << TemporalIndex(tL).toStringJulianTAI_ISO() << endl << flush; \
	    FMT("T ",ttmp);  cout << " " << TemporalIndex(ttmp).toStringJulianTAI_ISO()  << " - " << TemporalIndex(t0).toStringJulianTAI_ISO() << endl << flush; \
	    FMT("U ",ttmpU); cout << " " << TemporalIndex(ttmpU).toStringJulianTAI_ISO() << " - " << TemporalIndex(tU).toStringJulianTAI_ISO() << endl << flush; \
	    cout << "----" << endl << flush; }

	  if(false)
	  {
	    cout << "ck-bug-000" << endl << flush;

#if 0
	    int64_t ti_value = scidbNewTemporalValue(
					     fromStringJulianTAI_ISO("2003-01-18T12:00:00.000 (12 12) (1)"),
					     fromStringJulianTAI_ISO("2003-01-19T12:00:00.000 (12 12) (1)"),
					     fromStringJulianTAI_ISO("2003-01-26T12:00:00.000 (12 12) (1)"),
					     false
					     );
#endif

	    int64_t ti_value = scidbNewTemporalValue(
					     fromStringJulianTAI_ISO("2003-01-18T12:00:00.000 (12 12) (1)"),
					     fromStringJulianTAI_ISO("2003-01-19T12:00:00.000 (12 12) (1)"),
					     fromStringJulianTAI_ISO("2003-01-27T12:00:00.000 (12 12) (1)"),
					     true
					     );
	    
#if 0
	    int64_t ti_value = 0x1f4c180000001251;
#endif
	      
	    TemporalIndex tmpIndex(ti_value);
	    int64_t forward_resolution = tmpIndex.get_forward_resolution();

	    //////////////////////////////////
	    // Let's add an amount corresponding to the resolution.
	    int64_t delta = tmpIndex.millisecondsAtResolution(forward_resolution);
	    int64_t t0    = tmpIndex.toInt64Milliseconds();
	    int64_t t1    = t0 + delta;
	    tmpIndex.fromInt64Milliseconds(t1);

	    cout << "ckb-001: ti_v    " << toStringJulianTAI_ISO(ti_value) << endl << flush;
	    cout << "ckb-002: ti_v.ub " << toStringJulianTAI_ISO(scidbUpperBoundMS(ti_value)) << endl << flush;
	    cout << "ckb-100: delta   " << delta << endl << flush;
	    cout << "ckb-101: t0      " << t0 << endl << flush;
	    cout << "ckb-102: t1      " << t1 << endl << flush;
	    cout << "ckb-103: tmpSJT  " << tmpIndex.toStringJulianTAI() << endl << flush;
	    cout << "ckb-104: tmpSJTi " << tmpIndex.toStringJulianTAI_ISO() << endl << flush;
	    cout << "ckb-105: tivJT   " << toStringJulianTAI_ISO(scidbUpperBoundMS(ti_value)) << endl << flush;

	    // Set the terminator type. Elsewhere we hard code this to 63... The following is correct.
	    tmpIndex.set_forward_resolution(tmpIndex.data.get("forward_resolution")->getMask());
	    tmpIndex.set_reverse_resolution(tmpIndex.data.get("reverse_resolution")->getMask()); // Terminator doesn't care about the reverse resolution.
  
	    // int64_t idx_ = tmpIndex.scidbTemporalIndex();
	  }

	  if(false)
	  {
	  FMTNEWTIV("CASE-1 2003-01-10T12:00:00.000 (12 12) (1) - false",
		    FROM_SJ("2003-01-09T12:00:00.000 (12 12) (1)"),
		    FROM_SJ("2003-01-10T12:00:00.000 (12 12) (1)"),
		    FROM_SJ("2003-01-11T12:00:00.000 (12 12) (1)"),
		    false);

	  FMTNEWTIV("CASE-2 2003-01-10T12:00:00.000 (12 12) (1) - true",
		    FROM_SJ("2003-01-09T12:00:00.000 (12 12) (1)"),
		    FROM_SJ("2003-01-10T12:00:00.000 (12 12) (1)"),
		    FROM_SJ("2003-01-11T12:00:00.000 (12 12) (1)"),
		    true);

	  FMTNEWTIV("CASE-3 2003-01-10T12:00:00.000 (12 12) (1) - false",
		    FROM_SJ("2003-01-09T12:00:00.000 (12 12) (1)"),
		    FROM_SJ("2003-01-10T12:00:00.000 (12 12) (1)"),
		    FROM_SJ("2003-01-17T12:00:00.000 (12 12) (1)"),
		    false);

	  FMTNEWTIV( "CASE-4 2003-01-10T00:00:00.000 (12 12) (1) - false",
		    FROM_SJ("2003-01-09T00:00:00.000 (12 12) (1)"),
		    FROM_SJ("2003-01-10T00:00:00.000 (12 12) (1)"),
		    FROM_SJ("2003-01-17T00:00:00.000 (12 12) (1)"),
		    false);

	  FMTNEWTIV( "CASE-5 2003-01-10T00:00:00.000 (12 12) (1) - true",
		    FROM_SJ("2003-01-09T00:00:00.000 (12 12) (1)"),
		    FROM_SJ("2003-01-10T00:00:00.000 (12 12) (1)"),
		    FROM_SJ("2003-01-17T00:00:00.000 (12 12) (1)"),
		    true);

	  FMTNEWTIV( "CASE-6 2003-01-12T00:00:00.000 (12 12) (1) - 19 or 20? - true",
		    FROM_SJ("2003-01-10T00:00:00.000 (12 12) (1)"),
		    FROM_SJ("2003-01-12T00:00:00.000 (12 12) (1)"),
		    FROM_SJ("2003-01-20T00:00:00.000 (12 12) (1)"),
		    true);
	  
	  FMTNEWTIV( "CASE-7 2003-01-12T00:00:00.000 (12 12) (1) - 19 or 20? - false",
		    FROM_SJ("2003-01-10T00:00:00.000 (12 12) (1)"),
		    FROM_SJ("2003-01-12T00:00:00.000 (12 12) (1)"),
		    FROM_SJ("2003-01-20T00:00:00.000 (12 12) (1)"),
		    false);	  
	  }
	  
	  if(false)
	  {
	    string check_date = "2003-01-20T00:00:00.000 (12 12) (1)";
	    cout << "CHECK: " << check_date << endl << flush;
	    int64_t ti_check = FROM_SJ(check_date);
	    TemporalIndex tCheck(ti_check);
	    FMT("ti_check",ti_check); cout << " " << TemporalIndex(ti_check).toStringJulianTAI_ISO() << endl << flush;	    
	  }

	  if(false)
	  { // Res check
	  cout << "---Res check--" << endl << flush;
	  int64_t t2 = scidbNewTemporalValue(
					     -1,
					     fromStringJulianTAI_ISO("2003-01-12T12:00:00.000 (12 22) (1)"),
					     -1,
					     false
					     );
	  int64_t t2L = scidbLowerBoundMS(t2);
	  int64_t t2U = scidbUpperBoundMS(t2);
	  cout << "--false--" << endl << flush;
	  FMT("t2L",t2L); cout << " " << TemporalIndex(t2L).toStringJulianTAI_ISO() << endl << flush;
	  FMT("t2 ", t2); cout << " " << TemporalIndex(t2).toStringJulianTAI_ISO() << endl << flush;
	  FMT("t2U",t2U); cout << " " << TemporalIndex(t2U).toStringJulianTAI_ISO() << endl << flush;
	  ENDL(cout);
	  }

	  // cout << "x2000" << endl << flush;
	  
	  { // Asymetric test
	    // cout << "---Asym check--" << endl << flush;
	    
	    int64_t t2 = scidbNewTemporalValue(
					     fromStringJulianTAI_ISO("2003-01-10T00:00:00.000 (12 12) (1)"),
					     fromStringJulianTAI_ISO("2003-01-12T00:00:00.000 (12 12) (1)"),
					     fromStringJulianTAI_ISO("2003-01-20T00:00:00.000 (12 12) (1)"),
					     true
					     );
	  int64_t t2L = scidbLowerBoundMS(t2);
	  int64_t t2U = scidbUpperBoundMS(t2);
#if 0
	  cout << "--true--" << endl << flush;
	  FMT("t2L",t2L); cout << " " << TemporalIndex(t2L).toStringJulianTAI_ISO() << endl << flush;
	  FMT("t2 ", t2); cout << " " << TemporalIndex(t2).toStringJulianTAI_ISO() << endl << flush;
	  FMT("t2U",t2U); cout << " " << TemporalIndex(t2U).toStringJulianTAI_ISO() << endl << flush;
	  ENDL(cout);
#endif

	  if(false)
	  {
	  int64_t t2 = scidbNewTemporalValue(
					     fromStringJulianTAI_ISO("2003-01-10T00:00:00.000 (12 12) (1)"),
					     fromStringJulianTAI_ISO("2003-01-12T00:00:00.000 (12 12) (1)"),
					     fromStringJulianTAI_ISO("2003-01-20T00:00:00.000 (12 12) (1)"),
					     false
					     );
	  int64_t t2L = scidbLowerBoundMS(t2);
	  int64_t t2U = scidbUpperBoundMS(t2);
	  cout << "--false--" << endl << flush;
	  FMT("t2L",t2L); cout << " " << TemporalIndex(t2L).toStringJulianTAI_ISO() << endl << flush;
	  FMT("t2 ", t2); cout << " " << TemporalIndex(t2).toStringJulianTAI_ISO() << endl << flush;
	  FMT("t2U",t2U); cout << " " << TemporalIndex(t2U).toStringJulianTAI_ISO() << endl << flush;
	  ENDL(cout);
	  }
	  
	  // cout << "finest_resolution " << tIndex2.data.maxResolutionLevel() << endl << flush; // finest res is 48 MLR 2021-0614
	  

	  // cout << "x3000" << endl << flush;
	  
	  ASSERT(scidbContainsInstant( fromStringJulianTAI_ISO("2003-01-12T00:00:00.000 (17 21) (1)")
				      ,fromStringJulianTAI_ISO("2003-01-20T00:00:00.000 (22 22) (1)")));

	  ASSERT(scidbContainsInstant(0x1f4c180000001151,fromStringJulianTAI_ISO("2003-01-12T00:00:00.000 (17 20) (1)")));
	  ASSERT(scidbContainsInstant(0x1f4c180000001251,fromStringJulianTAI_ISO("2003-01-12T00:00:00.000 (18 20) (1)")));
	  ASSERT(!scidbContainsInstant(0x1f4c180000001251,fromStringJulianTAI_ISO("2003-01-20T00:00:00.000 (22 22) (1)")));
	  ASSERT(!scidbContainsInstant(0x1f4c180000001251,fromStringJulianTAI_ISO("2004-01-20T00:00:00.000 (22 22) (1)")));

	  // cout << "x4000" << endl << flush;	  
	  
	  ASSERT(scidbOverlapTAI(fromStringJulianTAI_ISO("2003-01-01T00:00:00.000 (12 12) (1)"),t2));

	  // cout << "x4500" << endl << flush;	  
	  
	  ASSERT(scidbOverlapTAI(fromStringJulianTAI_ISO("2003-01-21T00:00:00.000 (22 22) (1)"),t2)); // The upper bound is sloppy because of factor of 2.
	  ASSERT(!scidbOverlapTAI(fromStringJulianTAI_ISO("2003-01-27T00:00:00.000 (22 22) (1)"),t2));

	  // cout << "x5000" << endl << flush;
	  

	  }

	  ASSERT_EQUAL(20,reverse_resolution(0x1f4c180000001251));
	  ASSERT_EQUAL(18,forward_resolution(0x1f4c180000001251));
	  ASSERT_EQUAL(21,reverse_resolution(set_reverse_resolution(0x1f4c180000001251,21)));
	  ASSERT_EQUAL(0x1f4c180000001255,set_reverse_resolution(0x1f4c180000001251,21));
	  ASSERT_EQUAL(17,forward_resolution(set_forward_resolution(0x1f4c180000001251,17)));
	  ASSERT_EQUAL(0x1f4c180000001151,set_forward_resolution(0x1f4c180000001251,17));
	  ASSERT_EQUAL(0x1f4c180000001355,coarsen(0x1f4c180000001251,1,1));
	  
	  globalPrintFlag = false;
	}

	{

	  globalPrintFlag = false; // true

	    int64_t tiv0 = scidbNewTemporalValue(
					     fromStringJulianTAI_ISO("2003-01-18T12:00:00.000 (12 12) (1)"),
					     fromStringJulianTAI_ISO("2003-01-19T12:00:00.000 (12 12) (1)"),
					     fromStringJulianTAI_ISO("2003-01-27T12:00:00.000 (12 12) (1)"),
					     true
					     );

	    int64_t tiv1 = scidbNewTemporalValue(
					     fromStringJulianTAI_ISO("2003-01-28T12:00:00.000 (12 12) (1)"),
					     fromStringJulianTAI_ISO("2003-01-29T12:00:00.000 (12 12) (1)"),
					     fromStringJulianTAI_ISO("2003-01-30T12:00:00.000 (12 12) (1)"),
					     true
					     );

	    int64_t tiv2 = scidbNewTemporalValue(
					     fromStringJulianTAI_ISO("2003-02-12T12:00:00.000 (12 12) (1)"),
					     fromStringJulianTAI_ISO("2003-02-13T12:00:00.000 (12 12) (1)"),
					     fromStringJulianTAI_ISO("2003-02-14T12:00:00.000 (12 12) (1)"),
					     true
					     );

	    // cout << 1000 << endl << flush;
	    int64_t tiv0and1 = scidbTemporalValueIntersectionIfOverlap(tiv0,tiv1);
	    int64_t tiv0or1 = scidbTemporalValueUnionIfOverlap(tiv0,tiv1);
	    int64_t tiv02;

	    ASSERT_EQUAL(0x1f4c40c000001555,tiv0and1);
	    ASSERT_EQUAL(0x1f4c380000001145,tiv0or1);

	    try {
	      tiv02 = scidbTemporalValueIntersectionIfOverlap(tiv0,tiv2);
	    } catch (const SpatialException & e) {
	      ASSERT_EQUAL("scidbTemporalValueIntersectionIfOverlap:NoOverlap",e.what());
	    }
	    try {
	      tiv02 = scidbTemporalValueUnionIfOverlap(tiv0,tiv2);
	    } catch (const SpatialException & e) {
	      ASSERT_EQUAL("scidbTemporalValueUnionIfOverlap:NoOverlap",e.what());
	    }
	    
#if 0
	    cout << 1100 << endl << flush;
	    FMT("tiv0and1   ", tiv0and1); cout << " " << TemporalIndex(tiv0and1).toStringJulianTAI_ISO() << endl << flush;
	    
	    {
	    int64_t t2 = tiv0;
	    int64_t t2L = scidbLowerBoundMS(t2);
	    int64_t t2U = scidbUpperBoundMS(t2);
	    cout << "tiv0" << endl << flush;
	    FMT("t2L",t2L); cout << " " << TemporalIndex(t2L).toStringJulianTAI_ISO() << endl << flush;
	    FMT("t2 ", t2); cout << " " << TemporalIndex(t2).toStringJulianTAI_ISO() << endl << flush;
	    FMT("t2U",t2U); cout << " " << TemporalIndex(t2U).toStringJulianTAI_ISO() << endl << flush;
	    ENDL(cout);
	    }
	    {
	    int64_t t2 = tiv1;
	    int64_t t2L = scidbLowerBoundMS(t2);
	    int64_t t2U = scidbUpperBoundMS(t2);
	    cout << "tiv1" << endl << flush;
	    FMT("t2L",t2L); cout << " " << TemporalIndex(t2L).toStringJulianTAI_ISO() << endl << flush;
	    FMT("t2 ", t2); cout << " " << TemporalIndex(t2).toStringJulianTAI_ISO() << endl << flush;
	    FMT("t2U",t2U); cout << " " << TemporalIndex(t2U).toStringJulianTAI_ISO() << endl << flush;
	    ENDL(cout);
	    }	    
	    {
	    int64_t t2 = tiv0and1;
	    int64_t t2L = scidbLowerBoundMS(t2);
	    int64_t t2U = scidbUpperBoundMS(t2);
	    cout << "tiv0and1" << endl << flush;
	    FMT("t2L",t2L); cout << " " << TemporalIndex(t2L).toStringJulianTAI_ISO() << endl << flush;
	    FMT("t2 ", t2); cout << " " << TemporalIndex(t2).toStringJulianTAI_ISO() << endl << flush;
	    FMT("t2U",t2U); cout << " " << TemporalIndex(t2U).toStringJulianTAI_ISO() << endl << flush;
	    ENDL(cout);
	    }
	    {
	    int64_t t2 = tiv0or1;
	    int64_t t2L = scidbLowerBoundMS(t2);
	    int64_t t2U = scidbUpperBoundMS(t2);
	    cout << "tiv0or1" << endl << flush;
	    FMT("t2L",t2L); cout << " " << TemporalIndex(t2L).toStringJulianTAI_ISO() << endl << flush;
	    FMT("t2 ", t2); cout << " " << TemporalIndex(t2).toStringJulianTAI_ISO() << endl << flush;
	    FMT("t2U",t2U); cout << " " << TemporalIndex(t2U).toStringJulianTAI_ISO() << endl << flush;
	    ENDL(cout);
	    }
	    
#endif
	    globalPrintFlag = false;

	}

	if( true ) {
	  int n = 4;
	  int64_t t_index[4];
	  {
	    int i = 0;
	    t_index[i++] = fromStringJulianTAI_ISO("2003-02-13T12:00:00.000 (12 12) (1)");
	    t_index[i++] = fromStringJulianTAI_ISO("2004-02-13T12:00:00.000 (12 12) (1)");
	    t_index[i++] = fromStringJulianTAI_ISO("2004-03-13T12:00:00.000");
	    t_index[i++] = fromStringJulianTAI_ISO("2004-04-13T12:00:00");
	  }

	  set_temporal_resolutions_from_sorted_inplace(t_index,n);

	  vector< vector< string > > expected_
	    = {
             {"2003-02-13T11:59:59.999 (00 63) (1)", "2003-02-13T12:00:00.000 (12 48) (1)", "2004-02-13T12:00:00.000 (63 63) (1)"}
            ,{"2003-02-13T12:00:00.000 (00 63) (1)", "2004-02-13T12:00:00.000 (15 12) (1)", "2004-04-09T12:00:00.000 (63 63) (1)"}
            ,{"2004-01-17T12:00:00.000 (00 63) (1)", "2004-03-13T12:00:00.000 (15 15) (1)", "2004-05-08T12:00:00.000 (63 63) (1)"}
            ,{"2004-02-17T12:00:00.000 (00 63) (1)", "2004-04-13T12:00:00.000 (48 15) (1)", "2004-04-13T12:00:00.001 (63 63) (1)"}
	  };

	  vector< vector< string > > results_;
	  
	  for( int k=0; k<n; ++k ) {
	    vector< string > r_;
	    int64_t tiv = t_index[k];
	    r_.push_back( toStringJulianTAI_ISO( scidbLowerBoundTAI(tiv) ));
	    r_.push_back( toStringJulianTAI_ISO( tiv )); 
	    r_.push_back( toStringJulianTAI_ISO( scidbUpperBoundTAI(tiv) ));
	    results_.push_back(r_);
	  }

	  for( int k=0; k<n; ++k ) {
	    for( int j=0; j<3; ++j ) {
	      ASSERT_EQUAL(expected_[k][j],results_[k][j]);
	    }
	  }
	}
	
//	if( true ) {
//
//
//	}

	// FAIL();
}
