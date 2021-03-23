/*
 * SpatialRange.cpp
 *
 *  Created on: Sep 8, 2019
 *      Author: mrilee
 *
 *  Copyright (C) 2019 Rilee Systems Technologies LLC
 */

#include "SpatialRange.h"

#define DIAGOUT(x)


SpatialRange::SpatialRange() {
	this->range = new HstmRange;
}

SpatialRange::SpatialRange(STARE_SpatialIntervals intervals) {
	this->range = new HstmRange;
	this->addSpatialIntervals(intervals);
}

SpatialRange::~SpatialRange() {
  if( this->range != NULL ) {
	  delete this->range; // TODO mlr. Um. Dangerous if range is from elsewhere? Maybe use some sort of smart pointer?
  }
}

void SpatialRange::addSpatialRange(const SpatialRange& range) {
	this->range->addRange(range.range);
}

/**
 * Construct a range object (spatial region) from a vector of intervals.
 *
 * TODO: Have a SpatialRange class instead of an HstmRange?
 */
// #define DIAG
#undef DIAG
void SpatialRange::addSpatialIntervals(STARE_SpatialIntervals intervals) {
	EmbeddedLevelNameEncoding leftJustified, lj_cleared;
#ifdef DIAG
  int count = 0;
#endif  
	for(auto i0=intervals.begin(); i0 != intervals.end(); ++i0) {
#ifdef DIAG
    cout << "sr::adsi count = " << ++count << endl << flush;
#endif
		leftJustified.setIdFromSciDBLeftJustifiedFormat(*i0);
		uint64 a         = leftJustified.getId(), b = a;
    int    a_level   = leftJustified.getLevel();
    lj_cleared       = leftJustified.clearDeeperThanLevel(a_level);
    uint64 a_cleared = lj_cleared.getId();
		auto i1 = (i0+1);
		// ORIG 2019-1212 MLR		if( i1 <= intervals.end() ) {
		if( i1 != intervals.end() ) {
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
		// old this->range->addRange(a,b);
		this->range->addRange(a_cleared,b);
#ifdef DIAG
    cout << "sr::addsi addRange done" << endl << flush;    
		cout << "sr::addsi nr = " << this->range->range->nranges() << endl << flush;
#endif
	}
}
#undef DIAG

// #define DIAG
int SpatialRange::getNextSpatialInterval(STARE_SpatialIntervals &interval) {
	KeyPair kp(-1,-2);
  Key lo_return, hi_return;
	int istat = this->getNext(kp);
#ifdef DIAG
  cout << "\nsr::gnsi start ---" << endl << flush;
  cout << "\nsr::gnsi istat = " << istat << ", kp = " << hex << kp.lo << ", " << kp.hi << dec << endl << flush;
#endif
	if( istat > 0 ) {
		EmbeddedLevelNameEncoding leftJustified;
		leftJustified.setId(kp.lo);
    lo_return = leftJustified.getSciDBLeftJustifiedFormat();
    DIAGOUT("pushing " << hex << lo_return << dec);
		interval.push_back(lo_return);
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
      cout << "---" << endl << flush;
#endif

			if( term_hi != term_lo ) {
        hi_return = term_hi;
        DIAGOUT("pushing " << hex << hi_return << dec);
				interval.push_back( hi_return );
      }
		}
	}
	return istat;
}
#undef DIAG

STARE_SpatialIntervals SpatialRange::toSpatialIntervals() {
	STARE_SpatialIntervals intervals;
	if(this->range) {
		this->range->reset();
		while( this->getNextSpatialInterval(intervals) > 0 );
	}
	return intervals;
}

/*
 * Odd. The following does not seem to work if we just return the SpatialRange itself. Some of the pointers seem to be either corrupted or eliminated.
 */
// SpatialRange sr_intersect(const SpatialRange&a, const SpatialRange& b, bool compress) {
SpatialRange* sr_intersect(const SpatialRange& a, const SpatialRange& b, bool compress) {
	// cout << endl << flush << "**sr_intersect**" << endl << flush;
	HstmRange *range = new HstmRange(a.range->range->RangeFromIntersection(b.range->range,compress)); // NOTE mlr Probably about the safest way to inst. SpatialRange.
// #define DIAG
#ifdef DIAG
	KeyPair kp; 
    range->reset(); 
    range->getNext(kp);
	cout << "sr_i range,r->r,nr " << range << " " << range->range << " " << range->range->nranges() << " : "
			<< setw(16) << setfill('0') << hex << kp.lo << " "
			<< setw(16) << setfill('0') << hex << kp.hi << " "
			<< dec
			<< endl << flush;
	EmbeddedLevelNameEncoding leftJustified;
	leftJustified.setId(kp.lo); 
    cout << "kp.lo lj " << setw(16) << setfill('0') << hex << leftJustified.getSciDBLeftJustifiedFormat() << endl << flush;
	leftJustified.setId(kp.hi); cout << "kp.hi lj " << setw(16) << setfill('0') << hex << leftJustified.getSciDBLeftJustifiedFormat() << endl << flush;
	cout << " r-r-my_los " << hex << range->range->my_los << endl << flush;
	cout << dec;
#endif
	SpatialRange *sr = new SpatialRange(range);

#ifdef DIAG
	cout << "sr-r          " << hex << sr->range << endl << flush;
	cout << "sr-r-r        " << hex << sr->range->range << endl << flush;
	cout << "sr-r-r-my_los " << hex << sr->range->range->my_los << endl << flush;
#endif
	return sr;

//	return SpatialRange(range);
}


