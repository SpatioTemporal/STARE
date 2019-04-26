/*
 * SpatialRotation_test.cpp
 *
 *  Created on: Jan 23, 2019
 *      Author: mrilee
 */

#include "Test.h"
#include "SpatialRotation.h"

#define EXPAND_ASSERT_EQUALDM(str,v0,v1,tol) cout << str << " v0( " << v0 << " ) vs. v1( " << v1 << " ) " << tol << endl

void SpatialRotation_test() {
	const SpatialVector xhat(1.0,0.0,0.0);
	const SpatialVector yhat(0.0,1.0,0.0);
	const SpatialVector zhat(0.0,0.0,1.0);
	SpatialVector result;

	SpatialRotation x_to_x(xhat,0.0);
	SpatialRotation y_to_z(xhat,  gPio2);
	SpatialRotation y_to_mz(xhat,-gPio2);

	const float64 tol = 1.0e-14;

	ASSERT_EQUALDM("Identity",xhat,x_to_x.rotated_from(xhat),tol);
	ASSERT_EQUALDM("y to  z",       zhat, y_to_z.rotated_from(yhat),tol);
	ASSERT_EQUALDM("y to -z",  -1.0*zhat,y_to_mz.rotated_from(yhat),tol);

	ASSERT_EQUALDM("Identity, matrix",xhat,SpatialRotation(zhat,xhat).rotated_from(xhat),tol);
	// cout << "vecs: (" << zhat << ") == (" << SpatialRotation(-1*yhat,xhat).rotated_from(yhat) << ")" << endl;
	ASSERT_EQUALDM("z to -y, rotate y, matrix",zhat,SpatialRotation(-1*yhat,xhat).rotated_from(yhat),tol)
	ASSERT_EQUALDM("z to y, rotate y, matrix",-1*zhat,SpatialRotation(yhat,xhat).rotated_from(yhat),tol)

	/*
	cout << "vecs: "
	<< "( " << SpatialRotation(yhat,0.25*gPi).rotated_from(xhat) << " ) "
	<< "( " << SpatialRotation(
			SpatialRotation(yhat,0.25*gPi).rotated_from(zhat),
			SpatialRotation(yhat,0.25*gPi).rotated_from(xhat)
	).rotated_from(xhat) << " ) "
	<< endl;
	*/

	SpatialRotation roty45   = SpatialRotation(yhat,0.25*gPi);
	SpatialRotation roty45_m = SpatialRotation(
			roty45.rotated_from(zhat),
			roty45.rotated_from(xhat));

	ASSERT_EQUALDM("roty45 vs _m",roty45.rotated_from(xhat),roty45_m.rotated_from(xhat),tol);

	SpatialVector   axis0 = 0.5*xhat + 0.5*yhat; axis0.normalize();
	SpatialRotation rot0(axis0,gPio2);
	SpatialRotation rot0_m(rot0.rotated_from(zhat),rot0.rotated_from(xhat));

	ASSERT_EQUALDM("rot0 vs _m",rot0.rotated_from(xhat),rot0_m.rotated_from(xhat),tol);

	ASSERT_EQUALDM("rot zhat by pi around xhat: ",-1.0*zhat,SpatialRotation(xhat,gPi).rotated_from(zhat),tol);

	ASSERT_EQUALDM("rot xhat by pi around xhat: ",xhat,SpatialRotation(xhat,gPi).rotated_from(xhat),tol);

	SpatialVector nhat = 0.5*(zhat+yhat); nhat.normalize();
	SpatialRotation rot(nhat,xhat);

	/*
	cout << "nhat:              " << nhat << endl;
	cout << "yhat:              " << yhat << endl;
	cout << "rot.yhat:          " << rot.rotated_from(yhat) << endl;
	cout << "inv_rot(rot.yhat): " << rot.inverse_rotated_from(rot.rotated_from(yhat)) << endl;
	*/

	ASSERT_EQUALDM("inverse rotation, matrix 1: ",yhat,rot.inverse_rotated_from(rot.rotated_from(yhat)),tol);
	ASSERT_EQUALDM("inverse rotation, matrix 2: ",zhat,rot.inverse_rotated_from(rot.rotated_from(zhat)),tol);
	ASSERT_EQUALDM("inverse rotation, matrix 3: ",xhat,rot.inverse_rotated_from(rot.rotated_from(xhat)),tol);

	rot = SpatialRotation(nhat,0.25*gPi);
	ASSERT_EQUALDM("inverse rotation, Rodrigues 1: ",yhat,rot.inverse_rotated_from(rot.rotated_from(yhat)),tol);
	ASSERT_EQUALDM("inverse rotation, Rodrigues 2: ",zhat,rot.inverse_rotated_from(rot.rotated_from(zhat)),tol);
	ASSERT_EQUALDM("inverse rotation, Rodrigues 3: ",xhat,rot.inverse_rotated_from(rot.rotated_from(xhat)),tol);

	// TODO Test the two rotation methods against each other.

}
