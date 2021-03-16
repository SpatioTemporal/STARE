/*
 * SpatialRange.h
 *
 *  Created on: Sep 8, 2019
 *      Author: mrilee
 *
 *  Copyright (C) 2019 Rilee Systems Technologies LLC
 */

#ifndef INCLUDE_SPATIALRANGE_H_
#define INCLUDE_SPATIALRANGE_H_

#include "STARE.h"
#include "HstmRange.h"

/**
 * A wrapper for an HstmRange that knows about STARE to provide htm-range-like functions.
 *
 */
class SpatialRange {
public:
	SpatialRange();
	SpatialRange(STARE_SpatialIntervals intervals);
	SpatialRange(HstmRange *range) { this->range = range; }
	virtual ~SpatialRange();

	void addSpatialIntervals(STARE_SpatialIntervals intervals);
	void addSpatialRange(const SpatialRange& r);

	STARE_SpatialIntervals toSpatialIntervals();
	int getNextSpatialInterval(STARE_SpatialIntervals &interval);

	bool contains(STARE_ArrayIndexSpatialValue siv) {
	  // cout << "sr::c " << flush;
	  EmbeddedLevelNameEncoding leftJustified;
	  leftJustified.setIdFromSciDBLeftJustifiedFormat(siv);
	  return range->range->isIn(leftJustified.maskOffLevelBit());
	}

	bool intersects(STARE_ArrayIndexSpatialValue siv) {
		EmbeddedLevelNameEncoding leftJustified;
		leftJustified.setIdFromSciDBLeftJustifiedFormat(siv);
		Key lo = leftJustified.maskOffLevelBit();
		Key hi = lo;
		// KeyPair pr = KeyPair(lo,hi);
		int rstat = range->range->contains(lo,hi);
		bool intersectp = rstat != 0; // 0:no-intersection;-1:partial;1:full.
		return intersectp;
	}

	HstmRange *range;

	/////////// private: Maybe? ////////////

	// Maybe inherit these?
	// TODO Note the int in the following is a return code, not an index.
	int  getNext(KeyPair &kp) {
		int istat = range->getNext(kp);
//		cout << "<istat=" << istat << ">" << flush;
		return istat;
	};
	void reset() { range->reset(); } // range not null?
	void purge() { range->purge(); } // what if range null?
	void defrag() { range->range->defrag(); } // Defragment intervals without changing resolution
	void compress() { range->range->CompressionPass(); } // Defragment and coarsen resolution where possible

#define FMTX(x) " 0x" << setw(16) << hex << x << dec	
	void dump() {
	  EmbeddedLevelNameEncoding leftJustified;
	  KeyPair kp;
	  int k=0;
	  STARE_SpatialIntervals sivs;
	  this->reset();
	  while(this->getNextSpatialInterval(sivs)) {
	    /*
	    STARE_SpatialIntervals sivs = spatialIntervalFromHtmIDKeyPair(kp);
	    leftJustified.setId(BitShiftNameEkp.lo);
	    uint64 siv_lo = leftJustified.getSciDBLeftJustifiedFormat();
	    leftJustified.setId(kp.hi);
	    uint64 siv_hi = leftJustified.getSciDBLeftJustifiedFormat();
	    */

	    uint64 sivs_lo = sivs[0];
	    cout << k++ << " "
		 << FMTX(sivs_lo) << " ";
	    if( sivs.size() == 2 ) {
	      uint64 sivs_hi = sivs[1];
	      cout << FMTX(sivs_hi);
	    }
	    cout << endl << flush;
	    sivs.clear();
	  }

	  cout << endl << flush;
	  cout << "---expand---" << endl << flush;

	  k = 0;
	  this->reset();
	  while(this->getNextSpatialInterval(sivs)) {
	    /*
	    STARE_SpatialIntervals sivs = spatialIntervalFromHtmIDKeyPair(kp);
	    leftJustified.setId(BitShiftNameEkp.lo);
	    uint64 siv_lo = leftJustified.getSciDBLeftJustifiedFormat();
	    leftJustified.setId(kp.hi);
	    uint64 siv_hi = leftJustified.getSciDBLeftJustifiedFormat();
	    */

	    STARE_ArrayIndexSpatialValues vals = expandInterval(sivs);

	    for( int l=0; l < vals.size(); ++l ) {
	      cout << k++ << " " << FMTX(vals[l]) << endl;
	    }
	    vals.clear();
	    sivs.clear();
	  }
	}
};

SpatialRange* sr_intersect(const SpatialRange& a, const SpatialRange& b, bool compress = false);

inline SpatialRange* operator& ( const SpatialRange& a,  const SpatialRange& b) {
	return sr_intersect(a,b);
	// return new SpatialRange(new HstmRange(a.range->range->RangeFromIntersection(b.range->range))); // NOTE mlr Probably about the safest way to inst. SpatialRange.
}
void SpatialRange_test();


#endif /* INCLUDE_SPATIALRANGE_H_ */
