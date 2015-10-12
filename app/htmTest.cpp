
//dcdtmp #include <VarVecDef.h>
#include "SpatialInterface.h"
#include "SpatialDomain.h"
#include "VarStr.h"
#include "fstream.h"
#include <time.h>
#include <iostream.h>

int
main() {
/*******************************************************
  Initialization
 ******************************************************/

 bool verbose = false;		// verbosity flag
 bool count = false;		//countonly flag  
 bool onecol = false;		// single column output flag  
 bool text = false;		// text interface flag	
 
 int depth = 5;
 int savedepth = 2;

 SpatialIndex index(depth,savedepth);
    
    
 float64 avgArea = 0;
 float64 minArea = 100;
 float64 maxArea = 0;
 float64 leafArea;

 ofstream out("area.out");
 cout << index.leafCount() << endl;
 out.precision(10);

 for(size_t p = 0; p < index.leafCount(); p++) {
   leafArea = index.area(index.idByLeafNumber(p));
   out << index.nameByLeafNumber(p) << "   " << leafArea << endl;

   avgArea += leafArea;
   if(minArea > leafArea)minArea = leafArea;
   if(maxArea < leafArea)maxArea = leafArea;
 }
 cout << "Avg Area = " << avgArea / index.leafCount() << endl;
 cout << "Min Area = " << minArea << endl;
 cout << "Max Area = " << maxArea << endl;

 return 0;
} 

