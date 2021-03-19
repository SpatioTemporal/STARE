/**
 * HtmRangeMultiLevel.h
 *
 * A heavily modified version of HtmRange.cpp to support left-justified spatial index values, intervals, from multiple levels.
 *
 * 2016 June - M. Rilee
 *
 */


#ifndef _HtmRangeMultiLevelMULTILEVEL_H_
#define _HtmRangeMultiLevelMULTILEVEL_H_

#include <iostream>

#include <KeyPair.h>
#include <SkipList.h>
#include <NameEncoding.h>
#include <BitShiftNameEncoding.h>
#include <EmbeddedLevelNameEncoding.h>

namespace HtmRangeMultiLevel_NameSpace {

enum InclusionType {
	InclOutside = 0,  /* */
	InclInside,       /* */
	InclLo,			/* number is on low end of an interval */
	InclHi,			/* number is on high end of an interval */
	InclAdjacentXXX
};

struct TInsideResult {
	InclusionType incl;
	int           level;
	Key           mid;
	Key           GH;
	Key           GL;
	Key           SH;
	Key           SL;
};

/**
 * Maintain a set of htm intervals.
 *
 * The legacy HtmRange was fixed to a specific search level, because its leaf-IDs are right-justified.
 *
 */
class LINKAGE HtmRangeMultiLevel {

public:

	// For the stored indices
	bool embeddedLevel = true; // Set to false for deprecated legacy behavior.

	// For the range data type
	static int HIGHS;
	static int LOWS;
	static int BOTH;
	int getNext(Key &lo, Key &hi);
	int getNext(Key *lo, Key *hi);
	KeyPair getNext();
	int getNext(KeyPair &kp);

	void setSymbolic(bool flag);

	void addRange(const Key lohi);
	void addRange(const Key lo, const Key hi);
	void addRange(HtmRangeMultiLevel *range);
	void mergeRange(const Key lo, const Key hi);
	void defrag();
	void defrag(Key gap);
	void CompressionPass(bool onepass = false);
	void purge();
	int isIn(Key key);
	int isIn(Key lo, Key hi);
	int isIn(HtmRangeMultiLevel & otherRange);
	Key bestgap(Key desiredSize);

	HtmRangeMultiLevel getSpan();
	void parse(std::string rangeString);

	bool equalp(HtmRangeMultiLevel *other);

	int stats(int desiredSize);
	int nranges();
	int nindexes_in_ranges();
  int verify();

	int getLosLength() {
		return my_los->getLength();
	}
	int getHisLength() {
		return my_his->getLength();
	}
	void reset();

	void print(std::ostream& os, bool symbolic = false); // FIX THIS, so caller does not set symbolic here....
	void print(int what, std::ostream& os, bool symbolic = false); // FIX THIS, so caller does not set symbolic here....

	int compare(const HtmRangeMultiLevel & other) const;

	/** Hold the encoding scheme for the symbolic representation.
	 */
	EmbeddedLevelNameEncoding *encoding;
	/** Change or set symbolic encoding.
	 */
	void setEncoding(EmbeddedLevelNameEncoding *encoding) { this->encoding = encoding; }
	/** Return the encoding for translations, etc.
	 */
	EmbeddedLevelNameEncoding *getEncoding() { return encoding; }

	// Moved here per ajmendez.
	// TODO MLR Why two skip lists for ranges?  Why not one skip list? Or an interval arithmetic package?
	SkipList *my_los;
	SkipList *my_his;
	bool symbolicOutput;

	HtmRangeMultiLevel();
	HtmRangeMultiLevel(EmbeddedLevelNameEncoding *encoding);
	~HtmRangeMultiLevel(){
		purge();
		delete encoding;
		delete my_los;
		delete my_his;
	};

	HtmRangeMultiLevel *HtmRangeMultiLevelAtLevelFromIntersection(HtmRangeMultiLevel *range2, int htmIdLevel=-1);
	HtmRangeMultiLevel *RangeFromIntersection(HtmRangeMultiLevel *range2, bool compress = true, int force_htmIdLevel=-1);

	int contains(Key a, Key b);
	// TODO KeyPair contains(Key a, Key b);

	friend LINKAGE ostream& operator<<(ostream& os, const HtmRangeMultiLevel& range);

	// protected:
	TInsideResult tinside(const Key mid) const;
	// const char buffer[256];
	//  bool symbolicOutput;
	// private:
	//  SkipList *my_los;
	//  SkipList *my_his;
};

#define SKIP_PROB (0.5)

}

#endif // _HtmRangeMultiLevelMULTILEVEL_H_
