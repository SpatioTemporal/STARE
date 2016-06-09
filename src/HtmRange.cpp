#include <iostream> // cout
#include <iomanip>  // setw()
#include <HtmRange.h>
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

/**
 * Translate an HtmRange to one at a greater level.  If the desired level is
 * less that the level implicit in the input range (lo & hi), then just return
 * an HtmRange constructed from the input range without modification.
 * Note: Currently hardcoded for bit-shifted encoding
 * @param htmIdLevel
 * @param lo the low end of the range
 * @param hi the high end of the range
 * @return levelAdaptedRange an HtmRange
 */
KeyPair HTMRangeAtLevelFromHTMRange(int htmIdLevel, Key lo, Key hi) {
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
HtmRange *HtmRange::HTMRangeAtLevelFromIntersection(HtmRange *range2, int htmIdLevel){
	//	cout << "Comparing..." << endl << flush;
	HtmRange *range1 = this; // Rename to use existing code. TODO rename to this.
	if((!range1)||(!range2)) return 0;
	if((range1->nranges()<=0)||(range2->nranges()<=0)) return 0;
	HtmRange *resultRange = new HtmRange();
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
		KeyPair testRange1 = HTMRangeAtLevelFromHTMRange(htmIdLevel,lo1,hi1);
		range2->reset();
		uint64 indexp2 = range2->getNext(lo2,hi2);
		if(!indexp2) return 0;
		//		cout << "indexp2: " << indexp2 << endl << flush;
		//		cout << "l,lo,hi2: " << htmIdLevel << " " << lo2 << " " << hi2 << endl << flush;
		bool intersects = false;
		do {
			//			cout << "c" << endl << flush;
			KeyPair testRange2 = HTMRangeAtLevelFromHTMRange(htmIdLevel,lo2,hi2);
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


HtmRange::HtmRange() : HtmRange(new BitShiftNameEncoding()) {}

HtmRange::HtmRange(NameEncoding *encoding) {
	this->encoding = encoding;
	my_los = new SkipList(SKIP_PROB);
	my_his = new SkipList(SKIP_PROB);
	symbolicOutput = false;
}

/**
 * Set numeric or symbolic (string) output for true/false respectively.
 * @param flag
 */
void HtmRange::setSymbolic(bool flag)
{
	symbolicOutput = flag;
}

/**
 * Compare the number of intervals stored.
 * @param other HtmRange
 * @return true if this and the other store the same number of intervals.
 */
int HtmRange::compare(const HtmRange & other) const
{
	int rstat = 0;
	if (my_los->getLength() == other.my_los->getLength()){
		rstat = 1;
	}
	return rstat;
}

int HtmRange::isIn(Key a, Key b)
{

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
			else if (param[i] == -KEY_MAX)
				cout << " -MAX";
			else
				cout << setw(5) << param[i];
		}
		cout << ")" << endl;

	// 0 is intersect, -1 is out +1 is inside
	}

	if(GH_a < GL_a && GL_b < GH_b){ // a is in, b is not. TODO What about +/- MAX?
		rstat = 0;
		if(dbg) cout << " <<<<< X (0), GH_a < GL_a && GL_b < GH_b" << endl;
	} else if (GL_b == a && SH_a == b){
		rstat = 1;
//		if(dbg) cout << " <<<<< I (1), because SH_a == a and GL_b == b perfect match" << endl; // bug? error?
		if(dbg) cout << " <<<<< I (1), because SH_a == b and GL_b == a perfect match" << endl; // correction?
	} else if (GL_b > GL_a) {
		// kluge
		if( GL_b == a && SH_a >= b ) {
			rstat = 1;
			if(dbg) cout << " <<<<< I (1), because GL_b = a > GL_a && SH_a >= b" << endl;
		} else {
			rstat = 0;
			if(dbg) cout << " <<<<< X (0), because GL_b > GL_a" << endl;
		}
	} else if (GH_a < GL_a) {
		rstat = 1;
		if(dbg) cout << " <<<<< I (1), because GH_a < GL_a, and none of previous conditions" << endl;
	} else if (SL_a == b) {
		rstat = 0;
		if(dbg) cout << " <<<<< X (0), b coincides " << endl;
	} else {
		rstat = -1;
		if(dbg) cout << " <<<<< O (-1), because none of the above" << endl;
	}

	return rstat;
}

int HtmRange::isIn(Key key)
{
	InclusionType incl;
	int rstat = 0;
	incl = tinside(key);
	rstat = (incl != InclOutside);
	return rstat;
}
int HtmRange::isIn(HtmRange & otherRange)
{
	HtmRange &o = otherRange;
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

int HtmRange::contains(Key a, Key b) {
	HtmRange compare;
	compare.addRange(a,b);
	// int rstat = compare.isIn(*this);  // 0 is intersect, -1 is out +1 is inside // TODO What isIn what?
	// The following seems to work, but is somewhat inscrutable.
	int rstat = this->isIn(compare);  // 0 is intersect, -1 is out +1 is inside
	//	cout << " contains:  a,b,rstat " << a << " " << b << " " << rstat << endl << flush;
	if (rstat == 0) { rstat = -1; }
	else if (rstat == -1) { rstat = 0; }
	return rstat; // 0 is no-intersection, -1 is partial, 1 is full
}

InclusionType HtmRange::tinside(const Key mid) const
{
	// clearly out, inside, share a bounday, off by one to some boundary
	InclusionType result, t1, t2;
	Key GH, GL, SH, SL;
	// cout << "==========" << setw(4) << mid << " is ";

	GH = my_his->findMAX(mid);
	GL = my_los->findMAX(mid);

	if (GH < GL) { // GH < GL
		t1 = InclInside;
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
	// cout << " GH = " << my_his->findMAX(mid) << " GL = " << my_los->findMAX(mid);
	// cout << " SH = " << my_his->findMIN(mid) << " SL = " << my_los->findMIN(mid);

	// cout << endl;
	if (t1 == t2){
		result = t1;
	} else {
		result = (t1 == InclInside ? InclHi : InclLo);
	}

	//   if (result == InclOutside){
	//     if ((GH + 1 == mid) ||  (SL - 1 == mid)){
	//       result = InclAdjacent;
	//     }
	//   }
	return result;
}

void HtmRange::mergeRange(const Key lo, const Key hi)
{

	int lo_flag = tinside(lo);
	int hi_flag = tinside(hi);

	// delete all nodes (key) in his where lo < key < hi
	// delete all nodes (key) in los where lo < key < hi

	my_his->freeRange(lo, hi);
	my_los->freeRange(lo, hi);
	//  cout << "After freeRange" << endl;
	// my_los->list(cout);
	// my_his->list(cout);

	//
	// add if not inside a pre-existing interval
	//
	if (lo_flag == InclHi) {
		//cerr << "Do not Insert " << setw(20) << lo << " into lo" << endl;
	} else if (lo_flag == InclLo ||
			(lo_flag ==  InclOutside)
	){
		//cerr << "Insert        " << setw(20) << lo << " into lo" << endl;
		my_los->insert(lo, 33);
	}

	//   else {
	//     cerr << "Do not Insert " << setw(20) << lo << " into lo (other)" << endl;
	//   }

	if (hi_flag == InclLo){
		// cerr << "Do not Insert " << setw(20) << hi << " into hi" << endl;
		// my_his->insert(lo, 33);
	}
	else if (hi_flag == InclOutside ||
			hi_flag == InclHi) {
		// cerr << "Insert        " << setw(20) << hi << " into hi" << endl;
		my_his->insert(hi, 33);
	}
	//   else {
	//     cerr << "Insert        " << setw(20) << hi << " into hi (other) " << endl;
	//   }
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
void HtmRange::addRange(const Key lo, const Key hi)
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
void HtmRange::addRange(HtmRange *range) {
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


void HtmRange::defrag(Key gap)
{
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

void HtmRange::defrag()
{
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

		if (hi >= lo) { // TODO MLR Change... ???
//		if (hi + 1 == lo){ // TODO Original code, seems such a rare case!!!
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

/// Free the range los and his.
void HtmRange::purge()
{
	my_los->freeRange(-1, KEY_MAX);
	my_his->freeRange(-1, KEY_MAX);
}

/// Reset the lo and hi lists to their zeroth element.
void HtmRange::reset()
{
	my_los->reset();
	my_his->reset();

}
//
// return the number of ranges
//
/// The number of ranges.
int HtmRange::nranges()
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
Key HtmRange::bestgap(Key desiredSize)
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

int HtmRange::stats(int desiredSize)
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

std::ostream& operator<<(std::ostream& os, const HtmRange& range)
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

int HtmRange::getNext(Key &lo, Key &hi)
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
int HtmRange::getNext(Key *lo, Key *hi)
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
KeyPair HtmRange::getNext() {
	KeyPair key;
	int indexp = getNext(key.lo,key.hi);
	key.set = indexp;
	return key;
}

void HtmRange::print(int what, std::ostream& os, bool symbolic)
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

int HtmRange::LOWS = 1;
int HtmRange::HIGHS = 2;
int HtmRange::BOTH = 3;
