/*
 * STARE_Temporal_test.cpp
 *
 *  Created on: Apr 16, 2019
 *      Author: mrilee
 *
 *  Copyright (C) 2019 Rilee Systems Technologies LLC
 */

#include "Test.h"

#define TAG(X) cout << dec << X << hex << endl << flush;
#define TAGNC(X) cout << dec << X << hex << flush;
// #define TAG(x) {}

void TemporalIndex_test() {

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

#define INDEX_OUT0(lbl,var) cout << dec << lbl << " nat,trad,scidb: " << var.stringInNativeDate()	<< flush << ", " << var.toStringJ() << flush << endl;
#define INDEX_OUTN(lbl,var) cout << dec << lbl << " nat,trad,scidb: " << var.stringInNativeDate()	<< flush << endl;
#define INDEX_OUT(lbl,var)  cout << dec << lbl << " nat,trad,scidb: " << var.stringInNativeDate()	<< flush << ", " << var.toStringJ() << flush << ", 0x" << setw(16) << setfill('0') << hex << var.scidbTemporalIndex() << dec << ", " << setfill(' ') << setw(22) << var.scidbTemporalIndex() << endl << flush;
#define INDEX_OUTNC(lbl,var)  cout << dec << lbl << " nat,trad,scidb: " << var.stringInNativeDate()	<< flush << ", " << var.toStringJ() << flush << ", 0x" << setw(16) << setfill('0') << hex << var.scidbTemporalIndex() << dec << ", " << setfill(' ') << setw(22) << var.scidbTemporalIndex();

	// ASSERT_EQUAL("",tIndex.stringInNativeDate());
	// ASSERT_EQUAL("",tIndex.toStringJ());
	// ASSERT_EQUAL(,tIndex.scidbTemporalIndex());

	failureMessage = "'";
	try {
		TAG(1000)
		tIndex = TemporalIndex(0,0,0,0,0,0,0,0,0,0,2); // TAG(1000); // INDEX_OUT0(1000,tIndex); TAG(1001); INDEX_OUT(1001,tIndex);
		ASSERT_EQUAL("-000000000-00-0-0 00:00:00.000 (00) (2)",tIndex.stringInNativeDate());
		ASSERT_EQUAL("0 000000001-01-01 00:00:00.000 (00) (2)",tIndex.toStringJ());
		ASSERT_EQUAL(0xfffff20a041018fe,tIndex.scidbTemporalIndex());
		TAG(1100)
		tIndex = TemporalIndex(1,1,0,0,0,0,0,0,0,0,2); // TAG(1100); // INDEX_OUT0(1100,tIndex); TAG(1101); INDEX_OUT(1101,tIndex);
		ASSERT_EQUAL("+000000001-00-0-0 00:00:00.000 (00) (2)",tIndex.stringInNativeDate());
		ASSERT_EQUAL("1 000000001-01-01 00:00:00.000 (00) (2)",tIndex.toStringJ());
		ASSERT_EQUAL(0x0000100000000002,tIndex.scidbTemporalIndex());
		TAG(1200)
		cout << endl << flush;
	} catch (SpatialFailure& failure) {
		failureMessage += failure.what();
		cout << "failureMessage: " << failureMessage << "'" << endl << flush;
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
		tIndex = TemporalIndex(0,0,0,0,0,0,0,0,0,0,2); tIndex.data.decrementAtName("millisecond"); INDEX_OUT0(1500,tIndex);
	} catch (SpatialFailure& failure) {
		failureMessage += failure.what();
		cout << "failureMessage: " << failureMessage << "'" << endl << flush;
	};

	TAG(3)

	// remove CE from tai calls.
	tIndex.fromFormattedJulianTAI(1, 12, 31, 23, 59, 59, 998); INDEX_OUT(0,tIndex);
	tIndex1.fromStringJ(tIndex.toStringJ()); INDEX_OUT(0,tIndex1);
	ASSERT_EQUAL("+000000001-13-0-0 23:59:59.998 (00) (2)",tIndex.stringInNativeDate());
	ASSERT_EQUAL("1 000000001-12-31 23:59:59.998 (00) (2)",tIndex.toStringJ());

	tIndex.setZero().fromFormattedJulianTAI(1, 12, 31, 23, 59, 59, 999); // INDEX_OUT(1,tIndex);
	ASSERT_EQUAL("+000000001-13-0-0 23:59:59.999 (00) (2)",tIndex.stringInNativeDate());
	ASSERT_EQUAL("1 000000001-12-31 23:59:59.999 (00) (2)",tIndex.toStringJ());

	tIndex.fromFormattedJulianTAI(2, 1, 1, 0, 0, 0, 0); // INDEX_OUT(2,tIndex);
	ASSERT_EQUAL("+000000002-00-0-0 00:00:00.000 (00) (2)",tIndex.stringInNativeDate());
	ASSERT_EQUAL("1 000000002-01-01 00:00:00.000 (00) (2)",tIndex.toStringJ());

	failureMessage = "'";
	try {
		TAG(391)
	tIndex.fromFormattedJulianTAI(1-1, 12, 31, 23, 59, 59, 999); // INDEX_OUT(3,tIndex);
		TAG(392)
	ASSERT_EQUAL("-000000000-13-0-1 23:59:59.999 (00) (2)",tIndex.stringInNativeDate());
		TAG(393)
	ASSERT_EQUAL("0 000000001-12-31 23:59:59.999 (00) (2)",tIndex.toStringJ());
	} catch (SpatialFailure& failure) {
		failureMessage += failure.what();
		cout << "failureMessage: " << failureMessage << "'" << endl << flush;
	};
	TAG(4)

	// cout << endl << flush;

	tIndex.fromFormattedJulianTAI(1, 12, 31, 23, 59, 59, 999); // INDEX_OUT(4,tIndex);
	ASSERT_EQUAL("+000000001-13-0-0 23:59:59.999 (00) (2)",tIndex.stringInNativeDate());
	ASSERT_EQUAL("1 000000001-12-31 23:59:59.999 (00) (2)",tIndex.toStringJ());
	ASSERT_EQUAL(0x00001d05fbefe702,tIndex.scidbTemporalIndex());

	tIndex.fromFormattedJulianTAI(2, 12, 31, 23, 59, 59, 999); // INDEX_OUT(5,tIndex);
	ASSERT_EQUAL("+000000002-13-0-0 23:59:59.999 (00) (2)",tIndex.stringInNativeDate());
	ASSERT_EQUAL("1 000000002-12-31 23:59:59.999 (00) (2)",tIndex.toStringJ());
	ASSERT_EQUAL(0x00002d05fbefe702,tIndex.scidbTemporalIndex());

	tIndex.fromFormattedJulianTAI(3, 12, 31, 23, 59, 59, 999); // INDEX_OUT(6,tIndex);
	ASSERT_EQUAL("+000000003-13-0-0 23:59:59.999 (00) (2)",tIndex.stringInNativeDate());
	ASSERT_EQUAL("1 000000003-12-31 23:59:59.999 (00) (2)",tIndex.toStringJ());
	ASSERT_EQUAL(0x00003d05fbefe702,tIndex.scidbTemporalIndex());

	tIndex.fromFormattedJulianTAI(4, 12, 31, 23, 59, 59, 999); // INDEX_OUT(7,tIndex);
	ASSERT_EQUAL("+000000004-13-0-1 23:59:59.999 (00) (2)",tIndex.stringInNativeDate());
	ASSERT_EQUAL("1 000000004-12-31 23:59:59.999 (00) (2)",tIndex.toStringJ());
	ASSERT_EQUAL(0x00004d0dfbefe702,tIndex.scidbTemporalIndex());

	tIndex.fromFormattedJulianTAI(5, 12, 31, 23, 59, 59, 999); // INDEX_OUT(8,tIndex);
	ASSERT_EQUAL("+000000005-13-0-0 23:59:59.999 (00) (2)",tIndex.stringInNativeDate());
	ASSERT_EQUAL("1 000000005-12-31 23:59:59.999 (00) (2)",tIndex.toStringJ());
	ASSERT_EQUAL(0x00005d05fbefe702,tIndex.scidbTemporalIndex());

	// cout << endl << flush;

	TAG(110)
	tIndex.setZero().fromFormattedJulianTAI(1-1,  1,  1,  0,  0,  0, 0); // .set_BeforeAfterStartBit(0); // INDEX_OUT(1,tIndex);
	ASSERT_EQUAL("-000000000-00-0-0 00:00:00.000 (00) (2)",tIndex.stringInNativeDate());
	ASSERT_EQUAL("0 000000001-01-01 00:00:00.000 (00) (2)",tIndex.toStringJ());
	ASSERT_EQUAL(0xfffff20a041018fe,tIndex.scidbTemporalIndex());

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
	tIndex.set_BeforeAfterStartBit(0); // INDEX_OUT(2,tIndex);
	// cout << "2 nat,trad,scidb: " << tIndex.stringInNativeDate()	<< flush << ", " << tIndex.toStringJ() << flush << ", 0x" << setw(16) << setfill('0') << hex << tIndex.scidbTemporalIndex() << dec << tIndex.scidbTemporalIndex() << endl << flush;
	// cout << "2 nat,trad,scidb: " << tIndex.stringInNativeDate()	<< flush << ", " << tIndex.toStringJ() << flush << hex << tIndex.scidbTemporalIndex() << dec << endl << flush;
	// cout << "2 nat,trad: " << tIndex.stringInNativeDate()  << ", " << tIndex.toStringJ() << endl << flush;
	// cout << "2 scidb idx: 0x" << hex << tIndex.scidbTemporalIndex() << dec << endl << flush;

	TAG(170)

	ASSERT_EQUAL("-000000001-13-0-0 23:59:59.999 (00) (2)",tIndex.stringInNativeDate());
	ASSERT_EQUAL("0 000000002-12-31 23:59:59.999 (00) (2)",tIndex.toStringJ());
	ASSERT_EQUAL(0xffffef0ffffffffe,tIndex.scidbTemporalIndex());

	cout << 1000 << " " << flush << (tIndex == tIndex) << endl << flush;

	/*

	// TAG(200)
	tIndex.setZero(); tIndex.set_BeforeAfterStartBit(0); // INDEX_OUT(3,tIndex);
	tIndex.setJulianFromTraditionalDate(0, 1,  1,  1,  0,  0,  0, 0);	// INDEX_OUT(3,tIndex);

	cout << endl << flush;

	tIndex.setJulianFromTraditionalDate(0, 2, 12, 31, 23, 59, 59, 999); INDEX_OUT(3,tIndex);
	tIndex.setJulianFromTraditionalDate(0, 3, 12, 31, 23, 59, 59, 999); INDEX_OUT(4,tIndex);
	tIndex.setJulianFromTraditionalDate(0, 4, 12, 31, 23, 59, 59, 999); INDEX_OUT(5,tIndex);
	tIndex.setJulianFromTraditionalDate(0, 5, 12, 30, 23, 59, 59, 999); INDEX_OUT(6,tIndex);
	tIndex.setJulianFromTraditionalDate(0, 5, 12, 31, 23, 59, 59, 999); INDEX_OUT(6,tIndex);
	tIndex.setJulianFromTraditionalDate(0, 6, 12, 31, 23, 59, 59, 999); INDEX_OUT(6,tIndex);
	tIndex.setJulianFromTraditionalDate(0, 6,  1,  2, 23, 59, 59, 999); INDEX_OUT(6,tIndex);
	tIndex.setJulianFromTraditionalDate(0, 6,  1,  1, 23, 59, 59, 999); INDEX_OUT(6,tIndex);
	tIndex.setJulianFromTraditionalDate(0, 7, 12, 31, 23, 59, 59, 999); INDEX_OUT(7,tIndex);

	cout << endl << flush;

	tIndex.setJulianFromTraditionalDate(1, 2, 12, 31, 23, 59, 59, 999); INDEX_OUT(3,tIndex);
	tIndex.setJulianFromTraditionalDate(1, 3, 12, 31, 23, 59, 59, 999); INDEX_OUT(4,tIndex);
	tIndex.setJulianFromTraditionalDate(1, 4, 12, 31, 23, 59, 59, 999); INDEX_OUT(5,tIndex);
	tIndex.setJulianFromTraditionalDate(1, 5, 12, 30, 23, 59, 59, 999); INDEX_OUT(6,tIndex);
	tIndex.setJulianFromTraditionalDate(1, 5, 12, 31, 23, 59, 59, 999); INDEX_OUT(6,tIndex);
	tIndex.setJulianFromTraditionalDate(1, 6, 12, 31, 23, 59, 59, 999); INDEX_OUT(6,tIndex);
	tIndex.setJulianFromTraditionalDate(1, 6,  1,  2, 23, 59, 59, 999); INDEX_OUT(6,tIndex);
	tIndex.setJulianFromTraditionalDate(1, 6,  1,  1, 23, 59, 59, 999); INDEX_OUT(6,tIndex);
	tIndex.setJulianFromTraditionalDate(1, 7, 12, 31, 23, 59, 59, 999); INDEX_OUT(7,tIndex);
	cout << endl << flush;

	tIndex.setJulianFromTraditionalDate(0, 6, 1, 1, 0, 0, 0, 0); INDEX_OUT(7,tIndex);
	tIndex.setJulianFromTraditionalDate(0, 6, 1, 1, 0, 0, 0, 1); INDEX_OUT(7,tIndex);

	cout << endl << flush;

	tIndex.setJulianFromTraditionalDate(0, 4, 1, 1, 0, 0, 0, 0); INDEX_OUT(8,tIndex);
	tIndex.setJulianFromTraditionalDate(0, 4*1024+1, 12, 31, 23, 59, 59, 999); INDEX_OUT(8,tIndex);
	tIndex.setJulianFromTraditionalDate(0, 4*1024, 1, 1, 0, 0, 0, 0); INDEX_OUT(8,tIndex);
	tIndex.setJulianFromTraditionalDate(0, 4*1024, 1, 1, 0, 0, 0, 1); INDEX_OUT(8,tIndex);
	tIndex.setJulianFromTraditionalDate(1, 4*1024, 1, 1, 0, 0, 0, 0); INDEX_OUT(8,tIndex);
	tIndex.setJulianFromTraditionalDate(1, 4*1024, 1, 1, 0, 0, 0, 1); INDEX_OUT(8,tIndex);

	cout << endl << flush;
	tIndex.setJulianFromTraditionalDate(0, 256*1024, 1, 1, 0, 0, 0, 1); INDEX_OUT(8,tIndex);
	tIndex.setJulianFromTraditionalDate(1, 256*1024, 1, 1, 0, 0, 0, 1); INDEX_OUT(8,tIndex);

	TAG(300)

	cout << endl << flush;
	tIndex.setJulianFromTraditionalDate(1, 2007, 12, 31, 0, 0, 0, 0); INDEX_OUT(9,tIndex);
	tIndex.setJulianFromTraditionalDate(1, 2008, 12, 31, 0, 0, 0, 0); INDEX_OUT(9,tIndex);
	tIndex.setJulianFromTraditionalDate(1, 2009,  1,  1, 0, 0, 0, 0); INDEX_OUT(9,tIndex);
	cout << endl << flush;

	*/

	tIndex.fromFormattedJulianTAI(2009,  1,  1, 0, 0, 0, 0); // INDEX_OUT(9,tIndex);
	TemporalIndex tIndex0;

	try {
		tIndex0.fromStringJ(tIndex.toStringJ());
	} catch (SpatialFailure& failure) {
		failureMessage += failure.what();
		cout << "failureMessage: " << failureMessage << "'" << endl << flush;
	}

	TAG(301)

	cout << dec;

	INDEX_OUT(10,tIndex);
	INDEX_OUT(10,tIndex0);

	cout << endl << flush;

	TAG(302)
	// tIndex.data.print();
	// TAG(400)
	tIndex.set_zero();
	tIndex.set_year(5);
	ASSERT_EQUAL( 0, tIndex.get_millisecond());
	ASSERT_EQUAL( 0, tIndex.get_day());
	ASSERT_EQUAL( 0, tIndex.get_BeforeAfterStartBit());
	ASSERT_EQUAL( 5, tIndex.get_year());
	// Note 1 BCE goes to 0 0.
	// cout << "4  nat,trad: " << tIndex.stringInNativeDate()  << ", " << tIndex.toStringJ() << endl << flush;
	// cout << "4  scidb idx: 0x" << hex << tIndex.scidbTemporalIndex() << dec << endl << flush;
	// TAG(450)
	tIndex.set_millisecond(1);
	// TAG(460)
	// tIndex.data.print();
	// TAG(461)
	ASSERT_EQUAL( 1, tIndex.get_millisecond());
	ASSERT_EQUAL( 1, tIndex.set_millisecond(1).get_millisecond());
	// TAG(470)
	tIndex.data.decrementAtName("millisecond",1);
	// TAG(475)
	ASSERT_EQUAL( 2, tIndex.set_zero().set_millisecond(2).get_millisecond());
	// TAG(476)
	// tIndex.data.print();
	// TAG(477)
	tIndex.data.decrementAtName("millisecond",1);
	// TAG(480)
	// tIndex.data.print();
	// TAG(490)
	ASSERT_EQUAL( 1, tIndex.get_millisecond());
	// TAG(500)
	// tIndex.data.print();
	// TAG(600)
	tIndex.data.incrementAtName("millisecond",1);
	ASSERT_EQUAL( 2, tIndex.get_millisecond());
	// TAG(610)
	tIndex.data.incrementAtName("year",1);
	// TAG(620)
	// tIndex.data.print();
	ASSERT_EQUAL( 2, tIndex.get_millisecond());
	ASSERT_EQUAL( 1, tIndex.get_year());
	ASSERT_EQUAL( 0, tIndex.get_month());

	// TAG(622)
	tIndex.set_year(10);
	tIndex.set_millisecond(0);
	int64_t id = tIndex.data.getFieldId("year");
	// cout << "id: " << dec << id << hex << endl << flush;
	// TAG(6230)
	// tIndex.data.print();
	// TAG(6231)
	tIndex.data.decrementAtId(id, 1);
	// TAG(6232)
	// tIndex.data.print();
	// TAG(624)
	tIndex.data.decrementAtName("year",1);
	// TAG(625)
	// tIndex.data.print();
	ASSERT_EQUAL( 8, tIndex.get_year());
	// TAG(630)
	tIndex.data.setValue("BeforeAfterStartBit",1);
	// tIndex.data.decrementAtName("millisecond",1);
	// tIndex.data.print();
	// TAG(631)
	tIndex.data.decrementAtName("year",1);
	ASSERT_EQUAL( 7, tIndex.get_year());

//	cout << "                FEDCBA9876543210" << endl << flush;
//	for( int iRes = 0; iRes < 16; iRes += 1 ) {
//		// TAG(632)
//		tIndex.data.setValue("resolution", iRes);
//		// TAG(633)
//		// cout << "                FEDCBA9876543210" << endl << flush;
//		cout << "0 scidb idx:  0x" << setw(16) << setfill('0') << hex << tIndex.scidbTemporalIndex() << endl << flush;
//		// TAG(640)
//		// TemporalIndex tIndex1(tIndex.scidbTemporalIndex());
//		// cout << "1 scidb idx:  0x" << setw(16) << setfill('0') << hex << tIndex1.scidbTemporalIndex() << endl << flush;
//		// TAG(700)
//		int64_t iTerm = tIndex.scidbTerminator();
//		cout << "2 scidb term: 0x" << setw(16) << setfill('0') << hex << iTerm << endl << flush;
//		// tIndex.data.print();
//		// TAG(710)
//		// cout << "3 scidb term: 0x" << setw(16) << setfill('0') << hex << tIndex.scidbTerminator() << endl << flush;
//	}

	// tIndex.setZero();
	int64_t one = 1;
	cout << "                  FEDCBA9876543210" << endl << flush;

	int64_t offsetBase = tIndex.data.get("millisecond")->getOffset();
	int64_t offsetTop = tIndex.data.get("year")->getOffset()+tIndex.data.get("year")->getWidth()-1;

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
			tIndex.data.setValue("resolution",iResolution);
			TAG(732)
			tIndex.data.setValue("type",2);
			TAG(733)
			// tIndex.data.print();
			cout << setw(2) << setfill('0') << dec << i << hex;
			cout << " scidb idx:   0x" << hex << setw(16) << setfill('0') << tIndex.scidbTemporalIndex() << ", " << tIndex.scidbTerminatorp()
				<< ", " << tIndex.stringInNativeDate() << ", " << tIndex.toStringJ()
				<< dec << endl << flush;
			TemporalIndex tIndex1 = TemporalIndex(tIndex.scidbTerminator());
			cout << "            :   0x" << hex << setw(16) << setfill('0') << tIndex.scidbTerminator()
				<< ", " << tIndex1.scidbTerminatorp()
				<< ", " << tIndex1.stringInNativeDate() << ", " << tIndex1.toStringJ()
				<< dec << endl << endl << flush;
		}
	} catch (SpatialFailure& failure) {
		failureMessage += failure.what();
		cout << "failureMessage: " << failureMessage << "'" << endl << flush;
	};

	TemporalIndex tIndex2;
	tIndex2.fromFormattedJulianTAI(262144, 12, 31, 23, 59, 59, 999);
	cout << "            :   0x" << hex << setw(16) << setfill('0') << tIndex2.scidbTemporalIndex() << flush
	<< ", " << tIndex2.scidbTerminatorp() << flush
	<< ", " << tIndex2.stringInNativeDate() << flush
	<< ", " << tIndex2.toStringJ() << flush
	<< dec << endl << endl << flush;

	// int not_ok = tIndex2.eraTest();
	// cout << flush;

	// TAG(800)

	TemporalIndex tIndex3, tIndex4;

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

	// TAG(900)

	tIndex3.setZero();
	tIndex3.set_type(2);
	tIndex3.set_resolution(62);
	tIndex3.set_BeforeAfterStartBit(0);
	tIndex3.set_year(2);
	tIndex3.set_month(3);

	tIndex4.setZero();
	tIndex4.set_type(2);
	tIndex4.set_resolution(62);
	tIndex4.set_BeforeAfterStartBit(0);
	tIndex4.set_year(2);
	tIndex4.set_month(4);

	// cout << endl << flush;
	// cout << "0 tI3,tI4:        " << hex << &tIndex3 << ", " << &tIndex4 << dec << endl << flush;
	// cout << "0 tI3,tI4 (data): " << hex << &(tIndex3.data) << ", " << &(tIndex4.data) << dec << endl << flush;
	// cout << "0 tI3: " << tIndex3.stringInNativeDate()  << ", " << tIndex3.toStringJ() << endl << flush;
	// cout << "0 tI4: " << tIndex4.stringInNativeDate()  << ", " << tIndex4.toStringJ() << endl << flush;
	// cout << "0 (tI3 < tI4)? " << hex << tIndex3.scidbTemporalIndex() << " " << (tIndex3.scidbTemporalIndex() < tIndex4.scidbTemporalIndex()) << " " << tIndex4.scidbTemporalIndex() << dec << endl << flush;

	ASSERT_LESSM("BC 2-3-1 < BC 2-4-1",tIndex3.scidbTemporalIndex(),tIndex4.scidbTemporalIndex());

	tIndex4.setZero();
	tIndex4.set_type(2);
	tIndex4.set_resolution(62);
	tIndex4.set_BeforeAfterStartBit(0);
	tIndex4.set_year(1);
	tIndex4.set_month(3);

	ASSERT_LESSM("BC 2-3-1 < BC 1-3-1",tIndex3.scidbTemporalIndex(),tIndex4.scidbTemporalIndex());

	tIndex4.setZero();
	tIndex4.set_type(2);
	tIndex4.set_resolution(63);
	tIndex4.set_BeforeAfterStartBit(0);
	tIndex4.set_year(1);
	tIndex4.set_month(3);

	// cout << "tI3: " << tIndex3.toStringJ() << endl << flush;
	// cout << "tI4: " << tIndex4.toStringJ() << endl << flush;

	ASSERT_EQUAL("0 000000003-03-26 00:00:00.000 (62) (2)",tIndex3.toStringJ());
	ASSERT_EQUAL("0 000000002-03-26 00:00:00.000 (63) (2)",tIndex4.toStringJ());
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

	cout << endl << flush;

	tIndex3.fromFormattedJulianTAI(1-1,  1,  1,  0,  0,  0,   0);
	tIndex3.set_resolution(tIndex3.data.maxResolutionLevel());
	INDEX_OUT(3000,tIndex3);
	// tIndex4.setJulianFromTraditionalDate(0, 1, 12, 31, 23, 59, 59, 999);
	cout << dec;
//	cout << "30001"
//			<< " coarsest resolution level: " << dec <<  tIndex3.data.maxResolutionLevel()
//			<< flush << endl;
	int64_t itmp4 = tIndex3.scidbTerminator();
	cout << dec;
	cout << "30001" << " 0x"<< setw(16) << setfill('0') << hex << tIndex3.scidbTemporalIndex() << dec << flush << endl;
	cout << "30002" << " 0x"<< setw(16) << setfill('0') << hex << itmp4 << dec << flush << endl;
	failureMessage = "";
	try {
		tIndex4 = TemporalIndex(itmp4);
	} catch ( SpatialFailure& failure ) {
		failureMessage += failure.what();
		cout << "failureMessage: " << failureMessage << "'" << endl << flush;
	}
	cout << "30003" << " 0x" << setw(16) << setfill('0') << hex << itmp4 << " " << dec << itmp4 << " res: " << tIndex4.get_resolution() << flush << endl;
	INDEX_OUT(3000,tIndex4);

	tIndex3.set_resolution(53); INDEX_OUT(3100,tIndex3);

	itmp4 = tIndex3.scidbTerminator();
	cout << "z1"
			<< " 0x" << setw(16) << setfill('0') << hex << itmp4 << " " << dec << itmp4
			<< endl << flush;
	tIndex4 = TemporalIndex(itmp4);
	cout << "z2" << endl << flush;
	INDEX_OUT(3101,tIndex4);

	cout << 3200 << endl << flush;
	tIndex3.set_BeforeAfterStartBit(1).set_year(1).set_resolution(52); INDEX_OUT(3200,tIndex3);
	itmp4 = tIndex3.scidbTerminator(); tIndex4 = TemporalIndex(itmp4); INDEX_OUT(3201,tIndex4);

	cout << dec << 3300 << endl << flush;
	cout << "tI3:  idx: 0x" << setw(16) << setfill('0') << hex << tIndex3.scidbTemporalIndex() << " " << dec << tIndex3.scidbTerminatorp() << endl << flush;
	cout << "tI4:  idx: 0x" << setw(16) << setfill('0') << hex << tIndex4.scidbTemporalIndex() << " " << dec << tIndex4.scidbTerminatorp() << endl << flush;

	cout << endl << flush;
	cout << "tI3: term: 0x" << setw(16) << setfill('0') << hex << tIndex3.scidbTerminator() << " " << dec << tIndex3.scidbTerminatorp() << endl << flush;
	cout << "tI4: term: 0x" << setw(16) << setfill('0') << hex << tIndex4.scidbTerminator() << " " << dec << tIndex4.scidbTerminatorp() << endl << flush;

	tIndex3.set_BeforeAfterStartBit(1).set_year(1).set_resolution(51); INDEX_OUT(3202,tIndex3);
	itmp4 = tIndex3.scidbTerminator(); tIndex4 = TemporalIndex(itmp4); INDEX_OUT(3203,tIndex4);

	cout << dec << 3400 << endl << flush;
	cout << "tI3:  idx: 0x" << setw(16) << setfill('0') << hex << tIndex3.scidbTemporalIndex() << " " << dec << tIndex3.scidbTerminatorp() << endl << flush;
	cout << "tI4:  idx: 0x" << setw(16) << setfill('0') << hex << tIndex4.scidbTemporalIndex() << " " << dec << tIndex4.scidbTerminatorp() << endl << flush;

	cout << endl << flush;
	cout << "tI3: term: 0x" << setw(16) << setfill('0') << hex << tIndex3.scidbTerminator() << " " << dec << tIndex3.scidbTerminatorp() << endl << flush;
	cout << "tI4: term: 0x" << setw(16) << setfill('0') << hex << tIndex4.scidbTerminator() << " " << dec << tIndex4.scidbTerminatorp() << endl << flush;

	tIndex3.set_BeforeAfterStartBit(0);
	tIndex4.set_BeforeAfterStartBit(0);

	cout << dec << 3500 << endl << flush;
	cout << "tI3:  idx: 0x" << setw(16) << setfill('0') << hex << tIndex3.scidbTemporalIndex() << " " << dec << tIndex3.scidbTerminatorp() << endl << flush;
	cout << "tI4:  idx: 0x" << setw(16) << setfill('0') << hex << tIndex4.scidbTemporalIndex() << " " << dec << tIndex4.scidbTerminatorp() << endl << flush;

	cout << endl << flush;
	cout << "tI3: term: 0x" << setw(16) << setfill('0') << hex << tIndex3.scidbTerminator() << " " << dec << tIndex3.scidbTerminatorp() << endl << flush;
	cout << "tI4: term: 0x" << setw(16) << setfill('0') << hex << tIndex4.scidbTerminator() << " " << dec << tIndex4.scidbTerminatorp() << endl << flush;

	tIndex3.set_BeforeAfterStartBit(1).set_resolution(10);
	tIndex4 = TemporalIndex(tIndex3.scidbTerminator());


	{
	TemporalIndex a,b,c; int tag = 4400;

	a.set_BeforeAfterStartBit(1).set_year(1).set_month(3);
	TAG(tag)
	int64_t a_i = a.toInt64Milliseconds();
	TAG(++tag)
	cout << endl << flush;
	INDEX_OUT(++tag,a);
	cout << "a_i " << dec << a_i << endl << flush;
	b.fromInt64Milliseconds(a_i);
	ASSERT_EQUAL(a.stringInNativeDate(),b.stringInNativeDate());
	INDEX_OUT(++tag,b);

	tag = 4450;
	a.set_BeforeAfterStartBit(0).set_year(1).set_month(3);
	TAG(tag)
	a_i = a.toInt64Milliseconds();
	TAG(++tag)
	cout << endl << flush;
	INDEX_OUT(++tag,a);
	cout << "a_i " << dec << a_i << endl << flush;
	b.fromInt64Milliseconds(a_i);
	ASSERT_EQUAL(a.stringInNativeDate(),b.stringInNativeDate());
	INDEX_OUT(++tag,b);

	tag = 4460;
	a.set_BeforeAfterStartBit(0).set_year(4).set_week(2);
	// a.set_BeforeAfterStartBit(1).set_year(1).set_week(0);
	TAG(tag)
	a_i = a.toInt64Milliseconds();
	TAG(++tag)
	cout << endl << flush;
	INDEX_OUT(++tag,a);
	cout << "a_i " << dec << a_i << endl << flush;
	b.fromInt64Milliseconds(a_i);
	ASSERT_EQUAL(a.stringInNativeDate(),b.stringInNativeDate());
	INDEX_OUT(++tag,b);

	tag = 4465;
	a.set_BeforeAfterStartBit(0).setZero();
	TAG(tag)
	a_i = a.toInt64Milliseconds();
	TAG(++tag)
	cout << endl << flush;
	INDEX_OUT(++tag,a);
	cout << "a_i " << dec << a_i << endl << flush;
	b.fromInt64Milliseconds(a_i);
	ASSERT_EQUAL(a.stringInNativeDate(),b.stringInNativeDate());
	INDEX_OUT(++tag,b);

	tag = 446500;
	a.set_BeforeAfterStartBit(0).setZero();
	TAG(tag)
	a_i = a.toInt64Milliseconds() + 31536000000;
	TAG(++tag)
	cout << endl << flush;
	INDEX_OUT(++tag,a);
	cout << "a_i " << dec << a_i << endl << flush;
	b.fromInt64Milliseconds(a_i);
	ASSERT_EQUAL("+000000001-00-0-0 00:00:00.000 (00) (2)",b.stringInNativeDate());
	INDEX_OUT(++tag,b);

	cout << endl << flush;

	cout    << "bitOffsets " << endl
			<< " coarsest    " << tIndex.bitOffsetCoarsest() << endl
			<< " finest      " << tIndex.bitOffsetFinest() << endl
			<< " res(10)     " << tIndex.bitOffsetResolution(10) << endl
			<< " id(res(10)) " << tIndex.bitfieldIdFromResolution(10) << endl
			<< endl << flush;

	ASSERT_EQUAL(tIndex.bitOffsetCoarsest(),62);
	ASSERT_EQUAL(tIndex.bitOffsetFinest(),8);
	ASSERT_EQUAL(tIndex.bitOffsetResolution(10),52);
	ASSERT_EQUAL(tIndex.bitfieldIdFromResolution(10),1);

	cout << endl << flush;

	int64_t b_ms;
	int level = 54;
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
		cout << endl << flush;
		--level;
	} while (level >= 0);

	tag = 4500;
	TAG(tag)
	a.setZero().set_BeforeAfterStartBit(1).set_year(1);
	TAG(++tag)
	b.setZero().set_BeforeAfterStartBit(1).set_year(2);
	TAG(++tag)
	c = a + b;
	TAG(++tag)
	cout << endl << flush;
	INDEX_OUT(++tag,a);
	INDEX_OUT(++tag,b);
	INDEX_OUT(++tag,c);

	tag = 4510;
	a.setZero().set_BeforeAfterStartBit(1).set_year(1).set_minute(30);
	b.setZero().set_BeforeAfterStartBit(1).set_year(1).set_minute(30);
	c = a + b;
	cout << endl << flush;
	INDEX_OUT(tag,a);
	INDEX_OUT(++tag,b);
	INDEX_OUT(++tag,c);

	tag = 4520;
	a.setZero().set_BeforeAfterStartBit(0).set_year(0).set_minute(30);
	b.setZero().set_BeforeAfterStartBit(0).set_year(0).set_minute(30);
	c = a + b;
	cout << endl << flush;
	INDEX_OUT(tag,a);
	INDEX_OUT(++tag,b);
	INDEX_OUT(++tag,c);

	tag = 4530;
	a.setZero().set_BeforeAfterStartBit(0).set_year(0).set_month(13).set_day(0).set_minute(30);
	b.setZero().set_BeforeAfterStartBit(0).set_year(0).set_month(0 ).set_day(0).set_minute(30);
	c = a + b;
	cout << endl << flush;
	INDEX_OUT(tag,a);
	INDEX_OUT(++tag,b);
	INDEX_OUT(++tag,c);
	}

	cout << endl << flush;
	cout << "a" << endl << flush;
	cout << "tI3:  idx: 0x" << setw(16) << setfill('0') << hex << tIndex3.scidbTemporalIndex() << dec << endl << flush;
	cout << "tI3: term: 0x" << setw(16) << setfill('0') << hex << tIndex3.scidbTerminator() << " " << dec << tIndex3.scidbTerminatorp() << endl << flush;
	cout << "tI4: term: 0x" << setw(16) << setfill('0') << hex << tIndex4.scidbTerminator() << " " << dec << tIndex4.scidbTerminatorp() << endl << flush;

	tIndex3.set_year(0);
	// tIndex3.set_year(1);
	tIndex3.set_BeforeAfterStartBit(0);
	tIndex4 = TemporalIndex(tIndex3.scidbTerminator());

	cout << endl << flush;
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

	cout << endl << flush;
#define FMT(lbl,xval) cout << lbl << " 0x" << setw(16) << setfill('0') << hex << xval << dec << " " << xval << flush;
	FMT("tI4s vs. tI4t ",tIndex4.scidbTemporalIndex()); cout << endl << flush;
	FMT("              ",tIndex4.scidbTerminator());    cout << endl << flush;

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

	cout << endl << flush;

	int tag_id = 6000; int level = 30;
	cout << tag_id << endl << flush;
	tIndex3.setZero().set_BeforeAfterStartBit(1).set_year(1).set_resolution(--level); INDEX_OUTNC(++tag_id,tIndex3);
	itmp4 = tIndex3.scidbTerminator(); tIndex4 = TemporalIndex(itmp4); INDEX_OUT(" ",tIndex4);
	do {
		tIndex3.setZero().set_BeforeAfterStartBit(1).set_year(1).set_resolution(--level); INDEX_OUTNC(++tag_id,tIndex3);
		itmp4 = tIndex3.scidbTerminator(); tIndex4 = TemporalIndex(itmp4); INDEX_OUT(" ",tIndex4);
	} while ( level > 10 );


	tIndex3.setZero().set_BeforeAfterStartBit(1).set_year(2).set_resolution(--level); INDEX_OUTNC(++tag_id,tIndex3);
	itmp4 = tIndex3.scidbTerminator(); tIndex4 = TemporalIndex(itmp4); INDEX_OUT(" ",tIndex4);

	tIndex3.setZero().set_BeforeAfterStartBit(1).set_year(2).set_month(2).set_resolution(--level); INDEX_OUTNC(++tag_id,tIndex3);
	itmp4 = tIndex3.scidbTerminator(); tIndex4 = TemporalIndex(itmp4); INDEX_OUT(" ",tIndex4);

	tag_id = 7000; level = 30;
	cout << tag_id << endl << flush;
	tIndex3.setZero().set_BeforeAfterStartBit(0).set_year(1).set_resolution(--level); INDEX_OUTNC(++tag_id,tIndex3);
	itmp4 = tIndex3.scidbTerminator(); tIndex4 = TemporalIndex(itmp4); INDEX_OUT(" ",tIndex4);
	do {
		tIndex3.setZero().set_BeforeAfterStartBit(0).set_year(1).set_resolution(--level); INDEX_OUTNC(++tag_id,tIndex3);
		itmp4 = tIndex3.scidbTerminator(); tIndex4 = TemporalIndex(itmp4); INDEX_OUT(" ",tIndex4);
	} while ( level > 10 );

	tIndex3.setZero().set_BeforeAfterStartBit(0).set_year(2).set_resolution(--level); INDEX_OUTNC(++tag_id,tIndex3);
	itmp4 = tIndex3.scidbTerminator(); tIndex4 = TemporalIndex(itmp4); INDEX_OUT(" ",tIndex4);

	tIndex3.setZero().set_BeforeAfterStartBit(0).set_year(2).set_month(2).set_resolution(--level); INDEX_OUTNC(++tag_id,tIndex3);
	itmp4 = tIndex3.scidbTerminator(); tIndex4 = TemporalIndex(itmp4); INDEX_OUT(" ",tIndex4);

	TemporalIndex tIndex5;

	tag_id = 8000; level = 10;
	cout << tag_id << endl << flush;
	tIndex3.setZero().set_BeforeAfterStartBit(0).set_year(1).set_resolution(--level); INDEX_OUT(++tag_id,tIndex3);
	tIndex4.setZero().set_BeforeAfterStartBit(1).set_year(1).set_resolution(--level); INDEX_OUT(++tag_id,tIndex4);
	tIndex5 = tIndex3 + tIndex4; INDEX_OUT(++tag_id,tIndex5);

	tag_id = 8120; level = 20;
	cout << tag_id << endl << flush;
	tIndex3.setZero().set_BeforeAfterStartBit(0).set_year(10).set_resolution(--level); INDEX_OUT(++tag_id,tIndex3);
	tIndex4.setZero().set_BeforeAfterStartBit(1).set_year(5).set_resolution(--level); INDEX_OUT(++tag_id,tIndex4);
	tIndex5 = tIndex3 + tIndex4; INDEX_OUT(++tag_id,tIndex5);
	cout << endl << flush;

	tag_id = 8130; level = 15;
	cout << tag_id << endl << flush;
	tIndex3.setZero().set_BeforeAfterStartBit(0).set_year(0).set_month(5).set_resolution(--level); INDEX_OUT(++tag_id,tIndex3);
	tIndex4.setZero().set_BeforeAfterStartBit(0).set_year(0).set_month(9).set_resolution(--level); INDEX_OUT(++tag_id,tIndex4);
	tIndex5 = tIndex3 + tIndex4; INDEX_OUT(++tag_id,tIndex5);
	cout << endl << flush;

	tag_id = 8140; level = 15;
	cout << tag_id << endl << flush;
	int64_t i3 = 3, i4 = 8;
	tIndex3.setZero().fromInt64Milliseconds(i3).set_resolution(--level); INDEX_OUT(++tag_id,tIndex3);
	tIndex4.setZero().fromInt64Milliseconds(i4).set_resolution(--level); INDEX_OUT(++tag_id,tIndex4);
	tIndex5 = tIndex3 | tIndex4; INDEX_OUT(++tag_id,tIndex5);
	tIndex.setZero().set_millisecond(i3+i4).set_resolution(level);
	ASSERT_EQUAL(tIndex,tIndex5);

	cout << endl << flush;
	// SciDB checks
	int64_t max_coordinate = (1ULL << 62) - 1;
	int64_t min_coordinate = - max_coordinate;
	cout << "min,max coordinate: " << dec << min_coordinate << ", " << max_coordinate << endl << flush;

	cout << endl << flush;
	tag_id = 9999;
	cout << "Max SciDB coord" << endl << flush;
	// Note: max_coordinate actually has the type bits set to 3, which is invalid for this TemporalIndex,
	// which is of type=2.
	tIndex = TemporalIndex(max_coordinate); INDEX_OUT(++tag_id,tIndex);
	// .set_year(262143)
	cout << "Native maximum (scidb)" << endl << flush;
	tIndex.setZero().setEOY(1,262143);INDEX_OUT(++tag_id,tIndex);
	tIndex1 = TemporalIndex(tIndex.scidbTerminator()); INDEX_OUT(++tag_id,tIndex1);
	cout << "-" << endl << flush;
	tIndex.setZero()
			.set_BeforeAfterStartBit(1)
			.set_year(262144)
		; INDEX_OUT(++tag_id,tIndex);
	cout << "-" << endl << flush;


	int levels[7] = { 54, 50, 40, 20, 10, 5, 0 };

	for( int i = 0; i < 7; ++i ) {
		level = levels[i];
		tIndex.setZero().setEOY(1,2001).set_type(2).set_resolution(level);	INDEX_OUT(++tag_id,tIndex);
		tIndex1 = TemporalIndex(tIndex.scidbTerminator()); INDEX_OUT(++tag_id,tIndex1);
		tIndex1 = TemporalIndex(tIndex.scidbTerminatorJulianTAI()); INDEX_OUT(++tag_id,tIndex1);
		cout << "--" << endl << flush;
		{
			cout << "delta:     " << tIndex.millisecondsAtResolution(tIndex.get_resolution()) << endl << flush;
			cout << "deltaJDms: " << tIndex.daysAtResolution(tIndex.get_resolution())*86400000.0 << endl << flush;
			cout << "deltaJD:   " << tIndex.daysAtResolution(tIndex.get_resolution()) << endl << flush;
			double d1,d2,delta=0.0;
			delta = tIndex.daysAtResolution(tIndex.get_resolution());
			tIndex.toJulianTAI(d1, d2); tIndex.set_resolution(level); INDEX_OUT(++tag_id,tIndex);
			tIndex.setZero().fromJulianTAI(d1, d2+delta); INDEX_OUT(++tag_id,tIndex);
		}
		cout << "---" << endl << flush;
	}

	cout << endl << flush;
	cout << "Min SciDB coord" << endl << flush;
	// Note min_coordinate is not a valid stare temporal index value.
	tIndex = TemporalIndex(min_coordinate);	INDEX_OUT(++tag_id,tIndex);

	cout << endl << flush;
	cout << "Native minimum (scidb)" << endl << flush;
	tIndex.setZero().set_year(262143);   	INDEX_OUT(++tag_id,tIndex);
	cout << "nat-min0 " << scidbMinimumIndex() << endl << flush;
	cout << "nat-min > sci-min: " << (tIndex.scidbTemporalIndex() > min_coordinate) << endl << flush;
	tIndex = TemporalIndex(scidbMinimumIndex()); INDEX_OUT(++tag_id,tIndex);
	cout << endl << flush;

	cout << "nat-max " << scidbMaximumIndex() << endl << flush;
	cout << endl << flush;

	tIndex.setZero().setEOY(1,1024);
	INDEX_OUT(++tag_id,tIndex);
	cout << endl << flush;

	tIndex.fromFormattedJulianTAI(2000, 12, 31, 23, 59, 59, 999);
	INDEX_OUTNC(++tag_id,tIndex);
	cout << " " << setw(18) << setfill(' ') << tIndex.toInt64Milliseconds();
	cout << endl << flush;

	tIndex.setZero().set_BeforeAfterStartBit(1).set_year(2001).set_hour(23).set_minute(59).set_second(59).set_millisecond(999);
	INDEX_OUTNC(++tag_id,tIndex);
	cout << " " << setw(18) << setfill(' ') << tIndex.toInt64Milliseconds();
	cout << endl << flush;
	cout << endl << flush;

	double d1,d2;
	tIndex.fromFormattedJulianTAI(1999, 12, 31, 23, 59, 59, 999);
	INDEX_OUT(++tag_id,tIndex);
	tIndex.toJulianTAI(d1, d2);
	tIndex.setZero().fromJulianTAI(d1, d2);
	INDEX_OUTNC(++tag_id,tIndex); cout << " d1,d2: " << d1 << ", " << d2 << endl << flush;
	cout << endl << flush;

	tIndex.setZero().fromFormattedJulianTAI(2000, 12, 31, 23, 59, 59, 999);
	INDEX_OUT(++tag_id,tIndex);
	tIndex.toJulianTAI(d1, d2);
	tIndex.setZero().fromJulianTAI(d1, d2);
	INDEX_OUTNC(++tag_id,tIndex); cout << " d1,d2: " << d1 << ", " << d2 << endl << flush;
	cout << endl << flush;
	//

#define FMT_JD(d1,d2) cout << " d1,d2: " << setprecision(17) << setw(20) << scientific << d1 << ", " << d2; cout.copyfmt(ios(NULL));

	tag_id = 11000;
	try {
	tag_id = 11500;
	// tIndex.setZero().setJulianFromTraditionalDate(1, 2000, 12, 31, 23, 59, 45, 459);
	tIndex.setZero().fromFormattedJulianTAI(2000, 12, 31, 23, 59, 59, 999);	INDEX_OUT(++tag_id,tIndex);

	tIndex.toJulianTAI(d1, d2);
	tIndex.setZero().fromJulianTAI(d1, d2);  INDEX_OUTNC(++tag_id,tIndex); FMT_JD(d1,d2); cout << endl << flush;

	tIndex.toJulianTAI(d1, d2);
	tIndex.setZero().fromJulianTAI(d1, d2);  INDEX_OUTNC(++tag_id,tIndex); FMT_JD(d1,d2); cout << endl << flush;

	tIndex.toJulianTAI(d1, d2);
	tIndex.setZero().fromJulianTAI(d1, d2);  INDEX_OUTNC(++tag_id,tIndex); FMT_JD(d1,d2); cout << endl << flush;
	cout << endl << flush;

	tag_id = 11600;
	tIndex1.set_type(2); tIndex2.set_type(2); // 2 is the type for this index type
	tIndex1.setZero().fromFormattedJulianTAI(4,  1,  1,  0,  0,  0,   0);	INDEX_OUT(++tag_id,tIndex1);
	// tIndex1.setZero();	INDEX_OUT(++tag_id,tIndex1);
	tIndex2.setZero().fromFormattedJulianTAI(2,  1,  1,  0,  0,  0,   0);	INDEX_OUT(++tag_id,tIndex2);
	// tIndex2.setZero().fromJulianDoubleDay(0, 1); INDEX_OUT(++tag_id,tIndex2);
	tIndex4 = tIndex1 + tIndex2; INDEX_OUT(++tag_id,tIndex4);
	// tIndex3 = tIndex1 | tIndex2; INDEX_OUT(++tag_id,tIndex3);

	cout << endl << flush;

	tag_id = 11700;
	tIndex1.set_type(2); tIndex2.set_type(2); // 2 is the type for this index type
	tIndex1.setZero().fromFormattedJulianTAI(4,  1,  1,  0,  0,  0,   0);	INDEX_OUT(++tag_id,tIndex1);
	tIndex2.setZero().fromJulianTAI(0, 1); INDEX_OUT(++tag_id,tIndex2);
	tIndex4 = tIndex1 + tIndex2; INDEX_OUT(++tag_id,tIndex4);
	// tIndex3 = tIndex1 | tIndex2; INDEX_OUT(++tag_id,tIndex3);

	cout << endl << flush;
	} catch (SpatialFailure& failure) {
		failureMessage = "'";
		failureMessage += failure.what();
		cout << "failureMessage: " << failureMessage << "'" << endl << flush;
	};

	{
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


	{
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
//		tIndex1.setZero().setJulianFromTraditionalDate(1, 2016,  12,  31,  23,  59,  60,   999);	INDEX_OUT(++tag_id,tIndex1);
//		tIndex1.toJulianTAI(d1, d2);
//		tIndex2.fromJulianTAI(d1, d2+delta); INDEX_OUT(++tag_id,tIndex2);
//		cout << "d1,d2: " << d1 << ", " << (d2+delta) << endl << flush;
//		cout << "d1,d2: " << (int64_t)d1 << ", " << (int64_t)(d2+delta) << endl << flush;
		//		cout << "---" << endl << flush;
	}

	{
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

	{
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
	cout << " toStringJ0: " << tIndex.toStringJ() << endl << flush;
	tIndex1.fromStringJ(tIndex.toStringJ());
	cout << " toStringJ1: " << tIndex1.toStringJ() << endl << flush;
	cout << "---" << endl << flush;

	failureMessage = "";
	try {
		tIndex.setZero().fromFormattedJulianTAI(1-100,  5,  29,  6,  30,  1,   2);	INDEX_OUT(++tag_id,tIndex);
		cout << " toStringJ0: " << tIndex.toStringJ() << endl << flush;
		tIndex1.fromStringJ(tIndex.toStringJ());
		cout << " toStringJ1: " << tIndex1.toStringJ() << endl << flush;
		cout << "---" << endl << flush;
	} catch (SpatialFailure& failure) {
		failureMessage += failure.what();
		cout << "failureMessage: " << failureMessage << "'" << endl << flush;
	};

	tag_id = 22100;
	tIndex.setZero().fromFormattedJulianTAI(1972, 6, 30, 23, 59, 60, 1); INDEX_OUT(++tag_id,tIndex);
	cout << " toStringJ0: " << tIndex.toStringJ()
			<< ", ms: " << tIndex.toInt64MillisecondsFractionOfYear()
			<< ", msj: " << tIndex.toInt64MillisecondsFractionOfYearJ()
			<< ", msp: " << millisecondsInYear(tIndex.get_BeforeAfterStartBit(),tIndex.get_year())
			<< endl << flush;
	cout << "---" << endl << flush;

	tIndex.setZero().fromFormattedJulianTAI(1972, 6, 30, 23, 58, 60, 1); INDEX_OUT(++tag_id,tIndex);
	cout << " toStringJ0: " << tIndex.toStringJ()
			<< ", ms: " << tIndex.toInt64MillisecondsFractionOfYear()
			<< ", msj: " << tIndex.toInt64MillisecondsFractionOfYearJ()
			<< ", msp: " << millisecondsInYear(tIndex.get_BeforeAfterStartBit(),tIndex.get_year())
			<< endl << flush;
	cout << "---" << endl << flush;

	tIndex.setZero().fromFormattedJulianTAI(1972, 6, 29, 23, 58, 60, 1); INDEX_OUT(++tag_id,tIndex);
	cout << " toStringJ0: " << tIndex.toStringJ()
			<< ", ms: " << tIndex.toInt64MillisecondsFractionOfYear()
			<< ", msj: " << tIndex.toInt64MillisecondsFractionOfYearJ()
			<< ", msp: " << millisecondsInYear(tIndex.get_BeforeAfterStartBit(),tIndex.get_year())
			<< endl << flush;
	cout << "---" << endl << flush;

	tIndex.setZero().setEOY(1,1972); INDEX_OUT(++tag_id,tIndex);
	cout << " toStringJ0: " << tIndex.toStringJ()
		<< ", ms: " << tIndex.toInt64MillisecondsFractionOfYear()
		<< ", msj: " << tIndex.toInt64MillisecondsFractionOfYearJ()
		<< ", msp: " << millisecondsInYear(tIndex.get_BeforeAfterStartBit(),tIndex.get_year())
		<< endl << flush;
	cout << "---" << endl << flush;

	tIndex.setZero().setEOY(1,1973); INDEX_OUT(++tag_id,tIndex);
	cout << " toStringJ0: " << tIndex.toStringJ()
			<< ", ms: " << tIndex.toInt64MillisecondsFractionOfYear()
			<< ", msj: " << tIndex.toInt64MillisecondsFractionOfYearJ()
			<< ", msp: " << millisecondsInYear(tIndex.get_BeforeAfterStartBit(),tIndex.get_year())
			<< endl << flush;
	cout << "---" << endl << flush;

	tIndex.setZero().setEOY(1,1980); INDEX_OUT(++tag_id,tIndex);
	cout << " toStringJ0: " << tIndex.toStringJ()
					<< ", ms: " << tIndex.toInt64MillisecondsFractionOfYear()
					<< ", msj: " << tIndex.toInt64MillisecondsFractionOfYearJ()
					<< ", msp: " << millisecondsInYear(tIndex.get_BeforeAfterStartBit(),tIndex.get_year())
					<< endl << flush;
	cout << "---" << endl << flush;

	tIndex.setZero().setEOY(1,1992); INDEX_OUT(++tag_id,tIndex);
	cout << " toStringJ0: " << tIndex.toStringJ()
					<< ", ms: " << tIndex.toInt64MillisecondsFractionOfYear()
					<< ", msj: " << tIndex.toInt64MillisecondsFractionOfYearJ()
					<< ", msp: " << millisecondsInYear(tIndex.get_BeforeAfterStartBit(),tIndex.get_year())
					<< endl << flush;
	cout << "---" << endl << flush;

	tIndex.setZero().setEOY(1,1998); INDEX_OUT(++tag_id,tIndex);
	cout << " toStringJ0: " << tIndex.toStringJ()
					<< ", ms: " << tIndex.toInt64MillisecondsFractionOfYear()
					<< ", msj: " << tIndex.toInt64MillisecondsFractionOfYearJ()
					<< ", msp: " << millisecondsInYear(tIndex.get_BeforeAfterStartBit(),tIndex.get_year())
					<< endl << flush;
	cout << "---" << endl << flush;

	tIndex.setZero().setEOY(1,1999); INDEX_OUT(++tag_id,tIndex);
	cout << " toStringJ0: " << tIndex.toStringJ()
					<< ", ms: " << tIndex.toInt64MillisecondsFractionOfYear()
					<< ", msj: " << tIndex.toInt64MillisecondsFractionOfYearJ()
					<< ", msp: " << millisecondsInYear(tIndex.get_BeforeAfterStartBit(),tIndex.get_year())
					<< endl << flush;
	cout << "---" << endl << flush;

	tIndex.setZero().setEOY(1,2000); INDEX_OUT(++tag_id,tIndex);
	cout << " toStringJ0: " << tIndex.toStringJ()
					<< ", ms: " << tIndex.toInt64MillisecondsFractionOfYear()
					<< ", msj: " << tIndex.toInt64MillisecondsFractionOfYearJ()
					<< ", msp: " << millisecondsInYear(tIndex.get_BeforeAfterStartBit(),tIndex.get_year())
					<< endl << flush;
	cout << "---" << endl << flush;

	tIndex.setZero().setEOY(1,2001); INDEX_OUT(++tag_id,tIndex);
	cout << " toStringJ0: " << tIndex.toStringJ()
					<< ", ms: " << tIndex.toInt64MillisecondsFractionOfYear()
					<< ", msj: " << tIndex.toInt64MillisecondsFractionOfYearJ()
					<< ", msp: " << millisecondsInYear(tIndex.get_BeforeAfterStartBit(),tIndex.get_year())
					<< endl << flush;
	cout << "---" << endl << flush;

	tIndex.setZero().setEOY(1,2002); INDEX_OUT(++tag_id,tIndex);
	cout << " toStringJ0: " << tIndex.toStringJ()
					<< ", ms: " << tIndex.toInt64MillisecondsFractionOfYear()
					<< ", msj: " << tIndex.toInt64MillisecondsFractionOfYearJ()
					<< ", msp: " << millisecondsInYear(tIndex.get_BeforeAfterStartBit(),tIndex.get_year())
					<< endl << flush;
	cout << "---" << endl << flush;

	{

#define FMT_DT1(lbl) \
		cout \
		<< lbl \
		<< setw(9) << setfill(' ') << _year << "-" \
		<< setw(2) << setfill('0') << _month << "-" \
		<< setw(2) << setfill('0') << _day_of_month << " " \
		<< setw(2) << _hour << ":" \
		<< setw(2) << _minute << ":" \
		<< setw(2) << _second << "." \
		<< setw(3) << _millisecond \
		<< endl << flush;

		int _year, _month, _day_of_month, _hour, _minute, _second, _millisecond;
		tIndex.toUTC(_year, _month, _day_of_month, _hour, _minute, _second, _millisecond);
		FMT_DT1("")

		cout << "---" << endl << flush;

		_year         = 1972;
		_month        =   12;
		_day_of_month =   31;
		_hour         =   23;
		_minute       =   59;
		_second       =   60;
		_millisecond  =  999;
		FMT_DT1("1972 eoy leap second ")
		tIndex.setZero().fromUTC(_year, _month, _day_of_month, _hour, _minute, _second, _millisecond); INDEX_OUT(++tag_id,tIndex);

		_year         = 0;
		_month        = 0;
		_day_of_month = 0;
		_hour         = 0;
		_minute       = 0;
		_second       = 0;
		_millisecond  = 0;
		tIndex.toUTC(_year, _month, _day_of_month, _hour, _minute, _second, _millisecond);
		FMT_DT1("")

		cout << "---" << endl << flush;

		_year         = 1972;
		_month        =    6;
		_day_of_month =   30;
		_hour         =   23;
		_minute       =   59;
		_second       =   60;
		_millisecond  =  999;
		FMT_DT1("1972 6/30 leap second ")
		tIndex.setZero().fromUTC(_year, _month, _day_of_month, _hour, _minute, _second, _millisecond); INDEX_OUT(++tag_id,tIndex);

		_year         = 0;
		_month        = 0;
		_day_of_month = 0;
		_hour         = 0;
		_minute       = 0;
		_second       = 0;
		_millisecond  = 0;
		tIndex.toUTC(_year, _month, _day_of_month, _hour, _minute, _second, _millisecond);
		FMT_DT1("")

		cout << "---" << endl << flush;

		_year         = 1972;
		_month        =    6;
		_day_of_month =   30;
		_hour         =   23;
		_minute       =   59;
		_second       =   61;
		_millisecond  =  999;
		FMT_DT1("1972 6/30 leap second + 1")
		tIndex.setZero().fromUTC(_year, _month, _day_of_month, _hour, _minute, _second, _millisecond); INDEX_OUT(++tag_id,tIndex);

		_year         = 0;
		_month        = 0;
		_day_of_month = 0;
		_hour         = 0;
		_minute       = 0;
		_second       = 0;
		_millisecond  = 0;
		tIndex.toUTC(_year, _month, _day_of_month, _hour, _minute, _second, _millisecond);
		FMT_DT1("")

		cout << "---" << endl << flush;

		_year         = 1986;
		_month        =   12;
		_day_of_month =   31;
		_hour         =   23;
		_minute       =   59;
		_second       =   60;
		_millisecond  =  501;
		FMT_DT1("1986 eoy fake leap second")
		tIndex.setZero().fromUTC(_year, _month, _day_of_month, _hour, _minute, _second, _millisecond); INDEX_OUT(++tag_id,tIndex);

		_year         = 0;
		_month        = 0;
		_day_of_month = 0;
		_hour         = 0;
		_minute       = 0;
		_second       = 0;
		_millisecond  = 0;
		tIndex.toUTC(_year, _month, _day_of_month, _hour, _minute, _second, _millisecond);
		FMT_DT1("")

		cout << "---" << endl << flush;

		tIndex.setZero().fromJulianTAI(0, 0); INDEX_OUT(++tag_id,tIndex);

		cout << "---" << endl << flush;
		tag_id = 23000;
		failureMessage = "'";
		try {
			tIndex.setZero().fromJulianTAI(-1.0, 0); INDEX_OUT(++tag_id,tIndex);
			tIndex.setZero().fromJulianTAI(-10.0, 0); INDEX_OUT(++tag_id,tIndex);
			tIndex.setZero().fromJulianTAI(-100.0, 0); INDEX_OUT(++tag_id,tIndex);
			tIndex.setZero().fromJulianTAI(-1000.0, 0); INDEX_OUT(++tag_id,tIndex);
			tIndex.setZero().fromJulianTAI(-10000.0, 0); INDEX_OUT(++tag_id,tIndex);
			tIndex.setZero().fromJulianTAI(-20000.0, 0); INDEX_OUT(++tag_id,tIndex);
			tIndex.setZero().fromJulianTAI(-40000.0, 0); INDEX_OUT(++tag_id,tIndex);
			// tIndex.setZero().fromJulianTAIDouble2(-80000.0, 0); INDEX_OUT(++tag_id,tIndex);
		} catch (SpatialFailure& failure) {
			failureMessage += failure.what();
			cout << "failureMessage: " << failureMessage << "'" << endl << flush;
		};

		cout << "---" << endl << flush;

		{
			int64_t CE, year, milliseconds;

			tag_id = 24000;
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

		failureMessage = "'";
		try {
		tIndex.setZero().fromNativeCEYearAndMilliseconds(1,0,0); INDEX_OUT(++tag_id,tIndex);
		tIndex.toNativeCEYearAndMilliseconds(CE, year, milliseconds);
		cout << "CE,year,milliseconds: " << CE << " " << year << " " << milliseconds
				<< " : " << tIndex.toNativeYear()
				<< " : " << tIndex.scidbTemporalIndex()
				<< endl << flush;
		} catch (SpatialFailure& failure) {
			failureMessage += failure.what();
			cout << "failureMessage: " << failureMessage << "'" << endl << flush;
		};

		tIndex.setZero().fromNativeCEYearAndMilliseconds(0,10000,0); INDEX_OUTN(++tag_id,tIndex);
		tIndex.toNativeCEYearAndMilliseconds(CE, year, milliseconds);
		cout << "CE,year,milliseconds: " << CE << " " << year << " " << milliseconds
				<< " : " << tIndex.toNativeYear()
				<< " : " << tIndex.scidbTemporalIndex()
				<< endl << flush;

		tIndex.setZero().fromNativeCEYearAndMilliseconds(0,100000,120000); INDEX_OUTN(++tag_id,tIndex);
		tIndex.toNativeCEYearAndMilliseconds(CE, year, milliseconds);
		cout << "CE,year,milliseconds: " << CE << " " << year << " " << milliseconds
				<< " : " << tIndex.toNativeYear()
				<< " : " << tIndex.scidbTemporalIndex()
				<< endl << flush;

		tIndex.setZero().fromNativeCEYearAndMilliseconds(1,100000,120000); INDEX_OUTN(++tag_id,tIndex);
		tIndex.toNativeCEYearAndMilliseconds(CE, year, milliseconds);
		cout << "CE,year,milliseconds: " << CE << " " << year << " " << milliseconds
				<< " : " << tIndex.toNativeYear()
				<< " : " << tIndex.scidbTemporalIndex()
				<< endl << flush;

		double year_ms = 31536000000.0; double year_ti;

		cout << "----ti----" << endl << flush;

		tIndex.setZero().fromNativeCEYearAndMilliseconds(0,0,0.25*year_ms); INDEX_OUTN(++tag_id,tIndex);
		tIndex.toNativeCEYearAndMilliseconds(CE, year, milliseconds);
		cout << "CE,year,milliseconds: " << CE << " " << year << " " << milliseconds
				<< " : " << tIndex.toNativeYear()
				<< " : " << tIndex.scidbTemporalIndex()
				<< endl << flush;

		year_ti = tIndex.toNativeYear(); tIndex.setZero().fromNativeYear(year_ti);  INDEX_OUTN(++tag_id,tIndex);
		cout << "year_ti: " << year_ti << endl << flush;
		cout << "----" << endl << flush;

		tIndex.setZero().fromNativeCEYearAndMilliseconds(1,1,0.25*year_ms); INDEX_OUTN(++tag_id,tIndex);
		tIndex.toNativeCEYearAndMilliseconds(CE, year, milliseconds);
		cout << "CE,year,milliseconds: " << CE << " " << year << " " << milliseconds
				<< " : " << tIndex.toNativeYear()
				<< " : " << tIndex.scidbTemporalIndex()
				<< endl << flush;

		year_ti = tIndex.toNativeYear(); tIndex.setZero().fromNativeYear(year_ti);  INDEX_OUTN(++tag_id,tIndex);
		cout << "year_ti: " << year_ti << endl << flush;
		cout << "----" << endl << flush;

		// tIndex.setZero().setDateFromYearAndMilliseconds(1,1,0.25*year_ms); INDEX_OUTN(++tag_id,tIndex);
		cout << "Note that in the current configuration, the leap-year day is not captured in toNativeYear. Use toJulianTAI instead." << endl << flush;
		tIndex.setZero().fromFormattedJulianTAI(2000, 12, 31, 23, 59, 59, 999); INDEX_OUTN(++tag_id,tIndex);
		tIndex.toNativeCEYearAndMilliseconds(CE, year, milliseconds);
		cout << "CE,year,milliseconds: " << CE << " " << year << " " << milliseconds
				<< " : " << tIndex.toNativeYear()
				<< " : " << tIndex.scidbTemporalIndex()
				<< " , " << tIndex.scidbTerminator()
				<< " , " << tIndex.scidbTerminatorJulianTAI()
				<< endl << flush;

		year_ti = tIndex.toNativeYear(); tIndex.setZero().fromNativeYear(year_ti);  INDEX_OUTN(++tag_id,tIndex);
		cout << "year_ti: " << year_ti << endl << flush;
		cout << "----" << endl << flush;





		}
#undef FMT_DT1

	}

	FAIL();
}
