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

	// TAG(100)
	tIndex.data.setZero();
	// TAG(200)
	ASSERT_EQUAL( -0x0ll, tIndex.scidbTemporalIndex());
	// TAG(300)
	// tIndex.data.print();
	// TAG(400)
	ASSERT_EQUAL( 0, tIndex.get_millisecond());
	ASSERT_EQUAL( 0, tIndex.get_day());
	ASSERT_EQUAL( 0, tIndex.get_year());
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
	for(int i=offsetBase; i <= offsetTop; ++i) {
		int64_t iBit = one << i;
		int64_t iResolution = offsetTop-i;
		tIndex = TemporalIndex(iBit);
		tIndex.data.setValue("BeforeAfterStartBit",1);
		tIndex.data.setValue("resolution",iResolution);
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

	// STARE index;

	FAIL();
}
