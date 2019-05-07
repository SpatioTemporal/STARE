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

#include "BitField.h"
#include "TemporalWordFormat1.h"

#include "erfa.h"

namespace std {

class TemporalWordFormat : virtual public TemporalWordFormat1 {
public:
	TemporalWordFormat() {
		// resolutionLevelConstraint =  7; // Counts the number of levels, including
		nonDataLevels             =  3;

		// maxCoResolutionLevelValue = 9; // Is actually more than we count in resLevel.
		int64_t offset_base = 64;
//		int64_t coResolutionLevel = maxCoResolutionLevelValue;
//		int64_t coFieldId = maxCoResolutionLevelValue;
		int64_t fieldId = 0;
		// int64_t resolutionLevel = -2;  // TODO Fix hackery
		// resolutionLevelConstraint = coResolutionLevel + resolutionLevel;
		/// coResolutionLevel + resolutionLevel = maxCoResolutionLevelValue - 2
		/// Hence, we start the resolutionLevel at -1 on "Ma". It's like
		/// having another north-south bit (past/future bit)
		///
		/// TODO Put more thought into the "calendar" below...

		bitFields.push_back(
				new BitField("BeforeAfterStartBit",
						1, // maxValue
						1, // width
						offset_base, // address/offset // in: 64; set to: 63 out: 63
						-2,   // scale -- really need N/A here
						fieldId++
				)
		);
		bitFieldMap.insert(pair<string,BitField*>(bitFields.back()->getName(),bitFields.back()));

		pos_CoarsestResolutionLevel = fieldId;
		bitFields.push_back(
				new BitField("year",
						pow(2,19)-1, // maxValue
						19, // width
						offset_base,
						(13ll*28+1)*24*3600*1000,
						fieldId++
				)
		);
		bitFieldMap.insert(pair<string,BitField*>(bitFields.back()->getName(),bitFields.back()));

		bitFields.push_back(
				new BitField("month",
						13, //maxValue // TODO Repent: 13 months with 4 weeks each, and a 14th with one day... Or maybe you just use 12...
						4, // width
						offset_base,
						28ll*24*3600*1000,
						fieldId++
				)
		);
		bitFieldMap.insert(pair<string,BitField*>(bitFields.back()->getName(),bitFields.back()));

		bitFields.push_back(
				new BitField("week",
						3, // maxValue
						2, // width
						offset_base,
						7*24*3600*1000,
						fieldId++
				)
		);
		bitFieldMap.insert(pair<string,BitField*>(bitFields.back()->getName(),bitFields.back()));

		bitFields.push_back(
				new BitField("day",
						6, // maxValue
						3, // width
						offset_base,
						24*3600*1000,
						fieldId++
				)
		);
		bitFieldMap.insert(pair<string,BitField*>(bitFields.back()->getName(),bitFields.back()));

		bitFields.push_back(
				new BitField("hour",
						23, // maxValue
						5, // width
						offset_base,
						3600*1000,
						fieldId++
				)
		);
		bitFieldMap.insert(pair<string,BitField*>(bitFields.back()->getName(),bitFields.back()));

		bitFields.push_back(
				new BitField("minute",
						59, // maxValue
						6, // width
						offset_base,
						60*1000,
						fieldId++
				)
		);
		bitFieldMap.insert(pair<string,BitField*>(bitFields.back()->getName(),bitFields.back()));

		bitFields.push_back(
				new BitField("second",
						59, // maxValue
						6, // width
						offset_base,
						1000,
						fieldId++
				)
		);
		bitFieldMap.insert(pair<string,BitField*>(bitFields.back()->getName(),bitFields.back()));

		pos_FinestResolutionLevel = fieldId;
		bitFields.push_back(
				new BitField("millisecond",
						999, // maxValue
						10, // width
						offset_base,
						1, // time unit
						fieldId++ // the actual low, should equal zero
				)
		);
		bitFieldMap.insert(pair<string,BitField*>(bitFields.back()->getName(),bitFields.back()));

		bitFields.push_back(
				new BitField("resolution",
						pow(2,6)-1, // maxValue
						6, // width
						offset_base,
						-1, // time unit, not applicable
						fieldId++ // coResolutionLevel-- // -1, i.e. N/A
				)
		);
		bitFieldMap.insert(pair<string,BitField*>(bitFields.back()->getName(),bitFields.back()));

		bitFields.push_back(
				new BitField("type",
						3, // maxValue
						2, // width
						offset_base,
						-1, // time unit, not applicable
						fieldId++ // N/A
				)
		);
		bitFieldMap.insert(pair<string,BitField*>(bitFields.back()->getName(),bitFields.back()));


		// setValue("BeforeAfterStartBit",1); // Default to "positive" dates.
		setFieldMaxId(fieldId-1);
		setValue("type",2); //

		/*
		bitFields.push_back(
				BitField("coResolutionLevel",
						-1, // maxValue
						-1, // width
						offset_base,
						-1, // Not applicable
						-1 // coResolutionLevel-- // -1, i.e. N/A
				)
		);
		bitFieldMap.insert(pair<string,BitField>(bitFields.back().getName(),bitFields.back()));
		*/

	}
	virtual ~TemporalWordFormat();
};

// static const TemporalWordFormat temporalWordFormat;
// static const int64_t maxValue_coResolutionLevel =  7 ; // = 7

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

	TemporalIndex() {
		// data = temporalWordFormat; // Copy the format
		TemporalWordFormat data;
		// data.setValue("coResolutionLevel",maxValue_coResolutionLevel); // default
		data.setValue("resolution",63);
	};
	virtual ~TemporalIndex();
	TemporalIndex(
			int64_t BeforeAfterStartBit,
			int64_t year,
			int64_t month,
			int64_t week,
			int64_t day,
			int64_t hour,
			int64_t minute,
			int64_t second,
			int64_t millisecond,
			int64_t resolution,
			int64_t type
			)
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
		SET_VALUE( year );
		SET_VALUE( month );
		SET_VALUE( week );
		SET_VALUE( day );
		SET_VALUE( hour );
		SET_VALUE( minute );
		SET_VALUE( second );
		SET_VALUE( millisecond );
		SET_VALUE( resolution );
		SET_VALUE( type );
		// SET_VALUE( coResolutionLevel );
#undef SET_VALUE
	};

// #define SHIFT_AND_MASK(field) field = mask_##field & (idx_ >> offset_##field ) ;
#define SHIFT_AND_MASK(field) data.setValue(#field,\
		data.get(#field)->getMask() & (idx_ >> data.get(#field)->getOffset()));

#define REVERT(field) data.setValue(#field, data.get(#field)->getMaxValue()-data.getValue(#field) );

#define SET_MAX(field) data.setValue(#field,min(data.getValue(#field),data.get(#field)->getMaxValue()));

	int leapYearDay(int64_t _year) {
		// From wikipedia
		int leapyear_day = 0;
		if( (_year % 4) != 0 ) {
			leapyear_day = 0;
		} else if( (_year % 100) != 0 ) {
			leapyear_day = 1;
		} else if( (_year % 400) != 0 ) {
			leapyear_day = 0;
		} else { leapyear_day = 1; }
		return leapyear_day;
	}

//#define SHIFT_AND_MASK_RESOLUTION(field) \
//		data.setValue(#field, \
//				data.resolutionLevelConstraint \
//				- (data.get(#field)->getMask() & (idx_ >> data.get(#field)->getOffset())) );

	TemporalIndex(int64_t scidbTemporalIndex) {
		int64_t idx_ = scidbTemporalIndex;

		data.setValue("BeforeAfterStartBit",idx_ > 0 ? 1 : 0);
		// SHIFT_AND_MASK_RESOLUTION(coResolutionLevel)
		int64_t babit = 1;
		if(idx_<0) {
			babit = 0;
			idx_ = - idx_;
			// idx_ = idx_ | (1ll << 63);
		}
		SHIFT_AND_MASK(year)
		SHIFT_AND_MASK(month)
		SHIFT_AND_MASK(week)
		SHIFT_AND_MASK(day)
		SHIFT_AND_MASK(hour)
		SHIFT_AND_MASK(minute)
		SHIFT_AND_MASK(second)
		SHIFT_AND_MASK(millisecond)
		SHIFT_AND_MASK(resolution)
		SHIFT_AND_MASK(type)

		// if(false) {
		if(babit == 0) {
			// REVERT(year);
			REVERT(month);
			REVERT(week);
			REVERT(day);
			REVERT(hour);
			REVERT(minute);
			REVERT(second);
			REVERT(millisecond);
		}

		// Note: data.getValue("resolution") == 63 for a terminator.

		// TODO Come up with a better leap year handler
		// TODO Think through temporal design re: translation to & from dates.

		if( data.getValue("resolution") == 63 ) {
//			// Fix things up if this is a terminator
			if(data.getValue("year") > 0) {
				data.setValue("month",13);
				data.setValue("week",0);
				data.setValue("day",0);
				// data.setValue("day",leapYearDay(data.getValue("year")));
			}
			SET_MAX(year);
			if(data.getValue("month") > 12) {
				data.setValue("month",13);
				data.setValue("week",0);
				data.setValue("day",0);
				// data.setValue("day",leapYearDay(data.getValue("year")));
			}
			SET_MAX(month);
			SET_MAX(week);   // ok
			SET_MAX(day);    // ok
			SET_MAX(hour);   // ok
			SET_MAX(minute); // ok
			SET_MAX(second); // ok
			SET_MAX(millisecond); // ok
		}
	};
#undef REVERT
#undef SET_MAX
#undef SHIFT_AND_MASK

// #undef SHIFT_AND_MASK_RESOLUTION

	/*
	int64_t getCoResolutionLevel(string levelName) {
		return data.getCoResolutionLevel(levelName);
	}
	int64_t getResolutionLevel(string levelName) {
		return data.resolutionLevelConstraint - data.getCoResolutionLevel(levelName);
	}
	*/

	int64_t getCoFieldId(string levelName) {
		return data.getCoFieldId(levelName);
	}
	int64_t getFieldId(string levelName) {
		return data.getFieldId(levelName);
	}

	// TODO Check for errors/integrity of data value.
// #define MASK_AND_SHIFT(field) ((( mask_##field & field ) << offset_##field ))

 #define MASK_AND_SHIFT_REVERSE(babit,field) (( data.get(#field)->getMask() \
		& ( babit*data.getValue(#field)   + (1-babit)*(data.get(#field)->getMaxValue()-data.getValue(#field)))  ) \
		<< data.get(#field)->getOffset() )

#define MASK_AND_SHIFT(babit,field) (( data.get(#field)->getMask() \
		& data.getValue(#field) ) \
		<< data.get(#field)->getOffset() )

// #define CHECK_MASK_AND_SHIFT(babit,field) cout << "CMAS "<< #field << " " << data.getValue(#field) \
// 	<< " " << (data.get(#field)->getMaxValue()-data.getValue(#field)) << endl << flush;

// #define MASK_AND_SHIFT_RESOLUTION(field) (( data.get(#field)->getMask() & (data.resolutionLevelConstraint-data.getValue(#field)) ) \
//		<< data.get(#field)->getOffset() )

	int64_t scidbTemporalIndex() {

		int64_t babit = data.getValue("BeforeAfterStartBit");

		int64_t idx_;
//		cout << "100 ms: " << millisecond << " " << (0x3ffll  & millisecond) << endl << flush;
//		cout << "100 Ma: " << Ma << " " << (0x7fll  & Ma) << endl << flush;
//		cout << "100 rL: " << coResolutionLevel << endl << flush;
		idx_ =
				MASK_AND_SHIFT(babit,year) |
				MASK_AND_SHIFT_REVERSE(babit,month) |
				MASK_AND_SHIFT_REVERSE(babit,week) |
				MASK_AND_SHIFT_REVERSE(babit,day) |
				MASK_AND_SHIFT_REVERSE(babit,hour) |
				MASK_AND_SHIFT_REVERSE(babit,minute) |
				MASK_AND_SHIFT_REVERSE(babit,second) |
				MASK_AND_SHIFT_REVERSE(babit,millisecond) |
				MASK_AND_SHIFT(1,resolution) |
				MASK_AND_SHIFT(1,type)
				;

/*
		CHECK_MASK_AND_SHIFT(babit,year)
		CHECK_MASK_AND_SHIFT(babit,month);
		CHECK_MASK_AND_SHIFT(babit,week);
		CHECK_MASK_AND_SHIFT(babit,day);
		CHECK_MASK_AND_SHIFT(babit,hour);
		CHECK_MASK_AND_SHIFT(babit,minute);
		CHECK_MASK_AND_SHIFT(babit,second);
		CHECK_MASK_AND_SHIFT(babit,millisecond);
		CHECK_MASK_AND_SHIFT(1,resolution);
		CHECK_MASK_AND_SHIFT(1,type);
*/
		if(0x1 && data.getValue("BeforeAfterStartBit") == 0) {
			// past
			idx_ = -idx_;
			// idx_ = idx_ | (1ll << 63);
		} // else future...
		// old idx_ = idx_ | MASK_AND_SHIFT_RESOLUTION(coResolutionLevel);
		return idx_;
	};
#undef MASK_AND_SHIFT
// #undef MASK_AND_SHIFT_RESOLUTION

/*
 * Keep the level
 * Use resolution full (all bits set) to flag terminator.
 */
	int64_t scidbTerminator() {
//		cout << "TemporalIndex::scidbTerminator not implemented!!" << endl << flush;
		// cout << "a";
		TemporalIndex tmpIndex(this->scidbTemporalIndex());
		// cout << "b";
		// tmpIndex.data.setTerminatorBelowLevel(get_coResolutionLevel());
		tmpIndex.data.setTerminatorBelowResolution(get_resolution());
		// cout << "c";
		int64_t idx_ = tmpIndex.scidbTemporalIndex();
		// cout << "d";
		idx_ = idx_
				| (tmpIndex.data.get("resolution")->getMask() <<  tmpIndex.data.get("resolution")->getOffset())
				| ( (tmpIndex.data.get("type")->getValue() <<  tmpIndex.data.get("type")->getOffset()) )
				;
		return idx_;
	};

	bool scidbTerminatorp() {
		int64_t resolution = this->data.getValue("resolution");
		return resolution == 63;
	}

	TemporalIndex &set_zero() {
		data.setZero();
		return *this;
	}

// #define SET(field) TemporalIndex &set_##field(int64_t x) { field = x; if( (x < 0) || (x > maxValue_##field)) throw SpatialFailure("TemporalIndex:DomainFailure in ",name_##field.c_str()); return *this;}
#define SET(field) TemporalIndex &set_##field(int64_t x) { data.setValue(#field, x ); \
	if( (x < 0) || (x > data.get(#field)->getMaxValue()))\
	throw SpatialFailure("TemporalIndex:DomainFailure in ",data.get(#field)->getName().c_str()); return *this;}
	SET(BeforeAfterStartBit)
	SET(year)
	SET(month)
	SET(week)
	SET(day)
	SET(hour)
	SET(minute)
	SET(second)
	SET(millisecond)
	SET(resolution)
	SET(type)
	// SET(coResolutionLevel)
#undef  SET
	// below is obsolete and wrong
//	TemporalIndex &set_resolutionLevel(int64_t level) {
//		data.setValue("coResolutionLevel",data.resolutionLevelConstraint - level); // TODO refactor
//		return *this;
//	}

#define GET(field) int64_t get_##field() { return data.getValue(#field); }
	GET(BeforeAfterStartBit)
	GET(year)
	GET(month)
	GET(week)
	GET(day)
	GET(hour)
	GET(minute)
	GET(second)
	GET(millisecond)
	GET(resolution)
	GET(type)
	// GET(coResolutionLevel)
#undef GET
	// int64_t get_resolutionLevel() { return data.resolutionLevelConstraint - get_coResolutionLevel(); }


// #define OUTPUT(field) cout << name_##field << " " << field << " " << bitWidth_##field << " " << mask_##field << endl << flush;
#define OUTPUT(field) cout \
		<< setw(20) << data.get(#field)->getName() \
		<< setw(6) << data.getValue(#field) \
		<< setw(3) << data.get(#field)->getWidth() \
		<< setw(5) << data.get(#field)->getMask() \
		<< setw(4) << data.get(#field)->getOffset() \
		<< setw(4) << dec << data.get(#field)->getCoFieldId() << hex \
		<< endl << flush;
	void checkBitFormat() {
		cout << hex;
		OUTPUT(BeforeAfterStartBit);
		OUTPUT(year);
		OUTPUT(month);
		OUTPUT(week);
		OUTPUT(day);
		OUTPUT(hour);
		OUTPUT(minute);
		OUTPUT(second);
		OUTPUT(millisecond);
		OUTPUT(resolution);
		OUTPUT(type);
		// OUTPUT(coResolutionLevel);
		cout << dec;
	}
#undef OUTPUT

	void hackSetTraditionalDate(
			int64_t CE,             // 0 or 1: 0 = BCE, 1 = CE
			int64_t _year, 			// > 0
			int64_t _month, 		// 1..12 not 0..11
			int64_t _day_of_month, 	// 1..31
			int64_t _hour, 			// 0..23
			int64_t _minute, 		// 0..59
			int64_t _second, 		// 0..59
			int64_t _millisecond 	// 0..999
	) {

	  if(_year < 1)
	    { stringstream ss; ss << "TemporalIndex::hackSetTraditionalDate:CHECK_BOUND:ERROR _year < 1" << endl;
	      throw SpatialFailure(ss.str().c_str()); }

#define CHECK_BOUND(lo,val,hi) \
	if ((val < lo) || (hi < val)) \
	{ stringstream ss; ss << "TemporalIndex::hackSetTraditionalDate:CHECK_BOUND:ERROR in " << #val; \
		throw SpatialFailure(ss.str().c_str()); }
	CHECK_BOUND(1,_year,15999999);
    // CHECK_BOUND(1,_year,4700);
	CHECK_BOUND(1,_month,12);
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

		// days_in_month[1] += leapYearDay(_year);
		int lyd = leapYearDay(_year-1+CE); // TODO CHECK LYD
		days_in_month[1] += lyd;

		int64_t days_in_months = 0;

		// _month 1..12
		for(int imo=0; imo < _month-1; ++imo) {
			days_in_months += days_in_month[imo];
		}

		int64_t milliseconds_total =
				_millisecond +
				1000ll * (_second +
						60ll * (_minute +
								60ll * (_hour +
										24ll * ( _day_of_month - 1ll +
												days_in_months))));

		// 365ll * _year ))));
		// TODO HACK HACK HACK ?Use 365.25?

		data.setValue("year",_year);

#define SCALE(field) data.setValue(#field, \
	(milliseconds_total / data.get(#field)->getScale()) \
	% (data.get(#field)->getMaxValue() + 1) ); \
	milliseconds_total -= data.get(#field)->getScale()*data.getValue(#field);
		SCALE(month);
		SCALE(week);
		SCALE(day);
		SCALE(hour);
		SCALE(minute);
		SCALE(second);
#undef SCALE

		// There is no year zero.
		if( CE <= 0 ) {
			data.decrementAtName("year");
		}
		// data.incrementAtName("year", 0);
		data.setValue("millisecond",_millisecond);
		data.setValue("BeforeAfterStartBit",CE);
	}

	void hackGetTraditionalDate(
			int64_t &_BeforeAfterStartBit,
			int64_t &_year,
			int64_t &_month, // 1..12 not 0..11
			int64_t &_day_of_month, // 1..31
			int64_t &_hour, // 0..23
			int64_t &_minute, // 0..59
			int64_t &_second, // 0..59
			int64_t &_millisecond // 0..999
	) {

		int64_t milliseconds_total = 0;

		_BeforeAfterStartBit = data.getValue("BeforeAfterStartBit");

//		cout << "100" << endl << flush;

		int64_t CE = data.getValue("BeforeAfterStartBit");
		_year = data.getValue("year");
		// There is no year zero. Alias it to 1 BCE.
		if( CE == 0 ) {
			++_year;
		}
		int lyd = leapYearDay(_year-1+CE); // TODO CHECK LYD

#define SCALE(field) milliseconds_total += \
		  data.getValue(#field) * data.get(#field)->getScale() \
		  ;
		// SCALE(year);
		SCALE(month);
		SCALE(week);
		SCALE(day);
		SCALE(hour);
		SCALE(minute);
		SCALE(second);
		SCALE(millisecond);
#undef SCALE
		/*
		(* 3600 24)
		86400

		86400000

		(* 365 (* 3600 24))
		31536000

		(* 365.25 (* 3600 24))
		31557600.0
		 */

		int64_t days_per_year = 365ll+lyd;

		int64_t msec_per_year = days_per_year*24ll*3600ll*1000ll;
		// 365.25 int64_t msec_per_year = 31557600000;

		milliseconds_total += msec_per_year;

		int64_t _day_of_year = 0;

//		cout << "200" << endl << flush;

		/*
#define EXTRACT(field,max_val) cout << #field << " " << flush; \
	field = milliseconds_total % max_val; \
	cout << "-1-" << flush; \
	milliseconds_total -= field ; \
	cout << "-2-" << flush; \
	milliseconds_total /= max_val; \
	cout << "-3-" << endl << flush;
	*/
#define EXTRACT(field,max_val) \
	field = milliseconds_total % max_val; \
	milliseconds_total -= field ; \
	milliseconds_total /= max_val;

//#define EXTRACTF(field,max_val) \
//	field = (int64_t) fmod( milliseconds_total, max_val ); \
//	milliseconds_total -= field ; \
//	milliseconds_total /= max_val;

		EXTRACT(_millisecond,1000ll);
		EXTRACT(_second,     60ll);
		EXTRACT(_minute,     60ll);
		EXTRACT(_hour,       24ll);
		EXTRACT(_day_of_year,days_per_year);  // TODO HACK HACK HACK ?Use 365.25?
		// EXTRACT(_day_of_year,365ll);  // TODO HACK HACK HACK ?Use 365.25?
		//		EXTRACTF(_day_of_year,365.25);  // TODO HACK HACK HACK ?Use 365.25?
		// _year = milliseconds_total;

#undef EXTRACT

		// Already handled above.
		// There is no year zero. Alias it to 1 BCE.
//		if( CE == 0 ) {
//			++_year;
//		}
//		} else {
//			--_year;
//		}

		// int64_t milliseconds_per_year = 1000ll * 60ll * 60ll * 24ll * 365ll; // Note this is not 365.25...

//		cout << "300" << endl << flush;

		// Be a little silly.
		// TODO Fix for correct work
		int days_in_month[] = {
				31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
		};

		/*
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
		*/

		days_in_month[1] += lyd;

//		cout << "400" << endl << flush;

		int imo = 0;
		while( _day_of_year >= days_in_month[imo] ) { // bug mlr 2017-0602 was >
			// cout << "401 " << _day_of_year << " " << imo << endl;
			_day_of_year -= days_in_month[ imo++ ];
			if( imo > 11 ) {
				throw SpatialFailure("TemporalIndex:hackGetTraditionalDate:MonthArrayOverflow");
			}
		}
		_month = imo+1;
		_day_of_month = _day_of_year + 1;

//		cout << "500" << endl << flush;

	}

	string hackStringInTraditionalDate() {
	  
	  int64_t
	    BeforeAfterStartBit = 0,
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
	    ( BeforeAfterStartBit,
	      year,
	      month,
	      day_of_month,
	      hour,
	      minute,
	      second,
	      millisecond
	      );

	  stringstream ss;
	  // ss << tIndex;
	  // << setw(4) << setfill('0') << year << "-"
	  if( BeforeAfterStartBit> 0 ) {
		  ss << setw(2) << "1 ";
	  } else {
		  ss << setw(2) << "0 ";
	  }

	  ss
	    << setw(9) << setfill('0') << year << "-"
	    << setw(2) << setfill('0') << month << "-"
	    << setw(2) << setfill('0') << day_of_month << " "
	    << setw(2) << hour << ":"
	    << setw(2) << minute << ":"
	    << setw(2) << second << "."
	    << setw(3) << millisecond
		<< " (" << setw(2) << data.getValue("resolution") << ")"
		<< " (" << setw(1) << data.getValue("type") << ")"
		;

	  return ss.str(); // Traditional date
	}

	void hackFromTraditionalString(string traditionalString) {

	  // TODO repent the sin of hardcoding
	  int pos = 0;
#define PARSE_INT(field,width) \
	  int64_t field = atoi(traditionalString.substr(pos,width).c_str()); pos += width + 1;
	  PARSE_INT(CE,2);
	  PARSE_INT(year,traditionalString.find("-"));
	  PARSE_INT(month,2);
	  PARSE_INT(day_of_month,2);
	  PARSE_INT(hour,2);
	  PARSE_INT(minute,2);
	  PARSE_INT(second,2);
	  PARSE_INT(millisecond,3);
	  ++pos;
	  PARSE_INT(level,2);
#undef PARSE_INT

	  hackSetTraditionalDate (
			  CE,
			  year,
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
	  if(data.getValue("BeforeAfterStartBit") == 0 ) {
		  ss << setw(1) << "-";
	  } else {
		  ss << setw(1) << "+";
	  }
	  ss
	    << setw(9) << setfill('0') << data.getValue("year") << "-"
	    << setw(2) << setfill('0') << data.getValue("month") << "-"
	    << setw(1) << setfill('0') << data.getValue("week") << "-"
	    << setw(1) << setfill('0') << data.getValue("day") << " "
		<< setw(2) << setfill('0') << data.getValue("hour") << ":"
		<< setw(2) << setfill('0') << data.getValue("minute") << ":"
	    << setw(2) << setfill('0') << data.getValue("second") << "."
	    << setw(3) << setfill('0') << data.getValue("millisecond")
	    << " (" << setw(2) << data.getValue("resolution") << ")"
	    << " (" << setw(1) << data.getValue("type") << ")"
		;

	  return ss.str();
	}

	void fromNativeString(string nativeString) {
	  // TODO repent the sin of hardcoding
	  int pos = 0;
#define PARSE_INT(field,width) \
	  data.setValue(#field,atoi(nativeString.substr(pos,width).c_str())); \
	  pos += width;
	  /*
	   cout << endl \
	   << #field << ": " << atoi(nativeString.substr(pos,width).c_str()) *continuation*
	   << ", '" << nativeString.substr(pos,width) << "'" << endl; *continuation*
	   */
	  // PARSE_INT(year,nativeString.find("-")); ++pos;
	  PARSE_INT(year,9); ++pos;
	  PARSE_INT(month,2); ++pos;
	  PARSE_INT(week,1); ++pos;
	  PARSE_INT(day,1); ++pos;
	  PARSE_INT(hour,2); ++pos;
	  PARSE_INT(minute,2); ++pos;
	  PARSE_INT(second,2); ++pos;
	  PARSE_INT(millisecond,3); pos += 2;
	  PARSE_INT(resolutionLevel,2);
#undef PARSE_INT
	  data.setValue("BeforeAfterStartBit",1);
	}

	int eraTest() {
		int iy, im, id, ihour, imin, j, iymdf[4];
		double d1, d2, sec, d, fd;
		/* Date and time. */
		iy = 2008; im = 2; id = 29;ihour = 23; imin = 59; sec = 59.9;
		iy = -4700; im = 2; id = 1;ihour = 23; imin = 59; sec = 59.9;

		// iy = -4800; im = 2; id = 28;ihour = 23; imin = 59; sec = 59.9;

		printf ( "0: %4d/%2.2d/%2.2d%3d:%2.2d:%4.1f\n",iy, im, id, ihour, imin, sec );
		/* Express as 2-part JD. */
		j = eraCal2jd ( iy, im, id, &d1, &d2 );if ( j ) return 1;
		j = eraTf2d ( '+', ihour, imin, sec, &d );if ( j ) return 1;
		d2 += d;
		printf ( "1: %9.1f +%13.6f =%15.6f\n", d1, d2, d1 + d2 );
		/* Express as calendar date and fraction of a day. */
		j = eraJd2cal ( d1, d2, &iy, &im, &id, &fd );if ( j ) return 1;
		d = ( (double) id ) + fd;
		printf ( "2: %4d/%2.2d/%9.6f\n", iy, im, d );
		/* Round to 0.001 day. */
		j = eraJdcalf ( 3, d1, d2, iymdf );if ( j ) return 1;
		printf ( "3: %4d/%2.2d/%2.2d.%3.3d\n",iymdf[0], iymdf[1], iymdf[2], iymdf[3] );

		return 0;
	}

};

} /* namespace std */

void TemporalIndex_test();

#endif /* SRC_TEMPORALINDEX_H_ */
