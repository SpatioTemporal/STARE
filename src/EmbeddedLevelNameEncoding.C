/*
 * EmbeddedLevelNameEncoding.C
 *
 *  Created on: Jan 29, 2016
 *      Author: mrilee
 */

#include "EmbeddedLevelNameEncoding.h"
#include <string>
#include <iomanip>
#include <iostream>

EmbeddedLevelNameEncoding::EmbeddedLevelNameEncoding() {}

EmbeddedLevelNameEncoding::~EmbeddedLevelNameEncoding() {}

/**
 * Return the symbolic (string) name associated with the id in left-justified format.
 *
 *
 * @param id
 * @return
 */
char* EmbeddedLevelNameEncoding::nameById(uint64 id) {
	if(id == 0) {
		// Throw an exception?
		throw SpatialFailure("EmbeddedLevelNameEncoding::nameById-INVALID_ID_0");
	}

	int nameSize = levelById(id)+3; ///< levelById is local to the encoding
	char *returnedName = new char[nameSize];
	if(id & NorthSouthBit) {
		returnedName[0] = 'N';
	} else {
		returnedName[0] = 'S';
	}
	for(uint64 i=1;i<nameSize-1;i++) {
		int c = '0' + (int) ((id >> (62 - 2*i)) & (uint32) 3);
		returnedName[i] = (char) c;
	}
	returnedName[nameSize-1] = 0; // End string
	return returnedName;
}

uint64 EmbeddedLevelNameEncoding::idByName(const char* name) {
	uint64 out=0,i;
	size_t siz =0;

	if(name == 0)              // null pointer-name
		throw SpatialFailure("EmbeddedLevelNameEncoding:idByName-NullPointerName");
//		return 0;
	if(name[0] != 'N' && name[0] != 'S')  // invalid name
		throw SpatialFailure("EmbeddedLevelNameEncoding:idByName-InvalidName");

	siz = strlen(name);

	// at least size-2 required, don't exceed max
	if(siz < 2)
		throw SpatialFailure("EmbeddedLevelNameEncoding:idByName-NameTooSmall");

	if(siz > HTMNAMEMAX)
		throw SpatialFailure("EmbeddedLevelNameEncoding:idByName-NameTooBig");

	for(i=1;i<siz;i++) {
		if(name[i]>'3' || name[i] <'0') {
			throw SpatialFailure("EmbeddedLevelNameEncoding:idByName-InvalidDigit");
		}
		// 		out += ( (uint64)(name[i]-'0')) << 2*(siz - i -1);
		// Maybe the 32 below should be set to some sort of MAXLEVEL, like HTMNAMEMAX.
		// or perhaps getting up to HTMMAXBIT==64 or IDSIZE==64.
		out += ( (uint64)(name[i]-'0')) << ( 2*(32-i)-2 );
	}

	i = 2;
	if(name[0]=='N') i++;
	out += (i << 62);
	out += levelByName(name);
	return out;
}

uint32 EmbeddedLevelNameEncoding::levelById(uint64 id) const {
	// Should we throw an exception if id is 0?
	uint64 level = id & levelMask;
	return level;
}

/**
 * Returns a right justified (old-style) HTM id, without a depth bit. // TODO Verify
 */
uint64 EmbeddedLevelNameEncoding::bareId() const {
	// Should this be the old style id?
	uint64 stripId = id & stripMask;
	uint64 shiftId = stripId >> (60 - 2*(levelById(id)));
	return shiftId;
}

int EmbeddedLevelNameEncoding::getLocalTriangleNumber() const {
	uint64 ret = ( bareId() & 3 );
	return ret;
}

uint64 EmbeddedLevelNameEncoding::bareId_NoShift_NoEmbeddedLevel() const {
	// TODO This is left justified. Is bareId left or right justified?
	uint64 stripId = id & stripMask;
	return stripId;
}

uint64 EmbeddedLevelNameEncoding::getId_NoEmbeddedLevel() const {
	uint64 id = getId();
	return id & ~levelMask; // Only remove level.
//	return id & stripMask;
}

uint64 EmbeddedLevelNameEncoding::maskOffLevelAndLevelBit(uint64 id) const {
	return id & stripMask;
}

uint64 EmbeddedLevelNameEncoding::maskOffLevelAndLevelBit() const {
	uint64 id = getId();
	return id & stripMask;
}


uint64 EmbeddedLevelNameEncoding::maskOffLevel(uint64 id) {
	return id & (~levelMask);
}

uint64 EmbeddedLevelNameEncoding::maskOffLevel() {
	return getId_NoEmbeddedLevel();
}

uint64 EmbeddedLevelNameEncoding::maskOffLevelBit(uint64 id) const {
	return id & stripLevelBitMask;
}

uint64 EmbeddedLevelNameEncoding::maskOffLevelBit() const {
	uint64 id = getId();
	return id & stripLevelBitMask;
}

uint64 EmbeddedLevelNameEncoding::getIdTerminator_NoDepthBit() const {
	return getIdTerminator_NoDepthBit(
			this->maskOffLevelAndLevelBit(),
			this->levelById(this->id));
}

uint64 EmbeddedLevelNameEncoding::getIdTerminator_NoDepthBit(uint64 b, uint32 aLevel) const {
	uint64 one_mask_to_level = 0;
	for(uint64 shift = 2;
			shift <= ( (topBitPosition-3) - 2*aLevel);
			shift +=2 ) {
		one_mask_to_level = one_mask_to_level << 2;
		one_mask_to_level += 3;
	}
	uint64 terminator = b | one_mask_to_level; // Watch out -- b may have stuff in the wrong place. So use bitor and pray.
	// terminator +=  one_mask_to_level; // Fill up everything with 3s.
	return terminator;
}

uint64 EmbeddedLevelNameEncoding::idFromTerminatorAndLevel_NoDepthBit(uint64 terminator, uint32 level) {
	using namespace std;
	uint64 terminatorAtLevel = terminator; // Bump up one, but we still need the level.
	// Should clean up successor just in case terminator non-3 prefix is not consistent with level.
	uint64 one_mask_to_level = 0;
	uint64 one_at_level      = one;
	for(uint64 shift = 2;
			shift <= ( (topBitPosition-3) - 2*(level) );
			shift +=2 ) {
		one_mask_to_level = one_mask_to_level << 2;
		one_mask_to_level += 3;
		one_at_level = one_at_level << 2;
	}
//	one_at_level = one_at_level >> 2;

	terminatorAtLevel = terminatorAtLevel & (~one_mask_to_level);
//	terminatorAtLevel += one_at_level; // Don't add one because we want to stay at the Terminator.

//	cout << "one_at_level: "<< hex << one_at_level << dec << endl << flush;
//	cout << "one_mask_to_: "<< hex << one_mask_to_level << dec << endl << flush;

	// Check for overflow.
	if( terminatorAtLevel == TopBit ) {
		return 0; // It's invalid!
	}
	terminatorAtLevel += level;
	return terminatorAtLevel;
}

int64 EmbeddedLevelNameEncoding::getSciDBLeftJustifiedFormat(uint64 leftId) const {

	uint64 id_NoLevelBit = leftId & stripLevelBitMask; // Note this covers bits 0-5.
	uint64 level             = leftId & levelMask;

	// TODO Repent the sin of redundant code.
	int64 leftId_scidb = id_NoLevelBit;
	leftId_scidb = leftId_scidb >> 1;
	// Add the level bits back in.
	leftId_scidb = leftId_scidb & ~levelMaskSciDB; // Note this covers bits 0-4.
	leftId_scidb = leftId_scidb | level;

	return leftId_scidb;

}

int64 EmbeddedLevelNameEncoding::getSciDBLeftJustifiedFormat() const {

	// Left justified
	// 63 Empty/level bit/valid id
	// 62 N-S bit
	// 61-6 Tree traversal
	// 5 Empty (Masked with levelMask)
	// 4..0 Level

	// SciDB's new format for indexing. As long as we're keeping HTM indices positive.
	// 63 Empty/ 0 -> valid; 1 -> invalid
	// 62 Empty
	// 61 N-S bit // Positive numbers are in the northern hemisphere.
	// 60..5 Tree traversal
	// 4..0 Level (Masked with levelMaskSciDB)

	// *** TODO *** NEED ERROR CHECKING & SIGNALLING FOR INVALID IDs

	int64 leftId_scidb = 0; // Erf... // What's the invalid id now? Anything negative.

	uint64 id_NoLevelBit = this->maskOffLevelBit();
	int64  level         = this->getLevel();
//	iTmp = id_NoLevelBit;

	// TODO Repent the sin of redundant code.
	leftId_scidb = id_NoLevelBit;
	leftId_scidb = leftId_scidb >> 1;
	// Add the level bits back in.
	leftId_scidb = leftId_scidb & ~levelMaskSciDB;
	leftId_scidb = leftId_scidb | level;

	return leftId_scidb;
}

int64 EmbeddedLevelNameEncoding::getSciDBTerminatorLeftJustifiedFormat() const {
	return getSciDBLeftJustifiedFormat(getIdTerminator_NoDepthBit());
}

void EmbeddedLevelNameEncoding::setIdFromSciDBLeftJustifiedFormat( int64 id_scidb ) {
	// See bit format in getSciDBLeftJustifiedFormatKluge.
	// Note this does set this->id.
	uint64 iTmp = id_scidb & ~levelMaskSciDB;
	uint64 level = id_scidb & levelMaskSciDB;
	iTmp = iTmp << 1;
	iTmp = iTmp | level;
	iTmp = iTmp | TopBit;
//	this->id = iTmp; // hacking...
	this->setId(iTmp);
}

/**
 * Change the resolution level to level. If the result resolution is coarser, then we lose information.
 *
 * @param level
 */
EmbeddedLevelNameEncoding EmbeddedLevelNameEncoding::atLevel(uint64 level, bool keepAllBits ) {
	int oldLevel = this->getLevel();
	uint64 id_NoLevel = this->maskOffLevel();
	uint64 keepBits = one << 1; // Position 63
	keepBits++; // Position 62
	uint64 newId;
	if(level < oldLevel) {
		for(int i=62;i>5;i-=2){
			int levelAtI = (62-i)/2;
			keepBits = keepBits << 2;
			if((level < levelAtI) && (levelAtI < oldLevel)) {
				if(keepAllBits) { keepBits += 3; }
			}else{
				keepBits += 3;
			}
		}
		keepBits = keepBits << 4;
		newId = id_NoLevel & keepBits;
	} else {
		newId =  id_NoLevel;
	}
//	newId =  id_NoLevelBit;
	newId = newId | level;
//	EmbeddedLevelNameEncoding res(newId);
	return EmbeddedLevelNameEncoding(newId);
}


EmbeddedLevelNameEncoding EmbeddedLevelNameEncoding::clearDeeperThanLevel(uint64 level) {
	int oldLevel = this->getLevel();
	uint64 id_NoLevel = this->maskOffLevel();
	uint64 keepBits = one << 1; // Position 63
	keepBits++; // Position 62
	uint64 newId;

	for(int i=62;i>5;i-=2){
	  int levelAtI = (62-i)/2;
	  keepBits = keepBits << 2;
	  if(level >= levelAtI){
	    keepBits += 3;
	  }
	}
	keepBits = keepBits << 4;
	newId = id_NoLevel & keepBits;

	newId = newId | level;
	return EmbeddedLevelNameEncoding(newId);
}


// TODO Unit tests
/// Find terminator+
uint64 EmbeddedLevelNameEncoding::successorToTerminator_NoDepthBit(uint64 terminator, uint32 level) const {
	using namespace std;
	uint64 successor = terminator; // Bump up one, but we still need the level.
	// Should clean up successor just in case terminator non-3 prefix is not consistent with level.
	uint64 one_mask_to_level = 0;
	uint64 one_at_level      = one;
	for(uint64 shift = 2;
			shift <= ( (topBitPosition-3) - 2*(level) );
			shift +=2 ) {
		one_mask_to_level = one_mask_to_level << 2;
		one_mask_to_level += 3;
		one_at_level = one_at_level << 2;
	}
//	one_at_level = one_at_level >> 2;

	successor = successor & (~one_mask_to_level);
	successor += one_at_level;

//	cout << "one_at_level: "<< hex << one_at_level << dec << endl << flush;
//	cout << "one_mask_to_: "<< hex << one_mask_to_level << dec << endl << flush;

	// Check for overflow.
	if( successor == TopBit ) {
		return 0; // It's invalid!
	}
	successor += level;
	return successor;
}

// TODO Unit tests
/// Find lowerBound-
uint64 EmbeddedLevelNameEncoding::predecessorToLowerBound_NoDepthBit(uint64 lowerBound, uint32 lbLevel) const {
	uint64 decrement = one;
	// TODO We could take the level off of lowerBound... Are we ignoring the level field in lowerBound?  Guess so...
	// TODO Re: lbLevel -- I guess this is a little dangerous.
	for(uint64 shift = 2;
			shift <= ( (topBitPosition-3) - 2*lbLevel );
			shift +=2 ) {
		decrement = decrement << 2;
	}
	uint64 predecessor = lowerBound - decrement;
	uint64 terminator  = getIdTerminator_NoDepthBit(predecessor,lbLevel);
	return terminator;
}

bool EmbeddedLevelNameEncoding::terminatorp() {
	return terminatorp(this->id);
}

bool EmbeddedLevelNameEncoding::terminatorp(uint64 terminatorCandidate) {
	uint64 level = terminatorCandidate & levelMask;
	return level == 63;
}

uint64 EmbeddedLevelNameEncoding::increment(uint64 lowerBound, uint32 level, int n) const {
	using namespace std;
	uint64 successor = lowerBound; // Bump up one, but we still need the level.
	// Should clean up successor just in case terminator non-3 prefix is not consistent with level.
	uint64 one_mask_to_level = 0;
	uint64 one_at_level      = one;
	for(uint64 shift = 2;
			shift <= ( (topBitPosition-3) - 2*(level) );
			shift +=2 ) {
		one_mask_to_level = one_mask_to_level << 2;
		one_mask_to_level += 3;
		one_at_level = one_at_level << 2;
	}
//	one_at_level = one_at_level >> 2;

	successor = successor & (~one_mask_to_level);
	successor += one_at_level;

//	cout << "one_at_level: "<< hex << one_at_level << dec << endl << flush;
//	cout << "one_mask_to_: "<< hex << one_mask_to_level << dec << endl << flush;

	// Check for overflow.
	if( successor == TopBit ) {
		return 0; // It's invalid!
	}
	successor += n*level;
	return successor;
}
uint64 EmbeddedLevelNameEncoding::decrement(uint64 lowerBound, uint32 level, int n) const {
	using namespace std;
	uint64 successor = lowerBound; // Bump up one, but we still need the level.
	// Should clean up successor just in case terminator non-3 prefix is not consistent with level.
	uint64 one_mask_to_level = 0;
	uint64 one_at_level      = one;
	for(uint64 shift = 2;
			shift <= ( (topBitPosition-3) - 2*(level) );
			shift +=2 ) {
		one_mask_to_level = one_mask_to_level << 2;
		one_mask_to_level += 3;
		one_at_level = one_at_level << 2;
	}
//	one_at_level = one_at_level >> 2;

	successor = successor & (~one_mask_to_level);
	if( successor == 0 ) {
		return 0; // It's invald!
	}
	successor -= one_at_level;

//	cout << "one_at_level: "<< hex << one_at_level << dec << endl << flush;
//	cout << "one_mask_to_: "<< hex << one_mask_to_level << dec << endl << flush;

	// Check for overflow.
	if( successor == TopBit ) {
		return 0; // It's invalid!
	}
	successor += n*level;
	return successor;
}


