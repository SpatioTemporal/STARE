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
// #define TAG(x) {}

void TemporalIndex_test() {

	string failureMessage = "'";

	TemporalIndex tIndex;

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

#define INDEX_OUT0(lbl,var) cout << dec << lbl << " nat,trad,scidb: " << var.stringInNativeDate()	<< flush << ", " << var.hackStringInTraditionalDate() << flush << endl;
#define INDEX_OUT(lbl,var)  cout << dec << lbl << " nat,trad,scidb: " << var.stringInNativeDate()	<< flush << ", " << var.hackStringInTraditionalDate() << flush << ", 0x" << setw(16) << setfill('0') << hex << var.scidbTemporalIndex() << dec << ", " << setfill(' ') << setw(22) << var.scidbTemporalIndex() << endl << flush;
#define INDEX_OUTNC(lbl,var)  cout << dec << lbl << " nat,trad,scidb: " << var.stringInNativeDate()	<< flush << ", " << var.hackStringInTraditionalDate() << flush << ", 0x" << setw(16) << setfill('0') << hex << var.scidbTemporalIndex() << dec << ", " << setfill(' ') << setw(22) << var.scidbTemporalIndex();

	// ASSERT_EQUAL("",tIndex.stringInNativeDate());
	// ASSERT_EQUAL("",tIndex.hackStringInTraditionalDate());
	// ASSERT_EQUAL(,tIndex.scidbTemporalIndex());

	failureMessage = "'";
	try {
		TAG(1000)
		tIndex = TemporalIndex(0,0,0,0,0,0,0,0,0,0,2); // TAG(1000); // INDEX_OUT0(1000,tIndex); TAG(1001); INDEX_OUT(1001,tIndex);
		ASSERT_EQUAL("-000000000-00-0-0 00:00:00.000 (00) (2)",tIndex.stringInNativeDate());
		ASSERT_EQUAL("0 000000001-01-01 00:00:00.000 (00) (2)",tIndex.hackStringInTraditionalDate());
		ASSERT_EQUAL(0xfffff20a041018fe,tIndex.scidbTemporalIndex());
		TAG(1100)
		tIndex = TemporalIndex(1,1,0,0,0,0,0,0,0,0,2); // TAG(1100); // INDEX_OUT0(1100,tIndex); TAG(1101); INDEX_OUT(1101,tIndex);
		ASSERT_EQUAL("+000000001-00-0-0 00:00:00.000 (00) (2)",tIndex.stringInNativeDate());
		ASSERT_EQUAL("1 000000001-01-01 00:00:00.000 (00) (2)",tIndex.hackStringInTraditionalDate());
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

	tIndex.hackSetTraditionalDate(1, 1, 12, 31, 23, 59, 59, 998); // INDEX_OUT(0,tIndex);
	ASSERT_EQUAL("+000000001-13-0-0 23:59:59.998 (00) (2)",tIndex.stringInNativeDate());
	ASSERT_EQUAL("1 000000001-12-31 23:59:59.998 (00) (2)",tIndex.hackStringInTraditionalDate());

	tIndex.setZero().hackSetTraditionalDate(1, 1, 12, 31, 23, 59, 59, 999); // INDEX_OUT(1,tIndex);
	ASSERT_EQUAL("+000000001-13-0-0 23:59:59.999 (00) (2)",tIndex.stringInNativeDate());
	ASSERT_EQUAL("1 000000001-12-31 23:59:59.999 (00) (2)",tIndex.hackStringInTraditionalDate());

	tIndex.hackSetTraditionalDate(1, 2, 1, 1, 0, 0, 0, 0); // INDEX_OUT(2,tIndex);
	ASSERT_EQUAL("+000000002-00-0-0 00:00:00.000 (00) (2)",tIndex.stringInNativeDate());
	ASSERT_EQUAL("1 000000002-01-01 00:00:00.000 (00) (2)",tIndex.hackStringInTraditionalDate());

	tIndex.hackSetTraditionalDate(0, 1, 12, 31, 23, 59, 59, 999); // INDEX_OUT(3,tIndex);
	ASSERT_EQUAL("-000000000-13-0-1 23:59:59.999 (00) (2)",tIndex.stringInNativeDate());
	ASSERT_EQUAL("0 000000001-12-31 23:59:59.999 (00) (2)",tIndex.hackStringInTraditionalDate());

	TAG(4)

	// cout << endl << flush;

	tIndex.hackSetTraditionalDate(1, 1, 12, 31, 23, 59, 59, 999); // INDEX_OUT(4,tIndex);
	ASSERT_EQUAL("+000000001-13-0-0 23:59:59.999 (00) (2)",tIndex.stringInNativeDate());
	ASSERT_EQUAL("1 000000001-12-31 23:59:59.999 (00) (2)",tIndex.hackStringInTraditionalDate());
	ASSERT_EQUAL(0x00001d05fbefe702,tIndex.scidbTemporalIndex());

	tIndex.hackSetTraditionalDate(1, 2, 12, 31, 23, 59, 59, 999); // INDEX_OUT(5,tIndex);
	ASSERT_EQUAL("+000000002-13-0-0 23:59:59.999 (00) (2)",tIndex.stringInNativeDate());
	ASSERT_EQUAL("1 000000002-12-31 23:59:59.999 (00) (2)",tIndex.hackStringInTraditionalDate());
	ASSERT_EQUAL(0x00002d05fbefe702,tIndex.scidbTemporalIndex());

	tIndex.hackSetTraditionalDate(1, 3, 12, 31, 23, 59, 59, 999); // INDEX_OUT(6,tIndex);
	ASSERT_EQUAL("+000000003-13-0-0 23:59:59.999 (00) (2)",tIndex.stringInNativeDate());
	ASSERT_EQUAL("1 000000003-12-31 23:59:59.999 (00) (2)",tIndex.hackStringInTraditionalDate());
	ASSERT_EQUAL(0x00003d05fbefe702,tIndex.scidbTemporalIndex());

	tIndex.hackSetTraditionalDate(1, 4, 12, 31, 23, 59, 59, 999); // INDEX_OUT(7,tIndex);
	ASSERT_EQUAL("+000000004-13-0-1 23:59:59.999 (00) (2)",tIndex.stringInNativeDate());
	ASSERT_EQUAL("1 000000004-12-31 23:59:59.999 (00) (2)",tIndex.hackStringInTraditionalDate());
	ASSERT_EQUAL(0x00004d0dfbefe702,tIndex.scidbTemporalIndex());

	tIndex.hackSetTraditionalDate(1, 5, 12, 31, 23, 59, 59, 999); // INDEX_OUT(8,tIndex);
	ASSERT_EQUAL("+000000005-13-0-0 23:59:59.999 (00) (2)",tIndex.stringInNativeDate());
	ASSERT_EQUAL("1 000000005-12-31 23:59:59.999 (00) (2)",tIndex.hackStringInTraditionalDate());
	ASSERT_EQUAL(0x00005d05fbefe702,tIndex.scidbTemporalIndex());

	// cout << endl << flush;

	TAG(110)
	tIndex.setZero().hackSetTraditionalDate(0, 1,  1,  1,  0,  0,  0, 0); // .set_BeforeAfterStartBit(0); // INDEX_OUT(1,tIndex);
	ASSERT_EQUAL("-000000000-00-0-0 00:00:00.000 (00) (2)",tIndex.stringInNativeDate());
	ASSERT_EQUAL("0 000000001-01-01 00:00:00.000 (00) (2)",tIndex.hackStringInTraditionalDate());
	ASSERT_EQUAL(0xfffff20a041018fe,tIndex.scidbTemporalIndex());

	// cout << "1 nat,trad,scidb: " << tIndex.stringInNativeDate()	<< flush << ", " << tIndex.hackStringInTraditionalDate() << flush << ", 0x" << setw(16) << setfill('0') << hex << tIndex.scidbTemporalIndex() << dec << tIndex.scidbTemporalIndex() << endl << flush;
	// cout << "1 nat,trad,scidb: " << tIndex.stringInNativeDate()	<< flush << ", " << tIndex.hackStringInTraditionalDate() << flush << hex << tIndex.scidbTemporalIndex() << dec << endl << flush;
	// cout << "1 nat,trad: " << tIndex.stringInNativeDate()  << ", " << tIndex.hackStringInTraditionalDate() << endl << flush;
	// TAG(124)
	// cout << "1 scidb idx: 0x" << hex << tIndex.scidbTemporalIndex() << dec << endl << flush;

	TAG(150)
	// tIndex.setZero();
	// Months go 0..11 for hack-traditional
	tIndex.hackSetTraditionalDate(0, 2, 12, 31, 23, 59, 59, 999);
	// tIndex.hackSetTraditionalDate(0,  0,  1,  0,  0,  0, 0);
	tIndex.set_BeforeAfterStartBit(0); // INDEX_OUT(2,tIndex);
	// cout << "2 nat,trad,scidb: " << tIndex.stringInNativeDate()	<< flush << ", " << tIndex.hackStringInTraditionalDate() << flush << ", 0x" << setw(16) << setfill('0') << hex << tIndex.scidbTemporalIndex() << dec << tIndex.scidbTemporalIndex() << endl << flush;
	// cout << "2 nat,trad,scidb: " << tIndex.stringInNativeDate()	<< flush << ", " << tIndex.hackStringInTraditionalDate() << flush << hex << tIndex.scidbTemporalIndex() << dec << endl << flush;
	// cout << "2 nat,trad: " << tIndex.stringInNativeDate()  << ", " << tIndex.hackStringInTraditionalDate() << endl << flush;
	// cout << "2 scidb idx: 0x" << hex << tIndex.scidbTemporalIndex() << dec << endl << flush;

	TAG(170)

	ASSERT_EQUAL("-000000001-13-0-0 23:59:59.999 (00) (2)",tIndex.stringInNativeDate());
	ASSERT_EQUAL("0 000000002-12-31 23:59:59.999 (00) (2)",tIndex.hackStringInTraditionalDate());
	ASSERT_EQUAL(0xffffef0ffffffffe,tIndex.scidbTemporalIndex());

	cout << 1000 << " " << flush << (tIndex == tIndex) << endl << flush;

	/*

	// TAG(200)
	tIndex.setZero(); tIndex.set_BeforeAfterStartBit(0); // INDEX_OUT(3,tIndex);
	tIndex.hackSetTraditionalDate(0, 1,  1,  1,  0,  0,  0, 0);	// INDEX_OUT(3,tIndex);

	cout << endl << flush;

	tIndex.hackSetTraditionalDate(0, 2, 12, 31, 23, 59, 59, 999); INDEX_OUT(3,tIndex);
	tIndex.hackSetTraditionalDate(0, 3, 12, 31, 23, 59, 59, 999); INDEX_OUT(4,tIndex);
	tIndex.hackSetTraditionalDate(0, 4, 12, 31, 23, 59, 59, 999); INDEX_OUT(5,tIndex);
	tIndex.hackSetTraditionalDate(0, 5, 12, 30, 23, 59, 59, 999); INDEX_OUT(6,tIndex);
	tIndex.hackSetTraditionalDate(0, 5, 12, 31, 23, 59, 59, 999); INDEX_OUT(6,tIndex);
	tIndex.hackSetTraditionalDate(0, 6, 12, 31, 23, 59, 59, 999); INDEX_OUT(6,tIndex);
	tIndex.hackSetTraditionalDate(0, 6,  1,  2, 23, 59, 59, 999); INDEX_OUT(6,tIndex);
	tIndex.hackSetTraditionalDate(0, 6,  1,  1, 23, 59, 59, 999); INDEX_OUT(6,tIndex);
	tIndex.hackSetTraditionalDate(0, 7, 12, 31, 23, 59, 59, 999); INDEX_OUT(7,tIndex);

	cout << endl << flush;

	tIndex.hackSetTraditionalDate(1, 2, 12, 31, 23, 59, 59, 999); INDEX_OUT(3,tIndex);
	tIndex.hackSetTraditionalDate(1, 3, 12, 31, 23, 59, 59, 999); INDEX_OUT(4,tIndex);
	tIndex.hackSetTraditionalDate(1, 4, 12, 31, 23, 59, 59, 999); INDEX_OUT(5,tIndex);
	tIndex.hackSetTraditionalDate(1, 5, 12, 30, 23, 59, 59, 999); INDEX_OUT(6,tIndex);
	tIndex.hackSetTraditionalDate(1, 5, 12, 31, 23, 59, 59, 999); INDEX_OUT(6,tIndex);
	tIndex.hackSetTraditionalDate(1, 6, 12, 31, 23, 59, 59, 999); INDEX_OUT(6,tIndex);
	tIndex.hackSetTraditionalDate(1, 6,  1,  2, 23, 59, 59, 999); INDEX_OUT(6,tIndex);
	tIndex.hackSetTraditionalDate(1, 6,  1,  1, 23, 59, 59, 999); INDEX_OUT(6,tIndex);
	tIndex.hackSetTraditionalDate(1, 7, 12, 31, 23, 59, 59, 999); INDEX_OUT(7,tIndex);
	cout << endl << flush;

	tIndex.hackSetTraditionalDate(0, 6, 1, 1, 0, 0, 0, 0); INDEX_OUT(7,tIndex);
	tIndex.hackSetTraditionalDate(0, 6, 1, 1, 0, 0, 0, 1); INDEX_OUT(7,tIndex);

	cout << endl << flush;

	tIndex.hackSetTraditionalDate(0, 4, 1, 1, 0, 0, 0, 0); INDEX_OUT(8,tIndex);
	tIndex.hackSetTraditionalDate(0, 4*1024+1, 12, 31, 23, 59, 59, 999); INDEX_OUT(8,tIndex);
	tIndex.hackSetTraditionalDate(0, 4*1024, 1, 1, 0, 0, 0, 0); INDEX_OUT(8,tIndex);
	tIndex.hackSetTraditionalDate(0, 4*1024, 1, 1, 0, 0, 0, 1); INDEX_OUT(8,tIndex);
	tIndex.hackSetTraditionalDate(1, 4*1024, 1, 1, 0, 0, 0, 0); INDEX_OUT(8,tIndex);
	tIndex.hackSetTraditionalDate(1, 4*1024, 1, 1, 0, 0, 0, 1); INDEX_OUT(8,tIndex);

	cout << endl << flush;
	tIndex.hackSetTraditionalDate(0, 256*1024, 1, 1, 0, 0, 0, 1); INDEX_OUT(8,tIndex);
	tIndex.hackSetTraditionalDate(1, 256*1024, 1, 1, 0, 0, 0, 1); INDEX_OUT(8,tIndex);

	TAG(300)

	cout << endl << flush;
	tIndex.hackSetTraditionalDate(1, 2007, 12, 31, 0, 0, 0, 0); INDEX_OUT(9,tIndex);
	tIndex.hackSetTraditionalDate(1, 2008, 12, 31, 0, 0, 0, 0); INDEX_OUT(9,tIndex);
	tIndex.hackSetTraditionalDate(1, 2009,  1,  1, 0, 0, 0, 0); INDEX_OUT(9,tIndex);
	cout << endl << flush;

	*/

	tIndex.hackSetTraditionalDate(1, 2009,  1,  1, 0, 0, 0, 0); // INDEX_OUT(9,tIndex);
	TemporalIndex tIndex0;

	try {
		tIndex0.hackFromTraditionalString(tIndex.hackStringInTraditionalDate());
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
	// cout << "4  nat,trad: " << tIndex.stringInNativeDate()  << ", " << tIndex.hackStringInTraditionalDate() << endl << flush;
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
				<< ", " << tIndex.stringInNativeDate() << ", " << tIndex.hackStringInTraditionalDate()
				<< dec << endl << flush;
			TemporalIndex tIndex1 = TemporalIndex(tIndex.scidbTerminator());
			cout << "            :   0x" << hex << setw(16) << setfill('0') << tIndex.scidbTerminator()
				<< ", " << tIndex1.scidbTerminatorp()
				<< ", " << tIndex1.stringInNativeDate() << ", " << tIndex1.hackStringInTraditionalDate()
				<< dec << endl << endl << flush;
		}
	} catch (SpatialFailure& failure) {
		failureMessage += failure.what();
		cout << "failureMessage: " << failureMessage << "'" << endl << flush;
	};

	TemporalIndex tIndex2;
	tIndex2.hackSetTraditionalDate(1, 262144, 12, 31, 23, 59, 59, 999);
	cout << "            :   0x" << hex << setw(16) << setfill('0') << tIndex2.scidbTemporalIndex() << flush
	<< ", " << tIndex2.scidbTerminatorp() << flush
	<< ", " << tIndex2.stringInNativeDate() << flush
	<< ", " << tIndex2.hackStringInTraditionalDate() << flush
	<< dec << endl << endl << flush;

	// int not_ok = tIndex2.eraTest();
	// cout << flush;

	// TAG(800)

	TemporalIndex tIndex3, tIndex4;

	/*
	tIndex3.set_year(0);
	cout << "0 tI3: " << tIndex3.stringInNativeDate()  << ", " << tIndex3.hackStringInTraditionalDate() << endl << flush;
	tIndex3.set_BeforeAfterStartBit(0);
	cout << "1 tI3: " << tIndex3.stringInNativeDate()  << ", " << tIndex3.hackStringInTraditionalDate() << endl << flush;
	tIndex3.set_BeforeAfterStartBit(1);
	cout << "2 tI3: " << tIndex3.stringInNativeDate()  << ", " << tIndex3.hackStringInTraditionalDate() << endl << flush;

	tIndex3.set_year(1);
	cout << "3 tI3: " << tIndex3.stringInNativeDate()  << ", " << tIndex3.hackStringInTraditionalDate() << endl << flush;
	tIndex3.set_BeforeAfterStartBit(0);
	cout << "4 tI3: " << tIndex3.stringInNativeDate()  << ", " << tIndex3.hackStringInTraditionalDate() << endl << flush;
	tIndex3.set_BeforeAfterStartBit(1);
	cout << "5 tI3: " << tIndex3.stringInNativeDate()  << ", " << tIndex3.hackStringInTraditionalDate() << endl << flush;
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
	// cout << "0 tI3: " << tIndex3.stringInNativeDate()  << ", " << tIndex3.hackStringInTraditionalDate() << endl << flush;
	// cout << "0 tI4: " << tIndex4.stringInNativeDate()  << ", " << tIndex4.hackStringInTraditionalDate() << endl << flush;
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

	// cout << "tI3: " << tIndex3.hackStringInTraditionalDate() << endl << flush;
	// cout << "tI4: " << tIndex4.hackStringInTraditionalDate() << endl << flush;

	ASSERT_EQUAL("0 000000003-03-26 00:00:00.000 (62) (2)",tIndex3.hackStringInTraditionalDate());
	ASSERT_EQUAL("0 000000002-03-26 00:00:00.000 (63) (2)",tIndex4.hackStringInTraditionalDate());
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

	tIndex3.hackSetTraditionalDate(0, 1,  1,  1,  0,  0,  0,   0);
	tIndex3.set_resolution(tIndex3.data.maxResolutionLevel());
	INDEX_OUT(3000,tIndex3);
	// tIndex4.hackSetTraditionalDate(0, 1, 12, 31, 23, 59, 59, 999);
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
	tIndex5 = tIndex3 + tIndex4; INDEX_OUT(++tag_id,tIndex5);
	tIndex.setZero().set_millisecond(i3+i4).set_resolution(level);
	ASSERT_EQUAL(tIndex,tIndex5);

	// SciDB checks
	int64_t max_coordinate = (1ULL << 62) - 1;
	int64_t min_coordinate = - max_coordinate;

	tag_id = 9999;
	tIndex = TemporalIndex(max_coordinate);
	INDEX_OUT(++tag_id,tIndex);
	tIndex.setZero().set_BeforeAfterStartBit(1).set_year(262143);

	tIndex = TemporalIndex(min_coordinate);
	INDEX_OUT(++tag_id,tIndex);
	tIndex.setZero().set_year(262143);
	INDEX_OUT(++tag_id,tIndex);

	tIndex.setZero().setEOY(1024,1);
	INDEX_OUT(++tag_id,tIndex);

	tIndex.hackSetTraditionalDate(1, 2000, 12, 31, 23, 59, 59, 999);
	INDEX_OUTNC(++tag_id,tIndex);
	cout << " " << setw(18) << setfill(' ') << tIndex.toInt64Milliseconds();
	cout << endl << flush;

	tIndex.setZero().set_BeforeAfterStartBit(1).set_year(2001).set_hour(23).set_minute(59).set_second(59).set_millisecond(999);
	INDEX_OUTNC(++tag_id,tIndex);
	cout << " " << setw(18) << setfill(' ') << tIndex.toInt64Milliseconds();
	cout << endl << flush;
	cout << endl << flush;

	double d1,d2;
	tIndex.hackSetTraditionalDate(1, 1999, 12, 31, 23, 59, 59, 999);
	INDEX_OUT(++tag_id,tIndex);
	tIndex.toJulianDoubleDay(d1, d2);
	tIndex.setZero().fromJulianDoubleDay(d1, d2);
	INDEX_OUTNC(++tag_id,tIndex); cout << " d1,d2: " << d1 << ", " << d2 << endl << flush;
	cout << endl << flush;

	tIndex.setZero().hackSetTraditionalDate(1, 2000, 12, 31, 23, 59, 59, 999);
	INDEX_OUT(++tag_id,tIndex);
	tIndex.toJulianDoubleDay(d1, d2);
	tIndex.setZero().fromJulianDoubleDay(d1, d2);
	INDEX_OUTNC(++tag_id,tIndex); cout << " d1,d2: " << d1 << ", " << d2 << endl << flush;
	cout << endl << flush;

	// << setprecision(17) << setw(20) << scientific

	// tIndex.setZero().hackSetTraditionalDate(1, 2000, 12, 31, 23, 59, 45, 459);
	tIndex.setZero().hackSetTraditionalDate(1, 2000, 12, 31, 23, 59, 59, 999);
	INDEX_OUT(++tag_id,tIndex);
	tIndex.toJulianDoubleDay(d1, d2);
	tIndex.setZero().fromJulianDoubleDay(d1, d2);
	INDEX_OUTNC(++tag_id,tIndex); cout << " d1,d2: " << d1 << ", " << d2 << endl << flush;
	tIndex.toJulianDoubleDay(d1, d2);
	tIndex.setZero().fromJulianDoubleDay(d1, d2);
	INDEX_OUTNC(++tag_id,tIndex); cout << " d1,d2: " << d1 << ", " << d2 << endl << flush;
	tIndex.toJulianDoubleDay(d1, d2);
	tIndex.setZero().fromJulianDoubleDay(d1, d2);
	INDEX_OUTNC(++tag_id,tIndex); cout << " d1,d2: " << d1 << ", " << d2 << endl << flush;
	cout << endl << flush;

	FAIL();
}
