/*
 * BitShiftNameEncoding.C
 *
 *  Created on: Jan 23, 2016
 *      Author: mrilee
 */

#include "BitShiftNameEncoding.h"
#include <SpatialException.h>

BitShiftNameEncoding::BitShiftNameEncoding() {
	// TODO Auto-generated constructor stub
}

BitShiftNameEncoding::~BitShiftNameEncoding() {
	// TODO Auto-generated destructor stub
}

// char* BitShiftNameEncoding::nameById(uint64 id) {
string BitShiftNameEncoding::nameById(uint64 id) {

  // char *name = new char[32];
  string name(32,' ');
  
	uint32 size=0, i;
	int c; // a spare character;

#if defined(_WIN32)
	uint64 IDHIGHBIT = 1;
	uint64 IDHIGHBIT2= 1;
	IDHIGHBIT = IDHIGHBIT << 63;
	IDHIGHBIT2 = IDHIGHBIT2 << 62;
#endif

	// determine index of first set bit
	for(i = 0; i < IDSIZE; i+=2) {
		if ( (id << i) & IDHIGHBIT ) break;
		if ( (id << i) & IDHIGHBIT2 )  // invalid id
			throw SpatialFailure("BitShiftName:nameById:InvalidID-IDHIGHBIT2");
		// return HTM_INVALID_ID;
	}
	if(id == 0)
		throw SpatialFailure("BitShiftName:nameById:InvalidID-id=0");
	//	    return HTM_INVALID_ID;

	size=(IDSIZE-i) >> 1;

	// fill characters starting with the last one
	for(i = 0; i < size-1; i++) {
		c =  '0' + (int) ((id >> i*2) & (uint32) 3);
		name[size-i-1] = (char ) c;
	}

	// put in first character
	if( (id >> (size*2-2)) & 1 ) {
		name[0] = 'N';
	} else {
		name[0] = 'S';
	}
	// name[size] = 0; // end string
	name.erase(size,string::npos);

	// ORIG 2019-1212 MLR return name;
	// string ret(name);
	// delete name;
	// return ret;
	return name;
}

// uint64 BitShiftNameEncoding::idByName(const char* name) {
uint64 BitShiftNameEncoding::idByName(string name) {

	uint64 out=0, i;
	size_t siz = 0;

	//	if(name == 0)              // null pointer-name
	if(name.empty())              // null pointer-name	
		throw SpatialFailure("BitShiftName:idByName-NullPointerName");
//		return 0;
	if(name[0] != 'N' && name[0] != 'S')  // invalid name
		throw SpatialFailure("BitShiftName:idByName-InvalidName");
//		return 0;

	// siz = strlen(name);       // determine string length
	siz = name.length();       // determine string length
	// at least size-2 required, don't exceed max
	if(siz < 2)
		throw SpatialFailure("BitShiftName:idByName-NameTooSmall");
//		return 0;
	if(siz > HTMNAMEMAX)
		throw SpatialFailure("BitShiftName:idByName-NameTooBig");
//		return 0;

	for(i = siz-1; i > 0; i--) {// set bits starting from the end
		if(name[i] > '3' || name[i] < '0') {// invalid name
			throw SpatialFailure("BitShiftName:idByName-InvalidDigit");
//			return 0;
		}
		out += ( (uint64)(name[i]-'0')) << 2*(siz - i -1);
	}

	i = 2;                     // set first pair of bits, first bit always set
	if(name[0]=='N') i++;      // for north set second bit too
	out += (i << (2*siz - 2) );


	/************************
				   // This code may be used later for hashing !
				   if(size==2)out -= 8;
				   else {
				   size -= 2;
				   uint32 offset = 0, level4 = 8;
				   for(i = size; i > 0; i--) { // calculate 4 ^ (level-1), level = size-2
				   offset += level4;
				   level4 *= 4;
				   }
				   out -= level4 - offset;
				   }
	 **************************/
	return out;

}

uint32 BitShiftNameEncoding::levelById(uint64 htmid) const {
	  uint32 size=0, i;

	#if defined(_WIN32)
	  uint64 IDHIGHBIT = 1;
	  uint64 IDHIGHBIT2= 1;
	  IDHIGHBIT = IDHIGHBIT << 63;
	  IDHIGHBIT2 = IDHIGHBIT2 << 62;
	#endif


	  // determine index of first set bit
	  for(i = 0; i < IDSIZE; i+=2) {
		if ( (htmid << i) & IDHIGHBIT ) break;
		/*
		  if ( (id << i) & IDHIGHBIT2 )  // invalid id
		  return HTM_INVALID_ID;
		  // but we trust you now...
		  */
	  }
	/*   if(id == 0) */
	/*     return HTM_INVALID_ID; */

	  size=(IDSIZE-i) >> 1;
	  /* Size is the length of the string representing the name of the
	     trixel, the level is size - 2
	  */
	  return size-2;
}

/**
 * Strips the first nonzero bit, which determines the level of the index.
 * Stripping the depth bit allows a nodeIndex to be calculated from the
 * result. Other packing schemes may store depth information elsewhere.
 *
 * @return an htmId sans the depth bit, essentially nodeIndexMinusIOFFSET at that depth.
 */
uint64 BitShiftNameEncoding::bareId() const {
	uint32 depth = levelById(id)+1;
	uint64 one  = 1;
	uint64 mask = ~(one << (2*depth+1));
	uint64 nodeIndexMinusIOFFSET = (id & mask);  // Add + IOFFSET; to get nodeIndex.
	return nodeIndexMinusIOFFSET;
}

int BitShiftNameEncoding::getLocalTriangleNumber() const {
	uint64 ret = ( id & 3 );
	return ret;
}

uint64 BitShiftNameEncoding::leftJustifiedId_NoDepthBit() {
	return leftJustifiedId_NoDepthBit(this->id); // Assume this->id is an htmid in the correct format.
}
uint64 BitShiftNameEncoding::leftJustifiedId_NoDepthBit(uint64 htmid) {
	// See bareId above.
	// htmid is rightJustified.
	uint32 depth = levelById(htmid)+1;
	uint32 level = depth - 1;
	uint64 one  = 1;
	uint64 mask = ~(one << (2*depth+1));
	uint64 nodeIndexMinusIOFFSET = (htmid & mask);  // Add + IOFFSET; to get nodeIndex.
	// Now shift to right justification.
	uint32 const topBitPosition = 63;
	uint64 leftId = nodeIndexMinusIOFFSET << ( (topBitPosition-1) - 2*(level+1) );
	leftId += level;
	return leftId;
}

uint64 BitShiftNameEncoding::leftJustifiedId() {
	return leftJustifiedId(this->id); // Assume this->id is an htmid in the correct format.
}
uint64 BitShiftNameEncoding::leftJustifiedId(uint64 htmid) {
	// See bareId above.
	// htmid is rightJustified.
	uint32 depth = levelById(htmid)+1;
	uint32 level = depth - 1;
	uint64 one  = 1;
	uint64 mask = ~(one << (2*depth+1));
	uint64 nodeIndexMinusIOFFSET = (htmid & mask);  // Add + IOFFSET; to get nodeIndex.
//	uint64 nodeIndexMinusIOFFSET = htmid;
	// Now shift to right justification.
	uint32 const topBitPosition = 63;
	uint64 leftId = nodeIndexMinusIOFFSET << ( (topBitPosition-1) - 2*(level+1) );
	leftId = leftId | (one << topBitPosition); // Depth bit
	leftId += level;
	return leftId;
}

// TODO Maybe have two different kinds of htmid types: left & right justified.
uint64 BitShiftNameEncoding::maskOffLevel(uint64 htmid) {
	return htmid;
}

uint64 BitShiftNameEncoding::maskOffLevel() {
	return this->id;
}
