/*
 * SpatialPolygon_test.cpp
 *
 *  Created on: May 21, 2020
 *      Author: mrilee
 *
 *  Copyright (C) 2020 Rilee Systems Technologies LLC
 */

#include "SpatialPolygon.h"

using namespace std;

#include "Test.h"
#include "SpatialIndex.h"

#define hexOut1(a,b) cout << a << " 0x" << hex << setfill('0') << setw(16) << b << dec << endl << flush;

void SpatialPolygon_test() {

	// cout << "nodes" << endl << flush;
	Vertices nodes;
	nodes.push_back(SpatialVector(1,0,0));
	nodes.push_back(SpatialVector(0,0,1));
	nodes.push_back(SpatialVector(0,1,0));
	nodes.push_back(SpatialVector(1,0,0));

	// cout << "x and normalize" << endl << flush;
	SpatialVector x(0,1,1); x.normalize();

	// cout << "make polygon" << endl << flush;
	SpatialPolygon p(nodes);

	// cout << "execute intersection" << endl << flush;
	// cout << x << " x & p intersection: " << p.intersection(x) << endl << flush;
	ASSERT_EQUALM("011 in xyz",0,p.intersection(x));

	x = SpatialVector(0.5,0.5,0.5); x.normalize();
//	 cout << x << " x & p intersection: " << p.intersection(x) << endl << flush;
	ASSERT_EQUALM("111 in xyz",1,p.intersection(x));

	SpatialVector tr0 = SpatialVector(1,0,0); tr0.normalize();
	SpatialVector tr1 = SpatialVector(0,1,0); tr1.normalize();
	SpatialVector tr2 = SpatialVector(0,0,1); tr2.normalize();

	Triangle tr;
	tr.centroid = SpatialVector(0,0,0); // Not correct
	tr.vertices.clear();
	tr.vertices.push_back(tr0);
	tr.vertices.push_back(tr1);
	tr.vertices.push_back(tr2);
//	cout << "tr intersection " << p.intersect_triangle(tr) << endl << flush;
	ASSERT_EQUALM("identical",0,p.intersect_triangle(tr));

	tr0 = SpatialVector(1-0.1,1,1); tr0.normalize();
	tr.vertices[0] = tr0;
//	cout << "tr intersection " << p.intersect_triangle(tr) << endl << flush;
	ASSERT_EQUALM("tuck 1",3,p.intersect_triangle(tr));

	tr1 = SpatialVector(1,1-0.1,1); tr1.normalize();
	tr.vertices[1] = tr1;
//	cout << "tr intersection " << p.intersect_triangle(tr) << endl << flush;
	ASSERT_EQUALM("tuck 2",5,p.intersect_triangle(tr));

	tr2 = SpatialVector(1,1,1-0.1); tr2.normalize();
	tr.vertices[2] = tr2;
//	cout << "tr intersection " << p.intersect_triangle(tr) << endl << flush;
	ASSERT_EQUALM("tuck 3",6,p.intersect_triangle(tr));
}
