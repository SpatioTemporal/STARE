/*
 * SpatiallyAdaptiveDataCover.h
 *
 *  Created on: Jan 20, 2017
 *      Author: mrilee
 */

#ifndef INCLUDE_SPATIALLYADAPTIVEDATACOVER_H_
#define INCLUDE_SPATIALLYADAPTIVEDATACOVER_H_

#include "BitShiftNameEncoding.h"
#include "EmbeddedLevelNameEncoding.h"

namespace std {

class SpatiallyAdaptiveDataCover {
private:
	BitShiftNameEncoding rightJustified;
	const uint64 locationLevel = 24; ///< Position information associated with the triangle. level 24 -> 60 cm
	EmbeddedLevelNameEncoding leftJustified;
	int level; ///< The intrinsic data resolution associated with this triangle.
	/// The adaptiveCover is left justified, but has both high-resolution areal and location information.
	EmbeddedLevelNameEncoding adaptiveCover; ///< locationLevel location information, with level areal information
public:
	SpatiallyAdaptiveDataCover();
	virtual ~SpatiallyAdaptiveDataCover();

	SpatiallyAdaptiveDataCover(uint64 rightJustifiedId, int level) : level(level) {
		// TODO silently coerce id to the locationLevel.
		setRightJustifiedId(rightJustifiedId);
	}

	/// Make class data elements consistent.
	void update() {
		leftJustified = leftJustified.atLevel(locationLevel,true); ///< coerce to the location level, but keep all bits
		rightJustified.setId(leftJustified.rightJustifiedId()); ///< and back to the right
		adaptiveCover = leftJustified.atLevel(level,true); ///< Coerce to the data resolution level, and keep all bits
	}

	/// The intrinsic data resolution associated with this triangle.
	int getLevel() const {
		return level;
	}

	/// The intrinsic data resolution associated with this triangle.
	void setLevel(int level) {
		this->level = level;
		update();
	}

	uint64 getRightJustifiedId() const {
		return rightJustified.getId();
	}

	uint64 getLeftJustifiedId() const {
		return leftJustified.getId();
	}

	uint64 getSciDBLeftJustifiedFormat() {
		return leftJustified.getSciDBLeftJustifiedFormat();
	}

	uint64 getAdaptiveCover_SciDBLeftJustifiedFormat() {
		return adaptiveCover.getSciDBLeftJustifiedFormat(); // keep all of the location bits
	}

	/// Makes a terminator based on the data resolution level
	uint64 getAdaptiveCover_SciDBTerminatorLeftJustifiedFormat() {
		return adaptiveCover.getSciDBTerminatorLeftJustifiedFormat();
	}

	void setRightJustifiedId(uint64 rightJustifiedId) {
		rightJustified.setId(rightJustifiedId); // set the right id
		leftJustified.setId(rightJustified.leftJustifiedId()); // set the left from the right
		update();
	}



};

} /* namespace std */

#endif /* INCLUDE_SPATIALLYADAPTIVEDATACOVER_H_ */
