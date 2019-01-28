/*
 * SpatialIndex_test.cpp
 *
 *  Created on: Jan 24, 2019
 *      Author: mrilee
 */

#include "Test.h"
#include "SpatialIndex.h"

#define hexOut1(a,b) cout << a << " 0x" << hex << setfill('0') << setw(16) << b << dec << endl << flush;

void SpatialIndex_test() {

	cout << setprecision(17);

	int maxlevel   = 5;
	int buildlevel = 5;

	ASSERT_EQUAL("S010000",string(SpatialIndex(maxlevel,buildlevel,SpatialRotation(xhat,gPi)).nameByPoint(zhat)).c_str());
	// SpatialVector zhat_r = -1.0*zhat;
	SpatialVector zhat_r = zhat.reverse();
	ASSERT_EQUAL("N010000",string(SpatialIndex(maxlevel,buildlevel,SpatialRotation(xhat,gPi)).nameByPoint(zhat_r)).c_str());
	// TODO Figure out how to have the following work. Probably by littering the call sequence with const.
	// ASSERT_EQUAL("N010000",string(SpatialIndex(maxlevel,buildlevel,SpatialRotation(xhat,gPi)).nameByPoint(zhat.reverse())).c_str());

	SpatialVector axis = (0.5*( xhat + yhat )); axis.normalize();
	// float64 theta = 0.0;
	// float64 theta = 1.0e-6*gPi;
	float64 theta = 0.5*gPi;
	// float64 theta = gPi;
	// float64 theta = 2.0*gPi;

	SpatialRotation rot(axis,theta);
	SpatialRotation rot_i(axis,-theta);

	SpatialIndex index_rot(maxlevel,buildlevel,rot);
	SpatialIndex index_0(maxlevel,buildlevel);

	SpatialVector v; v.setLatLonDegrees(90,0);
	uint64 htm_Id_rot = index_rot.idByPoint(v);
	// uint64 htm_Id_0   = index_0.idByPoint(v);

	BitShiftNameEncoding rightJustified(htm_Id_rot);

	/*
	cout << "v:           " << v << endl;
	cout << "v.rot:       " << rot.rotated_from(v) << endl;
	cout << "axis:        " << axis << endl;
	cout << "axis.rot:    " << rot.rotated_from(axis) << endl;

	hexOut1("htm_Id_rot",htm_Id_rot);
	hexOut1("htm_Id_0  ",htm_Id_0);

	cout << "rot name: " << index_rot.nameByPoint(v) << endl;
	cout << "rot 0   : " << index_0  .nameByPoint(v) << endl;
	*/

	// ASSERT_EQUALM("Rotate 180 around x, check z.","S010000",index_rot.nameByPoint(v));;


	// Take an arbitrary vector pointing to the centroid of a triangle and an arbitrarily rotated index.
	// Determine the (rotated) indice, then determine the vector found by plugging the rotated indice
	// into an unrotated index.
	//
	axis  = 0.1*xhat + 0.4*yhat; axis.normalize();
	theta = 0.0;
	SpatialRotation rot1  (axis,theta);
	SpatialRotation rot1_i(axis,-theta);

	// Reset the SpatialIndex
	maxlevel   = 27;
	buildlevel = 5;
	index_rot = SpatialIndex(maxlevel,buildlevel,rot1);
	index_0   = SpatialIndex(maxlevel,buildlevel);

	// Start at a particular latlon, then shift to the center of a triangle
	// float64 lat = 45, lon = 45;
	float64 lat = 30, lon = 60;
	v.setLatLonDegrees(lat, lon);
	// cout << "v:   " << v << endl << flush;

	uint64 htm_Id_0h = index_0.idByPoint(v);
	// cout << "hi0h:" << htm_Id_0h << endl << flush;
	SpatialVector vch; index_0.pointByHtmId(vch, htm_Id_0h);
	// cout << "vch: " << vch << endl << flush;

	// cout << "delta vch - v:   " << (vch-v) << endl << flush;
	// cout << "|vch - v|:       " << (vch-v).length() << endl << flush;
	// cout << "|vch - vch_rr|:  " << (vch-rot_i.rotated_from(rot.rotated_from(vch))) << endl << flush;

	htm_Id_rot = index_rot.idByPoint(vch);
	// cout << "hir: " << htm_Id_rot << endl << flush;

	SpatialVector uch;
	index_0.pointByHtmId(uch, htm_Id_rot);
	// cout << "uch: " << uch << endl << flush;
	// cout << "ucr: " << rot1.rotated_from(uch) << endl << flush;
	// cout << "delta vch-uch: " << (vch-uch) << endl << flush;

	// The triangles don't line up exactly for arbitrary rotations, so
	// Let's see if we get to within a fraction of a triangle.
	const float64 tol = 0.5*(vch-v).length();
	ASSERT_EQUALDM("Feed rotated indice to unrotated index.",vch,rot1.rotated_from(uch),tol);

	// cout << "ucri:" << rot_i.rotated_from(uch) << endl << flush;

	// FAIL();
}




