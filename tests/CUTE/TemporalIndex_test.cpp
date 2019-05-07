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

	// TAG(0)
	tIndex.data.setZero();
	// TAG(1)

#define INDEX_OUT(lbl,var) cout << lbl << " nat,trad,scidb: " << var.stringInNativeDate()	<< flush << ", " << var.hackStringInTraditionalDate() << flush << ", 0x" << setw(16) << setfill('0') << hex << var.scidbTemporalIndex() << dec << ", " << setfill(' ') << setw(22) << var.scidbTemporalIndex() << endl << flush;

	tIndex.hackSetTraditionalDate(1, 1, 12, 31, 23, 59, 59, 998);
	// TAG(2)
	// tIndex.set_BeforeAfterStartBit(0);
	// TAG(3)
	INDEX_OUT(0,tIndex);
	// cout << "0 nat,trad,scidb: " << tIndex.stringInNativeDate()	<< flush << ", " << tIndex.hackStringInTraditionalDate() << flush << ", 0x" << setw(16) << setfill('0') << hex << tIndex.scidbTemporalIndex() << dec << tIndex.scidbTemporalIndex() << endl << flush;
	// TAG(5)

	// TAG(100)
	tIndex.data.setZero();
	// TAG(101)
	tIndex.hackSetTraditionalDate(1, 1, 12, 31, 23, 59, 59, 999);
	// TAG(102)
	// tIndex.set_BeforeAfterStartBit(0);
	// TAG(103)
	INDEX_OUT(1,tIndex);
	// cout << "0 nat,trad,scidb: " << tIndex.stringInNativeDate()	<< flush << ", " << tIndex.hackStringInTraditionalDate() << flush << ", 0x" << setw(16) << setfill('0') << hex << tIndex.scidbTemporalIndex() << dec << tIndex.scidbTemporalIndex() << endl << flush;
	// cout << "0 nat,trad: " << tIndex.stringInNativeDate()  << ", " << tIndex.hackStringInTraditionalDate() << endl << flush;
	// TAG(104)
	// cout << "0 scidb idx: 0x" << hex << tIndex.scidbTemporalIndex() << dec << endl << flush;
	// TAG(105)
	tIndex.hackSetTraditionalDate(1, 2, 1, 1, 0, 0, 0, 0);
	// TAG(2)
	// tIndex.set_BeforeAfterStartBit(0);
	// TAG(3)
	INDEX_OUT(2,tIndex);
	// cout << "0 nat,trad,scidb: " << tIndex.stringInNativeDate()	<< flush << ", " << tIndex.hackStringInTraditionalDate() << flush << ", 0x" << setw(16) << setfill('0') << hex << tIndex.scidbTemporalIndex() << dec << tIndex.scidbTemporalIndex() << endl << flush;

	tIndex.hackSetTraditionalDate(0, 1, 12, 31, 23, 59, 59, 999);
	INDEX_OUT(3,tIndex);
	// cout << "0 nat,trad,scidb: " << tIndex.stringInNativeDate()	<< flush << ", " << tIndex.hackStringInTraditionalDate() << flush << ", 0x" << setw(16) << setfill('0') << hex << tIndex.scidbTemporalIndex() << dec << tIndex.scidbTemporalIndex() << endl << flush;
	// cout << "0 nat,trad: " << tIndex.stringInNativeDate()  << ", " << tIndex.hackStringInTraditionalDate() << endl << flush;
	// cout << "0 scidb idx: 0x" << hex << tIndex.scidbTemporalIndex() << dec << endl << flush;

	cout << endl << flush;

	tIndex.hackSetTraditionalDate(1, 1, 12, 31, 23, 59, 59, 999); INDEX_OUT(4,tIndex);
	tIndex.hackSetTraditionalDate(1, 2, 12, 31, 23, 59, 59, 999); INDEX_OUT(5,tIndex);
	tIndex.hackSetTraditionalDate(1, 3, 12, 31, 23, 59, 59, 999); INDEX_OUT(6,tIndex);
	tIndex.hackSetTraditionalDate(1, 4, 12, 31, 23, 59, 59, 999); INDEX_OUT(7,tIndex);
	tIndex.hackSetTraditionalDate(1, 5, 12, 31, 23, 59, 59, 999); INDEX_OUT(8,tIndex);

	cout << endl << flush;

	// TAG(110)
	tIndex.data.setZero();
	// TAG(111)
	// tIndex.hackSetTraditionalDate(1, 11, 31, 23, 59, 59, 999);
	// Months go 1..12 for hack-traditional
	tIndex.hackSetTraditionalDate(0, 1,  1,  1,  0,  0,  0, 0);
	// TAG(112)
	//
	tIndex.set_BeforeAfterStartBit(0);
	// TAG(123)
	INDEX_OUT(1,tIndex);
	// cout << "1 nat,trad,scidb: " << tIndex.stringInNativeDate()	<< flush << ", " << tIndex.hackStringInTraditionalDate() << flush << ", 0x" << setw(16) << setfill('0') << hex << tIndex.scidbTemporalIndex() << dec << tIndex.scidbTemporalIndex() << endl << flush;
	// cout << "1 nat,trad,scidb: " << tIndex.stringInNativeDate()	<< flush << ", " << tIndex.hackStringInTraditionalDate() << flush << hex << tIndex.scidbTemporalIndex() << dec << endl << flush;
	// cout << "1 nat,trad: " << tIndex.stringInNativeDate()  << ", " << tIndex.hackStringInTraditionalDate() << endl << flush;
	// TAG(124)
	// cout << "1 scidb idx: 0x" << hex << tIndex.scidbTemporalIndex() << dec << endl << flush;

	// TAG(150)
	tIndex.data.setZero();
	// Months go 0..11 for hack-traditional
	tIndex.hackSetTraditionalDate(0, 2, 12, 31, 23, 59, 59, 999);
	// tIndex.hackSetTraditionalDate(0,  0,  1,  0,  0,  0, 0);
	tIndex.set_BeforeAfterStartBit(0);
	INDEX_OUT(2,tIndex);
	// cout << "2 nat,trad,scidb: " << tIndex.stringInNativeDate()	<< flush << ", " << tIndex.hackStringInTraditionalDate() << flush << ", 0x" << setw(16) << setfill('0') << hex << tIndex.scidbTemporalIndex() << dec << tIndex.scidbTemporalIndex() << endl << flush;
	// cout << "2 nat,trad,scidb: " << tIndex.stringInNativeDate()	<< flush << ", " << tIndex.hackStringInTraditionalDate() << flush << hex << tIndex.scidbTemporalIndex() << dec << endl << flush;
	// cout << "2 nat,trad: " << tIndex.stringInNativeDate()  << ", " << tIndex.hackStringInTraditionalDate() << endl << flush;
	// cout << "2 scidb idx: 0x" << hex << tIndex.scidbTemporalIndex() << dec << endl << flush;

	// TAG(200)
	tIndex.data.setZero();
	tIndex.set_BeforeAfterStartBit(0); INDEX_OUT(3,tIndex);

	tIndex.hackSetTraditionalDate(0, 1,  1,  1,  0,  0,  0, 0);	INDEX_OUT(3,tIndex);

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

	TAG(300)
	// tIndex.data.print();
	// TAG(400)
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

	// tIndex.data.setZero();
	int64_t one = 1;
	cout << "                  FEDCBA9876543210" << endl << flush;

	int64_t offsetBase = tIndex.data.get("millisecond")->getOffset();
	int64_t offsetTop = tIndex.data.get("year")->getOffset()+tIndex.data.get("year")->getWidth()-1;

	// for(int i=offsetBase; i < offsetBase+10; ++i) {
	// for(int i=offsetBase; i <= offsetTop; ++i) {
	for(int i=43; i <= 43; ++i) {
		int64_t iBit = one << i;
		int64_t iResolution = offsetTop-i;
		tIndex = TemporalIndex(iBit);
		tIndex.data.setValue("BeforeAfterStartBit",1);
		tIndex.data.setValue("resolution",iResolution);
		tIndex.data.setValue("type",2);
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

	TemporalIndex tIndex2;
	tIndex2.hackSetTraditionalDate(1, 0, 11, 31, 23, 59, 59, 999);
	cout << "            :   0x" << hex << setw(16) << setfill('0') << tIndex2.scidbTemporalIndex() << flush
	<< ", " << tIndex2.scidbTerminatorp() << flush
	<< ", " << tIndex2.stringInNativeDate() << flush
	<< ", " << tIndex2.hackStringInTraditionalDate() << flush
	<< dec << endl << endl << flush;

	int not_ok = tIndex2.eraTest();
	cout << flush;

	TemporalIndex tIndex3;
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

	TemporalIndex tIndex4;

	tIndex3.data.setZero();
	tIndex3.set_type(2);
	tIndex3.set_resolution(63);
	tIndex3.set_BeforeAfterStartBit(0);
	tIndex3.set_year(2);
	tIndex3.set_month(3);

	tIndex4.data.setZero();
	tIndex4.set_type(2);
	tIndex4.set_resolution(63);
	tIndex4.set_BeforeAfterStartBit(0);
	tIndex4.set_year(2);
	tIndex4.set_month(4);

	cout << endl << flush;
	cout << "0 tI3,tI4:        " << hex << &tIndex3 << ", " << &tIndex4 << dec << endl << flush;
	cout << "0 tI3,tI4 (data): " << hex << &(tIndex3.data) << ", " << &(tIndex4.data) << dec << endl << flush;
	cout << "0 tI3: " << tIndex3.stringInNativeDate()  << ", " << tIndex3.hackStringInTraditionalDate() << endl << flush;
	cout << "0 tI4: " << tIndex4.stringInNativeDate()  << ", " << tIndex4.hackStringInTraditionalDate() << endl << flush;
	cout << "0 tI3,tI4 s: " << hex << tIndex3.scidbTemporalIndex() << " " << (tIndex3.scidbTemporalIndex() < tIndex4.scidbTemporalIndex()) << " " << tIndex4.scidbTemporalIndex() << dec << endl << flush;

	// STARE index;

	FAIL();
}
