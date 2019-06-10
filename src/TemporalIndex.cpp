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

/**
 * Translate a number of milliseconds and a year into the native format.
 *
 * If the number of milliseconds is larger than the usual, then the extra are simply
 * added at the end of the native format year.
 *
 * This is a low-level routine. It sets the native format as directed, *except* for avoiding ambiguity
 * at year = 0. CE=0, _year=0 is allowed, CE=1, _year=0 is not.
 *
 * TODO Consider possibility of a latent overflow, i.e. _year or _milliseconds might be too large for the algorithm.
 */
TemporalIndex& TemporalIndex::fromNativeCEYearAndMilliseconds(
		int64_t CE,             // 0 or 1: 0 = BCE, 1 = CE
		int64_t _year, 			// > 0
		int64_t _milliseconds   // >= 0, < (* 365.0 86400.0 1000.0) 31536000000.0
		) {
	// TODO Check for consistency?
	if( CE != 0 && CE != 1 ) {
		throw SpatialFailure("TemporalIndex::setDateFromYearAndMilliseconds: CE != 0 or 1.");
	}
	if( _year < 1 && CE != 0 ) {
		throw SpatialFailure("TemporalIndex::setDateFromYearAndMilliseconds: _year < 1 and CE != 0. Out of bounds. Use CE convention.");
	}
	if( _milliseconds < 0 ) {
		throw SpatialFailure("TemporalIndex::setDateFromYearAndMilliseconds: _milliseconds < 0.");
	}

	this->set_BeforeAfterStartBit(CE); this->set_year(_year);
	int64_t total_left = _milliseconds;
	for( int i = data.pos_CoarsestResolutionLevel+1; i <= data.pos_FinestResolutionLevel; ++i ) {
		data.getBitFieldAtId(i)->setValue(
				(total_left / data.getBitFieldAtId(i)->getScale())
				% (data.getBitFieldAtId(i)->getMaxValue() + 1) );

		total_left -= data.getBitFieldAtId(i)->getValue()*data.getBitFieldAtId(i)->getScale();
	}
	return *this;
}
/**
 * Return CE, year, and fraction of year in milliseconds.
 *
 * A canonical year should be 365*86400*1000 in milliseconds here.
 */
void TemporalIndex::toNativeCEYearAndMilliseconds(int64_t& _CE, int64_t& _year, int64_t& _milliseconds) {
	_CE   = this->get_BeforeAfterStartBit();
	_year = this->get_year();
	_milliseconds = this->toInt64MillisecondsFractionOfYear();
}

/**
 * Ignore any leap modifications and assume each year is YearNativeCanonicalInMS_d long.
 *
 * For use when astronomical accuracy is not required, and when beyond the domain of
 * the ERFA (IAU SOFA) algorithms (about +/- 4800). Year zero exists.
 *
 * This *will* alias native dates that map beyond YearNativeCanonicalInMS_d to the
 * succeeding year.
 *
 * TODO Consider a hybrid scheme wherein ERFA is used in it's domain and *NativeYear routines used elsewhere.
 *
 */
double TemporalIndex::toNativeYear() {
	int64_t
	_CE   = this->get_BeforeAfterStartBit(),
	_milliseconds = this->toInt64MillisecondsFractionOfYear();
	double _year = (double)this->get_year();
	double delta_ms = ((double)_milliseconds/YearNativeCanonicalInMS_d);
	if( _CE < 1 ) {
		_year = - _year - 1 + delta_ms;
	} else {
		_year = _year + delta_ms;
	}
	return _year;
}

/**
 * Ignore any leap modifications and assume each year is YearNativeCanonicalInMS_d long.
 *
 * For use when astronomical accuracy is not required, and when beyond the domain of
 * the ERFA (IAU SOFA) algorithms (about +/- 4800). Year zero exists.
 */
TemporalIndex& TemporalIndex::fromNativeYear(double year) {
	int64_t _year = (int64_t)year;
	double fractionOfYear = year - _year;
	int64_t _CE = 1;
	if( _year < 1 ) {
		_CE = 0;
		fractionOfYear = 1.0+fractionOfYear;
		_year = - _year;
	}
	this->fromNativeCEYearAndMilliseconds(_CE, _year, rint(fractionOfYear*YearNativeCanonicalInMS_d));
	return *this;
}

string TemporalIndex::toStringJ() {
	double d1,d2; this->toJulianTAI(d1, d2);
	int not_ok, iy, im, id, year, month, day_of_month, hour, minute, second, millisecond, ihmsf[4];
	not_ok      = eraD2dtf ( TimeStandard, 3, d1, d2, &year, &month, &day_of_month, ihmsf );
	hour        = ihmsf[0];
	minute      = ihmsf[1];
	second      = ihmsf[2];
	millisecond = ihmsf[3];
	int64_t	CE = this->get_BeforeAfterStartBit();
	stringstream ss;
	// ss << tIndex;
	// << setw(4) << setfill('0') << year << "-"
	if( CE > 0 ) {
		ss << setw(2) << "1 ";
	} else {
		ss << setw(2) << "0 ";
		year = 1 - year;
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
	return ss.str();
}

TemporalIndex& TemporalIndex::fromStringJ(string inputString) {
	int pos = 0;
#define PARSE_INT(field,width) \
		int64_t field = atoi(inputString.substr(pos,width).c_str()); pos += width + 1;
	// cout << endl << "pi: " << inputString.substr(pos,width).c_str() << endl;
	PARSE_INT(CE,1);
	PARSE_INT(year,inputString.find("-")-2);
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
	if( CE < 1 ) {
		year = 1 - year;
	}
	// this->setJulianFromFormattedTAI(CE, year, month, day_of_month, hour, minute, second, millisecond);
	this->fromFormattedJulianTAI(year, month, day_of_month, hour, minute, second, millisecond);
	data.setValue("resolution",resolution);
	data.setValue("type",type);
	return *this;
}

void TemporalIndex::toUTC(
		int& _year,
		int& _month, 		// 1..12 not 0..11
		int& _day_of_month, 	// 1..31
		int& _hour, 			// 0..23
		int& _minute, 		// 0..59
		int& _second, 		// 0..59
		int& _millisecond 	// 0..999
) {
	int not_ok;
	double d1,d2; this->toJulianTAI(d1,d2); // Get the TAI encoded time.

	// Convert TAI to UTC.
	double utc1, utc2; not_ok = eraTaiutc(d1, d2, &utc1, &utc2);

	int ihmsf[4];
	// The following takes as input the date and then formats it...
	not_ok      = eraD2dtf ( "UTC", 3, utc1, utc2, &_year, &_month, &_day_of_month, ihmsf );
	_hour        = ihmsf[0];
	_minute      = ihmsf[1];
	_second      = ihmsf[2];
	_millisecond = ihmsf[3];
}

TemporalIndex& TemporalIndex::fromUTC(
		int _year,
		int _month, 		// 1..12 not 0..11
		int _day_of_month, 	// 1..31
		int _hour, 			// 0..23
		int _minute, 		// 0..59
		int _second, 		// 0..59
		int _millisecond 	// 0..999
		) {

	double utc1, utc2;
	int not_ok = eraDtf2d( "UTC", _year, _month, _day_of_month, _hour, _minute, _second+(_millisecond*0.001), &utc1, &utc2 );

	double d1,d2; not_ok = eraUtctai(utc1,utc2,&d1,&d2);

	this->fromJulianTAI(d1, d2);

	return *this;
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
 *
 * Use native milliseconds to set the terminator. A terminator is an upper bound to a temporal interval. It corresponds
 * to a particular time, but has a special resolution encoding marking it as a terminator.
 *
 * To calculate the terminator, determine the time scale associated with the resolution of this TemporalIndex.
 * Add this time scale to the time encoded to find the terminator, and set the resolution to 63, an otherwise invalid resolution level.
 *
 * In other words, use resolution full (all bits set) to flag terminator.
 *
 * This calculation is based on the native encoding and is not dependent on the limits of IAU standards.
 *
 * Since a terminator is an int64_t, a TemporalIndex can be initialized with it, and one can then print it's
 * calendar value, etc.
 *
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

/**
 * Use IAU TAI to set the terminator. A terminator is an upper bound to a temporal interval. It corresponds
 * to a particular time, but has a special resolution encoding marking it as a terminator.
 *
 * The calculation is similar to that for scidbTerminator(), except using IAU standards (ERFA), instead
 * of the native encoding.
 *
 */
int64_t TemporalIndex::scidbTerminatorJulianTAI() {
	int64_t idx_ = 0;
	TemporalIndex tmpIndex(this->scidbTemporalIndex());
	int64_t resolution = tmpIndex.get_resolution();

	// Set the terminator type. Elsewhere we hard code this to 63... The following is correct.
	tmpIndex.set_resolution(tmpIndex.data.get("resolution")->getMask());

	//////////////////////////////////
	// Let's add an amount corresponding to the resolution.
	double delta = tmpIndex.daysAtResolution(resolution);
	double d1, d2;
	tmpIndex.toJulianTAI(d1,d2);
	tmpIndex.fromJulianTAI(d1,d2+delta);
	idx_ = tmpIndex.scidbTemporalIndex();
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

int64_t millisecondsInYear(int64_t CE, int64_t year) {
	int64_t _year = year;
	if( CE < 1 ) { _year = 1 - _year; }
	//
	double d1_eoy, d2_eoy;
	// Get the beginning of next year
	int not_ok_1 = eraDtf2d( TimeStandard, _year+1, 1, 1, 0, 0, 0, &d1_eoy, &d2_eoy);
	// Back off a bit
	--d1_eoy;	++d2_eoy;	// d2_eoy -= 1.0 / 86400000.0;

	double d1_boy, d2_boy;
	int not_ok_2 = eraDtf2d( TimeStandard, _year, 1, 1, 0, 0, 0, &d1_boy, &d2_boy);

	double delta = (d1_eoy+d2_eoy) - (d1_boy+d2_boy);
	// return (int64_t)(delta*86400000.0);
	return rint(delta*86400000.0);
}

TemporalIndex& TemporalIndex::setEOY( int64_t CE, int64_t year ) {
	int64_t _year = year;
	if( CE < 1 ) { _year = 1 - _year; }
	// Get the beginning of next year
	double d0_1, d0_2;
	int not_ok_1 = eraDtf2d( TimeStandard, _year+1, 1, 1, 0, 0, 0, &d0_1, &d0_2 );
	// Go back a millisecond.
	--d0_1;	++d0_2;	d0_2 -= 1.0 / 86400000.0;
	this->fromJulianTAI(d0_1, d0_2);
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
void fractionalDayFromHMSM (double& fd, int  hour, int  minute, int  second, int  ms){
	//cout << endl << dec << flush;
	fd =  second + 0.001*ms;
	fd =  minute + (fd/60.0);
	fd =    hour + (fd/60.0);
	fd /= 24.0;
}

TemporalIndex& TemporalIndex::fromFormattedJulianTAI(
		//old int64_t _CE,            // 0 or 1: 0 = BCE, 1 = CE
		int64_t _year, 			// > 0
		int64_t _month, 		// 1..12 not 0..11
		int64_t _day_of_month, 	// 1..31
		int64_t _hour, 			// 0..23
		int64_t _minute, 		// 0..59
		int64_t _second, 		// 0..59
		int64_t _millisecond 	// 0..999
){
	// If we're in BCE, correct the _year ( 1 BCE goes to year 0) and reverse.
	//old if( _CE < 1 ) { _year = 1 - _year; }
	double d1,d2;
	int not_ok = eraDtf2d( TimeStandard, _year, _month, _day_of_month, _hour, _minute, _second+(_millisecond*0.001), &d1, &d2 );

	if(not_ok != 0) {
		// string msgs[4] = {"OK","bad year (JD not computed)","bad month (JD not computed)","bad day (JD computed)"};
		string msgs[10] = {
				"dubious year and time is after end of day", // 0
				"time is after end of day",
				"dubious year",
				"ok",
				"bad year",
				"bad month",
				"bad day",
				"bad hour",
				"bad minute",
				"bad second (<0)" // 9
		};
		stringstream ss;
		ss << "TemporalIndex:toJulianDoubleDay:eraDtf2d-failure " << msgs[3-not_ok];
		ss << endl;
#define FMT1(x) ss << #x << " : " << dec << x << endl << flush;
		FMT1(_year)
		FMT1(_month)
		FMT1(_day_of_month)
		FMT1(_hour)
		FMT1(_minute)
		FMT1(_second)
		FMT1(_millisecond)
		FMT1(d1)
		FMT1(d2)
#undef FMT1
		// TODO add some way to tune sensitivity to poorly formed dates & times
		if (not_ok < 0) {
			throw SpatialFailure(ss.str().c_str());
		}
	}
//#define FMT1(x) cout << "z100 " << #x << " : " << dec << x << endl << flush;
//		cout << endl << flush;
//		FMT1(_year)
//		FMT1(_month)
//		FMT1(_day_of_month)
//		FMT1(_hour)
//		FMT1(_minute)
//		FMT1(_second)
//		FMT1(_millisecond)
//		FMT1(d1)
//		FMT1(d2)
//#undef FMT1
	return fromJulianTAI(d1,d2);
}

void TemporalIndex::toJulianTAI(double& d1, double& d2) const {
	int64_t _babit, _year;
	_babit = this->get_BeforeAfterStartBit();
	_year  = this->get_year();
	if( _babit < 1 ) { _year = - _year; } // Note, for ERFA (IAU SOFA) Year 0 is valid.
	double d0_1, d0_2;
	int not_ok_1 = eraDtf2d( TimeStandard, _year, 1, 1, 0, 0, 0, &d0_1, &d0_2 );
	int64_t milliseconds = this->toInt64MillisecondsFractionOfYear();
	double  days         = ((double) milliseconds) / 86400000.0;
	d1 = d0_1; d2 = d0_2 + days;
};

TemporalIndex& TemporalIndex::fromJulianTAI( double d1, double d2) {
	int not_ok, iy, im, id,_hour, _minute, _second, _millisecond, ihmsf[4];
	int64_t CE = 1;
	not_ok = eraD2dtf ( TimeStandard, 3, d1, d2, &iy, &im, &id, ihmsf );
	// only using iy below...
	// not_ok = eraD2dtf ( TimeStandard, 4, d1, d2, &iy, &im, &id, ihmsf );
	_hour   = ihmsf[0];	_minute = ihmsf[1];	_second = ihmsf[2];	_millisecond = ihmsf[3];
	if(not_ok != 0) {
		// string msgs[4] = {"OK","bad year (JD not computed)","bad month (JD not computed)","bad day (JD computed)"};
		string msgs[10] = {
				"dubious year", // 0
				"ok",
				"unacceptable date" // 2
		};
		stringstream ss;
		ss << "TemporalIndex:fromJulianDoubleDay:eraD2dtf-failure " << msgs[1-not_ok];
		ss << endl;
#define FMT1(x) ss << #x << " : " << dec << x << endl << flush;
		FMT1(d1)
		FMT1(d2)
		FMT1(iy)
		FMT1(im)
		FMT1(id)
		FMT1(_hour)
		FMT1(_minute)
		FMT1(_second)
		FMT1(_millisecond)
#undef FMT1
		// TODO add some way to tune sensitivity to poorly formed dates & times
		if( not_ok < 0 ) {
			throw SpatialFailure(ss.str().c_str());
		}
	}
	// Find the number of milliseconds in the fractional year.
	double d0_1, d0_2;
	int not_ok_1 = eraDtf2d( TimeStandard, iy, 1, 1, 0, 0, 0, &d0_1, &d0_2 );
	double delta = ((d1-d0_1)+(d2-d0_2))*86400000.0;
//	cout << "a200 " << setw(24) << setprecision(20) << delta << flush;
	// int64_t milliseconds = (int64_t) delta;
	int64_t milliseconds = rint(delta);
//	cout << ", " << milliseconds << endl << flush;
	if( iy < 1) { CE = 0; iy = -iy;	}
	this->fromNativeCEYearAndMilliseconds(CE, (int64_t)iy, milliseconds);
	return *this;
};

/**
 * Count the number of milliseconds into the year this TemporalIndex is set to.
 *
 * No error checking. If the fields are set to weird values, then this will include
 * them in the sum.
 *
 * Leap days & seconds are added to the end of the year.
 */
int64_t TemporalIndex::toInt64MillisecondsFractionOfYear() const {
	int64_t sum = 0;
	// Coarsest is a year, so don't include.
	for( int i = data.pos_FinestResolutionLevel; i > data.pos_CoarsestResolutionLevel; --i ) {
		sum += data.getBitFieldAtId(i)->getValue() * data.getBitFieldAtId(i)->getScale();
	}
	return sum;
}
/**
 * Use Julian days.
 */
int64_t TemporalIndex::toInt64MillisecondsFractionOfYearJ() const {
	int64_t _year = this->get_year(), CE = this->get_BeforeAfterStartBit();
	if( CE < 1 ) { _year = 1 - _year; }
	// Get the beginning of the year
	double d0_1, d0_2;
	int not_ok_1 = eraDtf2d( TimeStandard, _year, 1, 1, 0, 0, 0, &d0_1, &d0_2 );
//	// Get the current date
//	int64_t milliseconds = toInt64MillisecondsFractionOfYear();
	double d1, d2;
	this->toJulianTAI(d1, d2);
	double delta = (d1+d2) - (d0_1+d0_2); // Find difference d-d0
	// return (int64_t) (delta * 86400000.0); // Convert to milliseconds
	return rint(delta * 86400000.0); // Convert to milliseconds
}
/// Note: for indexing, not astronomy. Support a kind of "addition".
int64_t TemporalIndex::toInt64Milliseconds() const {
	int i = data.pos_CoarsestResolutionLevel; // Should be for the 'year' field
	int64_t sum = (data.getBitFieldAtId(i)->getValue()) * data.getBitFieldAtId(i)->getScale();
	int64_t CE  = this->get_BeforeAfterStartBit();
	if(CE < 1) {
		sum = -sum;
	}
	sum += this->toInt64MillisecondsFractionOfYear();
//	for( int i = data.pos_FinestResolutionLevel; i > data.pos_CoarsestResolutionLevel; --i ) {
//		sum += data.getBitFieldAtId(i)->getValue() * data.getBitFieldAtId(i)->getScale();
//	}
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

/**
 * Return a number of days associated with a resolution level.
 */
double TemporalIndex::daysAtResolution(int64_t resolution) {
	return millisecondsAtResolution(resolution) / 86400.0e3;
}

int64_t scidbMinimumIndex() {
	TemporalIndex tIndex;
	tIndex.setZero().set_year(262143).set_type(2);
	return tIndex.scidbTemporalIndex();
}

int64_t scidbMaximumIndex() {
	TemporalIndex tIndex;
	tIndex.setZero().setEOY(1,262143).set_type(2);
	return tIndex.scidbTemporalIndex();
}

} /* namespace std */
