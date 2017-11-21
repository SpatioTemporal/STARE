/*
 * TemporalIndex.h
 *
 *  Created on: Apr 10, 2017
 *      Author: mrilee
 */

#ifndef SRC_TEMPORALINDEX_H_
#define SRC_TEMPORALINDEX_H_

#include "SpatialException.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <map>

#include <ctime>

namespace std {

int firstBitDifferenceFromLeft(int64_t i0, int64_t i1);
int64_t makeMask(int width);

// TODO Switch to templates for these different fields.
// TODO Use vectors for the following.
// TODO How to make a macro that does the following?

class BitField {
	string  name;
	int64_t value = 0;
	int64_t maxValue;
	int64_t width;
	int64_t mask;
	int64_t offset;
	int64_t scale;
	int64_t headroom;
	int64_t coResolutionLevel;
public:
	BitField();
	BitField(
			string  name,
			int64_t maxValue,
			int64_t width,
			int64_t &offset,
			int64_t scale,
			int64_t coResolutionLevel
			) :
	name(name),
	maxValue(maxValue),
	width(width),
	offset(offset-width),
	scale(scale),
	coResolutionLevel(coResolutionLevel)
	{
		mask = pow(2ll,width) -1;
		headroom = mask - maxValue;
		offset -= width;

	};
	virtual ~BitField();

	string toString() const {
		stringstream ss;
		ss
			<< setw(20) << name
			<< setw(5)  << maxValue
			<< setw(5)  << headroom
			<< setw(5)  << width
			<< setw(5)  << offset
			<< setw(20) << scale
			<< setw(5)  << coResolutionLevel;

		return ss.str();
	}

	string   getName()   const { return name; }
	int64_t  getMask()   const { return mask; }
	int64_t  getOffset() const { return offset; }
	int64_t  getMaxValue() const { return maxValue; }
	int64_t  getWidth()  const { return width; }
	int64_t  getScale()  const { return scale; }

	int64_t  getValue()  const { return value; }
	void     setValue(int64_t value) {
		this->value = value;
	}
	int64_t getCoResolutionLevel() const { return coResolutionLevel; }
};

class TemporalWordFormat {
	vector<BitField>     bitFields;   // Construct order... Verify object identity?
	map<string,BitField> bitFieldMap; // Holds the data, can we have the same obj as previous?
public:
	int64_t maxCoResolutionLevelValue =  9;  // Would be const in a perfect universe.
	int64_t minCoResolutionLevelValue = -1; // Corresponds to "Ma"
	int64_t resolutionLevelConstraint =  7;
	TemporalWordFormat() {
		// maxCoResolutionLevelValue = 9; // Is actually more than we count in resLevel.
		int64_t offset_base = 64;
		int64_t coResolutionLevel = maxCoResolutionLevelValue;
		// int64_t resolutionLevel = -2;  // TODO Fix hackery
		// resolutionLevelConstraint = coResolutionLevel + resolutionLevel;
		/// coResolutionLevel + resolutionLevel = maxCoResolutionLevelValue - 2
		/// Hence, we start the resolutionLevel at -1 on "Ma". It's like
		/// having another north-south bit (past/future bit)
		///
		/// TODO Put more thought into the "calendar" below...

		bitFields.push_back(
				BitField("BeforeAfterStartBit",
						1, // maxValue
						1, // width
						offset_base, // address/offset // in: 64; set to: 63 out: 63
						-2,   // scale -- really need N/A here
						coResolutionLevel--
				)
		);
		bitFieldMap.insert(pair<string,BitField>(bitFields.back().getName(),bitFields.back()));

		bitFields.push_back(
				BitField("Ma",
						15,
						4,
						offset_base, // in: 63; set to: 59; out: 59
						1000ll*1000*365*24*3600*1000,
						coResolutionLevel--
				)
		);
		bitFieldMap.insert(pair<string,BitField>(bitFields.back().getName(),bitFields.back()));

		bitFields.push_back(
				BitField("ka",
						999, // maxValue
						10, // width
						offset_base, // in: 59; set to: 49; out: 49
						1000ll*365*24*3600*1000,
						coResolutionLevel-- // the real start at rL = 7.
				)
		);
		bitFieldMap.insert(pair<string,BitField>(bitFields.back().getName(),bitFields.back()));

		bitFields.push_back(
				BitField("year",
						999, // maxValue
						10, // width
						offset_base,
						(13ll*28+1)*24*3600*1000,
						coResolutionLevel--
				)
		);
		bitFieldMap.insert(pair<string,BitField>(bitFields.back().getName(),bitFields.back()));

		bitFields.push_back(
				BitField("month",
						13, //maxValue // TODO Repent: 13 months with 4 weeks each, and a 14th with one day... Or maybe you just use 12...
						4, // width
						offset_base,
						28ll*24*3600*1000,
						coResolutionLevel--
				)
		);
		bitFieldMap.insert(pair<string,BitField>(bitFields.back().getName(),bitFields.back()));

		bitFields.push_back(
				BitField("week",
						3, // maxValue
						2, // width
						offset_base,
						7*24*3600*1000,
						coResolutionLevel--
				)
		);
		bitFieldMap.insert(pair<string,BitField>(bitFields.back().getName(),bitFields.back()));

		bitFields.push_back(
				BitField("day",
						6, // maxValue
						3, // width
						offset_base,
						24*3600*1000,
						coResolutionLevel--
				)
		);
		bitFieldMap.insert(pair<string,BitField>(bitFields.back().getName(),bitFields.back()));

		bitFields.push_back(
				BitField("hour",
						23, // maxValue
						5, // width
						offset_base,
						3600*1000,
						coResolutionLevel--
				)
		);
		bitFieldMap.insert(pair<string,BitField>(bitFields.back().getName(),bitFields.back()));

		bitFields.push_back(
				BitField("second",
						3599, // maxValue
						12, // width
						offset_base,
						1000,
						coResolutionLevel--
				)
		);
		bitFieldMap.insert(pair<string,BitField>(bitFields.back().getName(),bitFields.back()));

		bitFields.push_back(
				BitField("millisecond",
						999, // maxValue
						10, // width
						offset_base,
						1,
						coResolutionLevel-- // the actual low, should equal zero
				)
		);
		bitFieldMap.insert(pair<string,BitField>(bitFields.back().getName(),bitFields.back()));

		bitFields.push_back(
				BitField("coResolutionLevel",
						7, // maxValue
						3, // width
						offset_base,
						-1, // Not applicable
						coResolutionLevel-- // -1, i.e. N/A
				)
		);
		bitFieldMap.insert(pair<string,BitField>(bitFields.back().getName(),bitFields.back()));

	}
	virtual ~TemporalWordFormat();

	void print() const {
		int i = 10;
		for(vector<BitField>::const_iterator it = bitFields.begin(); it != bitFields.end(); ++it) {
			cout
			<< setw(2)
			<< --i << " "
			<< (*it).toString() << endl << flush;
		}

	}
	BitField get(string name) const {
		return bitFieldMap.at(name);
	}
	TemporalWordFormat& setValue(string name, int64_t value) {
		if(name != "resolutionLevel") {
			bitFieldMap.at(name).setValue(value);
		} else {
			bitFieldMap.at("coResolutionLevel").setValue(resolutionLevelConstraint-value);
		}
		return *this;
	}
	int64_t getValue(string name) {
	  if(name != "resolutionLevel") {
		return bitFieldMap.at(name).getValue();
	  } else {
	    return
	      resolutionLevelConstraint - bitFieldMap.at("coResolutionLevel").getValue();
	  }
	}

	void setZero() {
		int64_t coResolutionLevel = bitFieldMap.at("coResolutionLevel").getValue();
		for(map<string,BitField>::iterator it = bitFieldMap.begin(); it != bitFieldMap.end(); ++it) {
			(*it).second.setValue(0);
		}
		bitFieldMap.at("coResolutionLevel").setValue(coResolutionLevel);
	}

	// TODO throw excpetion if levelName is not a resolution level, i.e. a non-level field
	int64_t getCoResolutionLevel(string levelName) {
		return bitFieldMap.at(levelName).getCoResolutionLevel();
	}

	BitField &getBitFieldAtLevel(int64_t coResolutionLevel) {
		// TODO Repent hard coding here
		// int idx_coResolutionLevel = 9;
		int idx_coResolutionLevel = bitFields.size(); // rL should be the last one.
		int idx = idx_coResolutionLevel-2-coResolutionLevel; // Assume levels consecutive
		return bitFieldMap.at(bitFields[idx].getName());
	}

	void incrementAtLevel(int64_t level,int64_t delta) {
		if( (level < minCoResolutionLevelValue)
			|| (maxCoResolutionLevelValue < level) )
		{
			throw SpatialFailure("TemporalWordFormat:incrementAtLevel:LevelOutOfBounds");
		}
//		cout << "10000: " << level << endl;
		int64_t oldValue = getBitFieldAtLevel(level).getValue();
//		cout << "10010: " << oldValue << endl;
		int64_t newValue = oldValue + delta;

		// TODO Check for overflow & underflow? Carry?
		if(newValue>getBitFieldAtLevel(level).getMaxValue()) {
			// Carry!
			// TODO 8 is the "top" level here, even though we have a 3-bit level field here.
			// TODO fix hardcode
			if(level >= maxCoResolutionLevelValue) { // == 9 //
				throw SpatialFailure("TemporalWordFormat:OverflowError");
			}
//			cout << "10100: " << newValue << endl;
			incrementAtLevel(level+1,1); // level = 0 => highest resolution, greatest depth
			newValue-=getBitFieldAtLevel(level).getMaxValue()+1;
//			cout << "10110: " << newValue << endl;
		}
//		cout << "19000: " << newValue << endl;
		getBitFieldAtLevel(level).setValue(newValue);
//		cout << "19001: " << getBitFieldAtLevel(level).getValue() << endl;
	}
	void incrementAtLevel(string levelName, int64_t delta=1) {
		incrementAtLevel(getCoResolutionLevel(levelName),delta);
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
		int64_t oldValue = getBitFieldAtLevel(level).getValue();
		int64_t newValue = oldValue - delta;
		// TODO Check for overflow & underflow? Carry?
		if(newValue<0){
			if(level==0) {
				throw SpatialFailure("TemporalWordFormat:UnderflowError");
			}
			decrementAtLevel(level+1,1); // level = 0 => highest resolution, greatest depth
			newValue+=getBitFieldAtLevel(level).getMaxValue()+1;
		}
		getBitFieldAtLevel(level).setValue(newValue);
	}
	void decrementAtLevel(string levelName, int64_t delta = 1) {
		decrementAtLevel(getCoResolutionLevel(levelName), delta);
	}

	void setTerminatorBelowLevel(int64_t level){
		// TODO Biggest abs-value possible in the field?
		while( level-- > 0) {
			int64_t newValue = getBitFieldAtLevel(level).getMask(); // negation handled elsewhere
			// int64_t newValue = getBitFieldAtLevel(level).getMaxValue();
			getBitFieldAtLevel(level).setValue(newValue);
		}
	}
	void setTerminatorBelowLevel(string levelName) {
		setTerminatorBelowLevel(getCoResolutionLevel(levelName));
	}

};
static const TemporalWordFormat temporalWordFormat;
static const int64_t maxValue_coResolutionLevel =  7 ; // = 7

//int scidbTemporalIndexCommonLevel(int64_t i0, int64_t i1) {
//	int commonLevel = -3; // error
//	if(i0 > 0 && i1 < 0) {
//		commonLevel = -2; // No levels in common
//	} else {
//		int64_t i01 =
//				( i0 &  i1) &
//				(~i0 & ~i1)
//				;
//		int k = 0;
//		uint64_t one = 1, testBit = one << 63, test;
//		do {
//			test = testBit & i01;
//			testBit >> 1;
//		} while( test != 0 && testBit > 0);
//	}
//
//}

// TODO Question: Am I working myself towards an n-adic template?
class TemporalIndex {
public:
	TemporalWordFormat data;
	// TODO Make better use of temporalWordFormat...
//	int64_t BeforeAfterStartBit = 0;
//	int64_t Ma            = 0;
//	int64_t ka            = 0;
//	int64_t year          = 0;
//	int64_t month         = 0;
//	int64_t week          = 0;
//	int64_t day           = 0;
//	int64_t hour          = 0;
//	int64_t second        = 0;
//	int64_t millisecond   = 0;
//	int64_t coResolutionLevel = maxValue_coResolutionLevel; // full (millisecond) resolution; = 0 => ka resolution

	TemporalIndex() {
		data = temporalWordFormat; // Copy the format
		data.setValue("coResolutionLevel",maxValue_coResolutionLevel); // default
	};
	virtual ~TemporalIndex();
	TemporalIndex(
			int64_t BeforeAfterStartBit,
			int64_t Ma,
			int64_t ka,
			int64_t year,
			int64_t month,
			int64_t week,
			int64_t day,
			int64_t hour,
			int64_t second,
			int64_t millisecond,
			int64_t coResolutionLevel )
//	:
//				BeforeAfterStartBit(BeforeAfterStartBit),
//				Ma(Ma),
//				ka(ka),
//				year(year),
//				month(month),
//				day(day),
//				hour(hour),
//				second(second),
//				millisecond(millisecond),
//				coResolutionLevel(coResolutionLevel)
	{

	  if( BeforeAfterStartBit != 1 )
	    { stringstream ss; ss << "TemporalIndex::NOT_IMPLEMENTED_ERROR in TemporalIndex(...) BeforeAfterStartBit = 0 (past)" << endl;
	      ss << "TODO: Correct index scheme for the past. E.g. years go negative, but not months, weeks, etc." << endl;
	      throw SpatialFailure(ss.str().c_str()); }
	  
#define SET_VALUE(field) data.setValue(#field,field);
		SET_VALUE( BeforeAfterStartBit );
		SET_VALUE( Ma );
		SET_VALUE( ka );
		SET_VALUE( year );
		SET_VALUE( month );
		SET_VALUE( week );
		SET_VALUE( day );
		SET_VALUE( hour );
		SET_VALUE( second );
		SET_VALUE( millisecond );
		SET_VALUE( coResolutionLevel );
#undef SET_VALUE
	};

// #define SHIFT_AND_MASK(field) field = mask_##field & (idx_ >> offset_##field ) ;
#define SHIFT_AND_MASK(field) data.setValue(#field,data.get(#field).getMask() & (idx_ >> data.get(#field).getOffset()));
#define SHIFT_AND_MASK_RESOLUTION(field) \
		data.setValue(#field, \
				data.resolutionLevelConstraint \
				- (data.get(#field).getMask() & (idx_ >> data.get(#field).getOffset())) );

	TemporalIndex(int64_t scidbTemporalIndex) {
		int64_t idx_ = scidbTemporalIndex;

		data.setValue("BeforeAfterStartBit",idx_ > 0 ? 1 : 0);
		SHIFT_AND_MASK_RESOLUTION(coResolutionLevel)
		if(idx_<0) {
			idx_ = - idx_;
		}
		SHIFT_AND_MASK(Ma)
		SHIFT_AND_MASK(ka)
		SHIFT_AND_MASK(year)
		SHIFT_AND_MASK(month)
		SHIFT_AND_MASK(week)
		SHIFT_AND_MASK(day)
		SHIFT_AND_MASK(hour)
		SHIFT_AND_MASK(second)
		SHIFT_AND_MASK(millisecond)
	};
#undef SHIFT_AND_MASK
#undef SHIFT_AND_MASK_RESOLUTION

	int64_t getCoResolutionLevel(string levelName) {
		return data.getCoResolutionLevel(levelName);
	}

	int64_t getResolutionLevel(string levelName) {
		return data.resolutionLevelConstraint - data.getCoResolutionLevel(levelName);
	}

	// TODO Check for errors/integrity of data value.
// #define MASK_AND_SHIFT(field) ((( mask_##field & field ) << offset_##field ))
#define MASK_AND_SHIFT(field) (( data.get(#field).getMask() & data.getValue(#field) ) \
		<< data.get(#field).getOffset() )
#define MASK_AND_SHIFT_RESOLUTION(field) (( data.get(#field).getMask() & (data.resolutionLevelConstraint-data.getValue(#field)) ) \
		<< data.get(#field).getOffset() )
	int64_t scidbTemporalIndex() {
		int64_t idx_;
//		cout << "100 ms: " << millisecond << " " << (0x3ffll  & millisecond) << endl << flush;
//		cout << "100 Ma: " << Ma << " " << (0x7fll  & Ma) << endl << flush;
//		cout << "100 rL: " << coResolutionLevel << endl << flush;
		idx_ =
				MASK_AND_SHIFT(Ma) |
				MASK_AND_SHIFT(ka) |
				MASK_AND_SHIFT(year) |
				MASK_AND_SHIFT(month) |
				MASK_AND_SHIFT(week) |
				MASK_AND_SHIFT(day) |
				MASK_AND_SHIFT(hour) |
				MASK_AND_SHIFT(second) |
				MASK_AND_SHIFT(millisecond)
				;
		if(0x1 && data.getValue("BeforeAfterStartBit") == 0) {
			// past
			idx_ = -idx_;
		} // else future...
		idx_ = idx_ | MASK_AND_SHIFT_RESOLUTION(coResolutionLevel);
		return idx_;
	};
#undef MASK_AND_SHIFT
#undef MASK_AND_SHIFT_RESOLUTION

	int64_t scidbTerminator() {
//		cout << "TemporalIndex::scidbTerminator not implemented!!" << endl << flush;
		TemporalIndex tmpIndex(this->scidbTemporalIndex());
		tmpIndex.data.setTerminatorBelowLevel(get_coResolutionLevel());
		int64_t idx_ = tmpIndex.scidbTemporalIndex();
		idx_ = idx_ | tmpIndex.data.get("coResolutionLevel").getMask();
		return idx_;
	};

	TemporalIndex &set_zero() {
		data.setZero();
		return *this;
	}

// #define SET(field) TemporalIndex &set_##field(int64_t x) { field = x; if( (x < 0) || (x > maxValue_##field)) throw SpatialFailure("TemporalIndex:DomainFailure in ",name_##field.c_str()); return *this;}
#define SET(field) TemporalIndex &set_##field(int64_t x) { data.setValue(#field, x ); \
	if( (x < 0) || (x > data.get(#field).getMaxValue()))\
	throw SpatialFailure("TemporalIndex:DomainFailure in ",data.get(#field).getName().c_str()); return *this;}
	SET(BeforeAfterStartBit)
	SET(Ma)
	SET(ka)
	SET(year)
	SET(month)
	SET(week)
	SET(day)
	SET(hour)
	SET(second)
	SET(millisecond)
	SET(coResolutionLevel)
#undef  SET
	TemporalIndex &set_resolutionLevel(int64_t level) {
		data.setValue("coResolutionLevel",data.resolutionLevelConstraint - level); // TODO refactor
		return *this;
	}

#define GET(field) int64_t get_##field() { return data.getValue(#field); }
	GET(BeforeAfterStartBit)
	GET(Ma)
	GET(ka)
	GET(year)
	GET(month)
	GET(week)
	GET(day)
	GET(hour)
	GET(second)
	GET(millisecond)
	GET(coResolutionLevel)
#undef GET
	int64_t get_resolutionLevel() { return data.resolutionLevelConstraint - get_coResolutionLevel(); }

// #define OUTPUT(field) cout << name_##field << " " << field << " " << bitWidth_##field << " " << mask_##field << endl << flush;
#define OUTPUT(field) cout \
		<< setw(20) << data.get(#field).getName() \
		<< setw(6) << data.getValue(#field) \
		<< setw(3) << data.get(#field).getWidth() \
		<< setw(5) << data.get(#field).getMask() \
		<< endl << flush;
	void checkBitFormat() {
		cout << hex;
		OUTPUT(BeforeAfterStartBit);
		OUTPUT(Ma);
		OUTPUT(ka);
		OUTPUT(year);
		OUTPUT(month);
		OUTPUT(week);
		OUTPUT(day);
		OUTPUT(hour);
		OUTPUT(second);
		OUTPUT(millisecond);
		OUTPUT(coResolutionLevel);
		cout << dec;
	}
#undef OUTPUT

	void hackSetTraditionalDate(
				    int64_t _year, // > 0
			int64_t _month, // 0..11
			int64_t _day_of_month, // 1..31
			int64_t _hour, // 0..23
			int64_t _minute, // 0..59
			int64_t _second, // 0..59
			int64_t _millisecond // 0..999
	) {

	  if(_year < 0) 
	    { stringstream ss; ss << "TemporalIndex::hackSetTraditionalDate:CHECK_BOUND:ERROR _year < 0" << endl;
	      ss << "TODO: Correct hackSetTraditionalDate to handle negative years." << endl;
	      throw SpatialFailure(ss.str().c_str()); }
	  
#define CHECK_BOUND(lo,val,hi) \
	if ((val < lo) || (hi < val)) \
	{ stringstream ss; ss << "TemporalIndex::hackSetTraditionalDate:CHECK_BOUND:ERROR in " << #val; \
		throw SpatialFailure(ss.str().c_str()); }
	CHECK_BOUND(0,_year,15999999);
	CHECK_BOUND(0,_month,11);
	CHECK_BOUND(1,_day_of_month,31);
	CHECK_BOUND(0,_hour,23);
	CHECK_BOUND(0,_minute,59);
	CHECK_BOUND(0,_second,59);
	CHECK_BOUND(0,_millisecond,999);
#undef  CHECK_BOUND
		// Be a little silly.
		// TODO Fix for correct work
		int days_in_month[] = {
				31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
		};
		// From wikipedia
		int leapyear_day = 0;
		if( (_year % 4) != 0 ) {
			leapyear_day = 0;
		} else if( (_year % 100) != 0 ) {
			leapyear_day = 1;
		} else if( (_year % 400) != 0 ) {
			leapyear_day = 0;
		} else { leapyear_day = 1; }

		days_in_month[1] += leapyear_day;

		int64_t days_in_months = 0;

		for(int imo=0; imo < _month; ++imo) {
			days_in_months += days_in_month[imo];
		}

		int64_t milliseconds_total =
				_millisecond +
				1000ll * (_second +
						60ll * (_minute +
								60ll * (_hour +
										24ll * ( _day_of_month - 1ll +
												days_in_months +
												365ll * _year )))); // TODO HACK HACK HACK ?Use 365.25?

#define SCALE(field) data.setValue(#field, \
	(milliseconds_total / data.get(#field).getScale()) \
	% (data.get(#field).getMaxValue() + 1) ); \
	milliseconds_total -= data.get(#field).getScale()*data.getValue(#field);
		SCALE(Ma);
		SCALE(ka);
		SCALE(year);
		SCALE(month);
		SCALE(week);
		SCALE(day);
		SCALE(hour);
		SCALE(second);
#undef SCALE
		data.incrementAtLevel("ka",   0);
		data.incrementAtLevel("year", 0);
		data.setValue("millisecond",_millisecond);
		data.setValue("BeforeAfterStartBit",1);
	}

	void hackGetTraditionalDate(
			int64_t &_year,
			int64_t &_month, // 0..11
			int64_t &_day_of_month, // 1..31
			int64_t &_hour, // 0..23
			int64_t &_minute, // 0..59
			int64_t &_second, // 0..59
			int64_t &_millisecond // 0..999
	) {

		int64_t milliseconds_total = 0;

//		cout << "100" << endl << flush;

#define SCALE(field) milliseconds_total += \
		  data.getValue(#field) * data.get(#field).getScale() \
		  ;
		SCALE(Ma);
		SCALE(ka);
		SCALE(year);
		SCALE(month);
		SCALE(week);
		SCALE(day);
		SCALE(hour);
		SCALE(second);
		SCALE(millisecond);
#undef SCALE

		int64_t _day_of_year = 0;

//		cout << "200" << endl << flush;

//#define EXTRACT(field,max_val) cout << #field << " " << flush; \
//	field = milliseconds_total % max_val; \
//	cout << "-1-" << flush; \
//	milliseconds_total -= field ; \
//	cout << "-2-" << flush; \
//	milliseconds_total /= max_val; \
//	cout << "-3-" << endl << flush;
#define EXTRACT(field,max_val) \
	field = milliseconds_total % max_val; \
	milliseconds_total -= field ; \
	milliseconds_total /= max_val;
		EXTRACT(_millisecond,1000ll);
		EXTRACT(_second,     60ll);
		EXTRACT(_minute,     60ll);
		EXTRACT(_hour,       24ll);
		EXTRACT(_day_of_year,365ll);  // TODO HACK HACK HACK ?Use 365.25?
		_year = milliseconds_total;
#undef EXTRACT

//		cout << "300" << endl << flush;

		// Be a little silly.
		// TODO Fix for correct work
		int days_in_month[] = {
				31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
		};

		// From wikipedia
		int leapyear_day = 0;
		if( (_year % 4) != 0 ) {
			leapyear_day = 0;
		} else if( (_year % 100) != 0 ) {
			leapyear_day = 1;
		} else if( (_year % 400) != 0 ) {
			leapyear_day = 0;
		} else { leapyear_day = 1; }
		days_in_month[1] += leapyear_day;

//		cout << "400" << endl << flush;

		int imo = 0;
		while( _day_of_year >= days_in_month[imo] ) { // bug mlr 2017-0602 was >
			// cout << "401 " << _day_of_year << " " << imo << endl;
			_day_of_year -= days_in_month[ imo++ ];
			if( imo > 11 ) {
				throw SpatialFailure("TemporalIndex:hackGetTraditionalDate:MonthArrayOverflow");
			}
		}
		_month = imo;
		_day_of_month = _day_of_year + 1;

//		cout << "500" << endl << flush;

	}

	string hackStringInTraditionalDate() {
	  
	  int64_t
	    year = 0,
	    month = 0,
	    day_of_month = 0,
	    hour = 0,
	    minute = 0,
	    second = 0,
	    millisecond = 0
	    ;

	  // Traditional format
	  hackGetTraditionalDate
	    ( year,
	      month,
	      day_of_month,
	      hour,
	      minute,
	      second,
	      millisecond
	      );

	  stringstream ss;
	  // ss << tIndex;
	  ss
	    << setw(4) << setfill('0') << year << "-"
	    << setw(2) << setfill('0') << month << "-"
	    << setw(2) << setfill('0') << day_of_month << " "
	    << setw(2) << hour << ":"
	    << setw(2) << minute << ":"
	    << setw(2) << second << "."
	    << setw(3) << millisecond
	    << " (" << setw(2) << data.getValue("resolutionLevel") << ")";

	  return ss.str();
	}

	void hackFromTraditionalString(string traditionalString) {

	  // TODO repent the sin of hardcoding
	  int pos = 0;
#define PARSE_INT(field,width) \
	  int64_t field = atoi(traditionalString.substr(pos,width).c_str()); pos += width + 1;
	  PARSE_INT(year,4);
	  PARSE_INT(month,2);
	  PARSE_INT(day_of_month,2);
	  PARSE_INT(hour,2);
	  PARSE_INT(minute,2);
	  PARSE_INT(second,2);
	  PARSE_INT(millisecond,3);
	  ++pos;
	  PARSE_INT(level,2);
#undef PARSE_INT
	  hackSetTraditionalDate
	    ( year,
	      month,
	      day_of_month,
	      hour,
	      minute,
	      second,
	      millisecond
	      );
	  data.setValue("resolutionLevel",level);
	}

	string stringInNativeDate() {

	  stringstream ss;
	  ss
	    << setw(3) << setfill('0') << data.getValue("Ma") << "-"
	    << setw(3) << setfill('0') << data.getValue("ka")
	    << setw(3) << setfill('0') << data.getValue("year") << "-"
	    << setw(2) << setfill('0') << data.getValue("month") << "-"
	    << setw(1) << setfill('0') << data.getValue("week") << "-"
	    << setw(1) << setfill('0') << data.getValue("day") << " "
	    << setw(2) << setfill('0') << data.getValue("hour") << ":"
	    << setw(4) << setfill('0') << data.getValue("second") << "."
	    << setw(3) << setfill('0') << data.getValue("millisecond")
	    << " (" << setw(2) << data.getValue("resolutionLevel") << ")";

	  return ss.str();
	}

	void fromNativeString(string nativeString) {
	  // TODO repent the sin of hardcoding
	  int pos = 0;
#define PARSE_INT(field,width) \
	  data.setValue(#field,atoi(nativeString.substr(pos,width).c_str())); \
	  pos += width;
	  // cout << endl \
	  // << #field << ": " << atoi(nativeString.substr(pos,width).c_str()) *continuation*
	  // << ", '" << nativeString.substr(pos,width) << "'" << endl; *continuation*

	  PARSE_INT(Ma,3); ++pos;
	  PARSE_INT(ka,3);
	  PARSE_INT(year,3); ++pos;
	  PARSE_INT(month,2); ++pos;
	  PARSE_INT(week,1); ++pos;
	  PARSE_INT(day,1); ++pos;
	  PARSE_INT(hour,2); ++pos;
	  PARSE_INT(second,4); ++pos;
	  PARSE_INT(millisecond,3); pos += 2;
	  PARSE_INT(resolutionLevel,2);
#undef PARSE_INT
	  data.setValue("BeforeAfterStartBit",1);
	}


};

} /* namespace std */

#endif /* SRC_TEMPORALINDEX_H_ */
