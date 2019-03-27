/*
 * HstmRange.h
 *
 *  Created on: Jun 10, 2016
 *      Author: mrilee
 */


#include <HtmRangeMultiLevel.h>
#include <EmbeddedLevelNameEncoding.h>
#include <map>

#ifndef INCLUDE_HSTMRANGE_H_
#define INCLUDE_HSTMRANGE_H_

namespace std {

/**
 * for keeping a skip-list of left-justified intervals.
 *
 * Special treatment is necessary, because embedding the level complicates the comparison of index values.
 *
 * HstmRange strips the level off a index values (aka KeyPairs) before adding them to this->range.
 *
 */
class HstmRange {
public:
	HstmRange();
	virtual ~HstmRange();

	// TODO Note the int in the following is a return code, not an index.
	int  getNext(KeyPair &kp) {
		int istat = range->getNext(kp);
//		cout << "<istat=" << istat << ">" << flush;
		return istat;
	};
	void reset() { range->reset(); } // range not null?
	void purge() { range->purge(); } // what if range null?

	HtmRangeMultiLevel_NameSpace::HtmRangeMultiLevel *range;
	EmbeddedLevelNameEncoding leftJustifiedEncoding;

	void     addRange(Key a, Key b);
	void     addRange(HstmRange* r);

};

} /* namespace std */

#endif /* INCLUDE_HSTMRANGE_H_ */
