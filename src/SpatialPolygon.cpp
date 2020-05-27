/*
 * SpatialPolygon.cpp
 *
 *  Created on: May 21, 2020
 *      Author: mrilee
 *
 *  Copyright (C) 2020 Rilee Systems Technologies LLC
 */

#include "SpatialPolygon.h"
#include "SpatialVector.h"
#include "SpatialRotation.h"

using namespace std;

// namespace std {

SpatialPolygon::SpatialPolygon() {}
SpatialPolygon::~SpatialPolygon() {}

void SpatialPolygon::initialize() {

	edge_gcs.clear();
	left_gcs.clear();
	right_gcs.clear();

	int ns = nodes.size();
	for( int j = 0; j < ns-1; ++j ) {
		// SpatialVector edge_gc  = nodes[j]   ^ nodes[j+1 < ns ? j+1 : 0]; // Assume points are well ordered.
		SpatialVector edge_gc  = nodes[j]   ^ nodes[j+1]; // Assume all edges explicit. size of nodes should be even.
		SpatialVector left_gc  = edge_gc    ^ nodes[j];
		SpatialVector right_gc = nodes[j+1] ^ edge_gc;

		// cout << j << " [" << nodes[j] << "]^[" << nodes[j+1] << "] [" << edge_gc << "] [" << left_gc << "] [" << right_gc << "]" << endl << flush;

		edge_gcs.push_back(edge_gc);
		left_gcs.push_back(left_gc);
		right_gcs.push_back(right_gc);
	}

	ray_dest = SpatialVector(0,0,0);
	for( int j = 0; j < ns; ++j ) {
		ray_dest = ray_dest + nodes[j];
	}
	ray_dest.normalize(); // Maybe check the validity of ray_dest.
	ray_dest = -1.0*ray_dest; // Be cute.

	bool ok = false;
	int j = 0;
	while( !ok ) {
		if( ray_dest == nodes[j] ) {
			SpatialRotation R = SpatialRotation(SpatialVector(1,0,0),0.05);
			ray_dest = R.rotated_from(ray_dest);
			j = 0; ok = false;
		} else {
			++ j;
			if( j == ns ) {
				ok = true;
			}
		}
	}
	// cout << "r " << ray_dest << endl << flush;
}

bool SpatialPolygon::intersection(const SpatialVector& v ) {
	if( v == ray_dest ) {
		SpatialRotation R = SpatialRotation(SpatialVector(1,0,0),0.1);
		ray_dest = R.rotated_from(ray_dest);
	}
	int cross_count = 0;
	SpatialVector ray = v ^ ray_dest; // ray.normalize();
//	cout << "i:v: [" << v << "] r: [" << ray << "]" << endl << flush;
	for( int j = 0; j < edge_gcs.size(); ++j ) {
		SpatialVector intersection = edge_gcs[j] ^ ray;
//		if( intersection.length() > 1.0e-14 ) {
//			intersection.normalize();
//			cout << "i:j,inter: " << j << " [" << intersection << "] " << endl << flush;
			if( (intersection * left_gcs[j]) * (intersection * right_gcs[j]) > 0 ) {
//				cout << "i: v.e: [" << v*edge_gcs[j] << "] " << endl << flush;
//				if( v * edge_gcs[j] > 0 ) { // Which way round the polygon?
				if( v * edge_gcs[j] < 0 ) {
					cross_count += 1;
				} else {
					cross_count -= 1;
				}
			}
//		}
	}
	return cross_count != 0;
}

int SpatialPolygon::intersect_triangle( const Triangle& tr ) {
	// Maybe do something that actually solves something...
	// E.g. triangle-triangle, segment-triangle intersection?
	int included = 0;
	for( int i=0; i<3; ++i ) {
		if( intersection( tr.vertices[i] ) ) {
			++included;
		}
	}
	for( int i=0; i<3; ++i ) {
		SpatialVector a = tr.vertices[i] + tr.vertices[ i+1 < 3 ? i+1 : 0 ]; a.normalize();
		if( intersection( a ) ) {
			++included;
		}
	}
	if( intersection( tr.centroid ) ) {
		++included;
	}
	return included;
}


// } /* namespace std */
