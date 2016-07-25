#include <iostream> // cout
#include <iomanip>  // setw()
#include <HtmRangeMultiLevel.h>
#include <SpatialIndex.h> // levelOfId

#ifdef _WIN32
#include <stdio.h>
#include <string.h>
#endif

#define INSIDE     1
#define OUTSIDE   -1
#define INTERSECT  0
#define GAP_HISTO_SIZE 10000

using namespace std;
using namespace HtmRangeMultiLevel_NameSpace;

/**
 * Translate an HtmRangeMultiLevel to one at a greater level.  If the desired level is
 * less that the level implicit in the input range (lo & hi), then just return
 * an HtmRangeMultiLevel constructed from the input range without modification.
 * Note: Currently hardcoded for bit-shifted encoding
 * @param htmIdLevel
 * @param lo the low end of the range
 * @param hi the high end of the range
 * @return levelAdaptedRange an HtmRangeMultiLevel
 */
KeyPair HtmRangeMultiLevelAtLevelFromHtmRangeMultiLevel(int htmIdLevel, Key lo, Key hi) {
	// htmIdLevel is used to set maxlevel in the index. aka olevel.
	int levelLo = levelOfId(lo);
	if(levelLo<htmIdLevel) {
		lo = lo << (2*(htmIdLevel-levelLo));
	}
	int levelHi = levelOfId(hi);
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
	//	cout << "Comparing..." << endl << flush;
	HtmRangeMultiLevel *range1 = this; // Rename to use existing code. TODO rename to this.
	if((!range1)||(!range2)) return 0;
	if((range1->nranges()<=0)||(range2->nranges()<=0)) return 0;
	HtmRangeMultiLevel *resultRange = new HtmRangeMultiLevel();
	resultRange->purge();
	Key lo1,hi1,lo2,hi2;
	range1->reset();
	uint64 indexp1 = range1->getNext(lo1,hi1);
	if (!indexp1) return 0;

	if(htmIdLevel<0) {
		htmIdLevel = levelOfId(lo1);
	}

	//	cout << "indexp1: " << indexp1 << endl << flush;
	//	cout << "l,lo,hi1: " << htmIdLevel << " " << lo1 << " " << hi1 << endl << flush;
	//	cout << "a" << flush;
	do {
		//		cout << "b" << endl << flush;
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

// Note the default use of EmbeddedLevelNameEncoding.
HtmRangeMultiLevel::HtmRangeMultiLevel() : HtmRangeMultiLevel(new EmbeddedLevelNameEncoding()) {}

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
	Key GH_a, GL_a, SH_a, SL_a;
	Key GH_b, GL_b, SH_b, SL_b;
	Key param[8];
	int i;
	int rstat = 0;
	i = 0;
	param[i++] = GL_a = my_los->findMAX(a);
	param[i++] = GH_a = my_his->findMAX(a);

	param[i++] = SL_a = my_los->findMIN(a);
	param[i++] = SH_a = my_his->findMIN(a);


	param[i++] = GL_b = my_los->findMAX(b);
	param[i++] = GH_b = my_his->findMAX(b);

	param[i++] = SL_b = my_los->findMIN(b);
	param[i++] = SH_b = my_his->findMIN(b);

	/*
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

	while((a = o.my_los->getkey()) > 0){
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

TInsideResult HtmRangeMultiLevel::tinside(const Key mid) const
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
void HtmRangeMultiLevel::mergeRange(const Key lo, const Key hi)
{

	errorCount = 0;
	// TODO Main routine to modify to enable multi-level ranges.
//	cout << "8000 " << hex
//			<< "0x"	<< setfill('0') << setw(16) << lo
//			<< ", 0x" << setfill('0') << setw(16) << hi
//			<< endl << flush;
//	cout << "8001 " << dec << "  " << lo << ",   " << hi << endl << flush;

	// Add the first one.
	if( my_los->myHeader->getElement(0) == NIL ) {
		my_los->insert(lo,100);
		my_his->insert(hi,100);
		return;
	}

//	cout << "8000-400" << endl << flush;

	TInsideResult loFlag = tinside(lo);
	int lo_flag = loFlag.incl;

	TInsideResult hiFlag = tinside(hi);
	int hi_flag = hiFlag.incl;

	// TODO I think loFlag and hiFlag can help with the logic below.

//	cout << "8000-500" << endl << flush;
	// New stuff.

	uint32 level = encoding->levelById(lo);

//	cout << "8000-600" << endl << flush;

	// Possible values:
	//   loFlag:  InclInside, InclLo, InclHi, InclOutside
	//   hiFlag:  InclInside, InclLo, InclHi, InclOutside
    // Note: InclHi & InclLo imply a boundary point.

	// Iterate over the intervals ourselves.
	my_los->reset();
	my_his->reset();

//	cout << "8000-700" << endl << flush;

	Key lo1 = lo, hi1 = hi;
	Key l, h;
	bool done = false;
//	cout << "8000-1000-while" << endl << flush;
	while(((l = my_los->getkey()) >= 0) && (not done) ){
		h = my_his->getkey();
		uint32 l_level = encoding->levelById(l);

//		cout << "8000-1001-lo1,hi1: " << lo1 << " " << hi1 << " " << level << endl << flush;
//		cout << "8000-1002-    l,h: " << l << " " << h << " " << l_level << endl << flush;
//#define hexOut(a,b,c) cout << a << " 0x" << hex << setfill('0') << setw(16) << b << ".." << c << dec << endl << flush;
////		cout << endl << flush;
//		hexOut("lh1 ",lo1,hi1);
//		hexOut("lh  ",l,h);
//#undef hexOut
		if ( h < lo1 ) {
//			cout << "Don't know what's above h. Iterate." << endl << flush;
//			errorCount++; if(errorCount>errorCountMax) {
//				cout << "HRML::Iterate::errorCount" << endl << flush;
//				exit(1);
//			}
			my_los->step(); my_his->step();
			// Don't know what's above h.  Iterate.
		} else if( hi1 < l ) {
			// Case 1. A is below B.  Just add
			my_los->insert(lo1,10001);
			my_his->insert(hi1,10001);
			done = true;
		} else if( (lo1 < l) && ( (l <= hi1) && (hi1 <= h) ) ) {
			// Case 2. A.lo is below B.lo, but A.hi is in B.
//			cout << "8000-103x" << endl << flush;
			// The upper end of the new interval overlaps the current one, but not its lower part. Do surgery.
			// Bounds of the lower part below the current interval.
			Key l_m = lo1;
			Key h_m = encoding->predecessorToLowerBound_NoDepthBit(l,level); // l-
			// Bounds of the overlap.
			Key l_0 = l;
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
//				cout << "8000-1030" << endl << flush;
				// At the same level, merge the two.
				my_los->freeRange(l_m,h_p); // Freeing up to h_p is okay because h_p==h is still part of current interval.
				my_his->freeRange(l_m,h_p);
				my_los->insert(l_m,100021);
				my_his->insert(h_p,100021);
			} else {
				// The lower part overlaps an empty part. Just add.  // ??? Don't need a freeRange ??? Okay...
				if(level > l_level) {
//					cout << "8000-1031" << endl << flush;
					// If the new interval's level is greater, just skip in the current, add before.
					my_los->insert(l_m,100022);
					my_his->insert(h_m,100022);
				} else if(true) { // Case 2.3 level < l_level -- new interval wins
					// TODO WORRY -- What about collisions? If we have a collision, will we simply put in the value back in?
					// If the new level is lower than the current, then the new lower level wins. Current is trimmed.
					// Rewrite the current.
//					cout << "8000-1032" << endl << flush;
//					if(l_m == 0x7500000000000002) {
//#define hexOut(a,b,c) cout << a << " 0x" << hex << setfill('0') << setw(16) << b << ".." << c << dec << endl << flush;
//						hexOut("lm-hm ",l_m,h_m);
//						hexOut("l0-h0 ",l_0,h_0);
//						hexOut("lp-hp ",l_p,h_p);
//						cout << " h_0 " << h_0 << endl << flush;
//#undef hexOut
//					}
					my_los->freeRange(l_m,h_p);
					my_his->freeRange(l_m,h_p);
					my_los->insert(l_m,100023); // lo1 // this changes the level to level
					my_his->insert(h_0,100023); // hi1
					if(h_0 < h_p) {
						my_los->insert(l_p,100023);
						my_his->insert(h_p,100023);
					}
					my_los->reset(); my_his->reset();
				}
			}
			// Done!
			done = true;
		} else if( (lo1 < l) && (h < hi1) ) {
//			cout << "HRML::Case 4.2/4.4 A covers B." << endl << flush;
			// Case 4.2 A covers B.
			// Test.cpp 4.4.1
			if( level <= l_level ) { // New interval wins.
//				cout << "4.2.2" << endl << flush;
				// Case 4.2.2
				my_los->freeRange(l,h);
				my_his->freeRange(l,h);
				// Keep lo1 and h1 and try again.
				// Where does the iterator go?
//				cout << "4.2.2 end" << endl << flush;
			} else if( level > l_level ) { // Current interval wins.
//				cout << "HRML::4.2.1" << endl << flush;
				// Case 4.2.1
				Key l_m = lo1;
				Key h_m = encoding->predecessorToLowerBound_NoDepthBit(l,level); // TODO Verify no gaps
				Key l_0 = l;
				Key h_0 = h;
				Key l_p = encoding->successorToTerminator_NoDepthBit(h,level); // TODO Verify no gaps
				Key h_p = hi1;
				if( l_p > h_p ) {
					cout << "HtmRangeMultiLevel::mergeRange::ERROR!!! SUCC(H) > HI1, I.E. THEY'RE EQUIVALENT." << endl << flush;
				}
				if( l_m < h_m ) { // If lo1 and l are equivalent, current one wins, and we ignore the new one.
					my_los->insert(l_m,1000421);
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
//			cout << "HRML::Case 4. A is in B." << endl << flush;
			// Case 4. A is in B.
			if( level >= l_level ) {
				// Ignore. Triangles already included.
				done = true;
			} else { // level < l_level
				if(true){
				// Test.cpp 4.3 Collision at the end.
				Key l_m = l;
				Key h_m = encoding->predecessorToLowerBound_NoDepthBit(lo1,l_level);
//				if(l_m>h_m) cout << "HtmRangeMultiLevel::mergeRange::WARNING!!! 4 PRED WARNING. BOUNDS EQUIVALENT" << endl << flush;
				Key l_0 = lo1;
				Key h_0 = hi1;
				Key l_p = encoding->successorToTerminator_NoDepthBit(hi1,l_level);
				Key h_p = h;
//				if(l_p>h_p) cout << "HtmRangeMultiLevel::mergeRange::WARNING!!! 4 SUCC WARNING. BOUNDS EQUIVALENT" << endl << flush;
				// Blow away the old and add the three.
				my_los->freeRange(l_m,h_p);
				my_his->freeRange(l_m,h_p);
				if( l_m < h_m ) { // If l_m and l_0 are "equivalent", so the current interval wins and we ignore interval_m.
					my_los->insert(l_m,10004);
					my_his->insert(h_m,10004);
				}
				my_los->insert(l_0,10004);
				my_his->insert(h_0,10004); // TODO Subtle bug?  Need to verify edge case.
				if( l_p < h_p ) { // Non equivalent h_0 and h_p.
					my_los->insert(l_p,10004);
					my_his->insert(h_p,10004);
				}
				}
				done = true;
			}
		} else if ( (l <= lo1) && (h < hi1) ) { // Note the case h < lo1 is handled above.
			// Case 5.
//			cout << "HRML::Case 5" << endl << flush;
			if ( level >= l_level ) {
				// Case 5.1
//				cout << "HRML::Case 5.1" << endl << flush;
				Key l_m = l; // Note:  no need to add...
				Key h_m = h;
				Key l_p = encoding->successorToTerminator_NoDepthBit(h,level); // Might map l_p > h_p.
				Key h_p = hi1;
				if( l_p > h_p ) {
					l_p = h; // Abandoning the level bits for the moment.
					uint64 l__ = encoding->decrement(l_p,level);
					if (l__ < l) cout << "huh?" << endl << flush;
					l_p = l__;
					lo1 = l_p;
					hi1 = h_p;
					return; // without stepping.  Try again.
				}
//				cout << "m " << l_m << " " << h_m << endl << flush;
////				cout << "0 " << l_0 << " " << h_0 << endl << flush;
//				cout << "p " << l_p << " " << h_p << endl << flush;
//				cout << hex << "m " << l_m << " " << h_m << dec << endl << flush;
////				cout << "0 " << l_0 << " " << h_0 << endl << flush;
//				cout << hex << "p " << l_p << " " << h_p << dec << endl << flush;
//				// cout << "? " << encoding->successorToTerminator_NoDepthBit(h,l_level) << endl << flush;
//				// update for iteration
				errorCount++; if(errorCount>errorCountMax) {
					cout << "HRML::5.1::errorCount" << endl << flush;
					exit(1);
				}
				lo1 = l_p;
				hi1 = h_p;
				my_los->step(); my_his->step(); // Didn't change skiplists.
			} else if ( level < l_level ) {
				if(true){
//				cout << "HRML::Case 5.2" << endl << flush;
				Key l_m = l;
				Key h_m = encoding->predecessorToLowerBound_NoDepthBit(lo1,l_level);
				Key l_0 = lo1;
				Key h_0 = h; // But need to adjust to avoid creating holes. // Note this has the wrong level compared to l_0.
				Key l_p = encoding->successorToTerminator_NoDepthBit(h,level);
				h_0 = encoding->predecessorToLowerBound_NoDepthBit(l_p,level); // level is the level of lo1
				Key h_p = hi1;
				// Blow away the old and add two and defer the third.
				my_los->freeRange(l_m,h_0);
				my_his->freeRange(l_m,h_0);
				// TODO NOTE:  When predecessor is used, you have to check to see if pred(b) is less than inf(interval).
				if( l_m < h_m ) { // If l_m and l_0 are "equivalent", so the current interval wins and we ignore interval_m.
					my_los->insert(l_m,100052);
					my_his->insert(h_m,100052);
				}
				my_los->insert(l_0,100052);
				my_his->insert(h_0,100052); // TODO Subtle bug?  Need to verify edge case.
				// update for iteration
				lo1 = l_p;
				hi1 = h_p;
				my_los->reset(); my_his->reset();
				}
//				done = true;
			}
		}  else {
			cout << "8000-9999 ERROR" << endl << flush;
			exit(1);
		}
	}

	if(not done) {
		// The new interval goes at the end of the skiplists.
		// Case 6. We're at the top.  Just add.
		my_los->insert(lo1,10006);
		my_his->insert(hi1,10006);
		done = true;
	}

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
 * @param lo
 * @param hi
 */
void HtmRangeMultiLevel::addRange(const Key lo, const Key hi)
{
//	my_los->insert(lo, (Value) 0); // TODO Consider doing something useful with (Value)...
//	my_his->insert(hi, (Value) 0);

	// TODO Simplest thing that might possibly work.
	mergeRange(lo,hi);
	return;
}

/**
 * Merge a range into this by looping over sub ranges and merging them
 * one at a time.
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
	cout << "HtmRangeMultiLevel::defrag::NOT-IMPLEMENTED EXITING" << endl << flush;
	exit(1);

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
}

void HtmRangeMultiLevel::defrag()
{
#define hexOut(a,b,c) cout << a << " 0x" << hex << setfill('0') << setw(16) << b << ".." << c << dec << endl << flush;

	if(nranges()<2) return;

	Key lo0, hi0;
	Key lo1, hi1;
	Key save_key;
	uint32 level0, level1;
	my_los->reset();
	my_his->reset();

	// compare lo at i+1 to hi at i.
	// so step lo by one before anything
	//
	lo0 = my_los->getkey(); level0 = encoding->levelById(lo0);
	my_los->step();
	while((lo1 = my_los->getkey()) >= 0){
		level1 = encoding->levelById(lo1);
		hi0 = my_his->getkey();
//		hexOut("lh0 ",0,hi0);
//		hexOut("lh1 ",lo1,0);
//		cout << "level 0,1 " << level0 << " " << level1 << endl << flush;
		// cout << "compare " << hi << "---" << lo << endl;

		if( level0 == level1 ) { // Same level. Maybe merge?
//			cout << "levels equal" << endl << flush;
			Key hi0_pred = encoding->predecessorToLowerBound_NoDepthBit(lo1,level0);
			Key lo1_succ = encoding->successorToTerminator_NoDepthBit(hi0,level1);
//			hexOut("hi0_pred,hi0",hi0_pred,hi0);
//			hexOut("lo1_succ,lo1",lo1_succ,lo1);
			if( hi0_pred <= hi0 ) { // If the pred includes the actual...???
//				cout << "merging" << endl << flush;
				// Let's merge the intervals. Delete hi0 and lo1.
				my_los->step();  // Step to the next lo
				save_key = my_los->getkey();
				my_los->free(lo1);
				if( save_key >= 0 ) {
					my_los->search(save_key,1); // reset iter for stepping // TODO Maybe use this in  mergeRange
				}
				my_his->step();
				save_key = my_his->getkey();
				my_his->free(hi0);
				if( save_key >= 0 ) {
					my_his->search(save_key,1);
				}
			} else {

				my_los->step();
				my_his->step();
			}
		} else {
			my_los->step();
			my_his->step();
		}
		level0 = level1;

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

	// cout << "DONE looping"  << endl;
	// my_los->list(cout);
	// my_his->list(cout);
}

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
int HtmRangeMultiLevel::nranges()
{
//	cout << "z000" << endl << flush;
	Key lo, hi;
	int n_ranges;
	n_ranges = 0;
	my_los->reset();
	my_his->reset();
//	cout << "z010" << endl << flush;

	while((lo = my_los->getkey()) > 0){
		n_ranges++;
//		cout << "z020 " << n_ranges << flush;
		hi = my_his->getkey();
//		cout << " : " << lo << ", " << hi << " " << flush << endl;
		my_los->step();
		my_his->step();
	}

//	cout << "z100" << endl << flush;

	return n_ranges;
}

//
// return the smallest gapsize at which rangelist would be smaller than
// desired size
//
Key HtmRangeMultiLevel::bestgap(Key desiredSize)
{
	SkipList sortedgaps(SKIP_PROB);
	Key gapsize;
	Key key;
	Value val;

	Key lo, hi, oldhi = 0, del;

	int n_ranges, left_ranges;

	n_ranges = 0;
	my_los->reset();
	my_his->reset();

	while((lo = my_los->getkey()) > 0){
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

	while((lo = my_los->getkey()) > 0){
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

std::ostream& operator<<(std::ostream& os, const HtmRangeMultiLevel& range)
{
	char tmp_buf[256];
	Key lo, hi;
	// os << "Start Range " << endl;
	range.my_los->reset();
	range.my_his->reset();
	while((lo = range.my_los->getkey()) > 0){
		hi = range.my_his->getkey();
		if (range.symbolicOutput){
			strcpy(tmp_buf,range.encoding->nameById(lo));
			strcat(tmp_buf," ");
			strcat(tmp_buf,range.encoding->nameById(hi));
			os << tmp_buf;
		} else {
#ifdef _WIN32
			sprintf(tmp_buf, "%I64d %I64d ", lo, hi);
			os << tmp_buf;
#else
			os << lo << " " << hi;
			// sprintf(tmp_buf, "%llu %llu ", lo, hi);
			// sprintf(tmp_buf, "%llu %lld ", lo, hi);
#endif
		}
		// os << tmp_buf << endl; // TODO MLR Why gratuitously add an EOL here?
		// os << tmp_buf;

		// os << lo << " " << hi << endl;
		range.my_los->step();
		range.my_his->step();
	}
	// os << "End Range ";
	return os;
}

int HtmRangeMultiLevel::getNext(Key &lo, Key &hi)
{
//	cout << "a" << flush;
	lo = my_los->getkey();
	if (lo <= (Key) 0){
		hi = lo = (Key) 0;
		return 0;
	}
//	cout << "b" << flush;
//	cout << " " << lo << " " << flush;
	hi = my_his->getkey();
//	cout << " " << hi << " " << flush;
	if (hi <= (Key) 0){
		cout << endl;
		cout << " getNext error!! " << endl << flush;
		hi = lo = (Key) 0;
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
	if (*lo <= (Key) 0){
		*hi = *lo = (Key) 0;
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

	while((lo = my_los->getkey()) > 0){
		hi = my_his->getkey();
		if (what != BOTH) {
			if (symbolic){
				strcpy(tmp_buf,encoding->nameById(what == LOWS ? lo : hi));
			} else {
#ifdef _WIN32
				sprintf(tmp_buf, "%I64d", what == LOWS ? lo : hi);
#else
				sprintf(tmp_buf, "%llu", what == LOWS ? lo : hi);
#endif
			}
		} else {
			if (symbolic){
				strcpy(tmp_buf,encoding->nameById(lo));
				strcat(tmp_buf,"..");
				strcat(tmp_buf,encoding->nameById(hi));
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

int HtmRangeMultiLevel::LOWS = 1;
int HtmRangeMultiLevel::HIGHS = 2;
int HtmRangeMultiLevel::BOTH = 3;