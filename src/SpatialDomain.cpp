//#     Filename:       SpatialDomain.cpp
//#
//#     The SpatialDomain
//#     classes are defined here.
//#
//#     Author:         Peter Z. Kunszt based on A. Szalay's code
//#     
//#     Date:           October 23, 1998
//#
//#		Copyright (C) 2000  Peter Z. Kunszt, Alex S. Szalay, Aniruddha R. Thakar
//#                     The Johns Hopkins University
//#
//#     Modification History:
//#
//#     Oct 18, 2001 : Dennis C. Dinge -- Replaced ValVec with std::vector
//#

#include "SpatialDomain.h"
#include "SpatialException.h"

#include "Htmio.h"

// ===========================================================================
//
// Member functions for class SpatialDomain
//
// ===========================================================================

/////////////CONSTRUCTOR//////////////////////////////////
//
// Initialize
//
SpatialDomain::SpatialDomain(const SpatialIndex * i) :
  index(i) {
//	setOlevel(i->getLeafLevel());  Needed?
}

/////////////DESTRUCTOR///////////////////////////////////
//
SpatialDomain::~SpatialDomain()
{
}

/////////////SETINDEX/////////////////////////////////////
//
void
SpatialDomain::setIndex(const SpatialIndex * idx)
{
  index = idx;
}

/////////////ADD//////////////////////////////////////////
//
void
SpatialDomain::add(RangeConvex & c)
{
//	c.setOlevel(olevel); // TODO not needed?
  convexes_.push_back(c);
//  c.setOlevel(olevel);
}

///// Mischief here: SpatialDomain has an index (with a level) as well as a level?
//void SpatialDomain::setOlevel(int level)
//{
//  size_t i;
//  this->olevel = level;  /// What about index->maxlevel_? Or leaf level?
//
//  for(i = 0; i < convexes_.size(); i++)  // intersect every convex
//    convexes_[i].setOlevel(level);
//}

// //////////INTERSECT////////////////////////////////////
/// Intersect using a new SpatialIndex.
///
bool
SpatialDomain::intersect(const SpatialIndex * idx, HtmRange *htmrange, bool varlen, HtmRange *hrInterior, HtmRange *hrBoundary)
{
  index = idx;
  // size_t i;

  return intersect(htmrange,varlen,hrInterior,hrBoundary);
//  for(i = 0; i < convexes_.size(); i++)  // intersect every convex
//    convexes_[i].intersect(index, htmrange, varlen);
//  return true;
}
/// Intersect using the default SpatialIndex in the domain.
bool
SpatialDomain::intersect(HtmRange *htmrange, bool varlen, HtmRange *hrInterior, HtmRange *hrBoundary)
{
	size_t i;
	bool ok = false;
// #define DIAG
#ifdef DIAG
	cout << "sd::i: " << "1000" << endl << flush;
#endif
	if(index) {
#ifdef DIAG
		cout << "sd::i: " << "1001 size: " << convexes_.size() << endl << flush;
#endif
		for(i = 0; i < convexes_.size(); i++)  // intersect every convex
			convexes_[i].intersect(index, htmrange, varlen, hrInterior, hrBoundary);
#ifdef DIAG
		cout << "sd::i: " << "1002" << endl << flush;
#endif
		ok = true;
	}
#ifdef DIAG
	cout << "sd::i: " << "1003" << endl << flush;
#endif
// #undef DIAG
	return ok;
}
/////////////Write////////////////////////////////////////
//
void
SpatialDomain::write(ostream &out) const {
  out << "#DOMAIN" << endl;
  out << convexes_.size() << endl;
  for (size_t i = 0; i < convexes_.size() ; i++)
    out << convexes_[i];
}

/////////////COMPUINT64///////////////////////////////////
// compare ids
//
int 
compUint64(const void* v1, const void* v2) {
  return (  ( *((uint64 *)v1) < *((uint64 *)v2) ) ? -1 :
	    ( ( *((uint64 *)v1) > *((uint64 *)v2) ) ? 1 : 0 ) );
}

/////////////COMPRANGE///////////////////////////////////
// compare ids
//
int 
compRange(const void* v1, const void* v2) {
  uint64 a = *((uint64 *)v1);
  uint64 b = *((uint64 *)v2);

  while( (a & SpatialDomain::topBit_) == 0 ) a = a << 2 ;
  while( (b & SpatialDomain::topBit_) == 0 ) b = b << 2 ;

  return (  ( a < b ) ? -1 : ( ( a > b ) ? 1 : 0 ) );
}

uint64 SpatialDomain::topBit_ = 0;

/////////////>>///////////////////////////////////////////
// read from istream
//
istream& operator >>( istream& in, SpatialDomain & c) {
  Htmio::read(in, c);
  return(in);
}


/////////////<<///////////////////////////////////////////
// write to ostream
//
ostream& operator <<( ostream& out, const SpatialDomain & c) {
  c.write(out);
  return(out);
}
ostream& operator <<( ostream& out, SpatialDomain & c) {
  c.write(out);
  return(out);
}


