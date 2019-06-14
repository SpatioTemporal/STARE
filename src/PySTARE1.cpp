/*
 * PySTARE1.cpp
 *
 *  Created on: Jun 13, 2019
 *      Author: mrilee
 *
 *  Copyright (C) 2019 Rilee Systems Technologies LLC
 */


#include "PySTARE1.h"

double doublefun(double b);
void range(int *rangevec, int n);
double dot(int n, double *a, double *b);
void xdot(int n, double *a, double *b, double *ret);

void PySTARE1::ValueFromJDTAINP( int len, int64_t* indices, double* JDTAI, int resolution_days ) {
	for(int i=0; (i<len); ++i) {
		indices[i] = index.fromJulianDayTAI(JDTAI[i]).scidbTemporalIndex();
	}
}

void PySTARE1::JDTAIFromValueNP( int len, double* JDTAI, int64_t* indices ) {
	for(int i=0; (i<len); ++i) {
		// indices[i] = index.fromJulianDayTAI(JDTAI[i]).scidbTemporalIndex();
		double d1, d2;
		index.setArrayIndexTemporalValue(indices[i]).toJulianTAI(d1, d2);
		JDTAI[i] = d1 + d2;
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
