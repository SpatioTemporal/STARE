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
	void compress() { range->range->CompressionPass(); }
};

SpatialRange* sr_intersect(const SpatialRange& a, const SpatialRange& b, bool compress = false);

inline SpatialRange* operator& ( const SpatialRange& a,  const SpatialRange& b) {
	return sr_intersect(a,b);
	// return new SpatialRange(new HstmRange(a.range->range->RangeFromIntersection(b.range->range))); // NOTE mlr Probably about the safest way to inst. SpatialRange.
}
void SpatialRange_test();


#endif /* INCLUDE_SPATIALRANGE_H_ */
