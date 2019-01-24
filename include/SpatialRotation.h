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

	const float64 tol = 1.0e-14;

	const SpatialVector xhat = SpatialVector(1.0,0.0,0.0);
	const SpatialVector yhat = SpatialVector(0.0,1.0,0.0);
	const SpatialVector zhat = SpatialVector(0.0,0.0,1.0);

	SpatialVector body_xhat = xhat;
	SpatialVector body_yhat = yhat;
	SpatialVector body_zhat = zhat;

	// Rodrigues's formula
	SpatialVector axis        = zhat;
	float64       theta       = 0.0;
	float64       mu          = 1; /// cos(theta)
	float64       muComp      = 0; /// mu - 1
	float64       lambda      = 0; /// sin(theta)
	bool          matrix_flag = false;

	// Matrix form
	SpatialVector rotation_matrix_dual_x = SpatialVector(body_xhat.x(),body_yhat.x(),body_zhat.x());
	SpatialVector rotation_matrix_dual_y = SpatialVector(body_xhat.y(),body_yhat.y(),body_zhat.y());
	SpatialVector rotation_matrix_dual_z = SpatialVector(body_xhat.z(),body_yhat.z(),body_zhat.z());

	SpatialRotation(const SpatialVector axis, const float64 theta);
	SpatialRotation(const SpatialVector body_zhat, SpatialVector body_xhat);

	virtual ~SpatialRotation();

	SpatialVector rotated_from(const SpatialVector v);

};

void SpatialRotation_test();

#endif /* INCLUDE_SPATIALROTATION_H_ */
