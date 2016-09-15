#ifndef _HTMHANGE_H_
#define _HTMHANGE_H_

#include <iostream>

#include <KeyPair.h>
#include <SkipList.h>
#include <NameEncoding.h>
#include <BitShiftNameEncoding.h>

// class ostream;

KeyPair HTMRangeAtLevelFromHTMRange(int htmIdLevel, Key lo, Key hi);

namespace HtmRange_NameSpace {

enum InclusionType {
  InclOutside = 0,
  InclInside,
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
 */
class LINKAGE HtmRange {

 public:
  static int HIGHS;
  static int LOWS;
  static int BOTH;
  int getNext(Key &lo, Key &hi);
  int getNext(Key *lo, Key *hi);
  KeyPair getNext();
  int getNext(KeyPair &kp);

  void setSymbolic(bool flag);

  void addRange(const Key lo, const Key hi);
  void addRange(HtmRange *range);
  void mergeRange(const Key lo, const Key hi);

  HtmRange getSpan();

  void defrag();
  void defrag(Key gap);
  void purge();
  int isIn(Key key);
  int isIn(Key lo, Key hi);
  int isIn(HtmRange & otherRange);
  Key bestgap(Key desiredSize);
  int stats(int desiredSize);
  int nranges();

  KeyPair intersection(KeyPair kp);
//  bool    intersection(HtmRange otherRange, HtmRange &intersection);

  int getLosLength() {
	  return my_los->getLength();
  }
  int getHisLength() {
	  return my_his->getLength();
  }
  void reset();

  void print(int what, std::ostream& os, bool symbolic = false); // FIX THIS, so caller does not set symbolic here....

  int compare(const HtmRange & other) const;

  /** Hold the encoding scheme for the symbolic representation.
   * The default is BitShiftNameEncoding.
   */
  NameEncoding *encoding;
  /** Change or set symbolic encoding.
   */
  void setEncoding(NameEncoding *encoding) { this->encoding = encoding; }
  /** Return the encoding for translations, etc.
   */
  NameEncoding *getEncoding() { return encoding; }

  // Moved here per ajmendez.
  // TODO MLR Why two skip lists for ranges?  Why not one skip list? Or an interval arithmetic package?
  SkipList *my_los;
  SkipList *my_his;
  bool symbolicOutput;

  HtmRange();
  HtmRange(NameEncoding *encoding);
  ~HtmRange(){
    purge();
    delete encoding;
    delete my_los;
    delete my_his;
  };
  
  HtmRange *HTMRangeAtLevelFromIntersection(HtmRange *range2, int htmIdLevel=-1);
  int contains(Key a, Key b);

  friend LINKAGE ostream& operator<<(ostream& os, const HtmRange& range);

  void parse(std::string rangeString);

 protected:
  TInsideResult tinside(const Key mid) const;
  // const char buffer[256];
//  bool symbolicOutput;
// private:
//  SkipList *my_los;
//  SkipList *my_his;
};

#define SKIP_PROB (0.5)

}

#endif
