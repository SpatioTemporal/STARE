/*
 * SpatialGeneral.C
 *
 *  Created on: Jan 26, 2016
 *      Author: mrilee
 */

#include <vector>
#include "SpatialGeneral.h"

// float64* xyzFromLatLonRadians(float64 lat,float64 lon) {
std::vector<float64> xyzFromLatLonRadians(float64 lat,float64 lon) {
	// float64 *ret = new float64[3];
	// ret[0] = cos(lat)*cos(lon);
	// ret[1] = cos(lat)*sin(lon);
	// ret[2] = sin(lat);
	// return ret;
  std::vector<float64> ret { cos(lat)*cos(lon), cos(lat)*sin(lon), sin(lat) };
  return ret;
}

// float64* xyzFromLatLonDegrees(float64 lat,float64 lon) {
std::vector<float64> xyzFromLatLonDegrees(float64 lat,float64 lon) {
//	float64 piDiv180 = 4*atan(1.0)/180.;
	return xyzFromLatLonRadians(lat*piDiv180,lon*piDiv180);
}

// http://graphics.stanford.edu/~seander/bithacks.html#IntegerLog
// Find the log base 2 of an N-bit integer in O(lg(N)) operations
unsigned long
lg2(unsigned long v) {  // 64-bit value to find the log2 of
	const unsigned long b[] = {0x2, 0xC, 0xF0, 0xFF00, 0xFFFF0000, 0xFFFFFFFF00000000}; // Set up for 64 bits.
	const unsigned long S[] = {1, 2, 4, 8, 16, 32};
	int i;

	register unsigned long r = 0; // result of log2(v) will go here
	for (i = 5; i >= 0; i--) // unroll for speed...
	{
		if (v & b[i])
		{
			v >>= S[i];
			r |= S[i];
		}
	}
	return r;
}
