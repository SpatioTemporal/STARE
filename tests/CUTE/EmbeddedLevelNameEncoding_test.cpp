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

	if(true) {
	  for(int iDelta = 0; iDelta < 4; ++iDelta)
	  {

	    uint64 test_level = 27; // Bug at this level
	    // uint64 test_level = 26;
	    uint64 itri = iDelta << ( (6-1) + (27-test_level) ); // Get past the 5 level bits. -1 because we start on the 0th bit.
	    STARE_ArrayIndexSpatialValue siv0      = 0x2324000000000000+itri+test_level;
	    EmbeddedLevelNameEncoding    lj; lj.setIdFromSciDBLeftJustifiedFormat(siv0);
	    STARE_ArrayIndexSpatialValue siv0_term = lj.getSciDBTerminatorLeftJustifiedFormat();

	    STARE_ArrayIndexSpatialValue not_siv1  = lj.increment(lj.getId(), lj.getLevel(), 1);
	    EmbeddedLevelNameEncoding    lj1(not_siv1);
	    STARE_ArrayIndexSpatialValue siv1 = lj1.getSciDBLeftJustifiedFormat();
	    // STARE_ArrayIndexSpatialValue sivt_term = 0x232400000000001f;
	    STARE_ArrayIndexSpatialValue sivt_term = lj1.getSciDBTerminatorLeftJustifiedFormat();

	    // .. 7...F
	    // 11 11111
	    // ^^       level 27
	    //    ^^^^^ resolution bits

	    // .. 2...0
	    // 01 00000 one at level 27

	    // .. 1...F
	    // 00 11111 mask at level 27

	    //////////////////////

	    // itri = 0
	    // .. 1...B
	    // 00 11011
	    // .. 3...F  term
	    // 01 11111
	    // .. 1...F  tchk (one mask to level)
	    // 00 11111

	    // itri = 1
	    // .. 3...B
	    // 01 11011
	    // .. 3...F  term
	    // 01 11111

	    // 00 11011 // itri = 0 // .. 1...B
	    // 01 11011 // itri = 1 // .. 3...B

	    ///////////////////////////////////

	    // 00 11011 // itri = 0 // .. 1...B
	    // 00 11111 // TERM     // .. 1...F

	    // 01 11011 // itri = 1 // .. 3...B
	    // 01 11111 // TERM     // .. 3...F

	    // 10 11011 // itri = 2 // .. 5...B
	    // 10 11111 // TERM     // .. 5...F

	    // 11 11011 // itri = 3 // .. 7...B
	    // 11 11111 // TERM     // .. 7...F

	    ///////////////////////////////////

	    int level = lj.getLevel();
	    uint64 one_mask_to_level, one_at_level;
	    lj.SciDBincrement_LevelToMaskDelta(level, one_mask_to_level, one_at_level);
	    STARE_ArrayIndexSpatialValue siv2      = siv0 + one_at_level;
	    STARE_ArrayIndexSpatialValue siv2_term = siv2 | one_mask_to_level;

	    STARE_ArrayIndexSpatialValue siv0_tchk = (siv0 | one_mask_to_level);

	    lj.setIdFromSciDBLeftJustifiedFormat(siv0);

#define IDOUT(p,m,s)
	    // #define IDOUT(p,m,s) p << m << " " << setw(16) << setfill(' ') << hex << s << dec << endl << flush;
	    IDOUT(cout,"itri      ",itri);
	    IDOUT(cout,"one@level ",one_at_level);
	    IDOUT(cout,"masklevel ",one_mask_to_level);
	    IDOUT(cout,"siv0      ",siv0);
	    IDOUT(cout,"siv0_term ",siv0_term);
	    IDOUT(cout,"siv0_tchk ",siv0_tchk);
	    IDOUT(cout,"lj id     ",lj.getId_NoLevelBit());
	    IDOUT(cout,"lj term   ",lj.getIdTerminator_NoDepthBit());
	    IDOUT(cout,"not_siv1  ",not_siv1);
	    IDOUT(cout,"siv1      ",siv1);
	    IDOUT(cout,"sivt_term ",sivt_term);
	    IDOUT(cout,"siv2      ",siv2);
	    IDOUT(cout,"siv2_term ",siv2_term);
#ifdef DIAG
	    cout << endl << flush;
#endif
	    // IDOUT(cout,"lj id ",lj.getId());
#undef IDOUT

	    ASSERT_EQUAL( siv0_term  , siv0_tchk  );

	    /*
	      ASSERT_EQUAL( siv0       , 0x2324000000000005 );
	      ASSERT_EQUAL( siv0_term  , 0x2325ffffffffffff );
	      ASSERT_EQUAL( siv0_tchk  , 0x2325ffffffffffff );
	      ASSERT_EQUAL( not_siv1   , 0xc64c000000000005 );
	      ASSERT_EQUAL( siv1       , 0x2326000000000005 );
	      ASSERT_EQUAL( sivt_term  , 0x2327ffffffffffff );
	      ASSERT_EQUAL( siv2       , 0x2326000000000005 );
	      ASSERT_EQUAL( siv2_term  , 0x2327ffffffffffff );
	    */
	  }
	}
}
