#ifndef _SpatialVector_h
#define _SpatialVector_h

//#     Filename:       SpatialVector.h
//#
//#     Standard 3-d vector class
//#
//#     Author:         Peter Z. Kunszt, based on A. Szalay's code
//#     
//#     Date:           October 15, 1998
//#
//#		Copyright (C) 2000  Peter Z. Kunszt, Alex S. Szalay, Aniruddha R. Thakar
//#                     The Johns Hopkins University
//#
//#     Modification History:
//#
//#     Oct 18, 2001 : Dennis C. Dinge -- Replaced ValVec with std::vector
//#

//#include <math.h>
#include <cmath>
#include <stdio.h>
#include <iostream>
#include <SpatialGeneral.h>

//########################################################################
/**

   The SpatialVector is a 3D vector usually living on the surface of
   the sphere. The corresponding ra, dec can be obtained if the vector
   has unit length. That can be ensured with the normalize() function.

*/

class LINKAGE SpatialVector {
public:
  /// constructs (1,0,0), ra=0, dec=0.
  SpatialVector();

  /// Constructor from three coordinates, not necessarily normed to 1
  SpatialVector(float64 x,
		float64 y,
		float64 z);
  SpatialVector(float64 *x);

  /// Constructor from ra/dec, this is always normed to 1
  SpatialVector(float64 ra,
		        float64 dec);

  /// Copy constructor
  SpatialVector(const SpatialVector &);

  /// Assignment
  SpatialVector& operator =(const SpatialVector &);

  /// Set member function: set values - always normed to 1
  void set(const float64 &x,
	   const float64 &y,
	   const float64 &z);

  /// Set member function: set values - always normed to 1
  void set(const float64 &ra,
	   const float64 &dec);

  void setLatLonDegrees(const float64 &lat,
		  	  	 	 	const float64 &lon);

  bool getLatLonDegrees(float64 &lat, float64 &lon);

  /// Get x,y,z
  void get( float64 &x,
	    float64 &y,
	    float64 &z) const;

  /// Get ra,dec - normalizes x,y,z
  void get( float64 &ra,
	    float64 &dec);

  /// return length of vector
  float64 length() const;

  /// return x (only as rvalue)
  float64 x() const;

  /// return y
  float64 y() const;

  /// return z
  float64 z() const;

  /// return ra - this norms the vector to 1 if not already done so
  float64 ra();

  /// return dec - this norms the vector to 1 if not already done so
  float64 dec();

  /// return a copy rotated by theta about axis
  SpatialVector rotatedAbout(const SpatialVector axis, const float64 theta) const;

  /// Normalize vector length to 1
  void normalize();
  /// Normalize vector length to r
  void normalize(float64 r);

  // Return the reverse of the spatial vector.
  SpatialVector reverse();

  /// Printf it to stdout
  void show() const;

  /// Read vector from a stream
  void read(std::istream &);

  /// Write vector to a stream
  void write(std::ostream &) const;

  /// Write vector to a new string. Delete when done.
  const char* toString();

  /// Write vector to a new array. Delete when done.
  float64* toArray();

  /// Comparison
  int operator ==(const SpatialVector & ) const;

  /// dot product
  float64 operator *(const SpatialVector & ) const;

  /// cross product
  SpatialVector operator ^(const SpatialVector & ) const;

  /// addition
  SpatialVector operator +(const SpatialVector & ) const;

  /// subtraction
  SpatialVector operator -(const SpatialVector & ) const;

  /**@name Scalar products with int and float */
  //@{
  /**@name operator *= */
  SpatialVector & operator *=(float64);
  SpatialVector & operator *=(int);
  friend SpatialVector operator *(float64, const SpatialVector &);
  friend SpatialVector operator *(int,     const SpatialVector &);
  friend SpatialVector operator *(const SpatialVector &, float64);
  friend SpatialVector operator *(const SpatialVector &, int);
  //@}

private:
  float64 x_;
  float64 y_;
  float64 z_;
  float64 ra_;
  float64 dec_;
  bool okRaDec_;

  float64 latDegrees_;
  float64 lonDegrees_;
  bool latlon_=false;

  void updateXYZ();
  void updateRaDec();

  friend class SpatialIndex;
  friend class SpatialDomain;
  friend class sxSpatialDomain;
};

void SpatialVector_test();

int equal_within_tolerance(const SpatialVector &a, const SpatialVector &b, float64 tolerance);

/// A collection of vertices
typedef std::vector<SpatialVector> Vertices;
/// A Triangle struct for convenience & typing. Note vertices must have the appropriate ordering.
// TODO Consider a full-fledged Triangle class to refactor some geometry out of index, etc.
struct Triangle {SpatialVector centroid;  Vertices vertices;};

#include "SpatialVector.hxx"

#endif

