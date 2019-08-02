/*
 * SpatialVector_test.C
 *
 *  Created on: Jan 24, 2019
 *      Author: mrilee
 */

#include "Test.h"
#include "SpatialVector.h"

#define EXPAND_ASSERT_EQUALDM(str,v0,v1,tol) cout << str << " v0( " << v0 << " ) vs. v1( " << v1 << " ) " << tol << endl

void SpatialVector_test() {

	SpatialVector xhat(1,0,0);
	SpatialVector v(1,2,3);

	float64 a[] = {1,2,3};
	float64* b  = v.toArray();

	stringstream ss;

	for( int i=0; i<3; i++) {
		ss << "a==c[ " << i << " ] ";
		ASSERT_EQUALM(ss.str().c_str(),a[i],b[i]);
	}

	ASSERT_EQUAL(v,SpatialVector(v.toArray()));

	ASSERT_EQUAL(-1*xhat,xhat.reverse());
	ASSERT_EQUAL(SpatialVector(-0.5,1.0,-0.25),SpatialVector(0.5,-1.0,0.25).reverse());

}
