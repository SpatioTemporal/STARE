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

		if( true ) {

			// cout << endl << "----" << endl << endl << endl << flush;

			// Try a grid...
			int nLat = 11,nLon = 11, nLatLon=nLat*nLon;
			float64 lat[nLat], lon[nLon];
			SpatialVector vs[nLatLon], cs[nLatLon], vs1[nLatLon];
			for( int i = 0; i < nLon; ++i ) {
				lon[i] = i*5.0;
			}
			for( int j = 0; j < nLat; ++j ) {
				lat[j] = j*5.0;
			}
			int k = 0;
			for( int i = 0; i < nLon; ++i ) {
				for( int j = 0; j < nLat; ++j ) {
					vs[k++].setLatLonDegrees(lat[j], lon[i]);
				}
			}

			STARE index;
			htmInterface *htm;
			int resolution_level = 4;
			htm = new htmInterface(
					index.getIndex(resolution_level).getMaxlevel(),
					index.getIndex(resolution_level).getBuildLevel(),
					index.getIndex(resolution_level).getRotation());
			htm->polyCorners_.clear();
			SpatialVector v;

			k = 0;
			for( int i=0; i<nLon; ++i ) {
				for( int j=0; j<nLat; ++j ) {
					htm->setPolyCorner(vs[k]);
					// cout << k << " k, hpc size " << htm->polyCorners_.size() << endl << flush;
					float64 lat_,lon_; vs[k].getLatLonDegrees(lat_,lon_);
					// cout << k << " k, try "
					// << lat_ << " " << lon_	<< endl << flush;
					for(int l=0; l<htm->polyCorners_.size(); ++l) {
						htm->polyCorners_[l].c_.getLatLonDegrees(lat_,lon_);
						// cout << l << " l, hpc "
						//		<< lat_ << " " << lon_	<< endl << flush;
					}
					// cout << "-----" << endl << flush;
					++k;
					// if(k>5) exit(1);
				}
			}

			delete htm;
            
		}
		
		if(true) {
            htmInterface *htm;
            int resolution_level = 16;
            htm = new htmInterface(index.getIndex(resolution_level).getMaxlevel(),
                                   index.getIndex(resolution_level).getBuildLevel(),
                                   index.getIndex(resolution_level).getRotation());
            LatLonDegrees64ValueVector cornerVector;
            cornerVector.push_back(LatLonDegrees64(4.0003, 0.0017));
            cornerVector.push_back(LatLonDegrees64(4.0003, 0.0011));
            cornerVector.push_back(LatLonDegrees64(4.0003, 0.0003));
            cornerVector.push_back(LatLonDegrees64(4.0003, 0.0008));           
            int hullSteps = cornerVector.size();          
            HTMRangeValueVector r = htm->convexHull(cornerVector, hullSteps, true);
            delete htm;
        }
		if(true) {
            htmInterface *htm;
            int resolution_level = 16;
            htm = new htmInterface(index.getIndex(resolution_level).getMaxlevel(),
                                   index.getIndex(resolution_level).getBuildLevel(),
                                   index.getIndex(resolution_level).getRotation());
            LatLonDegrees64ValueVector cornerVector;
            cornerVector.push_back(LatLonDegrees64(4.0003,0.0017));
            cornerVector.push_back(LatLonDegrees64(4.0003,0.0011));
            cornerVector.push_back(LatLonDegrees64(4.0006,0.0011));
            cornerVector.push_back(LatLonDegrees64(4.0006,0.0003));
            cornerVector.push_back(LatLonDegrees64(4.0003,0.0003));
            cornerVector.push_back(LatLonDegrees64(4.0003,0.0008));
            cornerVector.push_back(LatLonDegrees64(4.0,   0.0008));
            cornerVector.push_back(LatLonDegrees64(4.0,   0.0));
            cornerVector.push_back(LatLonDegrees64(4.0003,0.0));
            cornerVector.push_back(LatLonDegrees64(4.003, 0.0007));
            int hullSteps = cornerVector.size();
            HTMRangeValueVector r = htm->convexHull(cornerVector, hullSteps, true);

            /*
            0 i: r[i]: 3d6feb7250000010 4427015834929463312
            1 i: r[i]: 3d6feb7320000010 4427015838419124240
            2 i: r[i]: 3d6feb7338000010 4427015838821777424
            3 i: r[i]: 3d6feb73a0000010 4427015840566607888
            4 i: r[i]: 3d6feb73a8000010 4427015840700825616
            5 i: r[i]: 3d6feb73b8000010 4427015840969261072
            6 i: r[i]: 3d6feb73e0000010 4427015841640349712
            cout << "----------" << endl << flush;
            cout << "r size: " << r.size() << endl << flush;
            */

            uint64 expected_lo[7] = {
            		0x3d6feb7250000010
					,0x3d6feb7320000010
					,0x3d6feb7338000010
					,0x3d6feb73a0000010
					,0x3d6feb73a8000010
					,0x3d6feb73b8000010
					,0x3d6feb73e0000010
            };

            for(int i = 0; i < r.size(); ++i ) {
            	uint64 lo = index.ValueFromHtmID(r[i].lo);
            	// uint64 hi = index.ValueFromHtmID(r[i].hi);
            	// cout << i << " i: r[i]: " << hex << lo << " " << dec << lo << endl << flush;
            	// cout << i << " i: r[i]: " << hex << hi << " " << dec << hi << endl << flush;
				ASSERT_EQUAL(expected_lo[i],lo);
            }
            // cout << "----------" << endl << flush;
            delete htm;
            // exit(1);
		}
	}
}
