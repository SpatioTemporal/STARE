/*********************************************************************
 * stareMapping.cc													*
 *																	*
 *  Created on: 2019-07-31											*
 *  																	* 
 *      Author: Niklas Griessbaum
 *      																*
 ********************************************************************/


#include "STARE.h"
#include "VarStr.h"
#include <getopt.h>

LatLonDegrees64ValueVector makeCornerVector(void){
    LatLonDegrees64ValueVector cornerVector;
    cornerVector.push_back(LatLonDegrees64(35.65007233330923, 61.21081709172574)); 
    cornerVector.push_back(LatLonDegrees64(35.27066396742229, 62.23065148300589)); 
    cornerVector.push_back(LatLonDegrees64(37.66116404881207, 65.74563073106683)); 
    cornerVector.push_back(LatLonDegrees64(38.25890534113216, 71.34813113799026)); 
    cornerVector.push_back(LatLonDegrees64(36.72000702569632, 72.92002485544447)); 
    cornerVector.push_back(LatLonDegrees64(31.90141225842444, 69.31776411324256)); 
    cornerVector.push_back(LatLonDegrees64(31.62018911389207, 68.92667687365767)); 
    cornerVector.push_back(LatLonDegrees64(29.88794342703618, 66.34647260932442)); 
    cornerVector.push_back(LatLonDegrees64(29.56003062592809, 65.04686201361611)); 
    cornerVector.push_back(LatLonDegrees64(29.34081920014597, 64.14800215033125)); 
    cornerVector.push_back(LatLonDegrees64(29.46833079682617, 63.55026085801117)); 
    cornerVector.push_back(LatLonDegrees64(31.37950613049267, 61.69931440618083)); 
    cornerVector.push_back(LatLonDegrees64(34.40410187431986, 60.80319339380745));     
    
    cornerVector.push_back(LatLonDegrees64(33.52883230237626, 60.96370039250601 ));     
    
    cornerVector.push_back(LatLonDegrees64(35.65007233330923, 61.21081709172574));
    return cornerVector;
}

STARE stare;

main(int argc, char *argv[]) {
    
    // Getting index Value
    STARE_SpatialIntervals indexValues;
    LatLonDegrees64ValueVector cornerVector = makeCornerVector();
    indexValues = stare.ConvexHull(cornerVector, 12);
    
    // Producing outputs
    std::cout << "Integer Value of STARE index: "<< std::endl;
    for(auto const& value: indexValues) {
        std::cout << " " << value << std::endl;
    }
};
    
    
    
