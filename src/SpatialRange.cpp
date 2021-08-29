/*
 * SpatialRange.cpp
 *
 *  Created on: Sep 8, 2019
 *      Author: mrilee
 *
 *  Copyright (C) 2019 Rilee Systems Technologies LLC
 */

#include "SpatialRange.h"

#define DIAGOUT(x)


SpatialRange::SpatialRange() {
	tree = new HTMSubTree();
}

SpatialRange::SpatialRange(STARE_SpatialIntervals intervals) {
	tree = new HTMSubTree(intervals);
}

SpatialRange::SpatialRange(std::list<STARE_ENCODE> * sids) {
	tree = new HTMSubTree(sids);
}
SpatialRange::~SpatialRange() {
  if(tree != NULL)
	delete tree;
}

void SpatialRange::addSpatialRange(const SpatialRange& range) {
	//this->range->addRange(range.range);
	std::list<STARE_ENCODE> * temp = new std::list<STARE_ENCODE>();
	if(tree->getAllLeaves(range.tree->root, temp)){
        std::list<STARE_ENCODE>::iterator it;
        for (it = temp->begin(); it != temp->end(); ++it){
            tree->addSTAREID(*it);
        }
	}
	temp->clear();
	delete temp;
}

/**
 * Construct a range object (spatial region) from a vector of intervals.
 *
 * TODO: Have a SpatialRange class instead of an HstmRange?
 */
// #define DIAG
//#undef DIAG
void SpatialRange::addSpatialIntervals(STARE_SpatialIntervals intervals) {
	if(!intervals.empty()){
        int size = intervals.size();
        unsigned long long curSID = 0;
        tree->root->isLeaf = false;//start to insert
        for (int i = 0; i < size; i++){
            curSID = intervals[i];
            tree->addSTAREID(curSID);
        }
    }
    else{
        std::cout << "Input Error: The list of STARE values is empty!";
    }
}
//#undef DIAG

// #define DIAG
int SpatialRange::getNextSpatialInterval(STARE_SpatialIntervals &interval) {
	KeyPair kp(-1,-2);
  Key lo_return, hi_return;
	int istat = this->getNext(kp);
#ifdef DIAG
  cout << "\nsr::gnsi start ---" << endl << flush;
  cout << "\nsr::gnsi istat = " << istat << ", kp = " << hex << kp.lo << ", " << kp.hi << dec << endl << flush;
#endif
	if( istat > 0 ) {
		EmbeddedLevelNameEncoding leftJustified;
		leftJustified.setId(kp.lo);
    lo_return = leftJustified.getSciDBLeftJustifiedFormat();
    DIAGOUT("pushing " << hex << lo_return << dec);
		interval.push_back(lo_return);
		if( kp.lo != kp.hi ) {
			STARE_ArrayIndexSpatialValue term_lo = leftJustified.getSciDBTerminatorLeftJustifiedFormat();
			leftJustified.setId(kp.hi); // Note: hi should be a terminator already.
			STARE_ArrayIndexSpatialValue term_hi = leftJustified.getSciDBLeftJustifiedFormat();

#ifdef DIAG
			cout << "lo,term_lo,term_hi: "
					<< setw(16) << setfill('0') << hex
					<< kp.lo << ","
					<< setw(16) << setfill('0') << hex
					<< term_lo << ","
					<< setw(16) << setfill('0') << hex
					<< term_hi << endl << flush << dec;
      cout << "---" << endl << flush;
#endif

			if( term_hi != term_lo ) {
        hi_return = term_hi;
        DIAGOUT("pushing " << hex << hi_return << dec);
				interval.push_back( hi_return );
      }
		}
	}
	return istat;
}
#undef DIAG

STARE_SpatialIntervals SpatialRange::toSpatialIntervals() {
	//STARE_SpatialIntervals intervals;
	//if(this->range) {
	//	this->range->reset();
	//	while( this->getNextSpatialInterval(intervals) > 0 );
	//}
	//return intervals;

	//Return the a vector of stare_ID
	std::list<STARE_ENCODE> *temp = new std::list<STARE_ENCODE>();
	STARE_SpatialIntervals intervals;
	if(tree->getAllLeaves(tree->root, temp)){
		std::list<STARE_ENCODE>::iterator it;
		for(it = temp->begin(); it != temp->end(); it++){
			intervals.push_back(*it);
		}
	}
	return intervals;
	//Return a list of intervals
	//Would need a function in tree->getAllIntervals(tree->root, temp)
}

/*
 * Odd. The following does not seem to work if we just return the SpatialRange itself. Some of the pointers seem to be either corrupted or eliminated.
 */
// SpatialRange sr_intersect(const SpatialRange&a, const SpatialRange& b, bool compress) {
SpatialRange* sr_intersect(const SpatialRange& a, const SpatialRange& b, bool compress) {
	std::cout << "sr_intersect..." << endl;
	std::list<STARE_ENCODE> *temp = a.tree->intersect(b.tree->root);
	SpatialRange *result = new SpatialRange(temp);
	temp->clear();
	delete temp;
	std::cout << "sr_intersect...done" << endl;
	return result;
}


std::list<list<STARE_ENCODE>>* SpatialRange::leftJoin(SpatialRange* sp){
	return tree->leftJoin(sp->tree->root);
}
std::list<list<STARE_ENCODE>>* SpatialRange::innerJoin(SpatialRange* sp){
	return tree->innerJoin(sp->tree->root);
}
std::list<list<STARE_ENCODE>>* SpatialRange::fullJoin(SpatialRange* sp){
	return tree->fullJoin(sp->tree->root);
}
