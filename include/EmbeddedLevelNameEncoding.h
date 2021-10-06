/*
 * EmbeddedLevelNameEncoding.h
 *
 *  Created on: Jan 29, 2016
 *      Author: mrilee
 *
 *  Copyright (C) 2021 Rilee Systems Technologies LLC
 *
 */

#ifndef INCLUDE_EMBEDDEDLEVELNAMEENCODING_H_
#define INCLUDE_EMBEDDEDLEVELNAMEENCODING_H_

using namespace std;

#include <string.h>
#include <SpatialException.h>
#include "NameEncoding.h"
#include "BitShiftNameEncoding.h"
#include <algorithm>

/// Left justified
class EmbeddedLevelNameEncoding: public NameEncoding {
public:
  EmbeddedLevelNameEncoding();
  virtual ~EmbeddedLevelNameEncoding();

  /// ?id is htmID?
  EmbeddedLevelNameEncoding(uint64 id) {setId(id);}
  // EmbeddedLevelNameEncoding(const char *name){setName(name);}
  EmbeddedLevelNameEncoding(string name){setName(name);}

  string nameById(uint64 id);
  // char  *nameById(uint64 id);
  //	uint64 idByName(const char *name);
  uint64 idByName(string name);
  uint32 levelById(uint64 id) const;

  uint64 bareId() const;  ///< Returns old-style HTM w/o depth bit.
  uint64 bareId_NoShift_NoEmbeddedLevel() const; ///< Returns HTM w/o depth bit or embedded level.

  uint64 rightJustifiedId() {
    return rightJustifiedId(id);
  }

  uint64 rightJustifiedId(uint64 leftJustifiedId) {
    BitShiftNameEncoding rightJustified;
    uint64 rightId;
    uint32 level = levelById(leftJustifiedId);
    rightId = leftJustifiedId >> ( (topBitPosition-1) - 2*(level+1) );
    //		rightId = leftJustifiedId | (one << (2*(level)) );
    //		rightId = 12;
    return rightId;
  }

  uint64 getId_NoEmbeddedLevel() const; /// Return left justified level w/o level information.
  uint64 getId_NoLevelBit() const { return maskOffLevelBit(); }

  uint64 maskOffLevelAndLevelBit(uint64 id) const;
  uint64 maskOffLevelAndLevelBit() const;

  uint64 maskOffLevel(uint64 id);
  uint64 maskOffLevel();

  uint64 maskOffLevelBit(uint64 id) const;
  uint64 maskOffLevelBit() const;

  /// Returns a uint64 that is greater than all IDs_w/o_TopBit in HtmId, but less than HtmId's successor.
  uint64 getIdTerminator_NoDepthBit() const;
  uint64 getIdTerminator_NoDepthBit(
				    uint64 idWithMaskOffLevelAndLevelBit,
				    uint32 aLevel
				    ) const; ///< Somewhat dangerous to use.
  uint64 idFromTerminatorAndLevel_NoDepthBit(uint64 terminator, uint32 level); ///< Also a little dangerous.

  bool terminatorp() const;
  bool terminatorp(uint64 terminatorCandidate) const;

  bool SciDBterminatorp() const;
  bool SciDBterminatorp(uint64 terminatorCandidate) const;

  /// What triangle is just after the terminator?
  uint64 successorToTerminator_NoDepthBit(uint64 terminator, uint32 level) const;
  /// What triangle is just before the lower bound?
  uint64 predecessorToLowerBound_NoDepthBit(uint64 lowerBound, uint32 lbLevel) const;

  /**
   * The *SciDB* routines provide an interface to convert
   * the 64-bit format to the int64 format.
   *
   */

  /// Convert a 64-bit left justified ID to a 62-bit int64 ID for SciDB.
  /// Note this->id is assumes the 64-bit format.
  int64 getSciDBLeftJustifiedFormat(uint64 leftId) const;

  /// Return the 62-bit int64 ID for SciDB.
  /// Note this->id is assumed to stay in 64-bit format.
  int64 getSciDBLeftJustifiedFormat() const;

  /// Return a terminator index in the int64 format.
  /// Note this->id is assumed to stay in 64-bit format.
  int64 getSciDBTerminatorLeftJustifiedFormat() const;

  /// Set this->id to correspond to the int64 id from SciDB.
  /// Note this->id is assumed to stay in 64-bit format.
  void setIdFromSciDBLeftJustifiedFormat( int64 id_scidb );

  /// Changes the level, loses info on coarsening.
  EmbeddedLevelNameEncoding atLevel( uint64 level, bool keepAllBits = false );

  EmbeddedLevelNameEncoding clearDeeperThanLevel(uint64 level);

  void SciDBincrement_LevelToMaskDelta(uint32 level,uint64 &one_mask_to_level,uint64 &one_at_level) const;
  void increment_LevelToMaskDelta(uint32 level,uint64 &one_mask_to_level,uint64 &one_at_level) const;

  /*
   * increment and decrement work on the internal bit format, not the SciDB format.
   */
  uint64 increment(uint64 lowerBound, uint32 level, int steps = 1) const;
  uint64 decrement(uint64 lowerBound, uint32 level, int steps = 1) const;

  EmbeddedLevelNameEncoding& operator=(EmbeddedLevelNameEncoding obj) {
    this->setId(obj.getId());
    return *this;
  }

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

  // char* greatestCommonName(const EmbeddedLevelNameEncoding& right) {
  string greatestCommonName(const EmbeddedLevelNameEncoding& right) {
    int gcl_index = greatestCommonLevel(right) + 2;
    // char *sharedAncestorName = new char[gcl_index+1];
    string sharedAncestorName(gcl_index+1,0);
    for(int l=0;l<gcl_index;l++)
      sharedAncestorName[l] = name[l];
    // sharedAncestorName[gcl_index] = 0;
    sharedAncestorName.erase(gcl_index,string::npos);
    return sharedAncestorName;
  }
  
  uint64 greatestCommonId(const EmbeddedLevelNameEncoding& right) {
    return idByName(this->greatestCommonName(right));
  }

  // Need relational operators. >, <
  // Need some sort of "and" to find the greatest shared root node.

  // const char* getEncodingType() const {return "EmbeddedLevelNameEncoding";}
  string getEncodingType() const {return "EmbeddedLevelNameEncoding";}

  int getLocalTriangleNumber() const;

  // private:
  const uint32 topBitPosition = 63; //
  const uint32 levelMaskWidth = 6; // Should be consistent with below.
  const uint64 levelMask = 63; // Probably could make do with 31 since each level takes 2 bits.
  const uint64 one = 1;
  const uint64 stripMask = ~(one << 63) & ~levelMask; // This 63 is topBitPosition.
  const uint64 LevelBit = (one << 63); // Do we have every permutation now?
  const uint64 stripLevelBitMask = ~(LevelBit); // one << 63); // Do we have every permutation now?
  const uint64 NorthSouthBit = (one << 62);
  const uint64 TopBit = one << topBitPosition;

  const uint64 levelMaskSciDB = 31;

};

void EmbeddedLevelNameEncoding_test();

#endif /* INCLUDE_EMBEDDEDLEVELNAMEENCODING_H_ */
