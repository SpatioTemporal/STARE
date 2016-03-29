/*
 * EmbeddedLevelNameEncoding.C
 *
 *  Created on: Jan 29, 2016
 *      Author: mrilee
 */

#include "EmbeddedLevelNameEncoding.h"
#include <string>

EmbeddedLevelNameEncoding::EmbeddedLevelNameEncoding() {}

EmbeddedLevelNameEncoding::~EmbeddedLevelNameEncoding() {}

char* EmbeddedLevelNameEncoding::nameById(uint64 id) {
	if(id == 0) {
		// Throw an exception?
		throw SpatialFailure("EmbeddedLevelNameEncoding::nameById-INVALID_ID_0");
	}

	int nameSize = levelById(id)+3;
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
		out += ( (uint64)(name[i]-'0')) << 2*(32-i)-2;
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

uint64 EmbeddedLevelNameEncoding::bareId() const {
	// Should this be the old style id?
	uint64 stripId = id & stripMask;
	uint64 shiftId = stripId >> (60 - 2*(levelById(id)));
	return shiftId;
}

uint64 EmbeddedLevelNameEncoding::bareId_NoShift_NoEmbeddedLevel() const {
	uint64 stripId = id & stripMask;
	return stripId;
}

uint64 EmbeddedLevelNameEncoding::getId_NoEmbeddedLevel() const {
	uint64 id = getId();
	return id & ~levelMask; // Only remove level.
//	return id & stripMask;
}
