/*
 * EmbeddedLevelNameEncoding_test.cpp
 *
 *  Created on: Sep 9, 2019
 *      Author: mrilee
 *
 *  Copyright (C) 2019 Rilee Systems Technologies LLC
 */

#include "Test.h"

void EmbeddedLevelNameEncoding_test() {
	STARE index;
	STARE_ArrayIndexSpatialValue sid = index.ValueFromLatLonDegrees( 30, 30, 8 );

	EmbeddedLevelNameEncoding leftJustified;

	leftJustified.setIdFromSciDBLeftJustifiedFormat(sid);
#ifdef DIAG
	cout << "sid tp?       " << leftJustified.terminatorp() << endl << flush;
#endif

	STARE_ArrayIndexSpatialValue term_sid1 = leftJustified.getSciDBTerminatorLeftJustifiedFormat();
	leftJustified.setIdFromSciDBLeftJustifiedFormat(term_sid1);
#ifdef DIAG
	cout << "term_sid1 tp? " << leftJustified.terminatorp() << endl << flush;
#endif

	STARE_ArrayIndexSpatialValue term_sid2 = leftJustified.getSciDBTerminatorLeftJustifiedFormat();

#ifdef DIAG
	cout << "test: id,mask,level  "
			<< setw(16) << setfill('0') << hex
			<< leftJustified.getId()
			<< " " << dec << leftJustified.levelMask
			<< " " << (leftJustified.getId() & leftJustified.levelMask)
			<< endl << flush;
	cout << "testS: id,mask,level "
			<< setw(16) << setfill('0') << hex
			<< leftJustified.getId()
			<< " " << dec << leftJustified.levelMaskSciDB
			<< " " << (leftJustified.getId() & leftJustified.levelMaskSciDB)
			<< endl << flush;
	cout << endl << flush;

#define IDOUT(p,m,s) p << m << " " << setw(16) << setfill(' ') << hex << s << dec << endl << flush;
	IDOUT(cout,"sid       ", sid);
	IDOUT(cout,"term_sid1 ", term_sid1);
	IDOUT(cout,"term_sid1*", leftJustified.getId());
	IDOUT(cout,"term_sid1*", leftJustified.getSciDBLeftJustifiedFormat());
	IDOUT(cout,"term_sid1t", leftJustified.getSciDBTerminatorLeftJustifiedFormat());
	IDOUT(cout,"term_sid2 ", term_sid2);
#undef IDOUT
#endif

	ASSERT(leftJustified.terminatorp(term_sid1));
	ASSERT_EQUALM("getSciDBTerminatorLeftJustifiedFormat idempotent?",term_sid1,term_sid2);
}
