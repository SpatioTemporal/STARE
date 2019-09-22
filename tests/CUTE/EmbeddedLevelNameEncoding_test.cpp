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

	if(true) {
		STARE_ArrayIndexSpatialValue siv0      = 0x2324000000000005;
		EmbeddedLevelNameEncoding    lj; lj.setIdFromSciDBLeftJustifiedFormat(siv0);
		STARE_ArrayIndexSpatialValue siv0_term = lj.getSciDBTerminatorLeftJustifiedFormat();

		STARE_ArrayIndexSpatialValue not_siv1  = lj.increment(lj.getId(), lj.getLevel(), 1);
		EmbeddedLevelNameEncoding    lj1(not_siv1);
		STARE_ArrayIndexSpatialValue siv1 = lj1.getSciDBLeftJustifiedFormat();
		STARE_ArrayIndexSpatialValue sivt_term = 0x2327ffffffffffff;

		int level = lj.getLevel();
		uint64 one_mask_to_level, one_at_level;
		lj.SciDBincrement_LevelToMaskDelta(level, one_mask_to_level, one_at_level);
		STARE_ArrayIndexSpatialValue siv2      = siv0 + one_at_level;
		STARE_ArrayIndexSpatialValue siv2_term = siv2 | one_mask_to_level;

		STARE_ArrayIndexSpatialValue siv0_tchk = (siv0 | one_mask_to_level);

#define IDOUT(p,m,s)
// #define IDOUT(p,m,s) p << m << " " << setw(16) << setfill(' ') << hex << s << dec << endl << flush;
		IDOUT(cout,"siv0      ",siv0);
		IDOUT(cout,"siv0_term ",siv0_term);
		IDOUT(cout,"siv0_tchk ",siv0_tchk);
		IDOUT(cout,"not_siv1  ",not_siv1);
		IDOUT(cout,"siv1      ",siv1);
		IDOUT(cout,"sivt_term ",sivt_term);
		IDOUT(cout,"siv2      ",siv2);
		IDOUT(cout,"siv2_term ",siv2_term);
		// IDOUT(cout,"lj id ",lj.getId());
#undef IDOUT
		ASSERT_EQUAL( siv0       , 0x2324000000000005 );
		ASSERT_EQUAL( siv0_term  , 0x2325ffffffffffff );
		ASSERT_EQUAL( siv0_tchk  , 0x2325ffffffffffff );
		ASSERT_EQUAL( not_siv1   , 0xc64c000000000005 );
		ASSERT_EQUAL( siv1       , 0x2326000000000005 );
		ASSERT_EQUAL( sivt_term  , 0x2327ffffffffffff );
		ASSERT_EQUAL( siv2       , 0x2326000000000005 );
		ASSERT_EQUAL( siv2_term  , 0x2327ffffffffffff );
	}


}
