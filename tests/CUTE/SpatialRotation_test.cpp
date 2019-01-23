/*
 * SpatialRotation_test.cpp
 *
 *  Created on: Jan 23, 2019
 *      Author: mrilee
 */

#include "Test.h"
#include "SpatialRotation.h"

void SpatialRotation_test() {
	SpatialVector xhat(1.0,0.0,0.0);
	SpatialVector yhat(0.0,1.0,0.0);
	SpatialVector zhat(0.0,0.0,1.0);
	SpatialVector result;

	SpatialRotation x_to_x(xhat,(float64)0.0);
	SpatialRotation y_to_z(xhat,  gPio2);
	SpatialRotation y_to_mz(xhat,-gPio2);
	float64 tol = 1.0e-12;

	ASSERT_EQUALDM("Identity",xhat,x_to_x.rotated_from(xhat),tol);
	ASSERT_EQUALDM("y to  z",       zhat, y_to_z.rotated_from(yhat),tol);
	ASSERT_EQUALDM("y to -z",  -1.0*zhat,y_to_mz.rotated_from(yhat),tol);

}
