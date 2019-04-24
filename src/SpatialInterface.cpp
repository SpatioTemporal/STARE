//#     Filename:       SpatialInterface.cpp
//#
//#     The htmInterface class is defined here.
//#
//#     Author:         Peter Z. Kunszt 
//#
//#     Date:           August 30 , 2000
//#
//#		Copyright (C) 2000  Peter Z. Kunszt, Alex S. Szalay, Aniruddha R. Thakar
//#                     The Johns Hopkins University
//#
//#     Modification History:
//#
//#     Oct 18, 2001 : Dennis C. Dinge -- Replaced ValVec with std::vector

#include <iostream>
#include <HtmRange.h>
#include "SpatialInterface.h"
#include "string.h"
#include "stdlib.h"
#include "math.h"
#include <iomanip> // setw

#define MAX_RANGES 100

// #define DIAG

#ifdef SpatialSGI
extern long long atoll (const char *str);
#endif



//==============================================================
//
// These are the implementations of the htm interface.
//
//==============================================================

///////////CONSTRUCTOR///////////////////////
htmInterface::htmInterface(size_t searchlevel, size_t buildlevel) : t_(NULL) {
	index_ = new SpatialIndex(searchlevel, buildlevel);
}

///////////DESTRUCTOR////////////////////////
htmInterface::~htmInterface() {
	delete index_;
	if(t_ != NULL) delete t_;
}

///////////LOOKUP METHODS////////////////////

uint64 htmInterface::lookupIDCmd(char *str) {

	cmd_ = str;
	if(t_ != NULL)delete t_;
	t_ = new VarStrToken(cmd_);

	float64 v[3];
	cmdCode code = getCode();

	if(code == NAME) {
		StdStr token = t_->next();
		if(token.empty())
			throw SpatialInterfaceError("htmInterface:lookupIDCmd: expected Name");
		return index_->idByName(token.data());
	}

	getDepth();
	if(! parseVec(code, v) )
		throw SpatialInterfaceError("htmInterface:lookupIDCmd: Expect vector in Command. ", cmd_.data());

	if( code == J2000 )
		return lookupID(v[0], v[1]);
	return lookupID(v[0], v[1], v[2]);

}

const char * htmInterface::lookupNameCmd(char *str) {

	cmd_ = str;
	if(t_ != NULL)delete t_;
	t_ = new VarStrToken(cmd_);

	float64 v[3];
	cmdCode code = getCode();

	if(code == ID) {
		uint64 id = getInt64();
		index_->nameById(id, name_);
	} else {
		getDepth();

		if(! parseVec(code, v) )
			throw SpatialInterfaceError("htmInterface:lookupNameCmd: Expect vector in Command. ", cmd_.data());

		if( code == J2000 )
			index_->nameByPoint(v[0], v[1], name_);
		else {
			SpatialVector tv(v[0], v[1], v[2]);
			index_->nameByPoint(tv, name_);
		}
	}
	return name_;
}

// get the depth, which is the first item in the first character argument.
#if defined(__sun)
cmdCode
#else
htmInterface::cmdCode
#endif
htmInterface::getCode() {

	cmdCode code;

	// parse incoming string. expect to have an integer indicating the
	// depth at first position.
	StdStr token = t_->next();

	if     ( token == "J2000" )
		code = J2000;
	else if( token == "CARTESIAN" )
		code = CARTESIAN;
	else if( token == "NAME" )
		code = NAME;
	else if( token == "ID" )
		code = ID;
	else if( token == "DOMAIN" )
		code = HTMDOMAIN;
	else
		throw SpatialInterfaceError("htmInterface:getCode: Unexpected command",token);

	return code;
}

void 
htmInterface::getDepth() { // TODO Why depth?  Switch to level.

	size_t depth = getInteger();
	if(depth > HTMMAXDEPTH)
		throw SpatialInterfaceError("htmInterface:getDepth: Depth too large: Max is HTMMAXDEPTH");

	changeDepth(depth); // TODO replace with level
}

// get an integer out of the command string
int32 
htmInterface::getInteger() {

	if(t_ == NULL)
		throw SpatialFailure("htmInterface:getInteger: No command to parse");

	// parse incoming string. expect to have an integer.
	const StdStr &token = t_->next();
	if(!isInteger(token))
		throw SpatialInterfaceError("htmInterface:getInteger: Expected integer at first position of Command. ",cmd_.data());

	return atoi(token.data());
}

// get an 64bit integer out of the command string
uint64 
htmInterface::getInt64() {

	if(t_ == NULL)
		throw SpatialFailure("htmInterface:getInt64: No command to parse");

	// parse incoming string. expect to have an integer.
	const StdStr &token = t_->next();
	if(!isInteger(token))
		throw SpatialInterfaceError("htmInterface:getInt64: Expected integer at first position of Command. ",cmd_.data());
#ifdef SpatialWinNT
	return _atoi64(token.data());
#elif defined(SpatialDigitalUnix)
	return atol(token.data());
#elif defined(__macosx)
	return atol(token.data());
#else
	return atoll(token.data());
#endif
}

// get a float64 out of the command string
float64 
htmInterface::getFloat() {

	if(t_ == NULL)
		throw SpatialFailure("htmInterface:getFloat: No command to parse");

	// parse incoming string. expect to have an integer.
	const StdStr &token = t_->next();
	if(!isFloat(token))
		throw SpatialInterfaceError("htmInterface:getFloat: Expected float at first position of Command. ",cmd_.data());

	return atof(token.data());
}

// parse the string, returning the number of floats
// that have been in the string.
bool
htmInterface::parseVec( cmdCode code, float64 *v) {

	typedef StdStr VaribleString;

	VaribleString token;
	size_t  i = 0, len;

	if(code == J2000)
		len = 2;
	else if(code == CARTESIAN)
		len = 3;
	else
		throw SpatialInterfaceError("htmInterface:parseVec: Expected code J2000 or CARTESIAN.");

	// parse the next len positions
	while( i < len  ) {
		token = t_->next();
		if(token.empty())break;

		if(!isFloat(token))
			throw SpatialInterfaceError("htmInterface:parse: Expected float at this position of Command. ",cmd_.data());
		if(i == len)
			throw SpatialInterfaceError("htmInterface:parse: Expect less floats in Command. ", cmd_.data());
		v[i++] = atof(token.data());
	}

	if(i < len)
		return false;
	return true;

}

// check whether string is an integer
bool htmInterface::isInteger(const StdStr &str) {
	if(str.empty()) return false;
	uint32 len = str.length();
	return (strspn(str.data(),"+0123456789") == len) ? true : false ;
}

// check whether string is a float
bool htmInterface::isFloat(const StdStr &str) {
	if(str.empty()) return false;
	uint32 len = str.length();
	return (strspn(str.data(),"+-.e0123456789") == len) ? true : false ;
}


// check whether an id is in a range
bool htmInterface::inRange( const HTMRangeValueVector &range, uint64 id) {
	size_t len = range.size() - 1;

	// completely outside range?
	if(id < range[0].lo || id > range[len].hi)return false;

	// check each range
	for(size_t i = 0; i <= len; i++)
		if(id <= range[i].hi && id >= range[i].lo) return true;
	return false;
}

// print the range
void 
htmInterface::printRange( const HTMRangeValueVector &range) {

#ifdef _WIN32
	char buf[20];
#endif

	//using namespace std;
	for(size_t i = 0; i < range.size(); i++) {
		cout << SpatialIndex::nameById(range[i].lo) << ":"
				<< SpatialIndex::nameById(range[i].hi) << "   ";

		// Windows has a bug in the << operator for int64
		// (see MSDN / Knowledge Base / Visual C++ / BUG: Stream Operator << Cannot Handle __int64 Type
#ifdef _WIN32
		sprintf(buf,"%I64d", range[i].lo );
		cout << buf;
		sprintf(buf,"%I64d", range[i].hi );
		cout << buf;
#else
		cout << range[i].lo << " - " << range[i].hi;
#endif
		cout << endl;
	}
}



//////////////////////CIRCLEREGION METHODS//////////////////////

const HTMRangeValueVector & 
htmInterface::circleRegion( float64 ra,
		float64 dec,
		float64 rad ) {

	SpatialDomain dom;

	RangeConvex cvx;

	float64 d = cos(gPi * rad/10800.0);
	SpatialConstraint c(SpatialVector(ra,dec),d);
	cvx.add(c); // [ed:RangeConvex::add]
	dom.add(cvx);
	return domain(dom);
}

const HTMRangeValueVector & 
htmInterface::circleRegion( float64 x,
		float64 y,
		float64 z,
		float64 rad ) {

	SpatialDomain dom;


	RangeConvex cvx;

	float64 d = cos(gPi * rad/10800.0);
	SpatialConstraint c(SpatialVector(x,y,z),d);

	cvx.add(c); // [ed:RangeConvex::add]
	dom.add(cvx);
	return domain(dom);
}

const HTMRangeValueVector & 
htmInterface::circleRegionCmd( char *str ) {

	cmd_ = str;
	if(t_ != NULL)delete t_;
	t_ = new VarStrToken(cmd_);

	float64 v[3];
	float64 d;

	cmdCode code = getCode();
	getDepth();
	if(! parseVec(code, v) )
		throw SpatialInterfaceError("htmInterface:circleRegionCmd: Expect vector in Command. ", cmd_.data());

	d = getFloat();
	if( code == J2000 )
		return circleRegion(v[0], v[1], d);

	return circleRegion(v[0], v[1], v[2], d);
}

//////////////////ConvexHull///////////////////////
const HTMRangeValueVector & 
htmInterface::convexHull( ValueVectorF64 ra,
		ValueVectorF64 dec ) {

	if(ra.size() != dec.size())
		throw SpatialBoundsError("htmInterface:convexHull: ra and dec list are not equal size");

	polyCorners_.clear();
	for(size_t i = 0; i < ra.size(); i++) {
		SpatialVector v(ra[i],dec[i]);
		setPolyCorner(v);
	}

	return doHull();
}

const HTMRangeValueVector & 
htmInterface::convexHull(
		ValueVectorF64 x,
		ValueVectorF64 y,
		ValueVectorF64 z ) {

	if(x.size() != y.size() || x.size() != z.size())
		throw SpatialBoundsError("htmInterface:convexHull: x,y,z lists are not equal size");

	polyCorners_.clear();
	for(size_t i = 0; i < x.size(); i++) {
		SpatialVector v(x[i],y[i],z[i]);
		setPolyCorner(v);
	}
	return doHull();
}

const HTMRangeValueVector & 
htmInterface::convexHull( LatLonDegrees64ValueVector latlon, size_t steps ) {
	polyCorners_.clear();
//	cout << " ch " << 2000 << " latlon-size=" << latlon.size() << flush ;
//	cout << endl;
	if (steps == (uint64) -1) {
		steps = latlon.size();
	} else {
		steps = min(steps,latlon.size());
	}
	for(size_t i = 0; i < steps; i++) {
//		cout << " " << i << flush;
//		cout << " ( " << latlon[i].lat << " " << latlon[i].lon << ")";
		float64 *x = xyzFromLatLonDegrees(latlon[i].lat,latlon[i].lon);
		SpatialVector v(x[0],x[1],x[2]);
		setPolyCorner(v);
//		cout << endl << flush;
	}
//	cout << endl << flush << 2100 << endl << flush;
	return doHull();
}

const HTMRangeValueVector &
htmInterface::convexHullCmd( char *str ) {

	cmd_ = str;
	if(t_ != NULL) delete t_;
	t_ = new VarStrToken(cmd_);

	float64 v[3];

	cmdCode code = getCode();
	getDepth();
	polyCorners_.clear();

	while(  parseVec( code, v ) ) {
		if(code == J2000) {
			SpatialVector tv(v[0],v[1]);
			setPolyCorner(tv);
		} else {
			SpatialVector tv(v[0],v[1],v[2]);
			setPolyCorner(tv);
		}
	}

	return doHull();
}

const HTMRangeValueVector &
htmInterface::doHull() {

//	cout << 3000
//			<< " pCorners-size="
//			<< polyCorners_.size()
//			<< endl << flush;
	if(polyCorners_.size() < 3) {
//		cout << 3001
//				<< endl << flush;
		throw SpatialInterfaceError("htmInterface:convexHull: empty hull: points on one line");
//		cout << 3002 << endl << flush;
	}

	SpatialVector v;

	RangeConvex cvx;

	const SpatialIndex index = this->index();
	SpatialDomain dom(&index); // mlr -- should domain have its own index?
//	dom.setOlevel(index.getMaxlevel()); // SpatialDomain should have an index! // TODO Weird
//	cout << 3005 << "index.getMaxlevel()=" << index.getMaxlevel() << endl << flush;
	// Note:  index.getMaxlevel is getLeafLevel

	// The constraint we have for each side is a 0-constraint (great circle)
	// passing through the 2 corners. Since we are in counterclockwise order,
	// the vector product of the two successive corners just gives the correct
	// constraint.
//	cout << 3100 << endl << flush;
	size_t i, len = polyCorners_.size();
	for(i = 0; i < len; i++) {
		v = polyCorners_[i].c_ ^ polyCorners_[ i == len-1 ? 0 : i + 1].c_;
#ifdef DIAG
		cerr << "doHull:: " << v << " " << i << "," << i+1 << endl;
#endif
		v.normalize();
		SpatialConstraint c(v,0);
		cvx.add(c); // [ed:RangeConvex::add]
	}
	dom.add(cvx);
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//	dom.convexes_[0].boundingCircle_.write(cout);
	//	dom.write(cout);
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	cout << 3999 << endl << flush;

	return domain(dom);
}

//*******************************
//
// polygon processing : generate the convex
// hull of the points given in POLY - and then generate
// the proper x,y,z constraint.
//
//----------------------------------------------
// get corner - pop off two last items from stack, which must be
// numbers - update poly list.
void 
htmInterface::setPolyCorner(SpatialVector &v) {

//	cout << " spc: v " << v.x() << " " << v.y() << " " << v.z() << " " ;

	size_t i,len = polyCorners_.size();
	// test for already existing points
	for(i = 0; i < len; i++)
		if(v == polyCorners_[i].c_)return;

	if(len < 2) {
		// just append first two points.
		// dcd: need two steps here: the resize adds one element to end of polyCorners
		// the old insert function also returned the new length so I had to get that
		// in the second step len = polyCorners_.size();
		polyCorners_.resize(polyCorners_.size() + 1);
		len = polyCorners_.size();

		polyCorners_[len-1].c_ = v;
	} else if (len == 2) {
		// first polygon: triangle. set correct orientation.
		if( (polyCorners_[0].c_ ^ polyCorners_[1].c_)*v > 0 ) {
			polyCorners_.resize(polyCorners_.size() + 1);
			polyCorners_[2].c_ = v;
		} else if( (polyCorners_[0].c_ ^ polyCorners_[1].c_)*v < 0 ) {
			polyCorners_.insert(polyCorners_.end(), polyCorners_[1]); // GYF polyCorners_[1] was missing!!!!
			polyCorners_[1].c_ = v;
		}
	} else {
		//
		// Now we set the flags for the existing polygon.
		// if the new point is inside (i.e. to the left) of
		// the half-sphere defined by the points polyCorners_[i],[i+1]
		// we set polyCorners_[i].inside_ to true.
		//
		// if it is outside, and the previous side was also outside,
		// set the replace_ flag to true(this corner will be dropped)
		// (be careful on the edges - that's the trackoutside flag)

		bool polyTrackOutside = false;
		for(i = 0 ; i < len; i++) {
			polyCorners_[i].replace_ = false;
			polyCorners_[i].inside_  = false;

			// test if new point is inside the constraint given by a,b
			if( (polyCorners_[i].c_ ^ polyCorners_[i+1==len ? 0 : i+1].c_)*v > 0 ) {
				polyCorners_[i].inside_ = true;
				polyTrackOutside = false;
			} else {
				if(polyTrackOutside) polyCorners_[i].replace_ = true;
				polyTrackOutside = true;
			}
		}
		if(polyTrackOutside && !polyCorners_[0].inside_)
			polyCorners_[0].replace_ = true;

#ifdef DIAG
		for(i = 0; i < len; i++)
			cerr << "setPolyCorner " << i << " : "
			<< (polyCorners_[i].replace_ ? "replace" : "keep   ")
			<< (polyCorners_[i].inside_ ? "inside  : " : "outside : ")
			<< polyCorners_[i].c_ << endl;
#endif
		// now delete all corners that have the 'replace' flag set
		// If called from test11, seems to die in the loop. Only when len > 2 (GYF)
		i = 0;
		while(i < len) {
			if(polyCorners_[i].replace_) {
//				cout << " deleting=-" << i << " ( "
//						<< polyCorners_[i].c_.x() << " "
//						<< polyCorners_[i].c_.y() << " "
//						<< polyCorners_[i].c_.z() << " "
//						<< " ) " << flush;
				// TODO The following erases the -i'th corner, but we test the i'th one above?
				//    		polyCorners_.erase(polyCorners_.end()-i); // remove returns new length
				polyCorners_.erase(polyCorners_.begin()+i); // remove returns new length
				len--;
			} else i++;
		}

		// now find first corner that is not inside (there is only one)
		// and insert the point after that.
		// if all points are inside we did nothing...

		for(i = 0; i < len; i++) {
			if(!polyCorners_[i].inside_) {
#ifdef DIAG
				cerr << "setPolyCorner: Insert after " << i << " length = " << len << endl;
#endif
				if(i == len-1) { // append if last
					//			polyCorners_.resize(polyCorners_.size() + 1);
					polyCorners_.push_back(htmPolyCorner(v)); // Really append...
				} else {
					//was: polyCorners_.insert(polyCorners_.end() - i - 1);
					// cf. AJM: polyCorners_.insert(polyCorners_.end() - i - 1);
					//???			cout << "setPolyCorner: the other case" << flush;
					// cerr << "setPolyCorner: the other case" << endl;
					polyCorners_.insert(polyCorners_.begin()+i+1,htmPolyCorner(v)); // Really insert...
				}
//				cout << " adding ( " << i+1 << " " << v << " ) " << flush;
				////		polyCorners_[i+1].c_ = v; // This is not insertion.
				//		polyCorners_.insert(polyCorners_.begin()+i+1,htmPolyCorner(v));
				break;
			}
			// cerr << "setPolyCorner: still living" << endl;
		}
	}
#ifdef DIAG
	cerr << "QL: Polygon: now " << polyCorners_.size() << endl;
	for(i = 0; i < polyCorners_.size(); i++)
		cerr << polyCorners_[i].c_ << endl;
#endif
}


//////////////////////////domain/////////////////////////
const HTMRangeValueVector & 
htmInterface::domain( SpatialDomain & domain ) {
	HtmRange htmRange;

//	cout << 4000 << endl << flush;

	Key gapsize;
	//  SpatialIndex idx(20, 5);
	//  domain.setOlevel(20);

	domain.intersect(index_, &htmRange, false);

//	cout << 4100 << endl << flush;

	gapsize = htmRange.bestgap(MAX_RANGES);
	htmRange.defrag(gapsize);

//	cout << 4200 << endl << flush;
//	htmRange.defrag();
	// DONT FORGET to: get best gap and defrag  htmRange.defrag(bestgap);

//	cout << 4300 << endl << flush;
	// Construct the valuevector...
	fillValueVec( htmRange, range_);
//	cout << 4999 << endl << flush;
	return range_;
}



const HTMRangeValueVector & 
htmInterface::domainCmd( char *str ) {

	cmd_ = str;
	if(t_ != NULL)delete t_;
	t_ = new VarStrToken(cmd_);

	cmdCode code = getCode();
	if(code != HTMDOMAIN)
		throw SpatialInterfaceError("htmInterface:domainCmd: missing keyword HTMDOMAIN");
	getDepth();

	int32 nx,nc;
	nx = getInteger();

	SpatialDomain dom;
	for(int32 i = 0 ; i < nx; i++ ) {

		RangeConvex convex;

		nc = getInteger();
		for(int32 j = 0; j < nc; j++ ) {
			float64 x = getFloat();
			float64 y = getFloat();
			float64 z = getFloat();
			float64 d = getFloat();
			SpatialConstraint c(SpatialVector(x,y,z),d);
			convex.add(c); // [ed:RangeConvex::add]
		}
		dom.add(convex);
	}
	// TODO Fix weirdness.
	dom.setIndex(new SpatialIndex(20)); // SpatialIndex? Memory leak possibility?
//	dom.setOlevel(20); 		// [ed:olevel] /// Domain should have an index that sets this...
	return domain(dom);
}

#ifdef _WIN32
ostream& operator<<(ostream& os, const int64 &i64)
{
	char buff[256];
	sprintf(buff, "%I64d", i64);
	os << buff;
	return os;
}
ostream& operator<<(ostream& os, const uint64 &ui64)
{
	char buff[256];
	sprintf(buff, "%I64d", ui64);
	os << buff;
	return os;
}
#endif

void htmInterface::fillValueVec(HtmRange &hr, HTMRangeValueVector &vec)
{
	htmRange ran;
	vec.clear();
	hr.reset();
	while(hr.getNext( (Key &) ran.lo, (Key &) ran.hi)){
		vec.push_back(ran);
	}	
}
