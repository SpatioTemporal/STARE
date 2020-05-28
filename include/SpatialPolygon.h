/*
 * SpatialPolygon.h
 *
 *  Created on: May 21, 2020
 *      Author: mrilee
 *
 *  Copyright (C) 2020 Rilee Systems Technologies LLC
 */

#include "SpatialVector.h"

#ifndef INCLUDE_SPATIALPOLYGON_H_
#define INCLUDE_SPATIALPOLYGON_H_

// namespace std {

class SpatialPolygon {
public:
	SpatialPolygon();
	SpatialPolygon(const Vertices& nodes) { this->nodes = nodes; initialize(); }
	virtual ~SpatialPolygon();

	void push_back(const SpatialVector& v) { nodes.push_back(v); }

	Vertices nodes;
	Vertices edge_gcs,left_gcs,right_gcs;

	SpatialVector ray_dest;

	void initialize();
	bool intersection(const SpatialVector& v );
	int intersect_triangle( const Triangle& tr );

	void update();

};

void SpatialPolygon_test();

// } /* namespace std */

#endif /* INCLUDE_SPATIALPOLYGON_H_ */
