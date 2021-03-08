/**
 * HtmRangeMultiLevel.cpp
 *
 * A heavily modified version of HtmRange.cpp to support left-justified spatial index values, intervals, from multiple levels.
 *
 * 2016 June - M. Rilee
 *
 */


#include <iostream> // cout
#include <iomanip>  // setw()
#include <Htmio.h> // various *RepresentationString elements
#include <HtmRangeMultiLevel.h>
#include <SpatialIndex.h> // levelOfId // Do wwe really need this? // TODO If levelOfId not needed, remove.

#ifdef _WIN32
#include <stdio.h>
#include <string.h>
#endif

#define INSIDE     1
#define OUTSIDE   -1
#define INTERSECT  0
#define GAP_HISTO_SIZE 10000

#define DIAGOUTFLUSH(x)
// #define DIAGOUTFLUSH(x) {cout << x << flush;}

#define DIAGOUT(x)
// #define DIAGOUT(x) {cout << x << endl << flush;}


using namespace std;
using namespace HtmRangeMultiLevel_NameSpace;

uint64 HRML_levelOfId(uint64 id,bool embeddedLevel,uint64 levelMask) {
	if( embeddedLevel ) {
		return id & levelMask;
	} else {
		// legacy behavior
		return levelOfId(id);
	}
}

/**
 * Translate an HtmRangeMultiLevel to one at a greater level.  If the desired level is
 * less that the level implicit in the input range (lo & hi), then just return
 * an HtmRangeMultiLevel constructed from the input range without modification.
 * Note: Currently hardcoded for bit-shifted encoding
 * Note: One sad part is that when we're working with left-justified outside, we probably convert to right-justified and then we do this levelOfId stuff below.
 * TODO Currently hardcoded for bit-shifted encoding, aka right-justified. Maybe extend API for left-justified? Might be more efficient.
 * @param htmIdLevel
 * @param lo the low end of the range
 * @param hi the high end of the range
 * @return levelAdaptedRange an HtmRangeMultiLevel
 */
KeyPair HtmRangeMultiLevelAtLevelFromHtmRangeMultiLevel(int htmIdLevel, Key lo, Key hi) {
	// htmIdLevel is used to set maxlevel in the index. aka olevel.
	int levelLo = HRML_levelOfId(lo,false,63);
	if(levelLo<htmIdLevel) {
		lo = lo << (2*(htmIdLevel-levelLo));
	}
	int levelHi = HRML_levelOfId(hi,false,63);
	if(levelHi<htmIdLevel) {
		for(int shift=0; shift < (htmIdLevel-levelHi); shift++) {
			hi = hi << 2;
			hi += 3; /// Increment hi by 3 to catch all of the faces at that level.
		}
	}
	KeyPair levelAdaptedRange;
	levelAdaptedRange.lo = lo;
	levelAdaptedRange.hi = hi;
	return levelAdaptedRange;
}

/**
 *
 * @param htmIdLevel
 * @param range1
 * @param range2
 * @return
 */
HtmRangeMultiLevel *HtmRangeMultiLevel::HtmRangeMultiLevelAtLevelFromIntersection(HtmRangeMultiLevel *range2, int htmIdLevel){
	// cout << "Comparing..." << endl << flush;
	HtmRangeMultiLevel *range1 = this; // Rename to use existing code. TODO rename to this.
	if((!range1)||(!range2)) return 0;
	if((range1->nranges()<=0)||(range2->nranges()<=0)) return 0;
	HtmRangeMultiLevel *resultRange = new HtmRangeMultiLevel();
	resultRange->purge();
	Key lo1,hi1,lo2,hi2;
	range1->reset();
	uint64 indexp1 = range1->getNext(lo1,hi1); // TODO indexp1 is actually a return code, with 0 == false -- "none found"
	if (!indexp1) return 0;

	if(htmIdLevel<0) {
		htmIdLevel = HRML_levelOfId(lo1,false,63);
	}

	//	cout << "indexp1: " << indexp1 << endl << flush;
	//	cout << "l,lo,hi1: " << htmIdLevel << " " << lo1 << " " << hi1 << endl << flush;
#ifdef DIAG
	cout << "a" << flush;
#endif
	do {
#ifdef DIAG
		cout << "b" << endl << flush;
#endif
		KeyPair testRange1 = HtmRangeMultiLevelAtLevelFromHtmRangeMultiLevel(htmIdLevel,lo1,hi1);
		range2->reset();
		uint64 indexp2 = range2->getNext(lo2,hi2);
		if(!indexp2) return 0;
		//		cout << "indexp2: " << indexp2 << endl << flush;
		//		cout << "l,lo,hi2: " << htmIdLevel << " " << lo2 << " " << hi2 << endl << flush;
		bool intersects = false;
		do {
			//			cout << "c" << endl << flush;
			KeyPair testRange2 = HtmRangeMultiLevelAtLevelFromHtmRangeMultiLevel(htmIdLevel,lo2,hi2);
			intersects = testRange2.lo <= testRange1.hi
					&& testRange2.hi >= testRange1.lo;
			//						cout << "lh1,lh2: "
			//								<< lo1 << " " << hi1 << ", "
			//								<< lo2 << " " << hi2 << ", "
			//								<< intersects << flush;
			if(intersects){
				Key lo_ = max(testRange1.lo,testRange2.lo);
				Key hi_ = min(testRange1.hi,testRange2.hi);
				resultRange->addRange(lo_,hi_);
				//								cout << ", added "
				//										<< lo_ << " "
				//										<< hi_ << flush;
			}
			//						cout << "." << endl << flush;
		} while (range2->getNext(lo2,hi2));
	} while (range1->getNext(lo1,hi1));
	//	cout << "d" << flush;
	if(resultRange->nranges()>0)resultRange->defrag();
	//	cout << "e" << flush;
	return resultRange;
}

KeyPair HRML_AtLevelFromMultiLevel(uint64 htmIdLevel, Key lo, Key hi, uint64 levelMask) {
	uint64 levelLo = lo & levelMask;
	const uint64 one = 1;
	if( levelLo < htmIdLevel) {
		lo = (lo & ~levelMask) | htmIdLevel;
		// Ignore weird cases where level(lo) != level(hi)
		// Make hi into a terminator for htmIdLevel.
		hi = hi | ( (one << (6+54-2*htmIdLevel)) - 1 );
	}
	KeyPair levelAdaptedRange;
	levelAdaptedRange.lo = lo;
	levelAdaptedRange.hi = hi;
	return levelAdaptedRange;
}

/**
 * Find intersection of two ranges and return as a range.
 *
 * TODO Replace double-nested-loop with SkipLists search or find functionality.
 *
 */
HtmRangeMultiLevel *HtmRangeMultiLevel::RangeFromIntersection(
		HtmRangeMultiLevel *range2, bool compress, int force_htmIdLevel ) {
	HtmRangeMultiLevel *range1 = this; // Just an alias
	if((!range1)||(!range2)) return 0;
	if((range1->nranges()<=0)||(range2->nranges()<=0)) return 0;
	Key lo1,hi1,lo2,hi2;
	range1->reset();
	uint64 indexp1 = range1->getNext(lo1,hi1);
	if(!indexp1) return 0;
	if(force_htmIdLevel<0) {
		force_htmIdLevel = lo1 & this->encoding->levelMask; // TODO Establish 31 or 63?
	}
	HtmRangeMultiLevel *resultRange = new HtmRangeMultiLevel();	resultRange->purge();

	do {
		KeyPair testRange1 = HRML_AtLevelFromMultiLevel(force_htmIdLevel,lo1,hi1,this->encoding->levelMask);
		range2->reset(); // Sigh. Reset and loop from the beginning. TODO Avoid restarting loop. There must be a faster way.
		/* Try to skip past by using SkipList functions. */
// #if 0
		// TODO Consider adding unit test for this.
		// Move the pointers in my_los and my_his, skipping to where we should check.
		// The following work by side effect.
		Key   loKey = range2->my_los->findMAX(testRange1.lo);
		Value hiKey = range2->my_los->search(loKey,true);
		/* Value vhi = */ range2->my_his->search(hiKey,true);
// #endif
		/**/
		uint64 indexp2 = range2->getNext(lo2,hi2); // TODO Implement a find or search for inserting.
		bool intersects = false, past_chance;
#define FMTX(x) setw(16) << setfill('0') << hex << x << dec
		// Search forward until we find an intersection. Once an intersection is found,
		// figure out what the intersection is and add it to the result range.
		// int kount=0;
		if(indexp2)
		do {
			// ++kount;
			KeyPair testRange2 = HRML_AtLevelFromMultiLevel(force_htmIdLevel,lo2,hi2,this->encoding->levelMask);
			intersects = testRange2.lo <= testRange1.hi
					&& testRange2.hi >= testRange1.lo;
// #define DIAG
#ifdef DIAG
			cout << "lh1,lh2: "
					<< FMTX(lo1) << " " << FMTX(hi1) << ", "
					<< FMTX(lo2) << " " << FMTX(hi2) << ", "
					<< intersects << flush;
#endif
			if(intersects){
				Key lo_ = max(testRange1.lo,testRange2.lo);
				Key hi_ = min(testRange1.hi,testRange2.hi);
				resultRange->addRange(lo_,hi_);
#ifdef DIAG
				cout << ", added "
						<< FMTX(lo_) << " "
						<< FMTX(hi_) << flush;
#endif
			}
#ifdef DIAG
			cout << "." << endl << flush;
#endif
#undef DIAG
			past_chance = (uint64) testRange2.lo > (uint64) testRange1.hi;
		} while (range2->getNext(lo2,hi2) && !past_chance);
		// cout << "kount = " << kount << endl << flush;
	} while (range1->getNext(lo1,hi1)); // TODO Can we replace getNext with some sort of find or search.
#undef FMTX
	// cout << "d" << flush;
	// cout << "d nr " << resultRange->nranges() << endl << flush;
	// cout << "d rr " << hex << resultRange << dec << endl << flush;
	if(resultRange->nranges()>0) {
		if(compress) {
			resultRange->CompressionPass();
		}
		resultRange->defrag();
	}
	// cout << "e" << flush;
	return resultRange;
}


// Note the default use of EmbeddedLevelNameEncoding.
// HtmRangeMultiLevel::HtmRangeMultiLevel() : HtmRangeMultiLevel(new EmbeddedLevelNameEncoding()) {}
HtmRangeMultiLevel::HtmRangeMultiLevel() {
	encoding = new EmbeddedLevelNameEncoding();
	my_los = new SkipList(SKIP_PROB);
	// cout << "hrml my_los " << hex << my_los << dec << endl << flush;
	my_his = new SkipList(SKIP_PROB);
	symbolicOutput = false;
}

HtmRangeMultiLevel::HtmRangeMultiLevel(EmbeddedLevelNameEncoding *encoding) {
	this->encoding = encoding;
	my_los = new SkipList(SKIP_PROB);
	my_his = new SkipList(SKIP_PROB);
	symbolicOutput = false;
}

/**
 * Set numeric or symbolic (string) output for true/false respectively.
 * @param flag
 */
void HtmRangeMultiLevel::setSymbolic(bool flag)
{
	symbolicOutput = flag;
}

/**
 * Compare the number of intervals stored.
 * Question: In what context is this comparison meaningful?
 * @param other HtmRangeMultiLevel
 * @return true if this and the other store the same number of intervals.
 */
int HtmRangeMultiLevel::compare(const HtmRangeMultiLevel & other) const
{
	int rstat = 0;
	if (my_los->getLength() == other.my_los->getLength()){
		rstat = 1;
	}
	return rstat;
}

int HtmRangeMultiLevel::isIn(Key a, Key b)
{

//	Key a = this->encoding->maskOffLevel(a_);
//	Key b = this->encoding->maskOffLevel(b_);

	/* The following is wrong.
	if( this->encoding->getEncodingType() == "EmbeddedLevelNameEncoding" ) {
		this->encoding->setId(a_);
		a = ((EmbeddedLevelNameEncoding*)(this->encoding))->getId_NoEmbeddedLevel();
		this->encoding->setId(b_);
		b = ((EmbeddedLevelNameEncoding*)(this->encoding))->getId_NoEmbeddedLevel();
	} else {
		a = a_; b = b_;
	}
	*/

	// First see if the interval a,b is in this.
	// Note: driven by error when a==b.
	// TODO Maybe we should do a real interval set api.
	Key ka = my_los->search(a); // Returns stored value: 0 if found, -1 if not found.
	Key kb = my_his->search(b);
	if( ka >= 0 ) { // a is a lower bound
		if( ka == kb ) {
			// Inside, because we're all 0-length.
//			cout << "100: " << a << " " << b << " " << ka << " " << kb << endl << flush;
			return 1;
		}
	}
	kb = my_los->search(b);
	if( kb >= 0 ) { // b is a lower bound! => intersection if != a.
		if( ka == kb ) {
			// Equality, because we're 0-length.
//			cout << "110: " << a << " " << b << " " << ka << " " << kb << endl << flush;
			return 1;
		}
	}

	// Continue on to previous logic.

	// If both lo and hi are inside some range, and the range numbers agree...
	//
	//	Key GH_a, GL_a, SH_a, SL_a;
	//	Key GH_b, GL_b, SH_b, SL_b;


	Key
	GL_a = my_los->findMAX(a),
	GH_a = my_his->findMAX(a),
	SL_a = my_los->findMIN(a),
	SH_a = my_his->findMIN(a),
	GL_b = my_los->findMAX(b),
	GH_b = my_his->findMAX(b);
	/* Not required in the logic following.
	Key
	SL_b = my_los->findMIN(b),
	SH_b = my_his->findMIN(b);
	*/

	/*
	Key param[8]; int i = 0;
	param[i++] =	GL_a;
	param[i++] =	GH_a;
	param[i++] =	SL_a;
	param[i++] =	SH_a;
	param[i++] =	GL_b;
	param[i++] =	GH_b;
	param[i++] =	SL_b;
	param[i++] =	SH_b;

//	bool dbg = a==150;
	bool dbg = a==50;
	dbg = (a == 60 && b == 80) || (a==50);
	if(dbg){
		cout << "(isIn checking a,b: " << a << " " << b << ")" << endl << flush;
		cout << "(isIn GL_a GH_a SL_a SH_a GL_b GH_b SL_b SH_b)" << endl;
		cout << "(isIn" ;

		for(i=0; i<8; i++){
			if (param[i] == KEY_MAX)
				cout << "  MAX";
			else if (param[i] == KEY_MIN)
				cout << " -MAX";
			else
				cout << setw(5) << param[i];
		}
		cout << ")" << endl;

	// 0 is intersect, -1 is out +1 is inside
	}
	*/

	int rstat = 0;

	if(GH_a < GL_a && GL_b < GH_b){ // a is in, b is not. TODO What about +/- MAX?
		rstat = 0;
//		if(dbg) cout << " <<<<< X (0), GH_a < GL_a && GL_b < GH_b" << endl;
	} else if (GL_b == a && SH_a == b){
		rstat = 1;
// //		if(dbg) cout << " <<<<< I (1), because SH_a == a and GL_b == b perfect match" << endl; // bug? error?
//		if(dbg) cout << " <<<<< I (1), because SH_a == b and GL_b == a perfect match" << endl; // correction?
	} else if (GL_b > GL_a) {
		// kluge
		if( GL_b == a && SH_a >= b ) {
			rstat = 1;
//			if(dbg) cout << " <<<<< I (1), because GL_b = a > GL_a && SH_a >= b" << endl;
		} else {
			rstat = 0;
//			if(dbg) cout << " <<<<< X (0), because GL_b > GL_a" << endl;
		}
	} else if (GH_a < GL_a) {
		rstat = 1;
//		if(dbg) cout << " <<<<< I (1), because GH_a < GL_a, and none of previous conditions" << endl;
	} else if (SL_a == b) {
		rstat = 0;
//		if(dbg) cout << " <<<<< X (0), b coincides " << endl;
	} else {
		rstat = -1;
//		if(dbg) cout << " <<<<< O (-1), because none of the above" << endl;
	}

	return rstat;
}

int HtmRangeMultiLevel::isIn(Key key)
{
//	Key key = this->encoding->maskOffLevel(key_);

	InclusionType incl;
	int rstat = 0;
	TInsideResult flags = tinside(key);
	incl = flags.incl;
	rstat = (incl != InclOutside);
	return rstat;
}
int HtmRangeMultiLevel::isIn(HtmRangeMultiLevel & otherRange)
{
	HtmRangeMultiLevel &o = otherRange;
	Key a, b;
	int rel, sav_rel;
	/*
	 * for each range in otherRange, see if there is an intersection,
	 * total inclusion or total exclusion
	 */
	o.reset();			// the builtin lows and highs lists
	//
	// Inside, Outside, Intersect.
	//
	sav_rel = rel = -2;			// nothing

	// TODO 2019-0327 ->getkey is suspect, esp. with > 0 here. Implicit assumption of key != 0...??? MLR
	// OLD while((a = o.my_los->getkey()) > 0){
	// TODO Add unit test.
	while((a = o.my_los->getkey()) >= 0){
		b = o.my_his->getkey();

		rel = isIn(a, b); // TODO MLR Am I in that other guy's sub-interval? I think a design pattern is near...
		//
		// decide what type rel is, and keep looking for a
		// different type
		//
		if (sav_rel != -2){ // first time
			switch (rel) {
			case INTERSECT:
				break;
			case OUTSIDE:
			case INSIDE:
				if (sav_rel != rel){
					// some out, some in, therefore intersect
					break;
				}
			}
		}
		sav_rel = rel;
		o.my_his->step();
		o.my_los->step();
	}
//	cout << "RETURNING " << rel << endl;
	return rel;
}

int HtmRangeMultiLevel::contains(Key a, Key b) {
	HtmRangeMultiLevel compare;
	compare.addRange(a,b);
	// int rstat = compare.isIn(*this);  // 0 is intersect, -1 is out +1 is inside // TODO What isIn what?
	// The following seems to work, but is somewhat inscrutable.
	int rstat = this->isIn(compare);  // 0 is intersect, -1 is out +1 is inside
	//	cout << " contains:  a,b,rstat " << a << " " << b << " " << rstat << endl << flush;
	if (rstat == 0) { rstat = -1; }
	else if (rstat == -1) { rstat = 0; }
	return rstat; // 0 is no-intersection, -1 is partial, 1 is full
}

HtmRangeMultiLevel_NameSpace::TInsideResult HtmRangeMultiLevel::tinside(const Key mid) const
{
	TInsideResult results;
	int level = -1;

	// clearly out, inside, share a boundary, off by one to some boundary
	InclusionType result, t1, t2;
	Key GH, GL, SH, SL;
	// cout << "==========" << setw(4) << mid << " is ";

	GH = my_his->findMAX(mid);
	GL = my_los->findMAX(mid);

	if (GH < GL) { // GH < GL
		t1 = InclInside;
		level = encoding->levelById(GL);
		// cout << "Inside";
	} else {
		t1 = InclOutside;
		// cout << "  no  ";
	}
	// cout << "   ";

	SH = my_his->findMIN(mid);
	SL = my_los->findMIN(mid);
	if (SH < SL) { // SH < SL
		t2 = InclInside;
		// cout << "Inside";
	} else {
		t2 = InclOutside;
		// cout << "  no  ";
	}
//	 cout << hex << " GH = " << my_his->findMAX(mid) << " GL = " << my_los->findMAX(mid) << dec << flush;
//	 cout << hex << " SH = " << my_his->findMIN(mid) << " SL = " << my_los->findMIN(mid) << dec << flush;

	// cout << endl;
	if (t1 == t2){
		result = t1; // Could be InclInside or InclOutside. I.e. mid is in an interval or not.
	} else {
		result = (t1 == InclInside ? InclHi : InclLo); // Mid is the hi or lo boundary.
	}

	//   if (result == InclOutside){
	//     if ((GH + 1 == mid) ||  (SL - 1 == mid)){
	//       result = InclAdjacent;
	//     }
	//   }

	results.level = level;
	results.incl  = result;
	results.mid   = mid;
	results.GH    = GH;
	results.GL    = GL;
	results.SH    = SH;
	results.SL    = SL;
//#define hexFormat << hex << "0x" << setfill('0') << setw(16)
//	cout << "<tinside: level,incl,GH,GL,SH,SL: "
//			<< level << " "
//			<< result << " "
//			hexFormat
//			<< mid << " "
//			hexFormat
//			<< GH << " "
//			hexFormat
//			<< GL << " "
//			hexFormat
//			<< SH << " "
//			hexFormat
//			<< SL << " "
//			<< dec
//			<< ">" << endl << flush;
//#undef hexFormat

//	return result;
	return results;
}

static int errorCount = 0;
static int const errorCountMax = 100;
/**
 * Merge lo..hi into the range.
 *
 * Contains logic associated with multiple level ranges.
 *
 * Note: TopBit cannot be set for lo & hi, i.e. one should find them using
 * leftJustified.getId_NoLevelBit() or leftJustified.getIdTerminator_NoDepthBit()
 *
 * @param lo
 * @param hi
 */
void HtmRangeMultiLevel::mergeRange(const Key lo, const Key hi)
{

	errorCount = 0;
	// TODO Main routine to modify to enable multi-level ranges.
#ifdef DIAG
	cout << "8000 " << hex
       << "0x"	<< setfill('0') << setw(16) << lo
       << ", 0x" << setfill('0') << setw(16) << hi
       << endl << flush;
	cout << "8001 " << dec << "  " << lo << ",   " << hi << endl << flush;
#endif
  
	// Add the first one.
	if( my_los->myHeader->getElement(0) == NIL ) {
		// my_los->insert(lo,100);
		my_los->insert(lo,hi);
		my_his->insert(hi,100);
		// cout << 8002 << " First one inserted. " << endl << flush;
		return;
	}
#ifdef DIAG
	cout << "8000-400" << endl << flush;
#endif

	// TInsideResult loFlag = tinside(lo);
	// int lo_flag = loFlag.incl;

	// TInsideResult hiFlag = tinside(hi);
	// int hi_flag = hiFlag.incl;

	// TODO I think loFlag and hiFlag can help with the logic below.
  
#ifdef DIAG
	cout << "8000-500" << endl << flush;
#endif
	// New stuff.

	uint32 level = encoding->levelById(lo);

#ifdef DIAG
	cout << "8000-600" << endl << flush;
#endif

	// Possible values:
	//   loFlag:  InclInside, InclLo, InclHi, InclOutside
	//   hiFlag:  InclInside, InclLo, InclHi, InclOutside
    // Note: InclHi & InclLo imply a boundary point.

	// Iterate over the intervals ourselves.
	my_los->reset();
	my_his->reset();

#ifdef DIAG
	cout << "8000-700" << endl << flush;
#endif
  
	Key lo1 = lo, hi1 = hi;
	Key l, h;
	bool done = false;

#ifdef DIAG
	cout << "8000-1000-while" << endl << flush;
#endif
// 2019-1212 ORIG	while(((l = my_los->getkey()) >= 0) && (not done) ){
	while( (not done) ){
#ifdef DIAG    
    cout << "8000-1001-while-----" << endl << flush;
#endif
	  l = my_los->getkey();
#ifdef DIAG
    cout << "8000-1001-000 l: " << hex << l << dec << endl << flush;
#endif
    
	  if( l < 0 ) break;
		h = my_his->getkey();
		uint32 l_level = encoding->levelById(l);

#ifdef DIAG    
		cout << "8000-1001-lo1,hi1: " << lo1 << " " << hi1 << " " << level << flush;

    if( lo1 < hi1 ) {
      cout << " valid " << flush;
    } else {
      cout << " invalid " << flush;
    }
    cout << endl << flush;
    
		cout << "8000-1002-    l,h: " << l << " " << h << " " << l_level << flush;
    if( l < h ) {
      cout << " valid " << flush;
    } else {
      cout << " invalid " << flush;
    }
    cout << endl << flush;

    cout << "8000-1003- h < hi1: " << (h < hi1) << flush << endl;
    cout << "8000-1003- h < lo1: " << (h < lo1) << flush << endl;
    cout << "8000-1004- l < hi1: " << (l < hi1) << flush << endl;
    cout << "8000-1004- l < lo1: " << (l < lo1) << flush << endl;
    
#define hexOut(a,b,c) cout << a << " 0x" << hex << setfill('0') << setw(16) << b << ".." << c << dec << endl << flush;
////		cout << endl << flush;
		hexOut("lh1 ",lo1,hi1);
		hexOut("hh  ",h,h);
		hexOut("lh  ",l,h);
#undef hexOut
#endif
    
		if ( h < lo1 ) {
		  // Case NOT-YET-THERE.
#ifdef DIAG
			cout << "Don't know what's above h. Iterate. " << errorCount + 1 << endl << flush;
#endif
			errorCount++; if(errorCount>errorCountMax) {
#ifdef DIAG        
				cout << "HRML::Iterate::errorCount" << endl << flush;
#endif
//				exit(1);
			}
#ifdef DIAG      
      cout << "8000-1010 not-yet-there" << endl << flush;
#endif
			my_los->step();
#ifdef DIAG      
      cout << "8000-1012 not-yet-there" << endl << flush;
#endif
      my_his->step();
#ifdef DIAG      
      cout << "8000-1013 not-yet-there" << endl << flush;
#endif
			// Don't know what's above h.  Iterate.
		} else if( hi1 < l ) {
			// Case 1. A is below B.  Just add
#ifdef DIAG      
      cout << "8000-1014 Case 1 a<b just add" << endl << flush;
#endif
			// my_los->insert(lo1,10001);
			my_los->insert(lo1,hi1);
#ifdef DIAG      
      cout << "8000-1015 Case 1 a<b just add" << endl << flush;
#endif
			my_his->insert(hi1,10001);
#ifdef DIAG      
      cout << "8000-1016 Case 1 a<b just add" << endl << flush;
#endif
			done = true;
		} else if( (lo1 < l) && ( (l <= hi1) && (hi1 <= h) ) ) {
			// Case 2. A.lo is below B.lo, but A.hi is in B.
#ifdef DIAG
			cout << "8000-103x" << endl << flush;
#endif
			// The upper end of the new interval overlaps the current one, but not its lower part. Do surgery.
			// Bounds of the lower part below the current interval.
			Key l_m = lo1;
			// Bug?
      Key h_m = encoding->predecessorToLowerBound_NoDepthBit(l,level); // l-
			// Bad hypothesis Key h_m = encoding->predecessorToLowerBound_NoDepthBit(l,27); // l-

//      if( h_m < l_m ) {
//        // If getting the pred of l jumps back too far...
//        h_m = encoding->getTerminator_NoDepthBit(l_m);
//      }
      
			// Bounds of the overlap.
			//+ Key l_0 = l;
			Key h_0 = hi1; // TODO Should we re-encode this with l_level? Only an issue if level ??? l_level?
			// Bounds of the new interval that extends into current interval.
			Key l_p = encoding->successorToTerminator_NoDepthBit(hi1,l_level); // hi1+
			// Note: l_p can overrun.
			// If troubled try the following:
			h_0 = encoding->predecessorToLowerBound_NoDepthBit(l_p,level); // pred(hi1+) or pred(succ(hi1)); // TODO Verify
			if(h_0 < 0) {
				// Implies we've overrun somehow.
				h_0 = h;
				// _p still okay for level==, level>?
			}
			Key h_p = h;    // No change. Level is correct.
			if( level == l_level ) { // Case 2.1 Merge.
        DIAGOUT("8000-1030");
				// At the same level, merge the two.
				my_los->freeRange(l_m,h_p); // Freeing up to h_p is okay because h_p==h is still part of current interval.
				my_his->freeRange(l_m,h_p);
				// my_los->insert(l_m,100021);
				my_los->insert(l_m,h_p);
				my_his->insert(h_p,100021);
			} else {
				// The lower part overlaps an empty part. Just add.  // ??? Don't need a freeRange ??? Okay...
				if(level > l_level) {
					DIAGOUT("8000-1031");
					// If the new interval's level is greater, just skip in the current, add before.
					// my_los->insert(l_m,100022);
#ifdef DIAG
#define hexOut(a,b,c) cout << a << " 0x" << hex << setfill('0') << setw(16) << b << ".." << c << dec << endl << flush;
						hexOut("lm-hm ",l_m,h_m);
            hexOut("l -h0 ",l  ,h_0);
						hexOut("lp-hp ",l_p,h_p);
						cout << " h_0 " << h_0 << endl << flush;
            cout << " lm < hm : " << (l_m<h_m) << endl << flush;
#undef hexOut
#endif
          
					my_los->insert(l_m,h_m);
					my_his->insert(h_m,100022);
				} else if(true) { // Case 2.3 level < l_level -- new interval wins
					// TODO WORRY -- What about collisions? If we have a collision, will we simply put in the value back in?
					// If the new level is lower than the current, then the new lower level wins. Current is trimmed.
					// Rewrite the current.
					DIAGOUT("8000-1032");
//					if(l_m == 0x7500000000000002) {
          if(true) {
#ifdef DIAG            
#define hexOut(a,b,c) cout << a << " 0x" << hex << setfill('0') << setw(16) << b << ".." << c << dec << endl << flush;
						hexOut("lm-hm ",l_m,h_m);
            hexOut("l -h0 ",l  ,h_0);
						hexOut("lp-hp ",l_p,h_p);
						cout << " h_0 " << h_0 << endl << flush;
#undef hexOut
#endif
					}
					my_los->freeRange(l_m,h_p);
					my_his->freeRange(l_m,h_p);
					// my_los->insert(l_m,100023); // lo1 // this changes the level to level
					my_los->insert(l_m,h_0); // lo1 // this changes the level to level
					my_his->insert(h_0,100023); // hi1
					if(h_0 < h_p) {
						// my_los->insert(l_p,100023);
						my_los->insert(l_p,h_p);
						my_his->insert(h_p,100023);
					}
					my_los->reset(); my_his->reset();
				}
			}
			// Done!
			done = true;
		} else if( (lo1 < l) && (h < hi1) ) {
			DIAGOUT("HRML::Case 4.2/4.4 A covers B.");
			// Case 4.2 A covers B.
			// Test.cpp 4.4.1
			if( level <= l_level ) { // New interval wins.
				DIAGOUT("4.2.2");
				// Case 4.2.2
				my_los->freeRange(l,h);
				my_his->freeRange(l,h);
				// Keep lo1 and h1 and try again.
				// Where does the iterator go?
				DIAGOUT("4.2.2 end");
			} else if( level > l_level ) { // Current interval wins.
				DIAGOUT("HRML::4.2.1");
				// Case 4.2.1
				Key l_m = lo1;
				Key h_m = encoding->predecessorToLowerBound_NoDepthBit(l,level); // TODO Verify no gaps
				//+ Key l_0 = l;
				//+ Key h_0 = h;
				Key l_p = encoding->successorToTerminator_NoDepthBit(h,level); // TODO Verify no gaps
				Key h_p = hi1;
#ifdef DIAG
				if( l_p > h_p ) {
					DIAGOUT("HtmRangeMultiLevel::mergeRange::ERROR!!! SUCC(H) > HI1, I.E. THEY'RE EQUIVALENT.");
				}
#endif
				if( l_m < h_m ) { // If lo1 and l are equivalent, current one wins, and we ignore the new one.
					// my_los->insert(l_m,1000421);
					my_los->insert(l_m,h_m);
					my_his->insert(h_m,1000421);
//					my_los->reset(); my_his->reset();
//				} else {
//					my_los->step(); my_his->step();
				}
				// Interval l..h does not change.
				// update for next iteration
				lo1 = l_p;
				hi1 = h_p;
			}
			// Have to iterate again. lo1 & hi1 have been trimmed for iteration.
			// There must be a better way than resetting...
			my_los->reset(); my_his->reset();
			// huh? -> 			my_los->freeRange(l_m,h_p);	my_his->freeRange(l_m,h_p);
		} else if ( (l <= lo1) && (hi1 <= h) ) {
			DIAGOUT("HRML::Case 4. A is in B.");
			// Case 4. A is in B.
			if( level >= l_level ) {
				// Ignore. Triangles already included.
				done = true;
			} else { // level < l_level
				if(true){
				// Test.cpp 4.3 Collision at the end.
				Key l_m = l;
				Key h_m = encoding->predecessorToLowerBound_NoDepthBit(lo1,l_level);
#ifdef DIAG
				if(l_m>h_m) cout << "HtmRangeMultiLevel::mergeRange::WARNING!!! 4 PRED WARNING. BOUNDS EQUIVALENT" << endl << flush;
#endif
				Key l_0 = lo1;
				Key h_0 = hi1;
				Key l_p = encoding->successorToTerminator_NoDepthBit(hi1,l_level);
				Key h_p = h;
#ifdef DIAG
				if(l_p>h_p) cout << "HtmRangeMultiLevel::mergeRange::WARNING!!! 4 SUCC WARNING. BOUNDS EQUIVALENT" << endl << flush;
#endif
				// Blow away the old and add the three.
				my_los->freeRange(l_m,h_p);
				my_his->freeRange(l_m,h_p);
				if( l_m < h_m ) { // If l_m and l_0 are "equivalent", so the current interval wins and we ignore interval_m.
					// my_los->insert(l_m,10004);
					my_los->insert(l_m,h_m);
					my_his->insert(h_m,10004);
				}
				// my_los->insert(l_0,10004);
				my_los->insert(l_0,h_0);
				my_his->insert(h_0,10004); // TODO Subtle bug?  Need to verify edge case.
				if( l_p < h_p ) { // Non equivalent h_0 and h_p.
					// my_los->insert(l_p,10004);
					my_los->insert(l_p,h_p);
					my_his->insert(h_p,10004);
				}
				}
				done = true;
			}
		} else if ( (l <= lo1) && (h < hi1) ) { // Note the case h < lo1 is handled above.
			// Case 5.
			DIAGOUT("HRML::Case 5");
			if ( level >= l_level ) {
				// Case 5.1
				DIAGOUT("HRML::Case 5.1");
				//+ Key l_m = l; // Note:  no need to add...
				//+ Key h_m = h;
				Key l_p = encoding->successorToTerminator_NoDepthBit(h,level); // Might map l_p > h_p.
				Key h_p = hi1;
				if( l_p > h_p ) {
					l_p = h; // Abandoning the level bits for the moment.
					uint64 l__ = encoding->decrement(l_p,level);
#ifdef DIAG
					if (l__ < (uint64) l) cout << "huh?" << endl << flush;
#endif
					l_p = l__;
					lo1 = l_p;
					hi1 = h_p;
					return; // without stepping.  Try again.
				}
// 				cout << "m " << l_m << " " << h_m << endl << flush;
// //				cout << "0 " << l_0 << " " << h_0 << endl << flush;
// 				cout << "p " << l_p << " " << h_p << endl << flush;
// 				cout << hex << "m " << l_m << " " << h_m << dec << endl << flush;
// //				cout << "0 " << l_0 << " " << h_0 << endl << flush;
// 				cout << hex << "p " << l_p << " " << h_p << dec << endl << flush;
				// cout << "? " << encoding->successorToTerminator_NoDepthBit(h,l_level) << endl << flush;
				// update for iteration
				errorCount++; if(errorCount>errorCountMax) {
					cout << "HRML::5.1::errorCount" << endl << flush;
					exit(1); // TODO -- we shouldn't just exit...
				}
				lo1 = l_p;
				hi1 = h_p;
				my_los->step(); my_his->step(); // Didn't change skiplists.
			} else if ( level < l_level ) {
///////////////////////////////////////////////////////////////////////////
/// 8000-1001-while-----
/// 8000-1001-000 l: 3f3e7dd29ef6348c
/// 8000-1001-lo1,hi1: 4557218219020197835 4557218411410620415 11
/// 8000-1002-    l,h: 4557218216509060236 4557218273971666943 12
/// lh1  0x3f3e7dd334a31fcb..3f3e7dffffffffff
/// hh   0x3f3e7ddfffffffff..3f3e7ddfffffffff
/// lh   0x3f3e7dd29ef6348c..3f3e7ddfffffffff
/// HRML::Case 5
/// HRML::Case 5.2
/// 8000-9999-100 okay exit, done = 0
/// 8000-1001-while-----
/// 8000-1001-000 l: 3f159921c28ef809
/// 8000-1001-lo1,hi1: 4557218411410620427 4557218411410620415 11
/// 8000-1002-    l,h: 4545707769177503753 4545710922714316799 9
/// lh1  0x3f3e7e000000000b..3f3e7dffffffffff
/// hh   0x3f159bffffffffff..3f159bffffffffff
/// lh   0x3f159921c28ef809..3f159bffffffffff
///////////////////////////////////////////////////////////////////////////
        /*
          Note we have the complication that the levels are different.

          level(l1) < level(l) (l1 is coarser than l). In the bug case level(l1)==b, level(l)==c.
          
          Need to review Case 5.2. Here we have lh1 (the interval to be added), with h1>h, where h is the current hi in my_his.
          Thus we have: l < l1 < h < h1.

          NOT THIS: We should remove l..h and add l..h1. 

          We should remove l..h and add l..l1--@level(l) and l1..h1. <- verify...

          This is what the following code attempts to do at present.

         */
				if(true){
          DIAGOUT("HRML::Case 5.2");

          // Low segment. To add in. l's level.
          Key l_m = l;
          // Bug?
          Key h_m = encoding->predecessorToLowerBound_NoDepthBit(lo1,l_level);
          // Bad hypothesis Key h_m = encoding->predecessorToLowerBound_NoDepthBit(lo1,27);

          // Next segment To add in. l1's level.
          Key l_0 = lo1;
          Key h_0 = h; // But need to adjust to avoid creating holes. // Note this has the wrong level compared to l_0. Replaced below.
        
          // Any leftover? Try again.
          Key l_p = encoding->successorToTerminator_NoDepthBit(h,level); // BUG: Found a case where h_p < l_p !?? Note: level for lo1


          // Bad hypothesis h_0 = encoding->predecessorToLowerBound_NoDepthBit(l_p,27); // level is the level of lo1
          // Bug?
          h_0 = encoding->predecessorToLowerBound_NoDepthBit(l_p,level); // level is the level of lo1
          Key h_p = hi1; // The original end of the term. // BUG: This is the old term, but we get hi1 < l_p...
          // Does this mean that in getting coarser, we lost the ability to distinguish between h and h1?
          // So instead of going from l < l1 < h < h1 to l..l1-, l1..h, h+..h1, we only do l..l1-,l1..h1. I.e. h1 ate h.

          // Let's check the terminator h_0.
        
          // Blow away the old and add two and defer the third.
          my_los->freeRange(l_m,h_0);
          my_his->freeRange(l_m,h_0);

          // The first segment
          // TODO NOTE:  When predecessor is used, you have to check to see if pred(b) is less than inf(interval).
          if( l_m < h_m ) { // If l_m and l_0 are "equivalent", so the current interval wins and we ignore interval_m.
            my_los->insert(l_m,h_m);
            my_his->insert(h_m,1000520);
          }

          // The second segment
          my_los->insert(l_0,h_0);
          my_his->insert(h_0,1000521); // TODO Subtle bug?  // Need to verify edge case.
          // update for iteration

#ifdef DIAG
//#define hexOut(a,b) cout << a << "0x" << hex << setfill('0') << setw(16) << b << dec << endl << flush;          
#define hexOut3(a,b,c) cout << a << "0x" << hex << setfill('0') << setw(16) << b << " " << "0x" << hex << setfill('0') << setw(16) << c << " " << dec << " validp: " << (b < c) << endl << flush;
          cout << "+++++" << endl << flush;
          hexOut3("1000520 l_m,h_m ",l_m,h_m);
          hexOut3("1000520 l_0,h_0 ",l_0,h_0);
          hexOut3("1000520 l_p,h_p ",l_p,h_p);
          cout << "-----" << endl << flush;
#undef hexOut3
          {
            cout << "h_m < l_0 " << (h_m < l_0) << endl << flush;
            cout << "h_0 < l_p " << (h_0 < l_p) << endl << flush;
            cout << "-----" << endl << flush;
          }
#endif
          // The third segment
          if( h_p > l_p )
          // if( true )
          {
            lo1 = l_p;
            hi1 = h_p; // Need to verify edge case. BUG Bit us. Fix and verify.
          } else {
            // The re-leveled middle ate the third segment, i.e., the end of the original interval to be added.
            done = true;
          }
          my_los->reset(); my_his->reset(); // TODO Is this too conservative? Where should the iter be?
                                     
        }
///////////////////////////////////////////////////////////////////////////
//				done = true;
			}
		}  else {
      cout << "8000-9999 ERROR" << endl << flush;
      exit(1);
		}
      DIAGOUT("8000-9999-100 okay exit, done = " << done);
	} // while( (not done) ) // line 608 cf 8000-1000-while

    DIAGOUT("8000-9999-200 okay exit");

	if(not done) {
		// The new interval goes at the end of the skiplists.
		// Case 6. We're at the top.  Just add.
		// my_los->insert(lo1,10006);
    DIAGOUT("8000-9999-300 l1,h1: " << hex << lo1 << " " << hi1 << dec);
		my_los->insert(lo1,hi1);
		my_his->insert(hi1,10006);
		done = true;
	}

	DIAGOUT(dec << 9000);
  
	return;

//	cout << dec << 9000 << endl << flush;
//	my_los->list(cout);
//	my_his->list(cout);
}

/**
 * Add a lo-hi interval to the skiplists my_los and my_his using lo and hi as
 * keys.  Currently does not store any useful value in the lists, but uses
 * the positions to keep track of the intervals. This routine makes no attempt
 * to merge or defrag overlapping intervals.
 *
 * Note: TopBit cannot be set for lo & hi, i.e. one should find them using
 * leftJustified.getId_NoLevelBit() or leftJustified.getIdTerminator_NoDepthBit()
 *
 * @param lo
 * @param hi
 */
void HtmRangeMultiLevel::addRange(const Key lo, const Key hi)
{

// #define DIAG
#undef DIAG
//	my_los->insert(lo, (Value) 0); // TODO Consider doing something useful with (Value)... Like storing hi...
//	my_his->insert(hi, (Value) 0);
	DIAGOUT("x200: " << hex << lo << " " << hi);
	// cout << "x201: " << (lo == hi) << endl;

	if( lo == hi ) {
		encoding->setId(lo);
		// cout << "lo,t: " << hex << " " << lo << " " << encoding->getIdTerminator_NoDepthBit() << endl;
		mergeRange(lo,encoding->getIdTerminator_NoDepthBit());
	} else {
		// TODO Simplest thing that might possibly work.
#ifdef DIAG
		cout << "x250: " << hex << lo << " " << hi << endl;
#endif
		// TODO should we coerce hi to be a terminator?
		mergeRange(lo,hi);
	}
	return;
}
#undef DIAG

void HtmRangeMultiLevel::addRange(const Key lohi)
{
//	cout << "x100: " << hex << lohi << endl;
	addRange(lohi,lohi);
	return;
}

/**
 * Merge a range into this by looping over sub ranges and merging them
 * one at a time.
 *
 * Note: TopBit cannot be set for lo & hi, i.e. one should find them using
 * leftJustified.getId_NoLevelBit() or leftJustified.getIdTerminator_NoDepthBit()
 *
 * @param range
 */
void HtmRangeMultiLevel::addRange(HtmRangeMultiLevel *range) {
	if(!range)return;
	if(range->nranges()==0)return;
	Key lo, hi;
	range->reset();
	if(range->getNext(lo,hi)){
		do {
//			this->addRange(lo,hi);
			this->mergeRange(lo,hi);
		} while( range->getNext(lo,hi) );
	}
//	this->defrag();
}


void HtmRangeMultiLevel::defrag(Key gap)
{
	// cout << "HtmRangeMultiLevel::defrag-Key-gap::NOT-IMPLEMENTED EXITING" << endl << flush;
  throw SpatialFailure("HtmRangeMultiLevel::defrag-key-gap::NotImplemented!!");

  /* ???

	if(nranges()<2) return;

	Key hi, lo, save_key;
	my_los->reset();
	my_his->reset();

	// compare lo at i+1 to hi at i.
	// so step lo by one before anything
	//
	my_los->step();

	while((lo = my_los->getkey()) >= 0){
		hi = my_his->getkey();
		// cout << "compare " << hi << "---" << lo << endl;

		// If no gap, then if (hi + 1 == lo) is same as if (hi + 1 + gap  == lo)
		//
		if (hi + (Key) 1 + gap >= lo){
			//
			// merge to intervals, delete the lo and the high
			// is iter pointing to the right thing?
			// need setIterator(key past the one you are about to delete
			//
			// cout << "Will delete " << lo << " from lo " << endl;
			//
			// Look ahead, so you can set iter to it
			//
			my_los->step();
			save_key = my_los->getkey();
			my_los->free(lo);
			// cout << "Deleted " << lo << " from lo " << endl;
			if (save_key >= 0) {
				my_los->search(save_key, 1); //resets iter for stepping
			}
			// cout << "Look ahead to " << save_key << endl << endl;

			// cout << "Will delete " << hi << " from hi " << endl;
			my_his->step();
			save_key = my_his->getkey();
			my_his->free(hi);
			// cout << "Deleted " << hi << " from hi " << endl;
			if (save_key >= 0){
				my_his->search(save_key, 1); //resets iter for stepping
			}
			// cout << "Look ahead to " << save_key << endl << endl;

		} else {
			my_los->step();
			my_his->step();
		}
	}
	// cout << "DONE looping"  << endl;
	// my_los->list(cout);
	// my_his->list(cout);

  */
}

#undef DIAG
#undef DIAGOUT
#define DIAG
// #define DIAGOUT(x)
#define DIAGOUT(x) {cout << x << endl << flush;}
void HtmRangeMultiLevel::defrag()
{
#define hexOut(a,b,c) cout << a << " 0x" << hex << setfill('0') << setw(16) << b << ".." << c << dec << endl << flush;

  DIAGOUT("HRML::defrag " << "0000");
	my_los->list(cout);
	my_his->list(cout);
  DIAGOUT("HRML::defrag " << "0001\n");

  if(nranges()<2) return;

  Key lo0, hi0;
  Key lo1;
  Key hi1;
  Key save_key;
  uint32 level0, level1;
  my_los->reset(); my_his->reset();

  // compare lo at i+1 to hi at i.
  // so step lo by one before anything
  //
  lo0 = my_los->getkey(); level0 = encoding->levelById(lo0);
  my_los->step();
  while((lo1 = my_los->getkey()) >= 0){
    
    DIAGOUT("\n---top of while---");
    hi0 = my_his->getkey();
    level1 = encoding->levelById(lo1);
    my_his->step();
    hi1 = my_his->getkey(); // Should be the same as my_los->getvalue().
#ifdef DIAG
    hexOut("lh0 ",lo0,hi0);
    hexOut("lh1 ",lo1,hi1);
    cout << "level 0,1 " << level0 << " " << level1 << endl << flush;
    cout << "compare " << hex << hi0 << "---" << lo1 << dec << endl << flush;
#endif
    if( level0 == level1 ) { // Same level. Maybe merge?
      DIAGOUT("levels equal");
      Key hi0_pred = encoding->predecessorToLowerBound_NoDepthBit(lo1,level0);
      Key lo1_succ = encoding->successorToTerminator_NoDepthBit(hi0,level1);
      encoding->successorToTerminator_NoDepthBit(hi0,level1);
#ifdef DIAG
      hexOut("hi0_pred,hi0",hi0_pred,hi0);
      hexOut("lo1_succ,lo1",lo1_succ,lo1);
#endif
      if( hi0_pred <= hi0 ) { // If the pred includes the actual...???
        DIAGOUT("merging");
        
        // Let's merge the intervals. Delete hi0 and lo1.
        my_los->step();  // Step to the next lo
        save_key = my_los->getkey();
        my_los->free(lo1);
        if( save_key >= 0 ) {
          my_los->search(save_key,1); // reset iter for stepping // TODO Maybe use this in  mergeRange
        }
        
        my_his->step();
        save_key = my_his->getkey(); // Should be hi1
        
        my_his->free(hi0);
        if( save_key >= 0 ) {
          my_his->search(save_key,1);
        }

        // MLR Addition
        // DIAGOUT("diag: los key: " << hex << my_los->getkey() << dec);
        
        my_los->insert(lo0,hi1);
        Value tag = my_his->search(hi1,1); // Puts iter on h1...
        my_his->insert(hi1,2000000000+tag);
                       
      } else {
        DIAGOUT("Stepping to next (1)");
        my_los->step(); my_his->step();
      }
    } else {
      DIAGOUT("Stepping to next (2), levels different");
      // my_los->step(); my_his->step(); Iterate the while
      my_los->step(); // Iterate for the while
      lo0 = lo1; hi0 = hi1;
    }
    level0 = level1;
    // #undef DIAGOUT
#undef hexOut

      //		if (hi0 >= lo1) { // TODO MLR Change... ???
      ////		if (hi + 1 == lo){ // TODO Original code, seems such a rare case!!!
      //			//
      //			// merge to intervals, delete the lo and the high
      //			// is iter pointing to the right thing?
      //			// need setIterator(key past the one you are about to delete
      //			//
      //			// cout << "Will delete " << lo << " from lo " << endl;
      //			//
      //			// Look ahead, so you can set iter to it
      //			//
      //			my_los->step();
      //			save_key = my_los->getkey();
      //			my_los->free(lo1);
      //			// cout << "Deleted " << lo << " from lo " << endl;
      //			if (save_key >= 0) {
      //				my_los->search(save_key, 1); //resets iter for stepping
      //			}
      //			// cout << "Look ahead to " << save_key << endl << endl;
      //
      //			// cout << "Will delete " << hi << " from hi " << endl;
      //			my_his->step();
      //			save_key = my_his->getkey();
      //			my_his->free(hi0);
      //			// cout << "Deleted " << hi << " from hi " << endl;
      //			if (save_key >= 0){
      //				my_his->search(save_key, 1); //resets iter for stepping
      //			}
      //			// cout << "Look ahead to " << save_key << endl << endl;
      //
      //		} else {
      //			my_los->step();
      //			my_his->step();
      //		}

      } // while

	cout << "DONE looping"  << endl;
	my_los->list(cout);
	my_his->list(cout);
}
#undef DIAG
#undef DIAGOUT

// #define DIAGOUTFLUSH(x)
#define DIAGOUTFLUSH(x) {cout << x << flush;}

// #define DIAGOUT(x)
#define DIAGOUT(x) {cout << x << endl << flush;}

/// Coalesce triangles and decrease resolution when possible.
void HtmRangeMultiLevel::CompressionPass1() {

  //	throw SpatialFailure("HtmRangeMultiLevel::CompressionPass::NotImplemented!!");

  Key lo0, hi0;
  int triangleNumber0;
  uint32 level0;
  my_los->reset(); my_his->reset();

  //	int nIters = 8;
  
  while((lo0 = my_los->getkey()) >= 0) {
    level0 = encoding->levelById(lo0);
    hi0 = my_his->getkey();

    DIAGOUT("100: lo-hi-0 " << hex << lo0 << ".." << hi0 << dec);

    encoding->setId(lo0);
    uint64 bareLo = encoding->bareId();
    triangleNumber0 = encoding->getLocalTriangleNumber();

    Key hi0_id = encoding->idFromTerminatorAndLevel_NoDepthBit(hi0,level0);
    encoding->setId(hi0_id);
    uint64 bareHi = encoding->bareId();

    DIAGOUT("110: tNum " << triangleNumber0);

    //		uint64 hi0_pred = encoding->predecessorToLowerBound_NoDepthBit(hi0,level0);
    //		cout << "111: hi0_pred " << hex << hi0_pred << dec << endl << flush;
    //		encoding->setId(hi0_pred);

    DIAGOUT("120: bare-lo-hi " << bareLo << ", " << bareHi);

    uint64 delta = bareHi - bareLo;
    /*    
    if( triangleNumber0 != 0 ) {
      // We're not at the start of a trixel!
      // Just continue to the next iteration of the while loop.
    } else {
      
      
    }
      

    */

    // triangleNumber0; // if( delta > 3) {}
		
    DIAGOUT("200: delta " << delta);
    if( delta < (uint64) (3 + triangleNumber0) ) {
      DIAGOUT("290: ");
      // No full triangles of leaves in bareLo..bareHi; so skip.
      my_los->step();
      my_his->step();
    } else {
      // At least one triangle can be coalesced
      // There is no change in level, but we need to add a newLo, which will pick up the old terminator, and insert a new terminator newLo-.
      if( triangleNumber0 != 0 ) {
	DIAGOUT("300: ");
	// Snip off the front and then recurse
	Key oldLo = lo0;
	Key newLo = oldLo;
	for(int i=triangleNumber0; i<4; i++) {
	  newLo = encoding->increment(newLo,level0);
	}
	Key newLoPredecessor = encoding->predecessorToLowerBound_NoDepthBit(newLo,level0);
	// oldLo..newLoPredecessor; newLo..hi0 Modify the skiplists.
	my_his->insert(newLoPredecessor,1024);
	// my_los->insert(newLo,1024);
	// 2021-0304+
	my_los->insert(newLo,newLoPredecessor); // Suspicious should be my_los->getvalue(oldLo), not newLoPredecessor.
	my_los->insert(oldLo,newLoPredecessor); // BROKEN 2021-0304 MLR
	// 2021-0304-
	// Set lists to the new lo
	my_los->reset(); my_his->reset(); // TODO Until we know better, start over. Bad, bad, bad.
	// TODO Perhaps instead try a find or a search that would set the iterators.
	// TODO e.g. my_los->search(something)...
      } else {
	DIAGOUT("400: ");
	DIAGOUT("400: blo = 0x" << setw(16) << setfill('0') << hex << bareLo);
	DIAGOUT("400: bhi = 0x" << setw(16) << setfill('0') << hex << bareHi);
	DIAGOUT("400: dlt = 0x" << setw(16) << setfill('0') << hex << delta);
	DIAGOUT("400: dlt =   " << setw(16)                 << dec << delta);
	// Snip off as much as possible
	int numberToCoalesce = (delta+1) / 4;
	DIAGOUT("410: ntc " << dec << numberToCoalesce);
	Key oldLo = lo0; Key newLo = oldLo;
	DIAGOUT("420: lo0 = 0x" << setw(16) << setfill('0') << hex << lo0);

	//				for(int i=0; i<numberToCoalesce; ++i) {
	//					for(int k=0; k<4; ++k) {
	//						newLo = encoding->increment(newLo,level0);
	//					}
	//				}

	my_los->free(oldLo);
	--oldLo; // Reduce level
	// my_los->insert(oldLo,1025);
        // TODO FIX THIS MLR
	// test fix
	my_los->insert(oldLo,my_his->getkey()); // What's the current key (for my_his)? // TODO Don't worry.
        // my_los->insert(oldLo);

	try {
	  // Scoop up a bunch
	  for(int i=0; i<numberToCoalesce; ++i) {
	    for(int k=0; k<4; ++k) {
	      newLo = encoding->increment(newLo,level0);
	    }
	  }
	  // Then break it in half.
	  Key newLoPredecessor = encoding->predecessorToLowerBound_NoDepthBit(newLo,level0);
	  if(newLoPredecessor != hi0) {
	    my_his->insert(newLoPredecessor,1025);
	    //- my_los->insert(newLo,1025);
            //? my_los->insert(newLo,newLoPredecessor); // mlr ???
	  }
          // TODO VERIFY THIS WHOLE ROUTINE MLR 2021-0304
          my_los->insert(oldLo,newLoPredecessor);
          //?
	  my_los->insert(newLo,hi0); // mlr ??? NOTE: this is just a wag. Not sure if correct.
	} catch ( SpatialException &e ) {
	  // What if we're at the top index already? Ooops, there's no new low at the new break.
	  // cout << "400: " << e.what() << endl << flush;
	  if( string(e.what()) != string("EmbeddedLevelNameEncoding::error-increment-overflow") ) {
	    throw SpatialFailure("HtmRangeMultiLevel::Compress::unknown error while incrementing to newLo.");
	  }
	}
	my_los->reset(); my_his->reset(); // TODO Reset is too drastic. Prefer to step back a little... Bad, bad, bad.
      }
    }
    //		if( !nIters-- ) exit(1);
  }

  //	// my_los->step();
  //
  //	while((lo1 = my_los->getkey())){
  //		level1 = encoding->levelById(lo1);
  //
  //		// Look for full pods of sibling leaves.
  //		if( level0 == level1 ) {
  //			int lo0Id = encoding->getLocalTriangleNumber();
  //			if(lo0Id == 0) { // Found the first triangle
  //				int lo1Id = encoding->getLocalTriangleNumber();
  //			if( lo0Id != lo1Id ) {
  //
  //			}
  //		}
  //	}
  //}
}

#undef DIAGOUT
#undef DIAGOUTFLUSH

#define DIAGOUT(x) 
#define DIAGOUTFLUSH(x) 

/// Free the range los and his.
void HtmRangeMultiLevel::purge()
{
	my_los->freeRange(-1, KEY_MAX);
	my_his->freeRange(-1, KEY_MAX);
}

/// Reset the lo and hi lists to their zeroth element.
void HtmRangeMultiLevel::reset()
{
	my_los->reset();
	my_his->reset();
}
//
// return the number of ranges
//
/// The number of ranges.

#define DIAGOUTFLUSH(x)
// #define DIAGOUTFLUSH(x) {cout << x << flush;}

#define DIAGOUT(x)
// #define DIAGOUT(x) {cout << x << endl << flush;}

int HtmRangeMultiLevel::nranges()
{

  bool flag_error = false;
  
	DIAGOUT("z000");
	Key lo; Value lo_value;
	Key hi; Value hi_value;
	int n_ranges;
	n_ranges = 0;
	DIAGOUT("z001");
	DIAGOUT("z001 my_los " << hex << my_los << dec);
	my_los->reset();
	DIAGOUT("z002");
	my_his->reset();
	DIAGOUT("z010");

	// This is a problem when lo can be zero. Is it?
	// getkey returns -1 if nothing is found, maybe fix the following using >= 0? Worry about id 0. Should be okay this low in the code. MLR 2019-0327
	// TODO skiplist works for keys above and below zero. Here using only positives. Fix.
	// OLD while((lo = my_los->getkey()) > 0){
	while((lo = my_los->getkey()) >= 0){
		n_ranges++;
    DIAGOUTFLUSH("z020 " << n_ranges);
		hi = my_his->getkey();
		// my_his->getkey();
    lo_value = my_los->getvalue();
    hi_value = my_his->getvalue();
    
#define FMTX(x) setw(16) << setfill('0') << hex << x << dec    
		DIAGOUTFLUSH(" : " << "( " << FMTX(lo) << ", " << FMTX(lo_value) << " ) "  << "( " << FMTX(hi) << ", " << hi_value << " ) ");
    
    if( lo_value != hi ) {
      DIAGOUTFLUSH(" +lo_value.ne.hi");
      flag_error = true;
    }
    if( lo > hi ) {
      DIAGOUTFLUSH("+lo.gt.hi");
      flag_error = true;
    }
    DIAGOUT("");
#undef FMTX
		// cout << " : " << lo << flush << endl;
		my_los->step();
		my_his->step();
	}

	DIAGOUT("z100");
  if( flag_error ) {
    cout << "HRML::nranges Error exit..." << endl << flush;
    // exit(1);
  }

	return n_ranges;
}

/// The number of indexes in the ranges.
int HtmRangeMultiLevel::nindexes_in_ranges()
{
//	cout << "z000" << endl << flush;
	Key lo, hi;
	int n_indexes_ranges = 0;
	my_los->reset();
	my_his->reset();
//	cout << "z010" << endl << flush;

	// OLD while((lo = my_los->getkey()) > 0){ // MLR
	while((lo = my_los->getkey()) >= 0){
		hi = my_his->getkey();
//		cout << " : " << lo << ", " << hi << " " << flush << endl;

		int levelLo = encoding->levelById(lo);
		encoding->setId(lo);

		// encoding->setId(hi);

		Key hi_term = hi; // Trust we have a terminator here.

		// int nIndexes = 0;
		Key newLo = lo;
		while(newLo<hi_term) {
			++n_indexes_ranges;
			newLo = encoding->increment(newLo,levelLo);
		}

		my_los->step();
		my_his->step();
	}

//	cout << "z100" << endl << flush;

	return n_indexes_ranges;
}

//
// return the smallest gapsize at which rangelist would be smaller than
// desired size
//
Key HtmRangeMultiLevel::bestgap(Key desiredSize)
{
	SkipList sortedgaps(SKIP_PROB);
	Key gapsize = -1;
	Key key;
	Value val;

	Key lo, hi, oldhi = 0, del;

	int n_ranges, left_ranges;

	n_ranges = 0;
	my_los->reset();
	my_his->reset();

	// OLD while((lo = my_los->getkey()) > 0){
	while((lo = my_los->getkey()) >= 0){
		hi = my_his->getkey();
		n_ranges++;
		if (oldhi > 0){
			del = lo - oldhi - (Key) 1;
			val = sortedgaps.search(del);
			if (val == SKIPLIST_NOT_FOUND) {
				// cerr << "Insert " << del << ", " << 1 << endl;
				sortedgaps.insert(del, 1); // value is number of times this gapsize appears
			} else {
				// cerr << "Adding " << del << ", " << val+1 << endl;
				sortedgaps.insert(del, (Value) (val+1));
			}
		}
		oldhi = hi;
		my_los->step();
		my_his->step();
	}
	if (n_ranges <= desiredSize) // no need to do anything else
		return 0;

	// increase gapsize until you find that
	// the if you remove the sum of all
	// gapsizes less than or equal to gapsize
	// leaves you with fewer than the deisred number of ranges
	//
	left_ranges = n_ranges;
	sortedgaps.reset();
	while((key = sortedgaps.getkey()) >= 0){
		gapsize = key;
		val = sortedgaps.getvalue();
		// cerr << "Ranges left " << left_ranges;
		left_ranges -= (int) val;
		if (left_ranges <= desiredSize)
			break;
		sortedgaps.step();
	}

	// sortedgaps.list(cerr);

	// sortedgaps.freeRange(-1, KEY_MAX);

	sortedgaps.freeAll();
	return gapsize;
}

HtmRangeMultiLevel HtmRangeMultiLevel::getSpan() {
	HtmRangeMultiLevel ret;
	Key lo, hi, first, last;
	hi = -1;
	my_los->reset();
	my_his->reset();
	lo = my_los->getkey();
	first = lo;
	if(lo<0) {
		throw SpatialFailure("HtmRangeMultiLevel::getSpan::EmptyRange!!");
	}
	while(lo > 0) {
		hi = my_his->getkey();
		my_los->step();
		my_his->step();
		lo = my_los->getkey();
	}
	last = hi;
	ret.addRange(first,last);
	return ret;
}

void HtmRangeMultiLevel::parse(std::string rangeString) {
	// char tmp_buf[256];
	std::string::size_type pos;
	// std::string::size_type lastPos = 0;

//	cout << "x000:" << rangeString << endl;

	pos = 0;
	std::string s0 = rangeString.substr(pos,1);
//	cout << "x050: " << s0  << endl;
//	cout << "x051: " << OpenRepresentationString << endl;
//	cout << "x052: " << (OpenRepresentationString == s0) << endl;
//	cout << "x053: " << pos << endl;

	if(s0 != OpenRepresentationString) {
		throw SpatialFailure("HtmRange::parse::NoOpenRepresentationString");
	}

	// int iSpace = rangeString.find_first_of(' ');
	// cout << "x120: " << iSpace << endl;

	int posSym = rangeString.find(SymbolicRepresentationString);
	int posS, posN, posNext;
//	cout << "x100: " << posSym << endl;
	if( posSym > 0 ) {
		// throw SpatialFailure("HtmRange::parse::Symbolic read not implemented.");
		rangeString = rangeString.substr(posSym+SymbolicRepresentationString.length()); // Strip the header.
		int count = 5;
		while(true) {
//			cout << "x200: rangeString:  '" << rangeString << "'" << endl;
			--count; if(count<0)throw SpatialFailure("HtmRange::parse::Symbolic read error count > 5.");
			posS = rangeString.find("S");
			posN = rangeString.find("N");
			posNext = posS;
			if(posNext < 0) {   // posS doesn't exist...
				posNext = posN; // Let the chips fall where they may.
			} else if( posN >= 0 ){  // posN exists!
				if( posNext > posN ) {  // and posN occurs earlier than posS, it's the first...
					posNext = posN;
				}
			}
			if(posNext < 0){
				return; // No data, so return.
				//	// throw(SpatialFailure("HtmRange::parse::symbolic::NoData"));
			}
			int posComma = rangeString.find(",");
			// std:string?
			string endSymbol;
			if(posComma<0) {
				endSymbol = ")";
			} else {
				endSymbol = ",";
			}
			int posClose = rangeString.find(endSymbol);
			if(posClose<0) {
				// No more data -- return; // Might be a syntax error though...
//				cout << "x112-return-1, can't find: '" << endSymbol << "'" << endl;
				return;
			}
			std::string first = rangeString.substr(posNext,posClose-posNext);
			rangeString = rangeString.substr(posClose+1);
//			cout << "y100: first: '" << first << "'" << endl;
//			cout << "y101: rest:  '" << rangeString << "'" << endl;
			vector<int64> keys; // Note first and rest are symbolic.
			//while(true) {
			int iSpace = first.find(" ");
			int64 i0 = -1, i1 = -1;
			int status;
			char s0[64], s1[64];
			if(iSpace<0) {
				status = sscanf(first.c_str(),"%s",s0);
				if(status == EOF){
					throw SpatialFailure("HtmRange::parse::symbolic::sscanf one variable yields EOF");
				}
				encoding->setName(s0);
				i0=encoding->getId_NoLevelBit();
				addRange(i0,i0);
			} else {
				status = sscanf(first.c_str(),"%s %s",s0,s1);
				if(status == EOF){
					throw SpatialFailure("HtmRange::parse::symbolic::sscanf two variables yields EOF");
				}
				encoding->setName(s0);
				i0=encoding->getId_NoLevelBit();
				encoding->setName(s1);
				i1=encoding->getId_NoLevelBit(); // Let addRange make this a terminator.
				addRange(i0,i1);
			}
//			cout << "i0,i1: " << i0 << " " << i1 << endl;
		}
	}

	int posHex = rangeString.find(HexRepresentationString);
//	cout << "x110: " << posHex << endl;
	rangeString = rangeString.substr(posHex+HexRepresentationString.length()+1);
//	cout << "x110a:'" << rangeString << "'" << endl;
	if( posHex > 0 ) {
		int count = 1024; // TODO Repent the sin of hard coded values.
		while(true) {
			--count; if(count<0) {
				string msg = "HtmRange::parse::hex::scanned more than "+to_string(count)+" items! ";
				throw SpatialFailure(msg.c_str());
			}
			posHex = rangeString.find("x");
			if(posHex<0) {
//				cout << "x111-return-1" << endl;
				return; // No more data -- return
				// throw(SpatialFailure("HtmRange::parse::hex::NoData"));
			}
			int posComma = rangeString.find(",");
			string endSymbol;
			if(posComma<0) {
				endSymbol = ")";
			} else {
				endSymbol = ",";
			}
			int posClose = rangeString.find(endSymbol);
			if(posClose<0) {
				// No more data -- return; // Might be a syntax error though...
//				cout << "x112-return-1, can't find: '" << endSymbol << "'" << endl;
				return;
			}
			string first = rangeString.substr(posHex,posClose-posHex);
			rangeString = rangeString.substr(posClose+1);
//			cout << "y100: first: '" << first << "'" << endl;
//			cout << "y101: rest:  '" << rangeString << "'" << endl;
			vector<int64> keys;
			//while(true) {
			int iSpace = first.find(" ");
			int64 i0 = -1, i1 = -1;
			int status;
			if(iSpace<0) {
				status = sscanf(first.c_str(),"x%llx",&i0);
				if(status == EOF){
					throw SpatialFailure("HtmRange::parse::hex::sscanf one variable yields EOF");
				}
				addRange(i0,i0);
			} else {
				status = sscanf(first.c_str(),"x%llx x%llx",&i0, &i1);
				if(status == EOF){
					throw SpatialFailure("HtmRange::parse::hex::sscanf two variables yields EOF");
				}
				addRange(i0,i1);
			}
//			cout << "i0,i1: " << i0 << " " << i1 << endl;
		}
	}
	// Error if you get here.
}

// TODO fix the arguments for more safety. I.e. const...
bool HtmRangeMultiLevel::equalp(HtmRangeMultiLevel *other) {
	Key lo0, lo1, hi0, hi1;
	if( this == other ) return true;
	int n0, n1;
	n0 = this->nranges(); n1 = other->nranges();
	if( n0 == 0 || n1 == 0 ) {
		return n0 == n1; // Both empty?
	}
	int errorCount = 5;
	this->reset();
	other->reset();
	bool equalp = true;
	while(equalp) {
		if((--errorCount) < 0) return false;
		lo0 = my_los->getkey();
		lo1 = other->my_los->getkey();
		equalp = equalp && (lo0 == lo1);
		if(equalp){
			if(lo0 < 0) return equalp;
			hi0 = my_his->getkey();
			hi1 = other->my_his->getkey();
			equalp = equalp && (hi0 == hi1);
			my_los->step();
			my_his->step();
			other->my_los->step();
			other->my_his->step();
		}
	}
	return false;
}

int HtmRangeMultiLevel::stats(int desiredSize)
{
	Key lo, hi;
	Key oldhi = (Key) -1;
	Key del;

	Key max_gap = 0;

	int histo[GAP_HISTO_SIZE], i, huges, n_ranges;
	int cumul[GAP_HISTO_SIZE];
	int bestgap = 0;
	int keeplooking;
	// cerr << "STATS=========================" << endl;
	for(i=0; i<GAP_HISTO_SIZE; i++){
		cumul[i] = histo[i] = 0;
	}
	n_ranges = 0;
	huges = 0;
	max_gap = 0;
	my_los->reset();
	my_his->reset();

	// OLD while((lo = my_los->getkey()) > 0){
	while((lo = my_los->getkey()) >= 0){
		// cerr << "Compare lo = "  << lo;
		n_ranges++;
		hi = my_his->getkey();
		if (oldhi > 0){

			del = lo - oldhi - 1;
			if (del < GAP_HISTO_SIZE){
				histo[del] ++;
			} else {
				max_gap = max_gap < del ? del : max_gap;
				huges++;
			}
			// cerr << " with hi = " << oldhi << endl;
		} else {
			// cerr << " with nothing" << endl;
		}
		oldhi = hi;
		//
		// Look at histogram of gaps
		//

		my_los->step();
		my_his->step();
	}

	if (n_ranges <= desiredSize){
		// cerr << "No work needed, n_ranges is leq desired size: " << n_ranges << " <= " << desiredSize << endl;
		return -1;			// Do not do unnecessary work
	}

	keeplooking = 1;
	// cerr << "Start looking with n_ranges = " << n_ranges << endl;


	// SUBTRACT FROM n_ranges, the number of 0 gaps, because they
	// will me merged automatically
	// VERY IMPORTANT!!!
	//
	n_ranges -= histo[0];
	//
	for(i=0; i<GAP_HISTO_SIZE; i++){
		if(i>0){
			cumul[i] = cumul[i-1];
		}
		//     else {
		//       cerr << setw(3) << i << ": " << setw(6) << histo[i] ;
		//       cerr << ", " << setw(6) << cumul[i] ;
		//       cerr << " => " << setw(6) << n_ranges - cumul[i];
		//       cerr << endl;
		//     }

		if(histo[i] > 0){
			cumul[i] += histo[i];
			cerr << setw(3) << i << ": " << setw(6) << histo[i] ;
			cerr << ", " << setw(6) << cumul[i] ;
			cerr << " => " << setw(6) << n_ranges - cumul[i];
			// You can stop looking when desiredSize becomes greater than or
			// equal to the n_ranges - the cumulative ranges removed
			//
			keeplooking = (desiredSize < (n_ranges - cumul[i]));
			if (keeplooking == 0)   cerr << "   ****";
			cerr << endl;
			if (!keeplooking){
				break;
			}
			bestgap = i;
		}
	}
	return bestgap;
}

namespace HtmRangeMultiLevel_NameSpace {

std::ostream& operator<<(std::ostream& os, const HtmRangeMultiLevel& range) {
	char tmp_buf[256];
	Key lo, hi;
	os << noshowbase;
	// os << "Start Range " << endl;
	// Preamble TODO change from symbolicOutput boolean to enumerated type.
	os << OpenRepresentationString;
	if (range.symbolicOutput){
		os << SymbolicRepresentationString << " ";
	} else {
		os << HexRepresentationString << " ";
	}
	range.my_los->reset();
	range.my_his->reset();
	// OLD while((lo = range.my_los->getkey()) > 0){
	while((lo = range.my_los->getkey()) >= 0){
		hi = range.my_his->getkey();
		if (range.symbolicOutput){
		  strcpy(tmp_buf,range.encoding->nameById(lo).c_str());
			strcat(tmp_buf," ");
			strcat(tmp_buf,range.encoding->nameById(hi).c_str());
			os << tmp_buf;
		} else {
#ifdef _WIN32
			sprintf(tmp_buf, "%I64d %I64d ", lo, hi);
			os << tmp_buf;
#else
			os << "x" << hex << lo << " " << "x" << hi << dec;
			// sprintf(tmp_buf, "%llu %llu ", lo, hi);
			// sprintf(tmp_buf, "%llu %lld ", lo, hi);
#endif
		}
		// os << tmp_buf << endl; // TODO MLR Why gratuitously add an EOL here?
		// os << tmp_buf;

		// os << lo << " " << hi << endl;
		range.my_los->step();
		range.my_his->step();
		// OLD if(range.my_los->getkey()>0) {
		if(range.my_los->getkey()>=0) {
			os << ", ";
		}
	}
	os << CloseRepresentationString;
	// os << "End Range ";
	return os;
}


}

int HtmRangeMultiLevel::getNext(Key &lo, Key &hi)
{
	// TODO Question: Can we have a key of 0? Or negative?
	// Clearly at one time I was not expecting a Key of 0, and it is written somewhere that
	// a key of zero is the "error" key. However, it is a valid rep of S0 in some contexts.
	// NEGATIVE is still an error... I think.
	// The real problem is when 0 is valid and then Key hi has meaning...
//	cout << "a" << flush;
	lo = my_los->getkey();
	// OLD if (lo <= (Key) 0){
	if (lo < (Key) 0){
		// hi = lo = (Key) 0;
		hi = lo = (Key) -1;
		return 0;
	}
//	cout << "b" << flush;
//	cout << " " << lo << " " << flush;
	hi = my_his->getkey();
//	cout << " " << hi << " " << flush;
	// OLD if (hi <= (Key) 0){
	if (hi < (Key) 0){
#if DIAG
		cout << endl;
		cout << " getNext error!! " << endl << flush;
#endif
		// hi = lo = (Key) 0;
		hi = lo = (Key) -1;
		return 0;
	}
	my_his->step();
//	cout << "c" << flush;
	my_los->step();
//	cout << "d " << flush;
	return 1;
}
int HtmRangeMultiLevel::getNext(Key *lo, Key *hi)
{
	*lo = my_los->getkey();
	// OLD if (*lo <= (Key) 0){
	// OLD	*hi = *lo = (Key) 0;
	if (*lo < (Key) 0){
		*hi = *lo = (Key) -1;
		return 0;
	}
	*hi = my_his->getkey();
	my_his->step();
	my_los->step();
	return 1;
}
KeyPair HtmRangeMultiLevel::getNext() {
	KeyPair key;
	int indexp = getNext(key.lo,key.hi);
	key.set = indexp;
	return key;
}
int HtmRangeMultiLevel::getNext(KeyPair &kp) {
	return getNext(kp.lo,kp.hi);
}

// TODO refactor print and << to use common code.
// TODO Note this format differs from HtmRange!!!
void HtmRangeMultiLevel::print(std::ostream& os, bool symbolic)
{
	char tmp_buf[256];
	Key hi, lo;

	os << OpenRepresentationString;
	if (symbolic){
		os << SymbolicRepresentationString << " ";
	} else {
		os << HexRepresentationString << " ";
	}

	// Though we always print either low or high here,
	// the code cycles through both skiplists as if // TODO MLR Why keep two skiplists?
	// both were printed. Saves code, looks neater
	// and since it is ascii IO, who cares if it is fast
	//
	my_los->reset();
	my_his->reset();

	/// TODO Need to be careful about all of the different id formats we have, esp
	/// that anything a user might see is consistent.
	///
	/// TODO Add input parser for symbolic format.

	// OLD while((lo = my_los->getkey()) > 0){
	while((lo = my_los->getkey()) >= 0){
		hi = my_his->getkey();
		if (symbolic){
		  strcpy(tmp_buf,encoding->nameById(lo).c_str());
			strcat(tmp_buf," ");
			encoding->setId(lo);
			uint64 level = encoding->getLevel();
			encoding->setId(hi);
			uint64 termp = encoding->getLevel();
			if( termp == 63 ) { // Found a terminator. Rightmost 6 are for the level.
				// uint64 hi_ = (hi & (~63)) + 27; // 27 is currently the maximum level, could use level here...
				uint64 hi_ = (hi & (~63)) + level; // 27 is currently the maximum level, could use level here...
				strcat(tmp_buf,encoding->nameById(hi_).c_str());
			} else {
			  strcat(tmp_buf,encoding->nameById(hi).c_str());
			}
		} else {
#ifdef _WIN32
			// TODO Fix Windows port...
			sprintf(tmp_buf, "%I64d %I64d",lo,hi);
#else
			//sprintf(tmp_buf, "x%llx x%llx",lo,hi);
			encoding->setId(lo);
			uint64 loTerm = encoding->getIdTerminator_NoDepthBit();
			if(hi != (Key) loTerm) {
				sprintf(tmp_buf, "x%llx x%llx",lo,hi);
			} else {
				sprintf(tmp_buf, "x%llx",lo);
			}
#endif
		}
		os << tmp_buf;
		my_los->step();
		my_his->step();
		// OLD if(my_los->getkey()>0) {
		if(my_los->getkey()>=0) {
			os << ", ";
		}
	}
	os << CloseRepresentationString;
	return;
}

// TODO refactor print and << to use common code.
void HtmRangeMultiLevel::print(int what, std::ostream& os, bool symbolic)
{

	Key hi, lo;
	char tmp_buf[256];
	// Though we always print either low or high here,
	// the code cycles through both skiplists as if // TODO MLR Why keep two skiplists?
	// both were printed. Saves code, looks neater
	// and since it is ascii IO, who cares if it is fast
	//
	my_los->reset();
	my_his->reset();

	// OLD while((lo = my_los->getkey()) > 0){
	while((lo = my_los->getkey()) >= 0){
		hi = my_his->getkey();
		if (what != BOTH) {
			if (symbolic){
			  strcpy(tmp_buf,encoding->nameById(what == LOWS ? lo : hi).c_str());
			} else {
#ifdef _WIN32
				sprintf(tmp_buf, "%I64d", what == LOWS ? lo : hi);
#else
				sprintf(tmp_buf, "%llu", what == LOWS ? lo : hi);
#endif
			}
		} else {
			if (symbolic){
			  strcpy(tmp_buf,encoding->nameById(lo).c_str());
				strcat(tmp_buf,"..");
				strcat(tmp_buf,encoding->nameById(hi).c_str());
			} else {
				sprintf(tmp_buf, "%llu..%llu", lo, hi);
			}
		}

		os << tmp_buf << " " << flush; //  << endl;
		my_los->step();
		my_his->step();
	}
	return;
}

#undef DIAGOUTFLUSH
#undef DIAGOUT

#define DIAGOUTFLUSH(x)
// #define DIAGOUTFLUSH(x) {cout << x << flush;}

#define DIAGOUT(x)
// #define DIAGOUT(x) {cout << x << endl << flush;}


int HtmRangeMultiLevel::verify()
{
  int flag_error = 0;
  
	Key lo; Value lo_value;
	Key hi; Value hi_value;
	int n_ranges;
	n_ranges = 0;
	my_los->reset();
	my_his->reset();

	// This is a problem when lo can be zero. Is it?
	// getkey returns -1 if nothing is found, maybe fix the following using >= 0? Worry about id 0. Should be okay this low in the code. MLR 2019-0327
	// TODO skiplist works for keys above and below zero. Here using only positives. Fix.
	// OLD while((lo = my_los->getkey()) > 0){
	while((lo = my_los->getkey()) >= 0){
		n_ranges++;
		hi = my_his->getkey();
		// my_his->getkey();
    lo_value = my_los->getvalue();
    hi_value = my_his->getvalue();
#define FMTX(x) setw(16) << setfill('0') << hex << x << dec    
		DIAGOUTFLUSH(" : " << "( " << FMTX(lo) << ", " << FMTX(lo_value) << " ) "  << "( " << FMTX(hi) << ", " << hi_value << " ) ");
    if( lo_value != hi ) {
      DIAGOUTFLUSH(" +lo_value.ne.hi");
      flag_error += 1;
    }
    if( lo > hi ) {
      DIAGOUTFLUSH("+lo.gt.hi");
      flag_error +=10;
    }
    DIAGOUT("");
#undef FMTX
		// cout << " : " << lo << flush << endl;
		my_los->step();
		my_his->step();
	}

  DIAGOUT("z100");
  if( flag_error ) {
    DIAGOUT("HRML::verify Error flagged...");
#ifdef DIAG
    DIAGOUT("Error exit...");
    exit(1);
#endif
  }

	return flag_error;
}

#undef DIAGOUTFLUSH
#undef DIAGOUT

#define DIAG

// #define DIAGOUTFLUSH(x)
#define DIAGOUTFLUSH(x) {cout << x << flush;}

// #define DIAGOUT(x)
#define DIAGOUT(x) {cout << x << endl << flush;}


#define FMTX(x) setw(16) << setfill('0') << hex << x << dec

void HtmRangeMultiLevel::CompressionPass(bool onepass)
{
  DIAGOUT("\n\nHRML::CompressionPass-----start");

#ifdef DIAG
        my_los->list(cout); // my_his->list(cout);
#endif
  
  Key lo0, hi0;
  Key lo1, hi1;
  uint32 level0, level1;
  
  // Start at the beginning
  my_los->reset(); my_his->reset();

  //
  bool done    = false;
  bool changed = false;


  //  int k = 0;
  while(not done) {
    //    if(++k > 4) { exit(1); }
    DIAGOUT("\n---top of while not done---");

    lo0 = my_los->getkey(); hi0 = my_his->getkey();

    if(lo0 < 0) { // Made it to the end
      DIAGOUT("Made it to end, changed & onepass: " << changed << " " << onepass);
      if( not changed || onepass ) {
        done = true;
      } else {
        changed = false; // try again, unless one_pass is set
#ifdef DIAG
        my_los->list(cout); // my_his->list(cout);
#endif
        my_los->reset(); my_his->reset();
      }
    } else { // Got data to work on
      DIAGOUT("---working---");
      level0 = encoding->levelById(lo0);
      
      // Can we combine any intervals?
      bool done_defrag = false;
      DIAGOUT("HRML::CP 880my_lh " << FMTX(my_los->getkey()) << " " << FMTX(my_his->getkey()));
      my_los->step(); my_his->step();
      
      while(not done_defrag) {
        // Peek ahead
        DIAGOUT("---top of while not done_defrag---");
        DIAGOUT("HRML::CP 890");
        DIAGOUT("HRML::CP 890my_lh " << FMTX(my_los->getkey()) << " " << FMTX(my_his->getkey()));        
        // my_los->step(); my_his->step();
        //        DIAGOUT("HRML::CP 891my_lh " << FMTX(my_los->getkey()) << " " << FMTX(my_his->getkey()));        
      
        lo1    = my_los->getkey();
        level1 = encoding->levelById(lo1);
        hi1    = my_his->getkey();

        Key hi_test = encoding->predecessorToLowerBound_NoDepthBit(lo1,level1);
        DIAGOUT("HRML::CP 900");
        DIAGOUT("HRML::CP 901 lh0 " << FMTX(lo0)     << " " << FMTX(hi0));
        DIAGOUT("HRML::CP 910 lh1 " << FMTX(lo1)     << " " << FMTX(hi1));
        DIAGOUT("HRML::CP 920 ht  " << FMTX(hi_test) << " " << FMTX(hi_test));

        // Do not change level? level0 .eq. level1 ?
        if( level0 != level1 || hi1 == -1 ) {
          DIAGOUT("HRML::CP: 1000");
          done_defrag = true;
        } else {
          if( hi_test != hi0 ) { // Is this too strict?
            DIAGOUT("HRML::CP: 2000");
            done_defrag = true;
          } else {
            // lo0..hi0 connects to lo1..hi1

            // stitch them together
            my_los->step();
            Key lo_next = my_los->getkey();
            my_los->free(lo1); // lo1 goes away.
            if( lo_next >= 0 ) {
              my_los->search(lo_next,1); // reset iter for stepping
            }

            my_his->step();
            Key hi_next = my_his->getkey();
            my_his->free(hi0); // hi0, its pred, goes away
            if( hi_next >= 0 ) {
              my_his->search(hi_next,1); // reset iter for stepping (the '1' indicates setting iter)
            }

            DIAGOUT("HRML::CP 950 lhn  " << FMTX(lo_next) << " " << FMTX(hi_next));
            //            DIAGOUT("hrml::CP 951");
            DIAGOUT("HRML::CP 960my_lh " << FMTX(my_los->getkey()) << " " << FMTX(my_his->getkey()));
            

            // my_los->insert(my_los->getkey(),hi_next); Should be the following...
            my_los->insert(lo0,hi1); // New interval is lo0..hi1.
            my_his->insert(hi1,3000000000+my_his->getvalue()); // my_his at hi1...

            DIAGOUT("HRML::CP 961my_lh " << FMTX(my_los->getkey()) << " " << FMTX(my_his->getkey()));

            // Get ready for the next iteration.
            // lo1 is unchanged
            hi0 = hi1;
            done_defrag = false; // Still false, try again.

            // hi0 will grow until we've eaten all of the contiguous intros
          } // end hi_test == hi0
        } // end level0 == level1.
      } // end while not done_defrag


      // lo1..hi1 is the next interval, lo0..hi0 is the current one.
      lo1    = my_los->getkey();
      level1 = encoding->levelById(lo1); // Should this be level1 or level0:
      hi1    = my_his->getkey();

      if( level0 == 0 ) {
        // No coalescense is possible at the root level.
      } else {      
        // Check to see if we have enough here to coalesce.
        encoding->setId(lo0);
        uint64 bareLo = encoding->bareId();
        uint64 triangleNumber0 = encoding->getLocalTriangleNumber();
        
        Key hi0_id = encoding->idFromTerminatorAndLevel_NoDepthBit(hi0,level0);
        encoding->setId(hi0_id);
        uint64 bareHi = encoding->bareId();
        
        uint64 delta = bareHi - bareLo; // How many triangles are here (-1)?
        
        if( delta < (uint64) (3 + triangleNumber0) ) {
          // There are no cycles of triangles possible in [bareLo..bareHi].
          DIAGOUT("Not enough triangles for a parent. delta = " << delta );
        } else {
          // There are cycles of triangles in [bareLo..bareHi]!
          
          // Find the first tNum 0.
          uint64 delta_to_first = (4 - triangleNumber0) % 4;
          uint64 number_of_full_parents = (1+delta-delta_to_first)/4;
          uint64 delta_in_last = 1+delta - delta_to_first - 4*number_of_full_parents;
          
          // Cut to tNum 0.
          Key lo_cut = lo0;
          for(int i = 0; i< delta_to_first; ++i ) {
            lo_cut = encoding->increment(lo_cut,level0);
	    encoding->setId(lo_cut);
	    Key tmp_term = encoding->predecessorToLowerBound_NoDepthBit(lo_cut,level0);
	    DIAGOUT("CP: l_cut,t_term " << FMTX(lo_cut) << " " << FMTX(tmp_term));
          }
          uint32 level_lo_cut = level0 - 1;
          
          DIAGOUT("\ndelta "<<delta<<"\nparents "<<number_of_full_parents<<"\ndelta1st "<<delta_to_first<<"\ndelta in last "<<delta_in_last);
          
          // Cut end from tNum last
          Key lo_last = lo_cut;
          if( delta_in_last > 0 ) {
            for(int i = 0; i< number_of_full_parents; ++i ) {
              // lo_last = encoding->increment(lo_last,level_lo_cut); // Use coarser level. ***BUG*** Wrong level?
              lo_last = encoding->increment(lo_last,level0); 
	      encoding->setId(lo_last);
	      Key tmp_term = encoding->predecessorToLowerBound_NoDepthBit(lo_last,level_lo_cut);
	      DIAGOUT("CP: l_lst,t_term " << FMTX(lo_last) << " " << FMTX(tmp_term));
            }
          }
          uint32 level_lo_last = level0;
        
          // Make the changes
          DIAGOUT("Making changes");
          
          if( lo_cut != lo0 ) {
            // old Key hi0_cutm = encoding->predecessorToLowerBound_NoDepthBit(lo_cut,level_lo_cut);
            // Key hi0_cutm = encoding->predecessorToLowerBound_NoDepthBit((lo_cut & ~31llu)|level_lo_cut,level_lo_cut); // ???
            Key hi0_cutm = encoding->predecessorToLowerBound_NoDepthBit((lo_cut & ~31llu)|level_lo_cut,level0); // ???

            DIAGOUT("CP: 5000 l0..h0ctm     " << FMTX(lo0)      << " " << FMTX(hi0_cutm) << " h0ctm..tag " << FMTX(hi0_cutm) << " " << FMTX(4000000001));
            
            my_los->insert(lo0,hi0_cutm);         // Change l0..h0, l0..hi0_cutm
            my_his->insert(hi0_cutm,4000000001);  // Change l0..h0, l0..hi0_cutm
          } else {
            DIAGOUT("CP: Freeing lo0: " << FMTX(lo0)); 
            my_los->free(lo0); // my_los will be set in the next clause
          }
          
          if( lo_last != lo_cut ) {
            // Add the last segment.
            Key hi0_lastm = encoding->predecessorToLowerBound_NoDepthBit(lo_last,level_lo_last); // ???
            // Key hi0_lastm = encoding->predecessorToLowerBound_NoDepthBit(lo_last,level0); // ???

            DIAGOUT("CP: 5010 loct+..h0lst  " << FMTX(((lo_cut & ~31llu)|level_lo_cut)) << " " << FMTX(hi0_lastm) << " h0lst..tag " << FMTX(hi0_lastm) << " " << FMTX(4000000001));
            
            my_los->insert((lo_cut & ~31llu)|level_lo_cut,hi0_lastm);
            my_his->insert(hi0_lastm,4000000002);
            my_los->insert(lo_last,hi0);
            
          } else {
            // Didn't need last segment.
            
            my_los->insert((lo_cut & ~31llu)|level_lo_cut,hi0);           // Add cut lo_cut..hi0
            
          }
          
          my_his->insert(hi0,4000000003); // Update the tag
          changed = true;
          // my_los, my_his updated
            
        } // end else if "delta" >= 4
      } // else check to see if we have enough to coalesce
    } // end else there's work to do
  } // While not done
#ifdef DIAG
  DIAGOUT("HRML::CompressionPass-----done+");
  my_los->list(cout); my_his->list(cout);
  DIAGOUT("HRML::CompressionPass-----done-");
#endif
} // function def.


int HtmRangeMultiLevel::LOWS = 1;
int HtmRangeMultiLevel::HIGHS = 2;
int HtmRangeMultiLevel::BOTH = 3;
