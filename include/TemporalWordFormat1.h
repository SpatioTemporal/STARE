/*
 * TemporalWordFormat1.h
 *
 *  Created on: Apr 16, 2019
 *
 *  Author: Michael Rilee, Rilee Systems Technologies LLC
 *
 *  Copyright (C) 2019 Rilee Systems Technologies LLC
 */

#ifndef INCLUDE_TEMPORALWORDFORMAT1_H_
#define INCLUDE_TEMPORALWORDFORMAT1_H_

#include "BitField.h"
#include <memory>

namespace std {

class TemporalWordFormat1 {
public:
  vector<shared_ptr<BitField> >     bitFields;   // Construct order... Verify object identity?
  map<string,shared_ptr<BitField> > bitFieldMap; // Holds the data, can we have the same obj as previous?

  // vector<BitField*>     bitFields;   // Construct order... Verify object identity?
  // map<string,BitField*> bitFieldMap; // Holds the data, can we have the same obj as previous?

	// It is assumed that pos_FinestResolutionLevel < pos_CoarsestResolutionLevel
	int64_t pos_CoarsestResolutionLevel = -1;
	int64_t pos_FinestResolutionLevel = -1;

	int64_t maxCoResolutionLevelValue =  9;  // Would be const in a perfect universe.
	int64_t minCoResolutionLevelValue = -1; // Corresponds to "Ma"
	// int64_t resolutionLevelConstraint =  7;
	// Not needed. int64_t nonDataLevels             =  3; // Was 2 when there was no type.

	TemporalWordFormat1(); // Overload this
	virtual ~TemporalWordFormat1();

	int64_t maxResolutionLevel() {
		int64_t offsetTop =
				bitFields[pos_CoarsestResolutionLevel]->getOffset() +
				bitFields[pos_CoarsestResolutionLevel]->getWidth()-1;
		int64_t offsetBottom = bitFields[pos_FinestResolutionLevel]->getOffset();
		return offsetTop - offsetBottom;
	}

	void print() const {

		bool first = true;
		int i = 0;
		for(vector<shared_ptr<BitField>>::const_iterator it = bitFields.begin(); it != bitFields.end(); ++it) {
			if(first) {
				cout << "    " << (*it)->toStringHeader() << endl << flush;
				first = false;
			}
			cout
			<< setw(4)
			<< dec << i++ << hex << " "
			<< (*it)->toString() << endl << flush;
			// << (*it).toString() << endl << flush;
		}
	}

	void setFieldMaxId(int64_t fieldMaxId) {
		for(vector<shared_ptr<BitField>>::const_iterator it = bitFields.begin(); it != bitFields.end(); ++it) {
			(*it)->setMaxFieldId(fieldMaxId);
		}
	}

	shared_ptr<BitField> get(string name) const {
		// cout << "x100 '" << endl << flush;
		// << bitFieldMap.at(name).getName() << "'" << endl << flush;
		return bitFieldMap.at(name);
	}
	TemporalWordFormat1& setValue(string name, int64_t value) {
		bitFieldMap.at(name)->setValue(value);
		return *this;
	}
	int64_t getValue(string name) const {
		return bitFieldMap.at(name)->getValue();
	}

	void setZero() {
		// Note we save type here.
		int64_t type = bitFieldMap.at("type")->getValue();
		for(map<string,shared_ptr<BitField>>::iterator it = bitFieldMap.begin(); it != bitFieldMap.end(); ++it) {
			(*it).second->setValue(0);
		}
		bitFieldMap.at("type")->setValue(type);
	}

	// TODO throw exception if levelName is not a resolution level, i.e. a non-level field
	int64_t getCoFieldId(string levelName) {
		return bitFieldMap.at(levelName)->getCoFieldId();
	}

	/**
	   Get the id (position in the bitFieldMap vector) from a field's name.
	 */
	int64_t getFieldId(string levelName) const {
		return bitFieldMap.at(levelName)->getFieldId();
	}
	/**
	   Get the bitfield from the bitFieldMap at location id.
	   
	   Thus one can get a mask for that field via data.getFieldId(i_year)->getMask(), where i_year = data.getFieldId("year");

	 */
	shared_ptr<BitField> getBitFieldAtId(int64_t id) const {
		return bitFieldMap.at(bitFields[id]->getName());
	}

	int64_t getValueAtId(int64_t id) const {
		return bitFieldMap.at(bitFields[id]->getName())->getValue();
	}

	/**
	   Increment the index value at the id'th bitfield by delta. Carry if the id'th maxvalue is exceeded.

	   Note: Not verified, e.g., if carry exceeds 'one?'

	 */
	void incrementAtId(int64_t id,int64_t delta) {
		if( (id < pos_CoarsestResolutionLevel)
			|| (pos_FinestResolutionLevel < id) )
		{
			throw SpatialFailure("TemporalWordFormat:incrementAtLevel:LevelIdOutOfBounds");
		}
//		cout << "10000: " << level << endl;
		int64_t oldValue = getBitFieldAtId(id)->getValue();
//		cout << "10010: " << oldValue << endl;
		int64_t newValue = oldValue + delta;

		// TODO Check for overflow & underflow? Carry?
		if(newValue>getBitFieldAtId(id)->getMaxValue()) {
			// Carry!
			// TODO 8 is the "top" level here, even though we have a 3-bit level field here.
			// TODO fix hardcode
			if(id < pos_CoarsestResolutionLevel) { // == 9 //
				throw SpatialFailure("TemporalWordFormat:OverflowError");
			}
//			cout << "10100: " << newValue << endl;
			incrementAtId(id-1,1); // level = 0 => highest resolution, greatest depth
			newValue-=getBitFieldAtId(id)->getMaxValue()+1;
//			cout << "10110: " << newValue << endl;
		}
//		cout << "19000: " << newValue << endl;
		getBitFieldAtId(id)->setValue(newValue);
//		cout << "19001: " << getBitFieldAtLevel(level).getValue() << endl;
	}
	void incrementAtName(string name, int64_t delta=1) {
		// incrementAtLevel(getCoResolutionLevel(levelName),delta);
		incrementAtId(getFieldId(name),delta);
	}

/**
 * Low-level decrement of a particular field. It doesn't know any details of the calendar,
 * i.e. CE/BCE, and throws an exception if you decrement below zero.
 */
	void decrementAtId(int64_t id, int64_t delta) {
//		if(level >= maxCoResolutionLevelValue) {
//			throw SpatialFailure("TemporalWordFormat:decrementAtLevel:MaxLevelExceeded");
//		}
		if( (id < pos_CoarsestResolutionLevel)
			|| ( pos_FinestResolutionLevel < id) )
		{
			throw SpatialFailure("TemporalWordFormat:decrementAtLevel:LevelOutOfBounds");
		}
		int64_t oldValue = getBitFieldAtId(id)->getValue();
		int64_t newValue = oldValue - delta;
		// TODO Check for overflow & underflow? Carry?
		if(newValue<0){
			if(id==pos_CoarsestResolutionLevel) {
				throw SpatialFailure("TemporalWordFormat:UnderflowError");
			}
			decrementAtId(id-1,1); // level = 0 => highest resolution, greatest depth
			newValue+=getBitFieldAtId(id)->getMaxValue()+1;
		}
		getBitFieldAtId(id)->setValue(newValue);
	}
	void decrementAtName(string name, int64_t delta = 1) {
		decrementAtId(getFieldId(name), delta);
	}

	bool resolutionNotInBounds(int64_t resolution) {
		int64_t offsetTop =
				bitFields[pos_CoarsestResolutionLevel]->getOffset() +
				bitFields[pos_CoarsestResolutionLevel]->getWidth()-1;
		int64_t offsetResolution = offsetTop - resolution;
		int64_t offsetBottom = bitFields[pos_FinestResolutionLevel]->getOffset();
		return ( offsetResolution > offsetTop || offsetBottom > offsetResolution );
	}

	// void fieldIdAndResolutionScaleFromResolution(int64_t &fieldId, int64_t &scale, int64_t &resolution) {
	// }

	//
	// The following mucks up on BCE/CE encodings.
	//
	//
//	// TODO Shift from level to bit position.
//	// TODO 2019-0419 MLR FIX
//	void setTerminatorBelowResolution(int64_t resolution){
//		int64_t bitPosition;
//		// cout << 1 << flush;
//		int64_t offsetTop =
//				bitFields[pos_CoarsestResolutionLevel]->getOffset() +
//				bitFields[pos_CoarsestResolutionLevel]->getWidth()-1;
//		int64_t offsetResolution = offsetTop - resolution;
//		int64_t offsetBottom = bitFields[pos_FinestResolutionLevel]->getOffset();
//		// cout << 2 << flush;
//		if( resolutionNotInBounds(resolution) ) {
//			if ( resolution != 63 ) {
//				throw SpatialFailure("TemporalWordFormat:setTerminatorBelowResolution:ResolutionOutOfBounds");
//			} else {
//				// The index is already a terminator.
//				offsetResolution = max((int64_t) 0,offsetResolution);
//			}
//		}
//		// cout << 3 << flush;
////		int64_t termMask = 0;
////		for( int i = offsetResolution; i >= 0; --i ){
////			termMask = termMask << 1;
////			if( i > offsetBottom ) {
////				++termMask;
////			}
////		}
//		// cout << 4 << endl << flush;
//		int iBit = offsetBottom;
//		int iPos = pos_FinestResolutionLevel;
////		cout << endl << flush;
////		cout << "offsetTop:        " << dec << offsetTop        << hex << endl << flush;
////		cout << "offsetResolution: " << dec << offsetResolution << hex << endl << flush;
////		cout << "offsetBottom:     " << dec << offsetBottom     << hex << endl << flush;
//		while( iBit <= offsetResolution ) {
//			int64_t iWidth = bitFields[iPos]->getWidth();
////			cout << dec << "iBit,iPos: "
////					<< iBit << "," << iPos << hex
////					<< " mask0: " << bitFields[iPos]->getMask()
////					<< ", mask1: "	<< (long long int) pow(2ll,offsetResolution - bitFields[iPos]->getOffset()+1) - 1
////					<< ", oR,chk: "
////					<< dec << offsetResolution << "," << bitFields[iPos]->getOffset() + iWidth
////					<< hex << endl << flush;
//			if( offsetResolution >= bitFields[iPos]->getOffset() + iWidth ) {
//				// The last item of each field.
//				getBitFieldAtId(iPos)->setValue(bitFields[iPos]->getMaxValue());
//			} else {
////				int64_t tmp = getBitFieldAtId(iPos)->getValue();
//				// Add the resolution scale to the value...
//				incrementAtId(iPos,
//						pow(2ll,offsetResolution - bitFields[iPos]->getOffset()+1) - 1 );
////				 getBitFieldAtId(iPos)->setValue(
////						pow(2ll,offsetResolution - bitFields[iPos]->getOffset()+1) - 1 );
//			}
//			iBit += iWidth; --iPos;
//		}
//		// cout << 5 << flush;
//		// print();
//	}

// We now set terminator below a resolution not a levelName. The resolution can be at any appropriate bit position.
//	void setTerminatorBelowLevel(string levelName) {
//		setTerminatorBelowLevel(getFieldId(levelName));
//	}

};

}

#endif /* INCLUDE_TEMPORALWORDFORMAT1_H_ */
