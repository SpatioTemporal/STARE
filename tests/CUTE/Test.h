/*
 * Test.h
 *
 *  Created on: Jan 23, 2019
 *      Author: mrilee
 */

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

#include "cute.h"
#include "ide_listener.h"
#include "xml_listener.h"
#include "cute_runner.h"

#include "HstmIndex.h"

// TODO #include the headers for the code you want to test
//#include "SpatialGeneral.h"
#include "SpatialException.h"
#include "SpatialIndex.h"
#include "TemporalIndex.h"
#include "SpatialVector.h"
#include "SpatialInterface.h"
#include "SpatialRange.h"
#include "SpatialRotation.h"

#include "SpatiallyAdaptiveDataCover.h"

#include "HtmRangeIterator.h"
#include "BitShiftNameEncoding.h"
#include "EmbeddedLevelNameEncoding.h"

#include "HtmRangeMultiLevel.h"
#include "HtmRangeMultiLevelIterator.h"

#include "HstmRange.h"

#include "STARE.h"


#ifndef TESTS_CUTE_TEST_H_
#define TESTS_CUTE_TEST_H_

double square(SpatialVector x);
double norm(SpatialVector x);

#define ASSERT_EQUALDM(msg,a,b,tol) ASSERT_LESSM(msg,norm(a-b),tol);
#define ASSERT_NOTEQUALDM(msg,a,b,tol) ASSERT_LESSM(msg,tol,norm(a-b));
#define componentCheck(xxx) 	{cout << setprecision(17); cout << "  xxx: " << xxx << endl << flush; cout << "v.xxx: " << v_.xxx() << endl << flush; cout << "delta: " << xxx - v_.xxx() << endl << flush;}

// #define hexOut1(a,b) cout << (a) << " 0x" << hex << setfill('0') << setw(16) << (b) << dec << endl << flush;
// #define decOut1(a,b) cout << (a) << " " << dec << setfill('0') << setw(16) << (b) << dec << endl << flush;

#endif /* TESTS_CUTE_TEST_H_ */
