/*
 * TemporalIndex.cpp
 *
 *  Created on: Apr 10, 2017
 *      Author: mrilee
 */

#include "TemporalIndex.h"

namespace std {

BitField::BitField() {}
BitField::~BitField() {}

TemporalWordFormat::~TemporalWordFormat() {}

TemporalIndex::~TemporalIndex() {
	// TODO Auto-generated destructor stub
}


int firstBitDifferenceFromLeft(int64_t i0, int64_t i1) {
	int base = 64; int address = base;
	int64_t i01 = (i0 & i1 ) | ( ~i0 & ~i1 );
	uint64_t one = 1, test;
	do {
		--address;
		uint64_t testBit = one << address;
		test = ( testBit & i01 ) >> address;
	} while ( test == one && address >= 0 );
	return base - address - 1;
}

int64_t makeMask(int width) { return pow(2ll,width) -1; }




} /* namespace std */
