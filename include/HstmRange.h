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
 * A range class, i.e. skip list of spatial STARE intervals. *Almost* a container for sSTARE index values. The HtmRangeMultiLevel handles the extensions to the legacy code needed to handle the logic for left-justified indexes.
 *
 * HstmRange is used when you can handle everything in spatial STARE index space. Translations to lat-lon or SpatialVector are handled elsewhere.
 *
 * The ranges in HstmRange are almost identical to sSTARE intervals. They do not contain level information. This is based on the realization that the lowest numbered triangle (at a higher resolution) in a given triangle simply has zeros in the finer resolution bits.
 * Thus, all the triangles in that parent triangle have sSTARE index values greater than that particular one (with zeros below the resolution level). Just as there is an infinum there is a supremum below which are the children's index values.
 * Setting the bits rightward of the level to one gives us a "terminator," i.e. the index value of the "final" child triangle in a particular parent. The range's skiplist actually contains two skiplists, one containing low
 * values of intervals and another containing the corresponding high values. The low skip list is set to triangle index values with finer bits set to zero, while the high skip list is set to terminators.
 * There is a great deal of logic devoted to detecting overlap and combining intervals when constructing ranges.
 *
 * A proper spatial interval is a left-justified index value without its embedded level followed by a terminator.
 *
 *
 * Older notes: For keeping a skip-list of left-justified intervals. Special treatment is necessary, because embedding the level complicates the comparison of index values. HstmRange strips the level off index values (aka KeyPairs) before adding them to this->range.
 *
 *
 */
class HstmRange {
public:
	HstmRange();
	HstmRange(HtmRangeMultiLevel_NameSpace::HtmRangeMultiLevel *range);
	// TOO Deep copy? HstmRange(HstmRange range);
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

	/**
	 * For STARE we may need to coarsen position information to the resolution level since it points to a 7cm triangle inside a larger triangle.
	 *
	 */
	void     addRange(Key a, Key b);
	void     addRange(HstmRange* r);

};

} /* namespace std */

#endif /* INCLUDE_HSTMRANGE_H_ */
