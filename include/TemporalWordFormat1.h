/*
 * TemporalWordFormat1.h
 *
 *  Created on: Apr 16, 2019
 *      Author: mrilee
 *
 *  Copyright (C) 2019 Rilee Systems Technologies LLC
 */

#ifndef INCLUDE_TEMPORALWORDFORMAT1_H_
#define INCLUDE_TEMPORALWORDFORMAT1_H_

#include "BitField.h"

namespace std {

class TemporalWordFormat1 {
public:
	vector<BitField*>     bitFields;   // Construct order... Verify object identity?
	map<string,BitField*> bitFieldMap; // Holds the data, can we have the same obj as previous?

	int64_t maxCoResolutionLevelValue =  9;  // Would be const in a perfect universe.
	int64_t minCoResolutionLevelValue = -1; // Corresponds to "Ma"
	int64_t resolutionLevelConstraint =  7;
	int64_t nonDataLevels             =  3; // Was 2 when there was no type.

	TemporalWordFormat1(); // Overload this
	virtual ~TemporalWordFormat1();

	void print() const {

		bool first = true;
		int i = 10;
		for(vector<BitField*>::const_iterator it = bitFields.begin(); it != bitFields.end(); ++it) {
			if(first) {
				cout << "    " << (*it)->toStringHeader() << endl << flush;
				first = false;
			}
			cout
			<< setw(4)
			<< dec << --i << hex << " "
			<< (*it)->toString() << endl << flush;
			// << (*it).toString() << endl << flush;
		}
	}
	BitField* get(string name) const {
		// cout << "x100 '" << endl << flush;
		// << bitFieldMap.at(name).getName() << "'" << endl << flush;
		return bitFieldMap.at(name);
	}
	TemporalWordFormat1& setValue(string name, int64_t value) {
		if(name != "resolutionLevel") {
			cout << "setting " << name << " to " << value << endl << flush;
			bitFieldMap.at(name)->setValue(value);
			cout << name << " set to " << bitFieldMap.at(name)->getValue() << endl << flush;
		} else {
			bitFieldMap.at("coResolutionLevel")->setValue(resolutionLevelConstraint-value);
		}
		return *this;
	}
	int64_t getValue(string name) {
	  if(name != "resolutionLevel") {
		return bitFieldMap.at(name)->getValue();
	  } else {
	    return
	      resolutionLevelConstraint - bitFieldMap.at("coResolutionLevel")->getValue();
	  }
	}

	void setZero() {
		int64_t type = bitFieldMap.at("type")->getValue();
		int64_t coResolutionLevel = bitFieldMap.at("coResolutionLevel")->getValue();
		for(map<string,BitField*>::iterator it = bitFieldMap.begin(); it != bitFieldMap.end(); ++it) {
			(*it).second->setValue(0);
		}
		bitFieldMap.at("type")->setValue(type);
		bitFieldMap.at("coResolutionLevel")->setValue(coResolutionLevel);
	}

	// TODO throw excpetion if levelName is not a resolution level, i.e. a non-level field
	int64_t getCoFieldId(string levelName) {
		return bitFieldMap.at(levelName)->getCoFieldId();
	}

	BitField* getBitFieldAtLevel(int64_t coResolutionLevel) {
		// TODO Repent hard coding here
		// int idx_coResolutionLevel = 9;
		int idx_coResolutionLevel = bitFields.size(); // rL should be the last one.
		int idx = idx_coResolutionLevel-nonDataLevels-coResolutionLevel; // Assume levels consecutive
		// int idx = idx_coResolutionLevel-2-coResolutionLevel; // Assume levels consecutive
		return bitFieldMap.at(bitFields[idx]->getName());
	}

	void incrementAtLevel(int64_t level,int64_t delta) {
		if( (level < minCoResolutionLevelValue)
			|| (maxCoResolutionLevelValue < level) )
		{
			throw SpatialFailure("TemporalWordFormat:incrementAtLevel:LevelOutOfBounds");
		}
//		cout << "10000: " << level << endl;
		int64_t oldValue = getBitFieldAtLevel(level)->getValue();
//		cout << "10010: " << oldValue << endl;
		int64_t newValue = oldValue + delta;

		// TODO Check for overflow & underflow? Carry?
		if(newValue>getBitFieldAtLevel(level)->getMaxValue()) {
			// Carry!
			// TODO 8 is the "top" level here, even though we have a 3-bit level field here.
			// TODO fix hardcode
			if(level >= maxCoResolutionLevelValue) { // == 9 //
				throw SpatialFailure("TemporalWordFormat:OverflowError");
			}
//			cout << "10100: " << newValue << endl;
			incrementAtLevel(level+1,1); // level = 0 => highest resolution, greatest depth
			newValue-=getBitFieldAtLevel(level)->getMaxValue()+1;
//			cout << "10110: " << newValue << endl;
		}
//		cout << "19000: " << newValue << endl;
		getBitFieldAtLevel(level)->setValue(newValue);
//		cout << "19001: " << getBitFieldAtLevel(level).getValue() << endl;
	}
	void incrementAtLevel(string levelName, int64_t delta=1) {
		// incrementAtLevel(getCoResolutionLevel(levelName),delta);
		incrementAtLevel(getCoFieldId(levelName),delta);
	}

	void decrementAtLevel(int64_t level, int64_t delta) {
		if(level >= maxCoResolutionLevelValue) {
			throw SpatialFailure("TemporalWordFormat:decrementAtLevel:MaxLevelExceeded");
		}
		if( (level < minCoResolutionLevelValue)
			|| (maxCoResolutionLevelValue < level) )
		{
			throw SpatialFailure("TemporalWordFormat:decrementAtLevel:LevelOutOfBounds");
		}
		int64_t oldValue = getBitFieldAtLevel(level)->getValue();
		int64_t newValue = oldValue - delta;
		// TODO Check for overflow & underflow? Carry?
		if(newValue<0){
			if(level==0) {
				throw SpatialFailure("TemporalWordFormat:UnderflowError");
			}
			decrementAtLevel(level+1,1); // level = 0 => highest resolution, greatest depth
			newValue+=getBitFieldAtLevel(level)->getMaxValue()+1;
		}
		getBitFieldAtLevel(level)->setValue(newValue);
	}
	void decrementAtLevel(string levelName, int64_t delta = 1) {
		// decrementAtLevel(getCoResolutionLevel(levelName), delta);
		decrementAtLevel(getCoFieldId(levelName), delta);
	}

	// TODO Shift from level to bit position.
	void setTerminatorBelowLevel(int64_t level){
		// TODO Biggest abs-value possible in the field?
		while( level-- > 0) {
			int64_t newValue = getBitFieldAtLevel(level)->getMask(); // negation handled elsewhere
			// int64_t newValue = getBitFieldAtLevel(level).getMaxValue();
			getBitFieldAtLevel(level)->setValue(newValue);
		}
	}

// We now set terminator below a resolution not a levelName. The resolution can be at any appropriate bit position.
//	void setTerminatorBelowLevel(string levelName) {
//		setTerminatorBelowLevel(getFieldId(levelName));
//	}

};

}

#endif /* INCLUDE_TEMPORALWORDFORMAT1_H_ */
