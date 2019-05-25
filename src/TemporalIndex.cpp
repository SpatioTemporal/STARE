/*
 * TemporalIndex.cpp
 *
 *  Created on: Apr 10, 2017
 *      Author: mrilee
 */

#include "TemporalIndex.h"

namespace std {

BitField::BitField() {}
BitField::~BitField() {}

TemporalWordFormat::~TemporalWordFormat() {}

TemporalIndex::~TemporalIndex() {
	// TODO Auto-generated destructor stub
}



// #define OUTPUT(field) cout << name_##field << " " << field << " " << bitWidth_##field << " " << mask_##field << endl << flush;
#define OUTPUT(field) cout \
		<< setw(20) << data.get(#field)->getName() \
		<< setw(6) << data.getValue(#field) \
		<< setw(3) << data.get(#field)->getWidth() \
		<< setw(5) << data.get(#field)->getMask() \
		<< setw(4) << data.get(#field)->getOffset() \
		<< setw(4) << dec << data.get(#field)->getCoFieldId() << hex \
		<< endl << flush;
void TemporalIndex::checkBitFormat() {
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

void TemporalIndex::hackSetTraditionalDate(
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

	if( CE == 1 && _year == 0 ){
		throw SpatialFailure("TemporalIndex:hackSetTraditionalDate:InvalidYearZeroCE");
	}

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



void TemporalIndex::hackGetTraditionalDate(
		int64_t &_BeforeAfterStartBit,
		int64_t &_year,
		int64_t &_month, // 1..12 not 0..11
		int64_t &_day_of_month, // 1..31
		int64_t &_hour, // 0..23
		int64_t &_minute, // 0..59
		int64_t &_second, // 0..59
		int64_t &_millisecond // 0..999
) const {

	int64_t milliseconds_total = 0;

	_BeforeAfterStartBit = data.getValue("BeforeAfterStartBit");

	//		cout << "100" << endl << flush;

	int64_t CE = data.getValue("BeforeAfterStartBit");
	_year = data.getValue("year");
	// There is no year zero.
	if( CE == 0 ) {
		++_year;
	} else if ( _year == 0 ){
		throw SpatialFailure("TemporalIndex:hackGetTraditionalDate:InvalidYearZeroCE");
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




string TemporalIndex::hackStringInTraditionalDate() {

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

void TemporalIndex::hackFromTraditionalString(string traditionalString) {

	// cout << endl << endl << "hfts1 " << traditionalString << endl << flush;
	// TODO repent the sin of hardcoding

	int pos = 0;
#define PARSE_INT(field,width) \
		int64_t field = atoi(traditionalString.substr(pos,width).c_str()); pos += width + 1;

	//	  cout << endl << "pi: " << traditionalString.substr(pos,width).c_str() << endl;

	// TAG(1000)
	PARSE_INT(CE,1);
	PARSE_INT(year,traditionalString.find("-")-2);
	PARSE_INT(month,2);
	PARSE_INT(day_of_month,2);
	PARSE_INT(hour,2);
	PARSE_INT(minute,2);
	PARSE_INT(second,2);
	PARSE_INT(millisecond,3);
	++pos;
	PARSE_INT(resolution,2);
	++pos; ++pos;
	PARSE_INT(type,1);
#undef PARSE_INT
	// TAG(2000)
	/*
#define CHECK(var) cout << #var << " 0x" << hex << var << dec << " " << var << endl << flush;
  CHECK(CE)
  CHECK(year)
  CHECK(month)
  CHECK(day_of_month)
  CHECK(hour)
  CHECK(minute)
  CHECK(second)
  CHECK(millisecond)
  CHECK(resolution)
  CHECK(type)
#undef CHECK
	 */
	// TAG(2100)
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
	// TAG(2200)
	data.setValue("resolution",resolution);
	data.setValue("type",type);
}

string TemporalIndex::stringInNativeDate() {

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

void TemporalIndex::fromNativeString(string nativeString) {
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

int TemporalIndex::eraTest() {
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


int firstBitDifferenceFromLeft(int64_t i0, int64_t i1) {
	int base = 64; int address = base;
	int64_t i01 = (i0 & i1 ) | ( ~i0 & ~i1 );
	uint64_t one = 1, test;
	do {
		--address;
		uint64_t testBit = one << address;
		test = ( testBit & i01 ) >> address;
	} while ( test == one && address >= 0 );
	return base - address - 1;
}

int64_t makeMask(int width) { return pow(2ll,width) -1; }



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


/**
 * Return a temporal index suitable for use with SciDB. Indexes a sparse temporal
 * dimension of an array.
 *
 * TODO Question: Should this be guaranteed to produce a valid index, throwing an
 * exception if not?
 */
int64_t TemporalIndex::scidbTemporalIndex() {

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
#undef MASK_AND_SHIFT_REVERSE
// #undef MASK_AND_SHIFT_RESOLUTION


int64_t TemporalIndex::bitOffsetFinest() {
	int64_t offsetBottom = data.bitFields[data.pos_FinestResolutionLevel]->getOffset();
	return offsetBottom;
}

int64_t TemporalIndex::bitOffsetCoarsest() {
	int64_t offsetTop =
			data.bitFields[data.pos_CoarsestResolutionLevel]->getOffset() +
			data.bitFields[data.pos_CoarsestResolutionLevel]->getWidth()-1;
	return offsetTop;
}

int64_t TemporalIndex::bitOffsetResolution(int64_t resolution) {
	return bitOffsetCoarsest() - resolution;
}

int64_t TemporalIndex::bitfieldIdFromResolution(int64_t resolution) {
	int offsetTop = bitOffsetCoarsest();
	int offsetResolution = bitOffsetResolution(resolution);
	// int iBit = bitOffsetFinest();
	int iPos = data.pos_FinestResolutionLevel;
	while( iPos > this->data.pos_CoarsestResolutionLevel
			&& offsetResolution > data.bitFields[iPos]->getOffset() + data.bitFields[iPos]->getWidth()
	) {
		--iPos;
	}
	return iPos;

	//	while( iBit <= offsetResolution ) {
	//		int64_t iWidth = data.bitFields[iPos]->getWidth();
	//		iBit+=iWidth; --iPos;
	//	}
	//	return iPos+1;
}

/*
 * Keep the level
 * Use resolution full (all bits set) to flag terminator.
 */
int64_t TemporalIndex::scidbTerminator() {
	//		cout << "TemporalIndex::scidbTerminator not implemented!!" << endl << flush;
	TemporalIndex tmpIndex(this->scidbTemporalIndex());
	int64_t resolution = tmpIndex.get_resolution();

	// Set the terminator type. Elsewhere we hard code this to 63... The following is correct.
	tmpIndex.set_resolution(tmpIndex.data.get("resolution")->getMask());

	//////////////////////////////////
	// Let's add an amount corresponding to the resolution.
	int64_t delta = tmpIndex.millisecondsAtResolution(resolution);
	int64_t t0    = tmpIndex.toInt64Milliseconds();
	int64_t t1    = t0 + delta;
	tmpIndex.fromInt64Milliseconds(t1);
	int64_t idx_ = tmpIndex.scidbTemporalIndex();
	return idx_;
}

int64_t TemporalIndex::scidbTerminatorJulian() {
	int64_t idx_ = 0;
	TemporalIndex tmpIndex(this->scidbTemporalIndex());
	int64_t resolution = tmpIndex.get_resolution();

	// Set the terminator type. Elsewhere we hard code this to 63... The following is correct.
	tmpIndex.set_resolution(tmpIndex.data.get("resolution")->getMask());

	//////////////////////////////////
	// Let's add an amount corresponding to the resolution.
	int64_t delta = tmpIndex.julianDoubleDayAtResolution(resolution);
	double d1, d2;
	tmpIndex.toJulianDoubleDay(d1,d2);
	tmpIndex.fromJulianDoubleDay(d1,d2+delta);
	int64_t idx_ = tmpIndex.scidbTemporalIndex();
	return idx_;
}

bool TemporalIndex::scidbTerminatorp() {
	int64_t resolution = this->data.getValue("resolution");
	return resolution == 63;
}

TemporalIndex& TemporalIndex::set_zero() {
	data.setZero();
	return *this;
}

TemporalIndex& TemporalIndex::setZero() {
	data.setZero();
	return *this;
}

TemporalIndex& TemporalIndex::setEOY(int64_t year, int64_t babit) {
	data.setZero();
	for(int iPos = data.pos_FinestResolutionLevel; iPos > data.pos_CoarsestResolutionLevel; --iPos) {
		data.getBitFieldAtId(iPos)->setValue(data.getBitFieldAtId(iPos)->getMaxValue());
	}
	data.setValue("BeforeAfterStartBit",babit);
	data.setValue("year",year);
	// Fix the last month.
	data.setValue("week", 0);
	data.setValue("day",  0);
	// Add a leap year?
	return *this;
}


void fractionalDayToHMSM(double fd, int& hour, int& minute, int& second, int& ms) {
	//cout << endl << dec << flush;
	double fd_left = fd;
	//cout << " fd_left " << setw(8) << setfill(' ') << fd_left << endl << flush;
	fd_left *= 24; hour = (int) fd_left; fd_left -= hour;
	//cout << " fd_left " << setw(8) << setfill(' ') << fd_left << ", hour   " << setw(8) << setfill(' ') << hour << endl << flush;
	fd_left *= 60; minute = (int) fd_left; fd_left -= minute;
	//cout << " fd_left " << setw(8) << setfill(' ') << fd_left << ", minute " << setw(8) << setfill(' ') << minute << endl << flush;
	fd_left *= 60; second = (int) fd_left; fd_left -= second;
	//cout << " fd_left " << setw(8) << setfill(' ') << fd_left << ", second " << setw(8) << setfill(' ') << second << endl << flush;
	fd_left *= 1000; ms = (int) fd_left; // discard remaining.
	//cout << " fd_left " << setw(8) << setfill(' ') << fd_left << ", ms     " << setw(8) << setfill(' ') << ms << endl << flush;
	//cout << endl << flush;
}

void TemporalIndex::toJulianDoubleDay(double& d1, double& d2) const {
	int64_t _babit, _year, _month, _day_of_month, _hour, _minute, _second, _millisecond;
	this->hackGetTraditionalDate(
			_babit, _year, _month, _day_of_month, _hour, _minute, _second, _millisecond);

	if( _babit < 1 ) {
		// If we're in BCE, correct the _year ( 1 BCE goes to year 0) and reverse.
		--_year; _year *= -1;
	}

	int not_ok;
	// double d1,d2,d; // [days]
	// double d;

	// j = iauDtf2d ( "TAI", 2009, 1, 1, 0, 0, 33.7, &a1, &a2 );
	not_ok = eraDtf2d( "UTC", _year, _month, _day_of_month, _hour, _minute, _second+(_millisecond*0.001), &d1, &d2 );
	// deprecated not_ok=eraCal2jd( _year, _month, _day_of_month, &d1, &d2 );
	// '+' Corresponds to "positive"
	// not_ok=eraTf2d  ( '+', _hour, _minute, _second, &d); d2 += d;
	// double dtot = d1 + d2 + d + (_millisecond/(1000.0*86400.0));
	// double dtot = d1 + d2 + d;
	// dtot += dtot*1000.0*86400.0 + _millisecond;
	// return (int64_t)dtot;
};
TemporalIndex& TemporalIndex::fromJulianDoubleDay( double d1, double d2) {
	int not_ok;
	int iy, im, id;
	int _hour, _minute, _second, _millisecond;
	// double fd;
	int64_t CE = 1;
	// not_ok = eraJd2cal ( d1, d2, &iy, &im, &id, &fd );
	// j = iauD2dtf ( "UTC", 3, u1, u2, &iy, &im, &id, ihmsf );
	int ihmsf[4];
	not_ok = eraD2dtf ( "UTC", 3, d1, d2, &iy, &im, &id, ihmsf );
	// int iymdf[4];
	// not_ok = eraJdcalf ( 8, d1, d2, iymdf );
	// not_ok = eraJdcalf ( 3, d1, d2, iymdf );
	//		iy=iymdf[0];
	//		im=iymdf[1];
	//		id=iymdf[2];
	//		fd=iymdf[3];
	_hour   = ihmsf[0];
	_minute = ihmsf[1];
	_second = ihmsf[2];
	_millisecond = ihmsf[3];
	if( iy < 1) {
		CE = 0;
		--iy;
	}
	// not_ok = eraJdcalf ( 3, d1, d2, iymdf );
	// 1/86400e3 = 1.16e-8. Keep day-fraction to 1.0e-8
	// fractionalDayToHMSM(fd,_hour, _minute, _second, _millisecond);
	this->hackSetTraditionalDate(CE, (int64_t)iy, (int64_t)im, (int64_t)id, (int64_t)_hour, (int64_t)_minute, (int64_t)_second, (int64_t)_millisecond);
	return *this;

};

/// Note: for indexing, not astronomy. Support a kind of "addition".
int64_t TemporalIndex::toInt64Milliseconds() const {

	int i = data.pos_CoarsestResolutionLevel; // Should be for the 'year' field
	int64_t sum = (data.getBitFieldAtId(i)->getValue()) * data.getBitFieldAtId(i)->getScale();

	int64_t CE  = this->get_BeforeAfterStartBit();
	if(CE < 1) {
		sum = -sum;
	}

	for( int i = data.pos_FinestResolutionLevel; i > data.pos_CoarsestResolutionLevel; --i ) {
		sum += data.getBitFieldAtId(i)->getValue() * data.getBitFieldAtId(i)->getScale();
	}

	return sum;
}
TemporalIndex& TemporalIndex::fromInt64Milliseconds(int64_t milliseconds) {
	int64_t sum = 0, total_left = milliseconds, CE = -1, year;

	int i = data.pos_CoarsestResolutionLevel;
	if( total_left < data.getBitFieldAtId(i)->getScale() ) {

		///////////////////////////////
		this->set_BeforeAfterStartBit(0);
		CE = 0;

		year = ((-(total_left / data.getBitFieldAtId(i)->getScale()) ) % (data.getBitFieldAtId(i)->getMaxValue() + 1));

		// Get rid of the years
		total_left += year*data.getBitFieldAtId(i)->getScale();

		if( total_left < 0 ) {
			total_left += data.getBitFieldAtId(i)->getScale();
			++year; // Recall we're in BCE territory.
		}
		///////////////////////////////
	} else {
		this->set_BeforeAfterStartBit(1);
		CE = 1;
		// year = (total_left / data.getBitFieldAtId(i)->getScale()) % (data.getBitFieldAtId(i)->getMaxValue() + 1);
		year = (total_left / data.getBitFieldAtId(i)->getScale()) % (data.getBitFieldAtId(i)->getMaxValue() + 1);
		total_left -= year*data.getBitFieldAtId(i)->getScale();
	}
	data.getBitFieldAtId(i)->setValue(year);

	for( int i = data.pos_CoarsestResolutionLevel+1; i <= data.pos_FinestResolutionLevel; ++i ) {
		data.getBitFieldAtId(i)->setValue(
				(total_left / data.getBitFieldAtId(i)->getScale())
				% (data.getBitFieldAtId(i)->getMaxValue() + 1) );

		total_left -= data.getBitFieldAtId(i)->getValue()*data.getBitFieldAtId(i)->getScale();
	}
	return *this;
}

/**
 * Return a number of milliseconds based on the resolution (bit position).
 */
int64_t TemporalIndex::millisecondsAtResolution(int64_t resolution) {

	TemporalIndex tmpIndex;
	int64_t bitPosition;
	int64_t offsetTop = bitOffsetCoarsest();
	int64_t offsetResolution = bitOffsetResolution(resolution);
	int64_t offsetBottom = bitOffsetFinest();
	int     iBit = offsetBottom;
	int     iPos = tmpIndex.data.pos_FinestResolutionLevel;
	int64_t sum = 0;
	// iPos indicates a field
	while( iBit <= offsetResolution ) {
		int64_t iWidth = tmpIndex.data.bitFields[iPos]->getWidth();

		if( offsetResolution >= tmpIndex.data.bitFields[iPos]->getOffset() + iWidth ) {
			// The last item of each field.
			// sum += tmpIndex.data.bitFields[iPos]->getMaxValue() * tmpIndex.data.bitFields[iPos]->getScale();
		} else {
			// This resolution bit position lands in the middle of a field.
			int64_t delta =
					min((int64_t) pow(2ll,offsetResolution - tmpIndex.data.bitFields[iPos]->getOffset()), // +1), // - 1,
							tmpIndex.data.bitFields[iPos]->getMaxValue() );
			sum += delta * tmpIndex.data.bitFields[iPos]->getScale();
		}
		iBit += iWidth; --iPos;
	}
	return sum;
}

double TemporalIndex::julianDoubleDayAtResolution(int64_t resolution) {
	return millisecondsAtResolutions(resolution) / 86400.0e3;
}

} /* namespace std */
