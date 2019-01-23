/*
 * SpatialRotation.h
 *
 *  Created on: Jan 23, 2019
 *      Author: mrilee
 */

#include "SpatialVector.h"

#ifndef INCLUDE_SPATIALROTATION_H_
#define INCLUDE_SPATIALROTATION_H_

class SpatialRotation {
public:
	const SpatialVector axis;
	const float64       theta;

	float64 mu     ; /// cos(theta)
	float64 muComp ; /// mu - 1
	float64 lambda ; /// sin(theta)

	SpatialRotation(const SpatialVector axis, const float64 theta);
	virtual ~SpatialRotation();

	SpatialVector rotated_from(const SpatialVector v);

};

void SpatialRotation_test();

#endif /* INCLUDE_SPATIALROTATION_H_ */
