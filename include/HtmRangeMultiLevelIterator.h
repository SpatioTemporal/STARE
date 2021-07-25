/**
 * HtmRangeMultiLevel.h
 *
 * A heavily modified version of HtmRange.cpp to support left-justified spatial index values, intervals, from multiple levels.
 *
 * 2016 June - M. Rilee
 *
 */

#ifndef _HtmRangeMultiLevelMULTILEVELITERATOR_H_
#define _HtmRangeMultiLevelMULTILEVELITERATOR_H_

#include <HtmRangeMultiLevel.h>

using namespace HtmRangeMultiLevel_NameSpace;

class HtmRangeMultiLevelIterator {
 public:
  Key next();
  char *nextSymbolic(char *buffer); /* User responsible for managing it */
  bool hasNext();
  HtmRangeMultiLevelIterator(HtmRangeMultiLevel *ran) {
    range = ran;
    range->reset();
    range->getNext(&currange[0], &currange[1]);
    nextval = currange[0] - 1;
    getNext();
  }
 protected:
  HtmRangeMultiLevel *range;
  void getNext();

 private:
  Key nextval;
  Key currange[2];		/* Low and High */
  HtmRangeMultiLevelIterator() : range(0), nextval(-1) {}

};

#endif // _HtmRangeMultiLevelMULTILEVELITERATOR_H_

