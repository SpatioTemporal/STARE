/*
 * EmbeddedLevelNameEncoding.h
 *
 *  Created on: Jan 29, 2016
 *      Author: mrilee
 */

#ifndef INCLUDE_EMBEDDEDLEVELNAMEENCODING_H_
#define INCLUDE_EMBEDDEDLEVELNAMEENCODING_H_

#include <SpatialException.h>
#include "NameEncoding.h"
#include <algorithm>
/// Left justified
class EmbeddedLevelNameEncoding: public NameEncoding {
public:
	EmbeddedLevelNameEncoding();
	virtual ~EmbeddedLevelNameEncoding();

	EmbeddedLevelNameEncoding(uint64 id) {setId(id);}
	EmbeddedLevelNameEncoding(const char *name){setName(name);}

	char  *nameById(uint64 id);
	uint64 idByName(const char *name);
	uint32 levelById(uint64 id) const;

	uint64 bareId() const;  ///< Returns old-style HTM w/o depth bit.
	uint64 bareId_NoShift_NoEmbeddedLevel() const; ///< Returns HTM w/o depth bit or embedded level.

	uint64 getId_NoEmbeddedLevel() const; /// Return left justified level w/o level information.

	/**
	 * Compare two id's without regard for the level.
	 * @param right
	 * @return
	 */
	bool operator>(const EmbeddedLevelNameEncoding& right) {
		return (this->id & levelMask) > (right.id & levelMask);
	}
	/**
	 * Compare two id's without regard for the level.
	 * @param right
	 * @return
	 */
	bool operator>=(const EmbeddedLevelNameEncoding& right) {
		return (this->id & levelMask) >= (right.id & levelMask);
	}
	/**
	 * Compare two id's without regard for the level.
	 * @param right
	 * @return
	 */
	bool operator<(const EmbeddedLevelNameEncoding& right) {
		return (this->id & levelMask) < (right.id & levelMask);
	}
	/**
	 * Compare two id's without regard for the level.
	 * @param right
	 * @return
	 */
	bool operator<=(const EmbeddedLevelNameEncoding& right) {
		return (this->id & levelMask) <= (right.id & levelMask);
	}

	/** Compare two id's, including level
	 *
	 * @param right
	 * @return
	 */
	bool operator==(const EmbeddedLevelNameEncoding& right) {
		return this->id == right.id;
	}
	/** Compare two id's, including level
	 *
	 * @param right
	 * @return
	 */
	bool operator!=(const EmbeddedLevelNameEncoding& right) {
		return this->id != right.id;
	}

	/**
	 * The level below which the two id's diverge.
	 * @param right another name
	 * @return -1 for a different hemisphere, otherwise the level of divergence
	 */
	int greatestCommonLevel(const EmbeddedLevelNameEncoding& right) {
		int maxlevel = std::min(getLevel(),right.getLevel());
		int l = 0;
		bool diff = false;
		while(l < maxlevel+2) {
			if(name[l] != right.name[l]) {
				diff = true;
				if(l==0) {
					l=-1;
				} else if (l==1) {
					l=0;
				} else {
					l-=2;
				}
				break;
			}
			l++;
		}
		if(!diff)l-=2;
		return l;
	}

	char* greatestCommonName(const EmbeddedLevelNameEncoding& right) {
		int gcl_index = greatestCommonLevel(right) + 2;
		char *sharedAncestorName = new char[gcl_index+1];
		for(int l=0;l<gcl_index;l++)
			sharedAncestorName[l] = name[l];
		sharedAncestorName[gcl_index] = 0;
		return sharedAncestorName;
	}
	uint64 greatestCommonId(const EmbeddedLevelNameEncoding& right) {
		return idByName(this->greatestCommonName(right));
	}

	// Need relational operators. >, <
	// Need some sort of "and" to find the greatest shared root node.

private:
	const uint32 levelMaskWidth = 6; // Should be consistent with below.
	const uint64 levelMask = 63; // Probably could make do with 31 since each level takes 2 bits.
	const uint64 one = 1;
	const uint64 stripMask = ~(one << 63) & ~levelMask;
	const uint64 NorthSouthBit = one << 62;
};

#endif /* INCLUDE_EMBEDDEDLEVELNAMEENCODING_H_ */
