/*
 * SpatialRange.h
 *
 *  Created on: Sep 8, 2019
 *      Author: mrilee
 *
 *  Copyright (C) 2019 Rilee Systems Technologies LLC
 */

#ifndef INCLUDE_SPATIALRANGE_H_
#define INCLUDE_SPATIALRANGE_H_

#include "STARE.h"
#include "HstmRange.h"
#include "HTMSubTree.h"

/**
 * A wrapper for an HstmRange that knows about STARE to provide htm-range-like functions.
 *
 */
class SpatialRange {
public:
	SpatialRange(); 
	SpatialRange(STARE_SpatialIntervals intervals); 
	SpatialRange(HTMSubTree *tree){ this->tree = tree;} 
	SpatialRange(std::list<STARE_ENCODE> *sids);
	virtual ~SpatialRange(); 

	void addSpatialIntervals(STARE_SpatialIntervals intervals);
	void addSpatialRange(const SpatialRange& r);

	STARE_SpatialIntervals toSpatialIntervals();
	int getNextSpatialInterval(STARE_SpatialIntervals &interval);//Don't need this

	bool contains(STARE_ArrayIndexSpatialValue siv) {
		return tree->isContain(siv);
	}

	void print(){
		tree->printTree();
	}

	bool intersects(STARE_ArrayIndexSpatialValue siv) {
		return tree->isIntersect(siv);
	}

	HTMSubTree *tree;

    std::list<list<STARE_ENCODE>>* leftJoin(SpatialRange* sp);
    std::list<list<STARE_ENCODE>>* innerJoin(SpatialRange* sp);
    std::list<list<STARE_ENCODE>>* fullJoin(SpatialRange* sp);

	//Mike suggested to remove these functions
	int  getNext(KeyPair &kp) {}
	void reset() {}
	void purge() { print();	} 
	void defrag() {} 
	void compress() {}
	//////////////////////////////////////////////

#define FMTX(x) " 0x" << setw(16) << hex << x << dec	
	void dump() {
	  EmbeddedLevelNameEncoding leftJustified;
	  KeyPair kp;
	  int k=0;
	  STARE_SpatialIntervals sivs;
	  this->reset();
	  while(this->getNextSpatialInterval(sivs)) {
	    /*
	    STARE_SpatialIntervals sivs = spatialIntervalFromHtmIDKeyPair(kp);
	    leftJustified.setId(BitShiftNameEkp.lo);
	    uint64 siv_lo = leftJustified.getSciDBLeftJustifiedFormat();
	    leftJustified.setId(kp.hi);
	    uint64 siv_hi = leftJustified.getSciDBLeftJustifiedFormat();
	    */

	    uint64 sivs_lo = sivs[0];
	    cout << k++ << " "
		 << FMTX(sivs_lo) << " ";
	    if( sivs.size() == 2 ) {
	      uint64 sivs_hi = sivs[1];
	      cout << FMTX(sivs_hi);
	    }
	    cout << endl << flush;
	    sivs.clear();
	  }

	  cout << endl << flush;
	  cout << "---expand---" << endl << flush;

	  k = 0;
	  this->reset();
	  while(this->getNextSpatialInterval(sivs)) {
	    /*
	    STARE_SpatialIntervals sivs = spatialIntervalFromHtmIDKeyPair(kp);
	    leftJustified.setId(BitShiftNameEkp.lo);
	    uint64 siv_lo = leftJustified.getSciDBLeftJustifiedFormat();
	    leftJustified.setId(kp.hi);
	    uint64 siv_hi = leftJustified.getSciDBLeftJustifiedFormat();
	    */

	    STARE_ArrayIndexSpatialValues vals = expandInterval(sivs);

	    for( int l=0; l < vals.size(); ++l ) {
	      cout << k++ << " " << FMTX(vals[l]) << endl;
	    }
	    vals.clear();
	    sivs.clear();
	  }
	}
};

SpatialRange* sr_intersect(const SpatialRange& a, const SpatialRange& b, bool compress = false);

inline SpatialRange* operator& ( const SpatialRange& a,  const SpatialRange& b) {
	return sr_intersect(a,b);
}
void SpatialRange_test();


#endif /* INCLUDE_SPATIALRANGE_H_ */
