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
#include <iomanip>

using namespace std;

// namespace std {

SpatialPolygon::SpatialPolygon() {}
SpatialPolygon::~SpatialPolygon() {}

#define VEC(out,txt,v){out << txt << " [ " << v << " ] " << endl << flush;};

void SpatialPolygon::initialize() {

	edge_gcs.clear();
	left_gcs.clear();
	right_gcs.clear();

	SpatialVector edge_gc_sum(0,0,0), nbar(0,0,0);

	/**
	 * Nodes presumes polygon closed, i.e. first & last nodes explicitly the same.
	 */
	int ns = nodes.size();

	for( int j = 0; j < ns-1; ++j ) {
		SpatialVector edge_gc  = nodes[j]   ^ nodes[j+1]; // Assume all edges explicit.
		SpatialVector left_gc  = edge_gc    ^ nodes[j];
		SpatialVector right_gc = nodes[j+1] ^ edge_gc;

		edge_gc_sum = edge_gc_sum + edge_gc;
		nbar        = nbar + nodes[j];
// #define DIAG
#undef DIAG
#ifdef DIAG
		cout << j << " [" << nodes[j] << "]^[" << nodes[j+1] << "] [" << edge_gc << "] [" << left_gc << "] [" << right_gc << "]" << endl << flush;
#endif

		edge_gcs.push_back(edge_gc);
		left_gcs.push_back(left_gc);
		right_gcs.push_back(right_gc);
	}
	nbar = (1.0/(ns-1)) * nbar;

	// nodes_mean
	ray_dest = SpatialVector(0,0,0);
	for( int j = 0; j < ns-1; ++j ) {
		ray_dest = ray_dest + nodes[j];
	}
	ray_dest.normalize(); // Maybe check the validity of ray_dest.
	ccw_orientation = edge_gc_sum*nbar > 0;

#ifdef DIAG
	VEC(cout,"ray_dest:    ", ray_dest)
	VEC(cout,"edge_gc_sum: ", edge_gc_sum);
	VEC(cout,"nbar:        ", nbar);
	cout << "parity: " << ( edge_gc_sum*nbar > 0 ? 1 : -1 )<< endl << flush;
	cout << "ccw_orientation: " << ccw_orientation << endl << flush;
#endif

	bool ok = false;
	int j = 0;
	while( !ok ) {
		if( ray_dest == nodes[j] ) {
			SpatialRotation R = SpatialRotation(SpatialVector(1,0,0),0.05);
			ray_dest = R.rotated_from(ray_dest);
			j = 0; ok = false;
		} else {
			++ j;
			if( j == ns-1 ) {
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
#undef DIAG
// #define DIAG
#ifdef DIAG
	cout << setprecision(17);
	cout << "-----" << endl << flush;
#endif
	int cross_count = 0;
	SpatialVector ray = v ^ ray_dest; // ray.normalize();
#ifdef DIAG
	cout << "i:v: [" << v << "] r: [" << ray << "]" << " rd: [" << ray_dest << "]"<< endl << flush;
#endif
	for( uint j = 0; j < edge_gcs.size(); ++j ) {
		SpatialVector intersection = edge_gcs[j] ^ ray;
		// What if intersection is zero?
#ifdef DIAG
			cout << endl << flush;
			cout << "i:j,inter: " << j << " [" << intersection << "] " << endl << flush;
			cout << "i:j,e,left,right: "
					<< j
					<< " [" << edge_gcs[j] << "] "
					<< " [" << left_gcs[j] << "] "
					<< " [" << right_gcs[j] << "] "
					<< endl << flush;
			cout << "i:j,dots:  "
					<< (intersection * left_gcs[j])
					<< " "
					<<  (intersection * right_gcs[j])
					<< endl << flush;
#endif
			if( (intersection * left_gcs[j]) * (intersection * right_gcs[j]) > 0 ) {
#ifdef DIAG
				cout << "i: v.e: [" << v*edge_gcs[j] << "] " << endl << flush;
#endif
				float64 ve =  v * edge_gcs[j];
				 if( ve > 0) { // CCW, and sim below.
					cross_count += 1;
				} else if ( ve < 0 ) { // CCW
					cross_count -= 1;
				} // Note ve==0 case is significant.

//				if( ve < 0 ) { // CW
//					cross_count += 1;
//				} else if ( ve > 0 ) { // CW
//					cross_count -= 1;
//          	} // Note ve==0 case is significant.
//
			}
	}
#ifdef DIAG
	cout << endl << "i: cross_count " << cross_count << endl << flush;
#endif
	return cross_count > 0;
#undef DIAG
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
	/*
	if( included == 0 ) {
		if(triangle_crossp(tr)) {
			++included;
		}
	}
	*/
	return included;
}

bool SpatialPolygon::triangle_crossp(const Triangle& tr) {
	for( int i=0; i<3; ++i ) {
		if(segment_crossp(tr.vertices[i],tr.vertices[ i+1 < 3 ? i+1 : 0 ])) {
			return true;
		}
	}
	return false;
}

float64 cos_xy(float64 x, float64 y) {
	float64 sx = sqrt(1-(x*x));
	float64 sy = sqrt(1-(y*y));
	return x*y - sx*sy;
}

bool SpatialPolygon::segment_crossp(const SpatialVector& v0,const SpatialVector& v1) {
// #define DIAG
#undef DIAG
#ifdef DIAG
	cout << setprecision(17);
	cout << "-----" << endl << flush;
#endif
	SpatialVector ray = v0 ^ v1; // ray.normalize();
#ifdef DIAG
	cout << "i:v0: [" << v0 << "] r: [" << ray << "]" << " rd: [" << ray_dest << "]"<< endl << flush;
#endif
	for( uint j = 0; j < edge_gcs.size(); ++j ) {
		SpatialVector intersection = edge_gcs[j] ^ ray; intersection.normalize(); // Put onto the sphere.
		// What if intersection is zero?
#ifdef DIAG
			cout << endl << flush;
			cout << "i:j,inter: " << j << " [" << intersection << "] " << endl << flush;
			cout << "i:j,e,left,right: "
					<< j
					<< " [" << edge_gcs[j] << "] "
					<< " [" << left_gcs[j] << "] "
					<< " [" << right_gcs[j] << "] "
					<< endl << flush;
			cout << "i:j,dots:  "
					<< (intersection * left_gcs[j])
					<< " "
					<<  (intersection * right_gcs[j])
					<< endl << flush;
#endif

			float64 axab0 = (v0^intersection)*ray;
			float64 bxab0 = (v1^intersection)*ray;
			bool between0 = (axab0>0) && (bxab0<0);

			SpatialVector n0 = nodes[j];
			SpatialVector n1 = nodes[j+1];
			float64 axab1 = (n0^intersection)*edge_gcs[j];
			float64 bxab1 = (n1^intersection)*edge_gcs[j];
			bool between1 = (axab1>0) && (bxab1<0);

			if(between0 && between1) {
				return true;
			}

#if 0
			float64 t01 = acos((v0*v1)/(v0.length()*v1.length()));

			float64 t0i = acos((intersection*v0)/v0.length());
			float64 ti1 = acos((intersection*v1)/v1.length());
			bool t01p = abs(t01-t0i-ti1) < 1.0e-11;

			if(!t01p) {
				t0i = acos((-1*intersection*v0)/v0.length());
				ti1 = acos((-1*intersection*v1)/v1.length());
				t01p = abs(t01-t0i-ti1) < 1.0e-11;
			}
#endif
#if 0
			SpatialVector n0 = nodes[j];
			SpatialVector n1 = nodes[j+1];
			float64 t01 = n0*n1;

			float64 t0i = intersection*n0;
			float64 ti1 = intersection*n1;
			bool   t01p = abs(t01-cos_xy(t0i,ti1)) < 1.0e-13;

			if(!t01p) {
				t01p = abs(t01-cos_xy(-t0i,-ti1)) < 1.0e-13;
			}

			if(t01p) {
				return true;
			}
#endif
	}
	return false;
// #undef DIAG
}
// } /* namespace std */
