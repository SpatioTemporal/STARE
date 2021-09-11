//#     Filename:       SpatialVector.cpp
//#
//#     The SpatialVector class is defined here.
//#
//#     Author:         Peter Z. Kunszt based on A. Szalay's code
//#
//#     Date:           October 15, 1998
//#
//#		Copyright (C) 2000  Peter Z. Kunszt, Alex S. Szalay, Aniruddha R. Thakar
//#                     The Johns Hopkins University
//#     Modification History:
//#
//#     Oct 18, 2001 : Dennis C. Dinge -- Replaced ValVec with std::vector
//#


#include "SpatialVector.h"
#include "SpatialException.h"
#include <iomanip>

//==============================================================
//
// This 3D vector lives on the surface of the sphere.
// Its length is always 1.
//
//==============================================================

/////////////CONSTRUCTOR//////////////////////////////////
//
SpatialVector::SpatialVector() : 
  x_(1), y_(0), z_(0), ra_(0), dec_(0), okRaDec_(true) {
}

/////////////CONSTRUCTOR//////////////////////////////////
/// TODO Maybe we should create a Coordinate class to handle R3, RA-DEC, LatLon, etc.
SpatialVector::SpatialVector(float64 x, float64 y, float64 z) :
  x_(x), y_(y), z_(z), ra_(0), dec_(0), okRaDec_(false) {
}

/////////////CONSTRUCTOR//////////////////////////////////
/// TODO Strengthen the following input type.
SpatialVector::SpatialVector(float64 *x) :
  x_(x[0]), y_(x[1]), z_(x[2]), ra_(0), dec_(0), okRaDec_(false) {
}
/////////////CONSTRUCTOR//////////////////////////////////
SpatialVector::SpatialVector(std::vector<float64> v) :
  x_(v[0]), y_(v[1]), z_(v[2]), ra_(0), dec_(0), okRaDec_(false) {
}
/////////////CONSTRUCTOR//////////////////////////////////
//
SpatialVector::SpatialVector(float64 ra, float64 dec) :
    ra_(ra), dec_(dec), okRaDec_(true) {
  updateXYZ();
  updateRaDec();
}

/////////////COPY CONSTRUCTOR/////////////////////////////
//
SpatialVector::SpatialVector(const SpatialVector & vv) :
  x_(vv.x_), y_(vv.y_), z_(vv.z_), ra_(vv.ra_), dec_(vv.dec_), 
  okRaDec_(vv.okRaDec_) {
}

/////////////ASSIGNMENT///////////////////////////////////
//
SpatialVector&
SpatialVector::operator =(const SpatialVector & vv)
{
  x_ = vv.x_;
  y_ = vv.y_;
  z_ = vv.z_;
  ra_ = vv.ra_;
  dec_ = vv.dec_;
  okRaDec_ = vv.okRaDec_;
  return *this;
}

/////////////SET//////////////////////////////////////////
//
void
SpatialVector::set(const float64 &x, const float64 &y, const float64 &z )
{
  x_ = x;
  y_ = y;
  z_ = z;
  normalize();
  updateRaDec();
}
/////////////SET//////////////////////////////////////////
//
void
SpatialVector::set(const float64 &ra, const float64 &dec)
{
  ra_ = ra;  
  dec_ = dec;
  updateXYZ();
}

void
SpatialVector::setLatLonDegrees(const float64 &lat, const float64 &lon)
{
//	throw SpatialUnimplemented();
	latDegrees_ = lat;
	lonDegrees_ = lon;
	latlon_     = true;
	// float64 *x = xyzFromLatLonDegrees(lat,lon);
	std::vector<float64> x = xyzFromLatLonDegrees(lat,lon);
	x_ = x[0];
	y_ = x[1];
	z_ = x[2];
	normalize();
//	updateRA(); // TODO Why should we do this?
}

// TODO The right thing to do is to implement a coordinate class for the different kinds of coords we use.

/**
 * Get the lat-lon in degrees. Presume we're a unit vector.
 *
 * TODO We should have a SpatialUnitVector class.
 *
 */
bool
SpatialVector::getLatLonDegrees(float64 &lat, float64 &lon) {
  
	if (latlon_) {
		lat = latDegrees_;
		lon = lonDegrees_;
	} else {
		float64 X,Y,Z;
		X = x_; Y = y_; Z = z_;
		if(length()!=1) { // TODO: Repent disrespecting machine precision
			// TODO A logger would be useful here.
			// throw SpatialFailure("SpatialVector::getLatLonDegrees::ERROR Calculating lat-lon-degrees from a non-unit vector.");

			// Deal with it.
			float64 L = sqrt( X*X + Y*Y + Z*Z );
			X /= L; Y /= L; Z /= L;
		}

		lat = asin(Z); // easy.
		float64 cd = cos(lat);
		lat /= gPr;

		float64 L =  sqrt( X*X + Y*Y + Z*Z );

		/*
		std::cout << std::setprecision(16) 
			  << " cd,geps: " << cd << " " << gEpsilon << std::endl
			  << " X,Y,Z,L: " << X << " " << Y << " " << " " << Z << " " << L  << std::endl
		          << " gPr:     " << gPr << std::endl
			  << std::flush
			  << " mu:      " << (X/cd) << std::endl
			  << " acs(mu): " << acos(X/cd) << std::endl
			  << " acs(mu): " << acos(std::max(-1.0,std::min(1.0,(X/cd)))) << std::endl
			  << " acs(mu): " << acos(X/(L*cd)) << std::endl
			  << std::flush;
		*/

		if(cd>gEpsilon || cd<-gEpsilon) {
			if(Y>gEpsilon || Y<-gEpsilon) {
//				if(abs(X) < 1.0e-7) {
//					std::cout << 1000 << " x_,X,cd,X/cd: " << x_ << "," << X  << "," << cd << "," << X/cd << std::endl << std::flush;
//				}
			  // The following didn't work on Ubuntu. MLR 2021-0910-1
			  // float64 mu = X/cd; // Note, we normalized above so this should be within [-1,1] to within machine error.
			  //
			        float64 mu = std::max(-1.0,std::min(1.0,(X/cd))); // Mea culpa. MLR 2021-0910-1
				if( abs(mu) > 1 ) {
					lon = (X < 0.0 ? 180.0 : 0.0);
				} else {
					if (Y < 0.0) {
						lon = 360.0 - acos(mu)/gPr;
					} else {
						lon = acos(mu)/gPr;
					}
				}
			} else {
				lon = (X < 0.0 ? 180.0 : 0.0);
			}
		} else {
			lon=0.0;
		}
	}

	return latlon_;
}

/////////////GET//////////////////////////////////////////
//
void
SpatialVector::get(float64 &x,float64 &y,float64 &z ) const
{
  x = x_;
  y = y_;
  z = z_;
}

/////////////GET//////////////////////////////////////////
//
void
SpatialVector::get(float64 &ra,float64 &dec )
{
  if(!okRaDec_) {
    normalize();
    updateRaDec();
  }
  ra = ra_;
  dec = dec_;
}

float64 SpatialVector::ra() {
  if(!okRaDec_) {
    normalize();
    updateRaDec();
  }
  return ra_;
}

float64 SpatialVector::dec() {
  if(!okRaDec_) {
    normalize();
    updateRaDec();
  }
  return dec_;
}

SpatialVector SpatialVector::rotatedAbout(const SpatialVector axis, const float64 theta) const {
	// Use Rodrigues's formula
	// Inefficient in that we're calling the transcendental functions each time.
	const float64 mu     = cos(theta);
	// muComp = mu - 1.0; // error? Wikipedia says -- sign error.
	// const float64 muComp = mu - 1.0;
	const float64 muComp = 1.0 - mu;
	const float64 lambda = sin(theta);

	SpatialVector vRot;
	vRot = (*this)*mu + (axis^(*this))*lambda + axis*((axis*(*this))*muComp);
	return vRot;
}

/////////////NORMALIZE////////////////////////////////////
//
void
SpatialVector::normalize()
{
float64 sum;
   sum = x_*x_ + y_*y_ + z_*z_;
   sum = sqrt(sum);
   x_ /= sum;
   y_ /= sum;
   z_ /= sum;
}
void
SpatialVector::normalize(float64 r)
{
float64 sum, scale;
   sum = x_*x_ + y_*y_ + z_*z_;
   scale = r/sqrt(sum);
   x_ *= scale;
   y_ *= scale;
   z_ *= scale;
}

/////////////REVERSE//////////////////////////////////////
//
SpatialVector
SpatialVector::reverse(){
	return -1.0*(*this);
}

/////////////LENGTH///////////////////////////////////////
//
float64
SpatialVector::length() const
{
float64 sum;
   sum = x_*x_ + y_*y_ + z_*z_;
   return sum > gEpsilon ? sqrt(sum) : 0.0;
}

/////////////UPDATERADEC//////////////////////////////////
//
void
SpatialVector::updateRaDec() {
  dec_ = asin(z_)/gPr; // easy.
  float64 cd = cos(dec_*gPr);
  if(cd>gEpsilon || cd<-gEpsilon)
    if(y_>gEpsilon || y_<-gEpsilon)
      if (y_ < 0.0)
	ra_ = 360 - acos(x_/cd)/gPr;
      else
	ra_ = acos(x_/cd)/gPr;
    else
      ra_ = (x_ < 0.0 ? 180.0 : 0.0);
  else 
    ra_=0.0;
  okRaDec_ = true;
}

/////////////UPDATEXYZ////////////////////////////////////
//
void
SpatialVector::updateXYZ() {
    float64 cd = cos(dec_*gPr);
    x_ = cos(ra_*gPr) * cd;
    y_ = sin(ra_*gPr) * cd;
    z_ = sin(dec_*gPr);
}
/////////////OPERATOR *=//////////////////////////////////
//
SpatialVector&
SpatialVector::operator *=(float64 a)
{
  x_ = a*x_;  
  y_ = a*y_;  
  z_ = a*z_;
  okRaDec_ = false;
  return *this;
}

/////////////OPERATOR *=//////////////////////////////////
//
SpatialVector&
SpatialVector::operator *=(int a)
{
  x_ = a*x_;  
  y_ = a*y_;  
  z_ = a*z_;
  okRaDec_ = false;
  return *this;
}

/////////////OPERATOR *///////////////////////////////////
// Multiply with a number
//
SpatialVector
operator *(float64 a, const SpatialVector& v)
{
  return SpatialVector(a*v.x_, a*v.y_, a*v.z_);
}

/////////////OPERATOR *///////////////////////////////////
// Multiply with a number
//
SpatialVector
operator *(const SpatialVector& v, float64 a)
{
  return SpatialVector(a*v.x_, a*v.y_, a*v.z_);
}

/////////////OPERATOR *///////////////////////////////////
// Multiply with a number
//
SpatialVector
operator *(int a, const SpatialVector& v) 
{
  return SpatialVector(a*v.x_, a*v.y_, a*v.z_);
}

/////////////OPERATOR *///////////////////////////////////
// Multiply with a number
//
SpatialVector
operator *(const SpatialVector& v, int a)
{
  return SpatialVector(a*v.x_, a*v.y_, a*v.z_);
}


/////////////OPERATOR *///////////////////////////////////
// dot product
//
float64
SpatialVector::operator *(const SpatialVector & v) const
{
   return (x_*v.x_)+(y_*v.y_)+(z_*v.z_);
}

/////////////OPERATOR +///////////////////////////////////
//
SpatialVector
SpatialVector::operator +(const SpatialVector & v) const
{
  return SpatialVector(x_+v.x_, y_+v.y_, z_+v.z_);
}

/////////////OPERATOR -///////////////////////////////////
//
SpatialVector
SpatialVector::operator -(const SpatialVector & v) const
{
  return SpatialVector(x_-v.x_, y_-v.y_, z_-v.z_);
}

/////////////OPERATOR ^///////////////////////////////////
// cross product
//
SpatialVector
SpatialVector::operator ^(const SpatialVector &v) const
{
  return SpatialVector(y_ * v.z_ - v.y_ * z_,
		 z_ * v.x_ - v.z_ * x_,
		 x_ * v.y_ - v.x_ * y_);
}

/////////////OPERATOR ==//////////////////////////////////
//
int
SpatialVector::operator ==(const SpatialVector & v) const
{
  return ( (x_ == v.x_ && y_ == v.y_ && z_ == v.z_) ? 1 : 0 );
}

int equal_within_tolerance(const SpatialVector &a, const SpatialVector &b, float64 tolerance2) {
	SpatialVector c = b-a;
	float64 delta_dot = c*c;
  return delta_dot < tolerance2;
}

/////////////SHOW/////////////////////////////////////////
// print to stdout
//
void 
SpatialVector::show() const
{
  printf(" %11.8f %11.8f %11.8f \n",x_,y_,z_);
}

/** Copy to a string. Don't forget to clean up if you use. **/
// const char*
std::string
SpatialVector::toString()
{
	char *buffer = new char[40];
	// int n = sprintf(buffer,"%4.2e\n%4.2e\n%4.2e\n",x_,y_,z_);
	sprintf(buffer,"%4.2e\n%4.2e\n%4.2e\n",x_,y_,z_);
	return buffer;
}

/** Copy to an array. Don't forget to clean up if you use. **/
// float64*
std::vector<float64>
SpatialVector::toArray()
{
  // float64 *buffer = new float64[3];
  // buffer[0] = x_;
  // buffer[1] = y_;
  // buffer[2] = z_;
  
  std::vector<float64> buffer { x_, y_, z_ };
  return buffer;

}

/////////////READ/////////////////////////////////////////
// print to stdout
//
void 
SpatialVector::read(std::istream &in)
{
  in.setf(std::ios::skipws);
  in >> x_ >> y_ >> z_;
  if(!in.good())
    throw SpatialFailure("SpatialVector:read: Could not read vector");
}

/////////////WRITE////////////////////////////////////////
// print to stdout
//
void 
SpatialVector::write(std::ostream &out) const
{
  out << x_ << ' ' << y_ << ' ' << z_ ;
}




