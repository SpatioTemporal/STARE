/*
 * BitField.h
 *
 *  Created on: Apr 16, 2019
 *      Author: mrilee
 *
 *  Copyright (C) 2019 Rilee Systems Technologies LLC
 */

#include "SpatialException.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <map>

#include <ctime>

#ifndef INCLUDE_BITFIELD_H_
#define INCLUDE_BITFIELD_H_

// namespace std {
using namespace std;

int firstBitDifferenceFromLeft(int64_t i0, int64_t i1);
int64_t makeMask(int width);

// TODO Switch to templates for these different fields.
// TODO Use vectors for the following.
// TODO How to make a macro that does the following?

class BitField {
	string  name = "nameless";
	int64_t value = 0;
	int64_t maxValue;
	int64_t width;
	int64_t mask;
	int64_t offset;
	int64_t scale;
	int64_t headroom;
	int64_t fieldId; /// Index bit fields from the right of the word.
	int64_t maxFieldId = 0;
public:
	BitField();
	BitField(
			string  name,
			int64_t maxValue,
			int64_t width,
			int64_t &offset,
			int64_t scale,
			int64_t fieldId
			) :
	name(name),
	maxValue(maxValue),
	width(width),
	offset(offset-width),
	scale(scale),
	fieldId(fieldId)
	{
		mask = pow(2ll,width) -1;
		headroom = mask - maxValue;
		offset -= width;

	};
	virtual ~BitField();

	string toStringHeader() const {
		stringstream ss;
		ss
			<< setw(20) << "name"
			<< setw(12) << "value"
			<< setw(12) << "maxValue"
			<< setw(9)  << "headroom"
			<< setw(7)  << "width"
			<< setw(8)  << "mask"
			<< setw(8)  << "offset"
			<< setw(20) << "scale"
			<< setw(4)  << "fId"
			<< setw(8)  << "max-fId"
			;
		return ss.str();
	}

	string toString() const {
		stringstream ss;
		ss
			<< setw(20) << name
			<< setw(12) << value
			<< setw(12) << maxValue
			<< setw(9)  << headroom
			<< setw(7)  << width
			<< setw(8)  << mask
			<< setw(8)  << offset
			<< setw(20) << scale
			<< setw(4)  << fieldId
			<< setw(8)  << maxFieldId
			;
		return ss.str();
	}

	string   getName()   const {
		// cout << "y100 '" << name << "'" << endl << flush;
		return name; }
	int64_t  getMask()   const { return mask; }
	int64_t  getOffset() const { return offset; }
	int64_t  getMaxValue() const { return maxValue; }
	int64_t  getWidth()  const { return width; }
	int64_t  getScale()  const { return scale; }

	int64_t  getValue()  const { return value; }
	void     setValue(int64_t value) {
		// cout << "- setValue - " << value << endl << flush;
		// cout << "- setValue 0 " << this->value << endl << flush;
		this->value = value;
		// cout << "- setValue 1 " << this->value << endl << flush;
	}

	void setMaxFieldId(int64_t maxFieldId) { this->maxFieldId = maxFieldId;	}
	int64_t getMaxFieldId()const { return maxFieldId;	}
	int64_t getFieldId()   const { return fieldId; }
	int64_t getCoFieldId() const { return maxFieldId - fieldId; }
};

// }

#endif /* INCLUDE_BITFIELD_H_ */
