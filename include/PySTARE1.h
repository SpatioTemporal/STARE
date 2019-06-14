/*
 * PySTARE1.h
 *
 *  Created on: Jun 13, 2019
 *      Author: mrilee
 *
 *  Copyright (C) 2019 Rilee Systems Technologies LLC
 */

#ifndef INCLUDE_PYSTARE1_H_
#define INCLUDE_PYSTARE1_H_

#include <stdio.h>
#include "STARE.h"

double doublefun(double b){
        printf("c(%g)",b);
        return b+12.5;
}

void range(int *rangevec, int n)
{
    int i;

    for (i=0; i< n; i++)
        rangevec[i] = i;
}

double dot(int n, double *a, double *b) {
	double sum = 0;
	for( int i=0; i<n; ++i ) {
		sum += a[i]*b[i];
	}
	return sum;
}

void xdot(int n, double *a, double *b, double *ret) {
	for( int i=0; i<n; ++i ) {
		ret[i] += a[i]*b[i];
	}
}


class PySTARE1 {
public:
	STARE index;
	PySTARE1() {};
	~PySTARE1() {};

	void ValueFromJDTAINP( int len, int64_t* indices, double* JDTAI, int resolution_days );
	void JDTAIFromValueNP( int len, double* JDTAI, int64_t* indices );
};

// PySTARE1* PySTARE() { return new PySTARE1; }

#endif /* INCLUDE_PYSTARE1_H_ */
