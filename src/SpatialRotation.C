/*
 * SpatialRotation.C
 *
 *  Created on: Jan 23, 2019
 *      Author: mrilee
 */

#include "SpatialRotation.h"
#include "SpatialVector.h"

SpatialRotation::SpatialRotation(const SpatialVector axis, const float64 theta)
: axis(axis), theta(theta){
	// Use Rodrigues's formula
	mu     = cos(theta);
	muComp = mu - 1.0;
	lambda = sin(theta);
}

SpatialRotation::~SpatialRotation() {
	// TODO Auto-generated destructor stub
}

SpatialVector SpatialRotation::rotated_from(const SpatialVector v) {
	// TODO Special treatment for identity.
	// Use Rodrigues's formula
	SpatialVector vRot;
	vRot = (v)*mu + (axis^(v))*lambda + axis*((axis*(v))*muComp);
	return vRot;
}

