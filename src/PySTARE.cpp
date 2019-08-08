/*
 * PySTARE.cpp
 *
 *  Created on: Jun 13, 2019
 *      Author: mrilee
 *
 *  Copyright (C) 2019 Rilee Systems Technologies LLC
 */


#include "PySTARE.h"

double doublefun(double b);
void range(int *rangevec, int n);
double dot(int n, double *a, double *b);
void xdot(int n, double *a, double *b, double *ret);

void PySTARE::ValueFromJDTAINP( int len, int64_t* indices, double* JDTAI, int resolution_days ) {
	for(int i=0; (i<len); ++i) {
		indices[i] = index.fromJulianDayTAI(JDTAI[i]).scidbTemporalIndex();
	}
}

void PySTARE::JDTAIFromValueNP( int len, double* JDTAI, int64_t* indices ) {
	for(int i=0; (i<len); ++i) {
		// indices[i] = index.fromJulianDayTAI(JDTAI[i]).scidbTemporalIndex();
		double d1, d2;
		index.setArrayIndexTemporalValue(indices[i]).toJulianTAI(d1, d2);
		JDTAI[i] = d1 + d2;
	}
}

void PySTARE::ValueFromLatLonDegreesLevelNP( int len, int64_t* indices, double* lat, double* lon, int resolutionLevel ) {
	for( int i=0; i < len; ++i) {
		indices[i] = index.ValueFromLatLonDegrees(lat[i], lon[i], resolutionLevel);
	}
}

void PySTARE::LatLonDegreesFromValueNP( int len, double* lat, double* lon, int64_t* indices ) {
	for( int i=0; i < len; ++i) {
		LatLonDegrees64 latlon = index.LatLonDegreesFromValue(indices[i]);
		lat[i] = latlon.lat; lon[i] = latlon.lon;
	}
}

/*
bn::ndarray JDTAIFromValueNP(bn::ndarray values) {
	const Py_intptr_t *shape = {values.get_shape()};
	Py_intptr_t const * strides = values.get_strides();
	bn::ndarray result_jdtai = bn::zeros(1,shape,bn::dtype::get_builtin<float64>());
	for(int i=0; i<shape[0]; ++i) {
		STARE_ArrayIndexTemporalValue idx = *reinterpret_cast<STARE_ArrayIndexTemporalValue*>(values.get_data() + i*strides[0]);
		index.setArrayIndexTemporalValue(idx);
		result_jdtai[i] = index.toJulianDayTAI();
	}
	return result_jdtai;
}
*/
