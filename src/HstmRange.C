/*
 * HstmRange.C
 *
 *  Created on: Jun 10, 2016
 *      Author: mrilee
 */

#include "HstmRange.h"

namespace std {

HstmRange::HstmRange() {
	// TODO Auto-generated constructor stub
	range = new HtmRangeMultiLevel_NameSpace::HtmRangeMultiLevel();
}

HstmRange::~HstmRange() {
	// TODO Auto-generated destructor stub
	delete range;
}



void HstmRange::addRange(Key a_, Key b_) {
	Key a = leftJustifiedEncoding.maskOffLevelBit(a_);
	int aLevel = leftJustifiedEncoding.levelById(a_);
	Key b = leftJustifiedEncoding.maskOffLevelAndLevelBit(b_);
	int bLevel = leftJustifiedEncoding.levelById(b_);

//	cout << dec << 900 << " : " << hex << "0x" << a_ << " 0x" << b_ << dec << endl << flush;
//	cout << dec << 901 << " : " << hex << "0x" << a << " 0x" << b << dec << endl << flush;
//	cout << dec << 902 << " : aLevel = " << aLevel << endl << flush;

	if( aLevel != bLevel ) {
		cout << "HstmRange::addRange::ERROR::KeyLevelMismatch "
				<< aLevel << " != " << bLevel
				<< endl << flush;
	}

    // Put a into HtmRange without change
	// Go to the last triangle in b and put that into HtmRange
	// Question: Level? hi can keep the level.

//	uint64 one_mask_to_level = 3 << ( (leftJustifiedEncoding.topBitPosition-3) - 2*aLevel );
//	b = b + one_mask_to_level;
//	range.addRange(a,b);

/*
	uint64 one_mask_to_level = 0;
	for(uint64 shift = 2;
			shift <= ( (leftJustifiedEncoding.topBitPosition-3) - 2*aLevel );
			shift +=2 ) {
		one_mask_to_level = one_mask_to_level << 2;
		one_mask_to_level += 3;
	}

	b = b + one_mask_to_level; // Fill up everything with 3s.
	Key bSave = b;
*/

	leftJustifiedEncoding.setId(b_);
	b = leftJustifiedEncoding.getIdTerminator_NoDepthBit();

//	cout << dec << 1000 << " : " << hex << "0x" << a_ << " 0x" << bSave << endl << flush;

//	cout << dec << 1001 << " : " << hex << "0x" << a << " 0x" << b << endl << flush;

	// How about...
	// TODO Need to adapt HtmRange (HtmRangeLeftJustified), so that it knows about the
	// TODO different formats of the lo and hi intervals.
	// TODO los will all be 0x01ID0........0LEVEL and his will all be 0x01ID3333...3333.
	// TODO Q: Need to check that only los (his) go into my_los (my_his).
	// TODO A: Seems that only los go into my_los. Have a problem with a new los level taking over
	// TODO the level when there's an overlap in ranges. Need to split the range instead of merge.
	// TODO I.e. merge while retaining level.
	// TODO Current implementation of SkipList depends on sign. Argh.
	range->addRange(a,b);
//	cout << dec << 1001 << " : nranges = " << range.nranges() << endl << flush;
}

// TODO Add iterators, get/set, parallel HtmRange, Translate out of the funny lo-hi format we're using.

} /* namespace std */
