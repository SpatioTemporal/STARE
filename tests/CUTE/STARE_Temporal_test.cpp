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

void STARE_Temporal_test() {

	TAG(100)
	TemporalIndex tIndex;
	TAG(200)
	ASSERT_EQUAL("year",  tIndex.data.get("year")->getName());
	TAG(300)
	ASSERT_EQUAL("minute",tIndex.data.get("minute")->getName());
	TAG(1000)
	tIndex.data.print();
	TAG(1100)
	tIndex.data.setValue("year",1);
	tIndex.data.setValue("month",2);
	tIndex.data.setValue("day",3);
	TAG(1190)
	cout << "day: " << tIndex.data.getValue("day") << endl << flush;
	TAG(1200)
	tIndex.data.print();
	TAG(1300)

	ASSERT_EQUAL( -0x0ll, tIndex.scidbTemporalIndex());

	// STARE index;

	FAIL();

}
