
/*
 * HstmIndex.h
 *
 *  Created on: Apr 20, 2016
 *      Author: mrilee
 */

#ifndef INCLUDE_HSTMINDEX_H_
#define INCLUDE_HSTMINDEX_H_

#include "HtmRangeIterator.h"

struct HstmIndex {
	HtmRange *range;
	HstmIndex() {
		range = new HtmRange;
		range->purge();
	}
	HstmIndex(uint64_t id) {
		range = new HtmRange;
		range->purge();
		range->addRange(id,id);
	}
	HstmIndex(uint64_t lo, uint64_t hi) {
		range = new HtmRange;
		range->purge();
		range->addRange(lo,hi);
	}
	~HstmIndex() {
		delete range;
	}
};


#endif /* INCLUDE_HSTMINDEX_H_ */
