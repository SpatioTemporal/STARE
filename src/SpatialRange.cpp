/*
 * SpatialRange.cpp
 *
 *  Created on: Sep 8, 2019
 *      Author: mrilee
 *
 *  Copyright (C) 2019 Rilee Systems Technologies LLC
 */

#include "SpatialRange.h"

namespace std {

SpatialRange::SpatialRange() {
	this->range = new HstmRange;
}

SpatialRange::SpatialRange(STARE_SpatialIntervals intervals) {
	this->range = new HstmRange;
	this->addSpatialIntervals(intervals);
}

SpatialRange::~SpatialRange() {
	delete this->range; // TODO mlr. Um. Dangerous if range is from elsewhere? Maybe use some sort of smart pointer?
}

void SpatialRange::addSpatialRange(SpatialRange range) {
	this->range->addRange(range.range);
}

/**
 * Construct a range object (spatial region) from a vector of intervals.
 *
 * TODO: Have a SpatialRange class instead of an HstmRange?
 */
// #define DIAG
void SpatialRange::addSpatialIntervals(STARE_SpatialIntervals intervals) {
	EmbeddedLevelNameEncoding leftJustified;
	for(auto i0=intervals.begin(); i0 != intervals.end(); ++i0) {
		leftJustified.setIdFromSciDBLeftJustifiedFormat(*i0);
		uint64 a = leftJustified.getId(), b = a;
		auto i1 = (i0+1);
		if( i1 <= intervals.end() ) {
			if(terminatorp(*i1)) {
				leftJustified.setIdFromSciDBLeftJustifiedFormat(*i1);
				b = leftJustified.getId();
				++i0; // Skip to next
			}
		}

#ifdef DIAG
		cout << "sr::addsi "
				<< setw(16) << setfill('0') << hex << a << " "
				<< setw(20) << dec << a << " "
				<< setw(16) << setfill('0') << hex << b << " "
				<< setw(20) << dec << b << " "
				<< endl << flush;
#endif
		this->range->addRange(a,b);
#ifdef DIAG
		cout << "sr::addsi nr = " << this->range->range->nranges() << endl << flush;
#endif
	}
}


int SpatialRange::getNextSpatialInterval(STARE_SpatialIntervals &interval) {
	KeyPair kp(-1,-2);
	int istat = this->getNext(kp);
#ifdef DIAG
	cout << "sr::gnsi istat = " << istat << ", kp = " << kp.lo << ", " << kp.hi << endl << flush;
#endif
	if( istat > 0 ) {
		EmbeddedLevelNameEncoding leftJustified;
		leftJustified.setId(kp.lo);
		interval.push_back(leftJustified.getSciDBLeftJustifiedFormat());
		if( kp.lo != kp.hi ) {
			STARE_ArrayIndexSpatialValue term_lo = leftJustified.getSciDBTerminatorLeftJustifiedFormat();
			leftJustified.setId(kp.hi); // Note: hi should be a terminator already.
			STARE_ArrayIndexSpatialValue term_hi = leftJustified.getSciDBLeftJustifiedFormat();

#ifdef DIAG
			cout << "lo,term_lo,term_hi: "
					<< setw(16) << setfill('0') << hex
					<< kp.lo << ","
					<< setw(16) << setfill('0') << hex
					<< term_lo << ","
					<< setw(16) << setfill('0') << hex
					<< term_hi << endl << flush << dec;
#endif

			if( term_hi != term_lo ) {
				interval.push_back( term_hi );
			}
		}
	}
	return istat;
}
#undef DIAG

STARE_SpatialIntervals SpatialRange::toSpatialIntervals() {
	STARE_SpatialIntervals intervals;
	this->range->reset();
	while( this->getNextSpatialInterval(intervals) > 0 );
	return intervals;
}


} /* namespace std */
