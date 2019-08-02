/*********************************************************************
 * stareMapping.cc													*
 *																	*
 *  Created on: 2019-07-31											*
 *  																	*
 *  		Purpose: Simple test of point intersection
 *      Author: Niklas Griessbaum
 *      																*
 ********************************************************************/


#include "STARE.h"

STARE stare;
int main(int argc, char *argv[]) {        
    STARE stare;
    cout << " " << stare.ValueFromLatLonDegrees(10.1, 15.2, 10) << endl;
    cout << " " << stare.ValueFromLatLonDegrees(10.1, 15.2, 15) << endl;    
} 
