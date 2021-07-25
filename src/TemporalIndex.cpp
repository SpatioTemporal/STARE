/*
 * TemporalIndex.cpp
 *
 *  Created on: Apr 10, 2017
 *      Author: mrilee
 */

#include "TemporalIndex.h"

using namespace std;
// namespace std {

BitField::BitField() {}
BitField::~BitField() {}

TemporalWordFormat::~TemporalWordFormat() {}

TemporalIndex::~TemporalIndex() {
  // TODO Auto-generated destructor stub
  // for(auto it = begin(data.bitFields); it != end(data.bitFields); ++ it) {
  //   delete (*it);
  // }
  // while( !data.bitFields.empty() ) {
  //   BitField *b = data.bitFields.back();
  //   delete b;
  //   data.bitFields.pop_back();
  // }
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
  cout << "TemporalIndex::checkBitFormat()" << endl << flush;
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
	OUTPUT(forward_resolution);
	OUTPUT(reverse_resolution);
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

string TemporalIndex::toStringJulianTAI() {
	double d1, d2; 
    this->toJulianTAI(d1, d2);
	int not_ok, /* iy, im, id,*/  year, month, day_of_month, hour, minute, second, millisecond, ihmsf[4];
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
	<< " (" << setw(2) << data.getValue("forward_resolution") << " "
	<<         setw(2) << data.getValue("reverse_resolution") << ")"
	<< " (" << setw(1) << data.getValue("type") << ")"
	;
	return ss.str();
}

void TemporalIndex::toFormattedJulianTAI(
        int &year, int &month, int &day, int &hour, int &minute, int &second, int &ms
) {
    double d1, d2;
    this->toJulianTAI(d1, d2);
    int ihmsf[4];
    int not_ok = eraD2dtf(TimeStandard, 3, d1, d2, &year, &month, &day, ihmsf);
    if (not_ok == 1)
        throw SpatialException("In TemporalIndex::toFormattedJulianTAI, eraD2dtf(...) failure.");

    hour = ihmsf[0];
    minute = ihmsf[1];
    second = ihmsf[2];
    ms = ihmsf[3];
}

TemporalIndex& TemporalIndex::fromStringJulianTAI(string inputString) {
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
	++pos; // 2020-1112 mlr why? To match toString format... See above.
	PARSE_INT(forward_resolution,2);
	PARSE_INT(reverse_resolution,2);
	++pos; ++pos;
	PARSE_INT(type,1);
#undef PARSE_INT
	if( CE < 1 ) {
		year = 1 - year;
	}
	// this->setJulianFromFormattedTAI(CE, year, month, day_of_month, hour, minute, second, millisecond);
	this->fromFormattedJulianTAI(year, month, day_of_month, hour, minute, second, millisecond);
	data.setValue("forward_resolution",forward_resolution);
	data.setValue("reverse_resolution",reverse_resolution);
	data.setValue("type",type);
	return *this;
}

// TemporalIndex& TemporalIndex::fromStringISO(string inputString) {
// 	int pos = 0;
// #define PARSE_INT(field,width) \
// 		int64_t field = atoi(inputString.substr(pos,width).c_str()); pos += width + 1;
// 	// cout << endl << "pi: " << inputString.substr(pos,width).c_str() << endl;
// 	//???? PARSE_INT(CE,1);
// 	PARSE_INT(year,inputString.find("-")-2);
// 	PARSE_INT(month,2); pos++; // Go past "-"
// 	PARSE_INT(day_of_month,2); pos++; // Go past "T"
// 	PARSE_INT(hour,2); pos++; // Skip :
// 	PARSE_INT(minute,2); pos++; // Skip :
// 	PARSE_INT(second,2); pos++; // Skip .
// 	PARSE_INT(millisecond,3); // Done
// 	//??? ++pos; // 2020-1112 mlr why? To match toString format... See above.
// 	//??? PARSE_INT(forward_resolution,2);
// 	//??? PARSE_INT(reverse_resolution,2);
// 	++pos; ++pos; //Skipping? What?
// 	PARSE_INT(type,1);
// #undef PARSE_INT
// 	if( CE < 1 ) {
// 		year = 1 - year;
// 	}
// 	// this->setJulianFromFormattedTAI(CE, year, month, day_of_month, hour, minute, second, millisecond);
// 	this->fromFormattedJulianTAI(year, month, day_of_month, hour, minute, second, millisecond);
// 	data.setValue("forward_resolution",forward_resolution);
// 	data.setValue("reverse_resolution",reverse_resolution);
// 	data.setValue("type",type);
// 	return *this;
// }

string TemporalIndex::toStringJulianTAI_ISO() {
	double d1, d2; 
    this->toJulianTAI(d1, d2);
	int not_ok, /* iy, im, id,*/  year, month, day_of_month, hour, minute, second, millisecond, ihmsf[4];
	not_ok      = eraD2dtf ( TimeStandard, 3, d1, d2, &year, &month, &day_of_month, ihmsf );
	hour        = ihmsf[0];
	minute      = ihmsf[1];
	second      = ihmsf[2];
	millisecond = ihmsf[3];
	// int64_t	CE = this->get_BeforeAfterStartBit();
	stringstream ss;
	ss
	<< year << "-"
	<< setw(2) << setfill('0') << month << "-"
	<< setw(2) << setfill('0') << day_of_month << "T"
	<< setw(2) << setfill('0') << hour << ":"
	<< setw(2) << setfill('0') << minute << ":"
	<< setw(2) << setfill('0') << second << "."
	<< setw(3) << setfill('0') << millisecond
	<< " (" << setw(2) << data.getValue("forward_resolution") << " "
	<<         setw(2) << data.getValue("reverse_resolution") << ")"
	<< " (" << setw(1) << data.getValue("type") << ")"
	;
	return ss.str();
}

TemporalIndex& TemporalIndex::fromStringJulianTAI_ISO(string inputString) {
	int pos = 0;
#define PARSE_INT(field,width) \
		int64_t field = atoi(inputString.substr(pos,width).c_str()); pos += width + 1;
	// cout << endl << "pi: " << inputString.substr(pos,width).c_str() << endl;
	
	this->set_BeforeAfterStartBit(1); // PARSE_INT(CE,1)
	PARSE_INT(year,inputString.find("-"));
	PARSE_INT(month,2);
	PARSE_INT(day_of_month,2);
	PARSE_INT(hour,2);
	PARSE_INT(minute,2);
	PARSE_INT(second,2);
	PARSE_INT(millisecond,3);
	++pos; // 2020-1112 mlr why? To match toString format... See above.
	PARSE_INT(forward_resolution,2);
	PARSE_INT(reverse_resolution,2);
	++pos; ++pos;
	PARSE_INT(type,1);
#undef PARSE_INT
	//if( CE < 1 ) {
	//	year = 1 - year;
	//}
	// this->setJulianFromFormattedTAI(CE, year, month, day_of_month, hour, minute, second, millisecond);
	this->fromFormattedJulianTAI(year, month, day_of_month, hour, minute, second, millisecond);
	data.setValue("forward_resolution",forward_resolution);
	data.setValue("reverse_resolution",reverse_resolution);
	data.setValue("type",type);
	return *this;
}


/**
 * Retrieve the UTC version of the stored TAI-based index value.
 */
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
	double d1,d2; 
    this->toJulianTAI(d1,d2); // Get the TAI encoded time.

	// Convert TAI to UTC.
	double utc1, utc2; not_ok = eraTaiutc(d1, d2, &utc1, &utc2);

	int ihmsf[4];
	// The following takes as input the date and then formats it...
	not_ok       = eraD2dtf ( "UTC", 3, utc1, utc2, &_year, &_month, &_day_of_month, ihmsf );
	_hour        = ihmsf[0];
	_minute      = ihmsf[1];
	_second      = ihmsf[2];
	_millisecond = ihmsf[3];
}
void TemporalIndex::toJulianUTC( double& utc1, double &utc2 ) const {
  int not_ok;
  double d1,d2;
  this->toJulianTAI(d1,d2); // Get the TAI encoded time.
  not_ok = eraTaiutc(d1, d2, &utc1, &utc2); // Convert to UTC's quasi JD.
}
TemporalIndex& TemporalIndex::fromJulianUTC( double utc1, double utc2 ) {
  double d1,d2; int not_ok = eraUtctai(utc1,utc2,&d1,&d2);
  if (not_ok == 1) {
    throw SpatialException("In TemporalIndex::fromJulianUTC, eraUtctai(...) failure.");
  }
  this->fromJulianTAI(d1, d2);
  return *this;
}
/**
 * Convert and store a UTC coordinate into the native TAI-based value.
 */
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
/**
 * Construct a string representation of the index value as stored.
 */
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
	<< " (" << setw(2) << data.getValue("forward_resolution") << " "
	<<         setw(2) << data.getValue("reverse_resolution") << ")"
	<< " (" << setw(1) << data.getValue("type") << ")"
	;

	return ss.str();
}

/**
 * Parse and store an index value represented in native format.
 */
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
	PARSE_INT(forward_resolution,2);
	PARSE_INT(reverse_resolution,2);
	// PARSE_INT(resolutionLevel,2);
#undef PARSE_INT
	data.setValue("BeforeAfterStartBit",1);
}

/**
 * Test calling ERFA routines.
 */
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

// TODO Check for errors/integrity of data value.
// #define MASK_AND_SHIFT(field) ((( mask_##field & field ) << offset_##field ))

/**
   MASK_AND_SHIFT_REVERSE retrieves the value, reverses sub-year calendrial values for negative years.
   This is because those terms, like month and day, always count forward in the calendar, even when they year
   is negative. So, while -0.1 and +0.1 are neighbors, Year 0 December is not a neighbor of Year+1 December.

   babit=1 corresponds to positive years in the epoch, while babit=0 is for negative years.
 */
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
			MASK_AND_SHIFT(1,forward_resolution) |
			MASK_AND_SHIFT(1,reverse_resolution) |
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


/**
   Return bit offset (bit location in word) for the finest resolution.
 */
int64_t TemporalIndex::bitOffsetFinest() const {
	int64_t offsetBottom = data.bitFields[data.pos_FinestResolutionLevel]->getOffset();
	return offsetBottom;
}

/**
   Return bit offset (bit location in word) for the coarsest resolution.
 */
int64_t TemporalIndex::bitOffsetCoarsest() const {
	int64_t offsetTop =
			data.bitFields[data.pos_CoarsestResolutionLevel]->getOffset() +
			data.bitFields[data.pos_CoarsestResolutionLevel]->getWidth()-1;
	return offsetTop;
}
/**
   Return the bit offset (location) of a given resolution.
 */
int64_t TemporalIndex::bitOffsetResolution(int64_t resolution) const {
	return bitOffsetCoarsest() - resolution;
}

/**
   Return the id of the bitfield associated with a resolution level.
 */
int64_t TemporalIndex::bitfieldIdFromResolution(int64_t resolution) const {
    // Note: int offsetTop = bitOffsetCoarsest(); // If needed.
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

/**
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
  return scidbUpperBoundMS(this->scidbTemporalIndex());
}

/**
   Use native milliseconds to set the lower bound from the temporal index. Note this doesn't respect leap year or second corrections.
 */
int64_t TemporalIndex::scidbLowerBound() {
  return scidbLowerBoundMS(this->scidbTemporalIndex());
}

/**
   Use IAU TAI to set the lower bound of an interval. It is the converse of a terminator.

   The calculation is similar to that for scidbTerminator(), except using IAU standards (ERFA), instead
   of the native encoding.

 */
int64_t TemporalIndex::scidbLowerBoundJulianTAI() {
  return scidbLowerBoundTAI(this->scidbTemporalIndex());
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
  return scidbUpperBoundTAI(this->scidbTemporalIndex());
}

/**
 * Determine if the stored index value is a terminator.
 */
bool TemporalIndex::scidbTerminatorp() {
	int64_t forward_resolution = this->data.getValue("forward_resolution");
	// return forward_resolution == 63;
	return forward_resolution == this->data.get("forward_resolution")->getMask();
	// reverse_resolution is also set to its mask.
}


/**
   Approximate the upper bound (terminator) of a temporal index value.

   TODO: Consider an even more approximate approach that does not calculate the millisecond representation, working directly with the integer format.
 */
int64_t scidbUpperBoundMS(int64_t ti_value) {
  TemporalIndex tmpIndex(ti_value);
  int64_t forward_resolution = tmpIndex.get_forward_resolution();

  //////////////////////////////////
  // Let's add an amount corresponding to the resolution.
  int64_t delta = tmpIndex.millisecondsAtResolution(forward_resolution);
  int64_t t0    = tmpIndex.toInt64Milliseconds();
  int64_t t1    = t0 + delta;
  tmpIndex.fromInt64Milliseconds(t1);

  // Set the terminator type. Elsewhere we hard code this to 63... The following is correct.
  tmpIndex.set_forward_resolution(tmpIndex.data.get("forward_resolution")->getMask());
  tmpIndex.set_reverse_resolution(tmpIndex.data.get("reverse_resolution")->getMask()); // Terminator doesn't care about the reverse resolution.
  
  int64_t idx_ = tmpIndex.scidbTemporalIndex();
  return idx_;
}

/**
   Approximate the lower bound of a temporal index value.
 */
int64_t scidbLowerBoundMS(int64_t ti_value) {
  //		cout << "TemporalIndex::scidbTerminator not implemented!!" << endl << flush;
  TemporalIndex tmpIndex(ti_value);
  int64_t reverse_resolution = tmpIndex.get_reverse_resolution();

  // Set the terminator type. Elsewhere we hard code this to 63... The following is correct.
  tmpIndex.set_forward_resolution(0);
  tmpIndex.set_reverse_resolution(tmpIndex.data.get("reverse_resolution")->getMask()); // Terminator doesn't care about the reverse resolution.

  //////////////////////////////////
  // Let's add an amount corresponding to the resolution.
  int64_t delta = -tmpIndex.millisecondsAtResolution(reverse_resolution);
  int64_t t0    = tmpIndex.toInt64Milliseconds();
  int64_t t1    = t0 + delta;
  tmpIndex.fromInt64Milliseconds(t1);
  int64_t idx_ = tmpIndex.scidbTemporalIndex();
  return idx_;
}

/**
   A more accurate the upper bound (terminator) of a temporal index value, based on TAI.
 */
int64_t scidbUpperBoundTAI(int64_t ti_value) {
	int64_t idx_ = 0;
	TemporalIndex tmpIndex(ti_value);
	int64_t forward_resolution = tmpIndex.get_forward_resolution();

	// Set the terminator type. Elsewhere we hard code this to 63... The following is correct.
	tmpIndex.set_forward_resolution(tmpIndex.data.get("forward_resolution")->getMask());
	tmpIndex.set_reverse_resolution(tmpIndex.data.get("reverse_resolution")->getMask());

	//////////////////////////////////
	// Let's add an amount corresponding to the resolution.
	double delta = tmpIndex.daysAtResolution(forward_resolution);
	double d1, d2;
	tmpIndex.toJulianTAI(d1,d2);
	tmpIndex.fromJulianTAI(d1,d2+delta);
	idx_ = tmpIndex.scidbTemporalIndex();
	return idx_;
}

/**
   A more accurate the lower bound of a temporal index value, based on TAI.
 */
int64_t scidbLowerBoundTAI(int64_t ti_value) {
  int64_t idx_ = 0;
  TemporalIndex tmpIndex(ti_value);
  int64_t reverse_resolution = tmpIndex.get_reverse_resolution();

  // TODO What should the forward_resolution be for a lower_bound?

  // Set the terminator type. Elsewhere we hard code this to 63... The following is correct.
  tmpIndex.set_forward_resolution(0);
  tmpIndex.set_reverse_resolution(tmpIndex.data.get("reverse_resolution")->getMask());
	
  //////////////////////////////////
  // Let's add an amount corresponding to the resolution.
  double delta = -tmpIndex.daysAtResolution(reverse_resolution);
  double d1, d2;
  tmpIndex.toJulianTAI(d1,d2);
  tmpIndex.fromJulianTAI(d1,d2+delta);
  idx_ = tmpIndex.scidbTemporalIndex();
  return idx_;
}

///////////////////////////////////////////////////////////////////////////

/**
   Set the fields of the TemporalIndex object to zero.
 */
TemporalIndex& TemporalIndex::set_zero() {
	data.setZero();
	return *this;
}

/**
   Set the fields of the TemporalIndex object to zero.
 */
TemporalIndex& TemporalIndex::setZero() {
	data.setZero();
	return *this;
}

/**
 * Find the number of TAI seconds in a particular year.
 */
int64_t millisecondsInYear(int64_t CE, int64_t year) {
	int64_t _year = year;
	if( CE < 1 ) { _year = 1 - _year; }
	//
	double d1_eoy, d2_eoy;
	// Get the beginning of next year
    int not_ok_1 = eraDtf2d( TimeStandard, _year+1, 1, 1, 0, 0, 0, &d1_eoy, &d2_eoy);
    if (not_ok_1 == 1)
        throw SpatialException("In TemporalIndex.cpp:millisecondsInYear, eraDtf2d(...) failure.");

	// Back off a bit
	--d1_eoy;	++d2_eoy;	// d2_eoy -= 1.0 / 86400000.0;

	double d1_boy, d2_boy;
    int not_ok_2 = eraDtf2d( TimeStandard, _year, 1, 1, 0, 0, 0, &d1_boy, &d2_boy);
    if (not_ok_2 == 1)
        throw SpatialException("In TemporalIndex.cpp:millisecondsInYear, eraDtf2d(...) failure.");

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
    if (not_ok_1 == 1)
        throw SpatialException("In TemporalIndex::setEOY, eraDtf2d(...) failure.");

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
    if (not_ok_1 == 1) {
      throw SpatialException("In TemporalIndex::toJulianTAI, eraD2dtf(...) failure.");
    }
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
                  // cout << "fromJulianTAI: " << d1 << " " << d2 << " " << ss.str() << endl << flush;
		// TODO add some way to tune sensitivity to poorly formed dates & times
		if( not_ok < 0 ) {
			throw SpatialFailure(ss.str().c_str());
		}
	}
	// Find the number of milliseconds in the fractional year.
	double d0_1=0, d0_2=0;

    int not_ok_1 = eraDtf2d( TimeStandard, iy, 1, 1, 0, 0, 0, &d0_1, &d0_2 );
    if (not_ok_1 == 1)
        throw SpatialException("In TemporalIndex::fromJulianTAI, eraD2dtf(...) failure.");

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
    if (not_ok_1 == 1)
        throw SpatialException("In TemporalIndex::toInt64MillisecondsFractionOfYearJ, eraD2dtf(...) failure.");

//	// Get the current date
//	int64_t milliseconds = toInt64MillisecondsFractionOfYear();
	double d1, d2;
	this->toJulianTAI(d1, d2);
	double delta = (d1+d2) - (d0_1+d0_2); // Find difference d-d0
	// return (int64_t) (delta * 86400000.0); // Convert to milliseconds
	return rint(delta * 86400000.0); // Convert to milliseconds
}
/**
   Convert the (already stored) temporal index value to milliseconds.

   Note: for indexing, not astronomy. Support a kind of "addition".
*/
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

/**
   Construct a temporal index value from milliseconds.
 */
TemporalIndex& TemporalIndex::fromInt64Milliseconds(int64_t milliseconds) {

    int64_t /*sum = 0,*/ total_left = milliseconds, CE = -1, year;


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
int64_t TemporalIndex::millisecondsAtResolution(const int64_t resolution) const {

	TemporalIndex tmpIndex;
	/*
	 * If needed...
	int64_t bitPosition;
        int64_t offsetTop = bitOffsetCoarsest();
	*/

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
double TemporalIndex::daysAtResolution(const int64_t resolution) const {
	return millisecondsAtResolution(resolution) / 86400.0e3;
}

/**
 * Determine the resolution level immediately finer than the resolution given in milliseconds.
 *
 * TODO Consider there might be a faster (formulaic) way to do this based on the bit position.
 */
int64_t TemporalIndex::coarsestResolutionFinerOrEqualMilliseconds(int64_t milliseconds) {
	int64_t resolution = this->data.maxResolutionLevel();
	bool done = false;
	while(  resolution >= 0 && !done ) {
		int64_t ms = millisecondsAtResolution(resolution);
		done = ms >= milliseconds;
		if( done ) {
			if( ms > milliseconds ) {
				++resolution;
			}
		} else {
			--resolution;
		}
	}
	return resolution;
}

/**
 * The minimum scidb temporal index.
 */
int64_t scidbMinimumTemporalIndex() {
	TemporalIndex tIndex;
	tIndex.setZero().set_year(8192/2).set_type(1);
	// tIndex.setZero().set_year(262143).set_type(2);
	return tIndex.scidbTemporalIndex();
}

/**
 * The maximum scidb temporal index.
 */
int64_t scidbMaximumTemporalIndex() {
	TemporalIndex tIndex;
	tIndex.setZero().setEOY(1,8192/2).set_type(1);
	// tIndex.setZero().setEOY(1,262143).set_type(2);
	return tIndex.scidbTemporalIndex();
}


/**
   Set bits at finer resolutions to zero.

   Note, calling with res=48 when max_res=48 has no effect.
 */
int64_t scidbClearBitsFinerThanResolution(int64_t ti_value, int resolution) {
  // throw SpatialFailure("TemporalIndex.scidbClearBitsFinerOrEqualResolution::NotImplemented!!");
  // 1. Get ti_value type and find finest bit
  // 2. Make mask
  // 3. And it into the result

  TemporalIndex tIndex = TemporalIndex(ti_value);
  
  // int64_t offsetBottom = tIndex.data.bitFields[tIndex.data.pos_FinestResolutionLevel]->getOffset();
  int64_t offsetBottom = tIndex.bitOffsetFinest();
  int64_t offsetTop    = tIndex.bitOffsetResolution(resolution);

  int64_t mask = ~(((1ll << offsetTop)-1) & ~((1ll << offsetBottom)-1));
  return ti_value & mask;
}

/**
   Set bits at finer resolutions to one.

   Setting the bits corresponding to finer resolutions yields a
   "terminator" of sorts with the bits corresponding to ~1ms less than
   the nominal scale of the resolution. I.e. if a resolution
   corresponds to 2 days, then setting the bits finer than that
   corresponds to 2 days minus ~1ms, or whatever is the finest
   temporal increments.

   This routine might be helpful in constructing terminators to
   intervals.  Terminators are index values that are >= to any encoded
   time in the preceding interval, but < any time in the next
   interval.

   Note, calling with res=48 when max_res=48 has no effect.
 */
int64_t scidbSetBitsFinerThanResolution(int64_t ti_value, int resolution) {
  // throw SpatialFailure("TemporalIndex.scidbSetBitsFinerThanResolution::NotImplemented!!");
  // 1. Get ti_value type and find finest bit
  // 2. Make mask
  // 3. Or it into the result

  TemporalIndex tIndex = TemporalIndex(ti_value);
  
  // int64_t offsetBottom = tIndex.data.bitFields[tIndex.data.pos_FinestResolutionLevel]->getOffset();
  int64_t offsetBottom = tIndex.bitOffsetFinest();
  int64_t offsetTop    = tIndex.bitOffsetResolution(resolution);

  int64_t mask = (((1ll << offsetTop)-1) & ~((1ll << offsetBottom)-1));
  return ti_value | mask;
}
/**
   The same as scidbSetBitsFinerThanResolution, except limited to valid temporal scales.
 */
int64_t scidbSetBitsFinerThanResolutionLimited(int64_t ti_value, int resolution) {
  int64_t idx = scidbSetBitsFinerThanResolution(ti_value,resolution);
  TemporalIndex tIndex(idx);
  int iPos = tIndex.data.pos_FinestResolutionLevel;
  while( iPos >= tIndex.data.pos_CoarsestResolutionLevel ) {
    if( tIndex.data.getValueAtId(iPos) > tIndex.data.getBitFieldAtId(iPos)->getMaxValue() ) {
      tIndex.data.getBitFieldAtId(iPos)->setValue(tIndex.data.getBitFieldAtId(iPos)->getMaxValue());
    }
    --iPos;
  }
  return tIndex.scidbTemporalIndex();
}


/**
   True if temporal index values overlap using approximate millisecond calculations.
 */
bool scidbOverlap(int64_t ti_value_0, int64_t ti_value_1) {

#if 0
  cout
    << "sO:lu0,lu1: "
    << toStringJulianTAI_ISO(ti_value_0) << " "
    << toStringJulianTAI_ISO(ti_value_1) << " "
    << endl << flush;
#endif
  
  int64_t tMask = temporal_mask(ti_value_0);
  
  // These are all start/term index values
  int64_t lower0 = scidbLowerBoundMS(ti_value_0) & tMask;
  int64_t upper0 = scidbUpperBoundMS(ti_value_0) & tMask;
  int64_t lower1 = scidbLowerBoundMS(ti_value_1) & tMask;
  int64_t upper1 = scidbUpperBoundMS(ti_value_1) & tMask;

#if 0
  cout
    << "sO:lu0,lu1: "
    << toStringJulianTAI_ISO(lower0) << " "
    << toStringJulianTAI_ISO(upper0) << " "
    << toStringJulianTAI_ISO(lower1) << " "
    << toStringJulianTAI_ISO(upper1) << " "
    << toStringJulianTAI_ISO(ti_value_0) << " "
    << toStringJulianTAI_ISO(ti_value_1) << " "
    << endl << flush;

  cout
    << "s0 " << (lower0 <= upper1) << " " << (upper0 >= lower1) << endl << flush;

  int64_t mask = ~((2ll^12)-1ll);
  cout
    << "s0 " << ((lower0 & mask) <= (upper1 & mask)) << " " << ((upper0 & mask) >= (lower1 & mask)) << endl << flush;
#endif
  
  return (lower0 <= upper1) && (upper0 >= lower1);
}

/**
   True if temporal index values overlap using more accurate but expensive TAI calculations.
 */
bool scidbOverlapTAI(int64_t ti_value_0, int64_t ti_value_1) {

#if 0
  cout
    << "sOTAI:lu0,lu1: "
    << toStringJulianTAI_ISO(ti_value_0) << " "
    << toStringJulianTAI_ISO(ti_value_1) << " "
    << endl << flush;
#endif

  int64_t tMask = temporal_mask(ti_value_0);
  
  int64_t lower0 = scidbLowerBoundTAI(ti_value_0) & tMask;
  int64_t upper0 = scidbUpperBoundTAI(ti_value_0) & tMask;
  int64_t lower1 = scidbLowerBoundTAI(ti_value_1) & tMask;
  int64_t upper1 = scidbUpperBoundTAI(ti_value_1) & tMask;
  return (lower0 <= upper1) && (upper0 >= lower1);
}

/**
   True if the instant of ti_value_query is in the span of ti_value.

   Note: Probably approximate, using MS-type functions.

   TODO: Redo this using the ti_values themselves.
*/
bool scidbContainsInstant(int64_t ti_value, int64_t ti_value_query) {
  TemporalIndex tIndex;
  int64_t finest_resolution = tIndex.data.maxResolutionLevel();  
  int64_t tiv_instant = set_forward_resolution(set_reverse_resolution(ti_value_query,finest_resolution),finest_resolution);

#if 0
#undef FMTX
#define FMTX(x) " 0x" << setfill('0') << setw(16) << hex << x << dec
  cout << "ti_value   " << FMTX(ti_value)       << " " << toStringJulianTAI_ISO(ti_value) << endl << flush;
  cout << "ti_valueq  " << FMTX(ti_value_query) << " " << toStringJulianTAI_ISO(ti_value_query) << endl << flush;
  cout << "ti_inst    " << FMTX(tiv_instant)    << " " << toStringJulianTAI_ISO(tiv_instant) << endl << flush;
  cout << endl << flush;
#undef FMTX
#endif
  return scidbOverlap(ti_value,tiv_instant);
}

/**
  Return a mask for extracting the temporal location (i.e. the instant).
*/
int64_t temporal_mask(int64_t ti_value) {

#if 0
  cout
    << "t_mask: "
    << toStringJulianTAI_ISO(ti_value) << " "
    << endl << flush;
#endif
  
  int64_t temporal_type = ti_value & 3;
  if( temporal_type != 1 ) {
    stringstream ss;
    ss << "TemporalIndex.cpp-temporal_mask-temporal_type.eq." << temporal_type << ".ne.1_NotImplemented-ti=0x" << hex << setw(16) << setfill('0') << ti_value;
    throw SpatialFailure(ss.str().c_str());
  }
  return ~((2ll^12)-1ll);
}

/**
   Return forward resolution "level" of the temporal index value.

   TODO: Check on temporal format, if needed.
 */
int64_t forward_resolution(int64_t ti_value) {
  TemporalIndex tIndex;
  return (ti_value 
	  >> tIndex.data.get("forward_resolution")->getOffset())
    & tIndex.data.get("forward_resolution")->getMask();
}

/**
   Return reverse resolution "level" of the temporal index value.

   TODO: Check on temporal format, if needed.
 */
int64_t reverse_resolution(int64_t ti_value) {
  TemporalIndex tIndex;  
  return (ti_value
	  >> tIndex.data.get("reverse_resolution")->getOffset())
    & tIndex.data.get("reverse_resolution")->getMask();
// #undef FMTX
// #define FMTX(x) " 0x" << setfill('0') << setw(16) << hex << x << dec
//   TemporalIndex tIndex;
//   int64_t mask   = 
//   int64_t offset = tIndex.data.get("reverse_resolution")->getOffset();
//   cout << "ti_val   " << FMTX(ti_value) << endl << flush;
//   cout << "mask     " << FMTX(mask) << endl << flush;
//   cout << "offset   " << FMTX(offset) << endl << flush;
//   cout << "t>>o     " << FMTX((ti_value >> offset)) << endl << flush;
//   cout << "(t>>o)&m " << FMTX(((ti_value >> offset) & mask)) << endl << flush;
//   return ((ti_value >> offset) & mask);							   
}

int64_t set_reverse_resolution(int64_t ti_value, int64_t resolution) {
  TemporalIndex tIndex;
  int64_t mask   = tIndex.data.get("reverse_resolution")->getMask();
  int64_t offset = tIndex.data.get("reverse_resolution")->getOffset();
  return (ti_value & ~(mask << offset)) | (resolution << offset);
}

// TODO Make array versions of these.

int64_t set_forward_resolution(int64_t ti_value, int64_t resolution) {
  TemporalIndex tIndex;
  int64_t mask   = tIndex.data.get("forward_resolution")->getMask();
  int64_t offset = tIndex.data.get("forward_resolution")->getOffset();
  return (ti_value & ~(mask << offset)) | (resolution << offset);
}

int64_t coarsen(int64_t ti_value,int64_t reverse_increment,int64_t forward_increment) {
  return set_forward_resolution(
				set_reverse_resolution(ti_value,
						       reverse_resolution(ti_value)+reverse_increment),
				forward_resolution(ti_value)+forward_increment);
}

/**
   True if the temporal index value is a lower bound.
 */
bool lowerBoundP(int64_t ti_value) {
  return (forward_resolution(ti_value) == 0) && (reverse_resolution(ti_value) == 63);
};

/**
   True if the temporal index value is an upper bound.
 */
bool upperBoundP(int64_t ti_value) {
  return (forward_resolution(ti_value) == 63) && (reverse_resolution(ti_value) == 63);
};

bool validBoundP(int64_t ti_value) {
  int64_t fr = forward_resolution(ti_value);
  int64_t rr = reverse_resolution(ti_value);
  return (rr == 63) && ( (fr == 0) || (fr == 63) );
}

bool validResolutionP(int64_t resolution) {
  TemporalIndex tIndex;
  return ( 0 <= resolution ) && ( resolution <= tIndex.data.maxResolutionLevel() );
}

/**
   Make a new temporal index value from a triple of values (lower, t0, upper).

   A negative value for lower or upper will cause that resolution to be set to the finest resolution (i.e. maximum resolution).

   A negative value for the "center" or tiv will be set to the mean of the lower and upper.
 */
int64_t scidbNewTemporalValue(int64_t tiv_lower, int64_t tiv, int64_t tiv_upper, bool include_bounds) {
  TemporalIndex ti0;
  int64_t reverse_resolution,forward_resolution;
  int64_t finest_resolution = ti0.data.maxResolutionLevel();
  
  if( tiv < 0 ) {
    TemporalIndex tiL(tiv_lower), tiU(tiv_upper);

    // cout << "100:sntv: " << tiU.toStringJulianTAI_ISO() << " vs. " << toStringJulianTAI_ISO(tiv_upper) << endl << flush;
    
    int64_t tL = tiL.toInt64Milliseconds();
    int64_t tU = tiU.toInt64Milliseconds();
    ti0.fromInt64Milliseconds((tL+tU)/2); /// Approximate calculation
    int64_t resolution = max(ti0.coarsestResolutionFinerOrEqualMilliseconds((tU-tL)/2),(int64_t)0ll);
    ti0.set_reverse_resolution(resolution).set_forward_resolution(resolution);
    reverse_resolution = resolution;
    forward_resolution = resolution;
  } else {
    ti0 = TemporalIndex(tiv);
    ti0.set_reverse_resolution(finest_resolution).set_forward_resolution(finest_resolution);
    int64_t t0 = ti0.toInt64Milliseconds();
    if( tiv_lower < 0 ) {
      // Keep that passed in.
      // Following is in error.
      // ti0.set_reverse_resolution(63);
      // reverse_resolution = 63;      
    } else {
      TemporalIndex tiL(tiv_lower);
      int64_t tL = tiL.toInt64Milliseconds();
      int64_t resolution = max(ti0.coarsestResolutionFinerOrEqualMilliseconds((t0-tL)),(int64_t)0ll);
      ti0.set_reverse_resolution(resolution);
      reverse_resolution = resolution;
    }
    if( tiv_upper < 0 ) {
      // Keep that passed in.
      // Following is in error.
      // ti0.set_forward_resolution(63);
      // forward_resolution = 63;      
    } else {
      TemporalIndex tiU(tiv_upper);

      // cout << "200:sntv: " << tiU.toStringJulianTAI_ISO() << " vs. " << toStringJulianTAI_ISO(tiv_upper) << endl << flush;
      
      int64_t tU = tiU.toInt64Milliseconds();
      int64_t resolution = max(ti0.coarsestResolutionFinerOrEqualMilliseconds((tU-t0)),(int64_t)0ll);
      ti0.set_forward_resolution(resolution);

      // cout << "201:sntv: " << tiU.toStringJulianTAI_ISO() << " vs. " << toStringJulianTAI_ISO(tiv_upper) << endl << flush;
      
      forward_resolution = resolution;      
    }
  }

  int64_t tiv_return = ti0.scidbTemporalIndex();
  TemporalIndex tI(tiv_return);
  
#if 0
  cout << "900:sntv: tI " << TemporalIndex(tI.scidbTerminator()).toStringJulianTAI_ISO()
       << " vs. ti0 " << TemporalIndex(ti0.scidbTerminator()).toStringJulianTAI_ISO()
       << " vs. toS " << toStringJulianTAI_ISO(scidbUpperBoundMS(tiv_return))
       << " vs. iU " << toStringJulianTAI_ISO(tiv_upper) << endl << flush;
#endif
  
  if( include_bounds ) {
    // cout << 1000 << endl << flush;
    // First lower
    if( tiv_lower >= 0 ) {
      if( !scidbContainsInstant(tiv_return,tiv_lower) ) {
        // cout << 2000 << endl << flush;    
        tiv_return = set_reverse_resolution(tiv_return,max(reverse_resolution-1,(int64_t)0ll));
      }
    } /* else {
      tiv_return = set_reverse_resolution(tiv_return,finest_resolution);
      } */
    
    // Then upper
    if( tiv_upper >= 0 ) {
      if( !scidbContainsInstant(tiv_return,tiv_upper) ) {
        // cout << 3000 << endl << flush;    
        tiv_return = set_forward_resolution(tiv_return,max(forward_resolution-1,(int64_t) 0ll));
      }
    } /* else {
      // cout << 4000 << endl << flush;
      tiv_return = set_forward_resolution(tiv_return,finest_resolution);      
      } */
  }
  return tiv_return;
}

/**
   Return a temporal index value from a TAI-based ISO string representation 
 */
int64_t fromStringJulianTAI_ISO(string inputString) {
  return TemporalIndex().fromStringJulianTAI_ISO(inputString).scidbTemporalIndex();
}

/**
   Return a TAI-based ISO string representation from a temporal index value
 */
string toStringJulianTAI_ISO(int64_t tiv) {
  return TemporalIndex(tiv).toStringJulianTAI_ISO();
}

int64_t scidbTemporalValueUnionIfOverlap(int64_t ti_value_0, int64_t ti_value_1) {

#if 0
  cout
    << "sO:lu0,lu1: "
    << toStringJulianTAI_ISO(ti_value_0) << " "
    << toStringJulianTAI_ISO(ti_value_1) << " "
    << endl << flush;
#endif
  
  int64_t tMask = temporal_mask(ti_value_0);
  int64_t lower0 = scidbLowerBoundMS(ti_value_0) & tMask;
  int64_t upper0 = scidbUpperBoundMS(ti_value_0) & tMask;
  int64_t lower1 = scidbLowerBoundMS(ti_value_1) & tMask;
  int64_t upper1 = scidbUpperBoundMS(ti_value_1) & tMask;

  if( !( (lower0 <= upper1) && (upper0 >= lower1) ) ) {
    throw SpatialFailure("scidbTemporalValueUnionIfOverlap:NoOverlap");
  }

  return scidbNewTemporalValue(lower0 < lower1 ? lower0 : lower1,
			       -1,
			       upper0 > upper1 ? upper0 : upper1,
			       true
			       );
  /*  
  TemporalIndex tIndex;
  int64_t lower = TemporalIndex( lower0 < lower1 ? lower0 : lower1 ).toInt64Milliseconds();
  int64_t upper = TemporalIndex( upper0 > upper1 ? upper0 : upper1 ).toInt64Milliseconds();
  int64_t mean  = (lower+upper)/2;
  int64_t dup   = upper-mean;
  int64_t dlo   = mean-lower;
  int64_t resu  = tIndex.coarsestResolutionFinerOrEqualMilliseconds(dup);
  int64_t resl  = tIndex.coarsestResolutionFinerOrEqualMilliseconds(dlo);
  tIndex = tIndex.fromInt64Milliseconds(mean).set_reverse_resolution(resl).set_forward_resolution(resu);

  return tIndex.scidbTemporalIndex();
  */
}

int64_t scidbTemporalValueIntersectionIfOverlap(int64_t ti_value_0, int64_t ti_value_1) {
  int64_t tMask = temporal_mask(ti_value_0);
  int64_t lower0 = scidbLowerBoundMS(ti_value_0) & tMask;
  int64_t upper0 = scidbUpperBoundMS(ti_value_0) & tMask;
  int64_t lower1 = scidbLowerBoundMS(ti_value_1) & tMask;
  int64_t upper1 = scidbUpperBoundMS(ti_value_1) & tMask;
  if( !( (lower0 <= upper1) && (upper0 >= lower1) ) ) {
    throw SpatialFailure("scidbTemporalValueIntersectionIfOverlap:NoOverlap");
  }
  return scidbNewTemporalValue(lower0 < lower1 ? lower1 : lower0,
			       -1,
			       upper0 > upper1 ? upper1 : upper0,
			       true
			       );

  /*
  TemporalIndex tIndex;
  int64_t lower = TemporalIndex( lower0 < lower1 ? lower1 : lower0 ).Toint64milliseconds();
  int64_t upper = TemporalIndex( upper0 > upper1 ? upper1 : upper0 ).toInt64Milliseconds();
  int64_t mean  = (lower+upper)/2;
  int64_t dup   = upper-mean;
  int64_t dlo   = mean-lower;
  cout << 2000 << endl << flush;
  int64_t resu  = tIndex.coarsestResolutionFinerOrEqualMilliseconds(dup);
  cout << 2010 << endl << flush;
  int64_t resl  = tIndex.coarsestResolutionFinerOrEqualMilliseconds(dlo);
  cout << 2020 << endl << flush;
  tIndex = tIndex.fromInt64Milliseconds(mean).set_reverse_resolution(resl).set_forward_resolution(resu);

  return tIndex.scidbTemporalIndex();  
  */
}

// } /* namespace std */
