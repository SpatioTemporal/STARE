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

	SpatialVector
		xhat(1.0,0.0,0.0),
		yhat(0.0,1.0,0.0),
		zhat(0.0,0.0,1.0);

	// cout << setprecision(17);

	int maxlevel   = 5;
	int buildlevel = 5;

	// cout << 100 << endl << flush;

	// SpatialVector zhat_r = -1.0*zhat;
	SpatialVector zhat_r = zhat.reverse();

	ASSERT_EQUAL("N010000",string(SpatialIndex(maxlevel,buildlevel,SpatialRotation(xhat,0)).nameByPoint(zhat)).c_str());
	ASSERT_EQUAL("S010000",string(SpatialIndex(maxlevel,buildlevel,SpatialRotation(xhat,0)).nameByPoint(zhat_r)).c_str());

	ASSERT_EQUAL("S010000",string(SpatialIndex(maxlevel,buildlevel,SpatialRotation(xhat,gPi)).nameByPoint(zhat)).c_str());
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

	// cout << 200 << endl << flush;

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

	// cout << 300 << endl << flush;

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

	// cout << 400 << endl << flush;

	uint64 htm_Id_0h = index_0.idByPoint(v);

	// cout << 500 << endl << flush;

	// cout << "hi0h:" << htm_Id_0h << endl << flush;
	SpatialVector vch; index_0.pointByHtmId(vch, htm_Id_0h);
	// cout << "vch: " << vch << endl << flush;

	// cout << 600 << endl << flush;

	// cout << "delta vch - v:   " << (vch-v) << endl << flush;
	// cout << "|vch - v|:       " << (vch-v).length() << endl << flush;
	// cout << "|vch - vch_rr|:  " << (vch-rot_i.rotated_from(rot.rotated_from(vch))) << endl << flush;

	// cout << 700 << endl << flush;

	htm_Id_rot = index_rot.idByPoint(vch);
	// cout << "hir: " << htm_Id_rot << endl << flush;

	// cout << 800 << endl << flush;

	SpatialVector uch;
	index_0.pointByHtmId(uch, htm_Id_rot);
	// cout << "uch: " << uch << endl << flush;
	// cout << "ucr: " << rot1.rotated_from(uch) << endl << flush;
	// cout << "delta vch-uch: " << (vch-uch) << endl << flush;

	// cout << 900 << endl << flush;

	// The triangles don't line up exactly for arbitrary rotations, so
	// Let's see if we get to within a fraction of a triangle.
	const float64 tol = 0.5*(vch-v).length();
	ASSERT_EQUALDM("Feed rotated indice to unrotated index.",vch,rot1.rotated_from(uch),tol);

	// cout << 1000 << endl << flush;


	// cout << "ucri:" << rot_i.rotated_from(uch) << endl << flush;


	/*
	LEVEL ZERO BUG 2019-0325

	cout << 1000 << "kp.lo: " << hex << kp.lo << dec << endl;
	cout << 1000 << "kp.hi: " << hex << kp.hi << dec << endl;
	leftJustified.setId(kp.lo);
	cout << 1001 << endl;
	level = leftJustified.getLevel();
	cout << 1002 << "level: " << level << endl;
	string loName = leftJustified.getName();
	cout << 1003 << "loName: " << loName << endl;
	uint64 termId = leftJustified.idFromTerminatorAndLevel_NoDepthBit(kp.hi,level);
	cout << 1004 << "termId: " << hex << termId << dec << endl << flush;
	leftJustified.setId(termId);

	cout << 1005 << endl << flush;
	string hiName = leftJustified.getName();


	 latlon = -34,10, idx0: 1c0508ca216dd580, idx: 1c0508ca216dd584, idx_gfx: b80a119442dbab00
130
1000kp.lo: 380a119442dbab00
1000kp.hi: 3fffffffffffffff
1001
1002level: 0
1003loName: S3
1004termId: 3000000000000000
1005
1100 lo,hi name: S3 S3
1101 level:      0
1102 kp:         380a119442dbab00 3fffffffffffffff
140
1000kp.lo: 38029883332414c4
1000kp.hi: 380fffffffffffff
1001
1002level: 4
1003loName: S32000
1004termId: 3800000000000004
1005
1100 lo,hi name: S32000 S32000
1101 level:      4
1102 kp:         38029883332414c4 380fffffffffffff
120
 latlon = -32,10, idx0: 1c014c4199920a60, idx: 1c014c4199920a64, idx_gfx: b8029883332414c0
130
1000kp.lo: 38029883332414c0
1000kp.hi: 3fffffffffffffff
1001
1002level: 0
1003loName: S3
1004termId: 3000000000000000
1005
1100 lo,hi name: S3 S3
1101 level:      0
1102 kp:         38029883332414c0 3fffffffffffffff
140
1000kp.lo: a2901367184
1000kp.hi: fffffffffffff
1001
1002level: 4
1003loName: S00000
1004termId: 4
1005
1100 lo,hi name: S00000 S00000
1101 level:      4
1102 kp:         a2901367184 fffffffffffff
120
 latlon = -30,10, idx0: 514809b38c0, idx: 514809b38c4, idx_gfx: 80000a2901367180
130
1000kp.lo: a2901367180
1000kp.hi: fffffffffffffff
1001
1002level: 0
1003loName: S0
1004termId: 0
terminate called after throwing an instance of 'SpatialFailure'


	*/

	// FAIL();
}




