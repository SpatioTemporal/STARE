/*
 * SpatialInterface_test.cpp
 *
 *  Created on: Sep 2, 2019
 *      Author: mrilee
 *
 *  Copyright (C) 2019 Rilee Systems Technologies LLC
 */


#include <iostream>
#include "Test.h"

void SpatialInterface_test() {

	STARE index;

	{
		if(true) {
			htmInterface *htm;
			int resolution_level = 4;
			htm = new htmInterface(
					index.getIndex(resolution_level).getMaxlevel(),
					index.getIndex(resolution_level).getBuildLevel(),
					index.getIndex(resolution_level).getRotation());
			htm->polyCorners_.clear();
			SpatialVector v;
			v.setLatLonDegrees(  0,  0); htm->setPolyCorner(v);
			v.setLatLonDegrees(  0, 10); htm->setPolyCorner(v);
			v.setLatLonDegrees(  5, 10); htm->setPolyCorner(v);
			v.setLatLonDegrees( 10, 10); htm->setPolyCorner(v);
			v.setLatLonDegrees( 10,  0); htm->setPolyCorner(v);
			// htm->polyCorners_[0].c_
			// ASSERT_EQUALDM(msg,a,b,tol)
			v.setLatLonDegrees(  0,  0);ASSERT_EQUALDM("1polyC.0",htm->polyCorners_[0].c_,v,1.0e-12);
			v.setLatLonDegrees(  0, 10);ASSERT_EQUALDM("1polyC.1",htm->polyCorners_[1].c_,v,1.0e-12);
			v.setLatLonDegrees( 10, 10);ASSERT_EQUALDM("1polyC.2",htm->polyCorners_[2].c_,v,1.0e-12);
			v.setLatLonDegrees( 10,  0);ASSERT_EQUALDM("1polyC.3",htm->polyCorners_[3].c_,v,1.0e-12);
			delete htm;
		}

		if(true) {
			htmInterface *htm;
			int resolution_level = 4;
			htm = new htmInterface(
					index.getIndex(resolution_level).getMaxlevel(),
					index.getIndex(resolution_level).getBuildLevel(),
					index.getIndex(resolution_level).getRotation());
			htm->polyCorners_.clear();
			SpatialVector v;
			v.setLatLonDegrees(  0,  0); htm->setPolyCorner(v);
			v.setLatLonDegrees(  0, 10); htm->setPolyCorner(v);
			v.setLatLonDegrees( 10, 10); htm->setPolyCorner(v);
			v.setLatLonDegrees(  5, 10); htm->setPolyCorner(v);
			v.setLatLonDegrees( 10,  0); htm->setPolyCorner(v);
			// htm->polyCorners_[0].c_
			// ASSERT_EQUALDM(msg,a,b,tol)
			v.setLatLonDegrees(  0,  0);ASSERT_EQUALDM("2polyC.0",htm->polyCorners_[0].c_,v,1.0e-12);
			v.setLatLonDegrees(  0, 10);ASSERT_EQUALDM("2polyC.1",htm->polyCorners_[1].c_,v,1.0e-12);
			v.setLatLonDegrees( 10, 10);ASSERT_EQUALDM("2polyC.2",htm->polyCorners_[2].c_,v,1.0e-12);
			v.setLatLonDegrees( 10,  0);ASSERT_EQUALDM("2polyC.3",htm->polyCorners_[3].c_,v,1.0e-12);
			delete htm;
		}

		if(true) {
			htmInterface *htm;
			int resolution_level = 4;
			htm = new htmInterface(
					index.getIndex(resolution_level).getMaxlevel(),
					index.getIndex(resolution_level).getBuildLevel(),
					index.getIndex(resolution_level).getRotation());
			htm->polyCorners_.clear();
			SpatialVector v;
			v.setLatLonDegrees(  0,  0); htm->setPolyCorner(v);
			v.setLatLonDegrees(  5, 10); htm->setPolyCorner(v);
			v.setLatLonDegrees( 10,  0); htm->setPolyCorner(v);
			v.setLatLonDegrees(  0, 10); htm->setPolyCorner(v);
			v.setLatLonDegrees( 10, 10); htm->setPolyCorner(v);
			// htm->polyCorners_[0].c_
			// ASSERT_EQUALDM(msg,a,b,tol)
			v.setLatLonDegrees(  0,  0);ASSERT_EQUALDM("3polyC.0",htm->polyCorners_[0].c_,v,1.0e-12);
			v.setLatLonDegrees(  0, 10);ASSERT_EQUALDM("3polyC.1",htm->polyCorners_[1].c_,v,1.0e-12);
			v.setLatLonDegrees( 10, 10);ASSERT_EQUALDM("3polyC.2",htm->polyCorners_[2].c_,v,1.0e-12);
			v.setLatLonDegrees( 10,  0);ASSERT_EQUALDM("3polyC.3",htm->polyCorners_[3].c_,v,1.0e-12);
			delete htm;
		}

		if(true) {
			htmInterface *htm;
			int resolution_level = 4;
			htm = new htmInterface(
					index.getIndex(resolution_level).getMaxlevel(),
					index.getIndex(resolution_level).getBuildLevel(),
					index.getIndex(resolution_level).getRotation());
			htm->polyCorners_.clear();
			SpatialVector v;

//			 0 ( 30 -140)
//			 1 ( 30 -110)
//			 2 ( 45 -110)
//			 3 ( 45 -140)
//			 4 ( 37.5 -140)

			v.setLatLonDegrees(  30, -140); htm->setPolyCorner(v);
			v.setLatLonDegrees(  30, -110); htm->setPolyCorner(v);
			v.setLatLonDegrees(  45, -110); htm->setPolyCorner(v);
			v.setLatLonDegrees(  45, -140); htm->setPolyCorner(v);
			v.setLatLonDegrees( 37.5,-140); htm->setPolyCorner(v);
			// htm->polyCorners_[0].c_
			// ASSERT_EQUALDM(msg,a,b,tol)
			v.setLatLonDegrees(  30, -140);ASSERT_EQUALDM("4polyC.0",htm->polyCorners_[0].c_,v,1.0e-12);
			v.setLatLonDegrees(  30, -110);ASSERT_EQUALDM("4polyC.1",htm->polyCorners_[1].c_,v,1.0e-12);
			v.setLatLonDegrees(  45, -110);ASSERT_EQUALDM("4polyC.2",htm->polyCorners_[2].c_,v,1.0e-12);
			v.setLatLonDegrees(  45, -140);ASSERT_EQUALDM("4polyC.3",htm->polyCorners_[3].c_,v,1.0e-12);
			delete htm;
		}
	}
}
