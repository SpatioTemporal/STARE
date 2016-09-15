
/*
 * HstmIndex.h
 *
 *  Created on: Apr 20, 2016
 *      Author: mrilee
 */

#ifndef INCLUDE_HSTMINDEX_H_
#define INCLUDE_HSTMINDEX_H_

// #include "HtmRangeIterator.h"
#include "HtmRangeMultiLevelIterator.h"

struct HstmIndex {
	// HtmRange *range;
	HtmRangeMultiLevel *range;
	HstmIndex() {
		range = new HtmRangeMultiLevel;
		range->purge();
	}
	HstmIndex(uint64_t id) {
		range = new HtmRangeMultiLevel;
		range->purge();
		range->addRange(id,id);
	}
	HstmIndex(uint64_t lo, uint64_t hi) {
		range = new HtmRangeMultiLevel;
		range->purge();
		range->addRange(lo,hi);
	}
	~HstmIndex() {
		delete range;
	}
};


#endif /* INCLUDE_HSTMINDEX_H_ */
