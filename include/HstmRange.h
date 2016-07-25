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

class HstmRange {
public:
	HstmRange();
	virtual ~HstmRange();

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

};

} /* namespace std */

#endif /* INCLUDE_HSTMRANGE_H_ */
