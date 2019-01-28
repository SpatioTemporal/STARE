/*
 * SpatialRotation.C
 *
 *  Created on: Jan 23, 2019
 *      Author: mrilee
 */

#include "SpatialRotation.h"

SpatialRotation::SpatialRotation(const SpatialVector axis, const float64 theta)
: axis(axis), theta(theta){
	// Use Rodrigues's formula
	mu     = cos(theta);
	// muComp = mu - 1.0; // error? Wikipedia says -- sign error.
	muComp = 1.0 - mu;
	lambda = sin(theta);
	matrix_flag = false;
}

SpatialRotation::SpatialRotation(const SpatialVector body_zhat, const SpatialVector body_xhat) {
	if (abs(body_zhat*body_xhat) > tol) {
		throw SpatialFailure("Body x and z axes are not orthogonal.");
	}
	if ( (abs(body_zhat.length()-1) > tol) ||
		 (abs(body_xhat.length()-1) > tol) ) {
		throw SpatialFailure("One of body x and z are not unit vectors.");
	}
	SpatialVector body_yhat = body_zhat^body_xhat;
	rotation_matrix_dual_x = SpatialVector(body_xhat.x(),body_yhat.x(),body_zhat.x());
	rotation_matrix_dual_y = SpatialVector(body_xhat.y(),body_yhat.y(),body_zhat.y());
	rotation_matrix_dual_z = SpatialVector(body_xhat.z(),body_yhat.z(),body_zhat.z());
	matrix_flag = true;
}

SpatialRotation::~SpatialRotation() {
	// TODO Auto-generated destructor stub
}

SpatialVector SpatialRotation::rotated_from(const SpatialVector v) {
	// TODO Special treatment for identity.

	SpatialVector vRot;

	if (matrix_flag) {
		// Matrix
		vRot.set(
				rotation_matrix_dual_x*v,
				rotation_matrix_dual_y*v,
				rotation_matrix_dual_z*v
				);
	} else {
		// Use Rodrigues's formula
		vRot = (v)*mu + (axis^(v))*lambda + axis*((axis*(v))*muComp);
		// vRot = (*this)*mu + (axis^(*this))*lambda + axis*((axis*(*this))*muComp);
	}
	return vRot;
}

