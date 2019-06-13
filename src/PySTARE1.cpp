/*
 * PySTARE1.cpp
 *
 *  Created on: Jun 13, 2019
 *      Author: mrilee
 *
 *  Copyright (C) 2019 Rilee Systems Technologies LLC
 */


#include <stdio.h>

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


