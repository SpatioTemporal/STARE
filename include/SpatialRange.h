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
	void addSpatialRange(SpatialRange r);

	STARE_SpatialIntervals toSpatialIntervals();
	int getNextSpatialInterval(STARE_SpatialIntervals &interval);

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

};

inline SpatialRange operator& ( const SpatialRange& a,  const SpatialRange& b) {
	return SpatialRange(new HstmRange(a.range->range->HtmRangeMultiLevelAtLevelFromIntersection(b.range->range))); // NOTE mlr Probably about the safest way to inst. SpatialRange.
}

void SpatialRange_test();


#endif /* INCLUDE_SPATIALRANGE_H_ */
