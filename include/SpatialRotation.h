/*
 * SpatialRotation.h
 *
 *  Created on: Jan 23, 2019
 *      Author: mrilee
 */

#include "SpatialVector.h"
#include "SpatialException.h"

#ifndef INCLUDE_SPATIALROTATION_H_
#define INCLUDE_SPATIALROTATION_H_

class SpatialRotation {
public:

	// const -- requires specialized = operator
	float64 tol = 1.0e-14;

	SpatialVector body_xhat = SpatialVector(1,0,0);
	SpatialVector body_yhat = SpatialVector(0,1,0);
	SpatialVector body_zhat = SpatialVector(0,0,1);

	// Rodrigues's formula
	SpatialVector axis        = SpatialVector(0,0,1);
	float64       theta       = 0.0;
	float64       mu          = 1; /// cos(theta)
	float64       muComp      = 0; /// mu - 1
	float64       lambda      = 0; /// sin(theta)
	bool          matrix_flag = false;

	// Matrix form
	SpatialVector rotation_matrix_dual_x = SpatialVector(body_xhat.x(),body_yhat.x(),body_zhat.x());
	SpatialVector rotation_matrix_dual_y = SpatialVector(body_xhat.y(),body_yhat.y(),body_zhat.y());
	SpatialVector rotation_matrix_dual_z = SpatialVector(body_xhat.z(),body_yhat.z(),body_zhat.z());

	SpatialVector inverse_rotation_matrix_dual_x = SpatialVector(body_xhat);
	SpatialVector inverse_rotation_matrix_dual_y = SpatialVector(body_yhat);
	SpatialVector inverse_rotation_matrix_dual_z = SpatialVector(body_zhat);

	SpatialRotation() {};
	SpatialRotation(const SpatialVector axis, const float64 theta);
	SpatialRotation(const SpatialVector body_zhat, SpatialVector body_xhat);

	virtual ~SpatialRotation();

	SpatialVector rotated_from(const SpatialVector v);
	SpatialVector inverse_rotated_from(const SpatialVector v);

};

void SpatialRotation_test();
const SpatialRotation rot_identity = SpatialRotation(SpatialVector(0.0,0.0,1.0),0.0);

#endif /* INCLUDE_SPATIALROTATION_H_ */
