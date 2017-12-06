#include <SpatialInterface.h>
#include <HtmRangeIterator.h>

bool varlength = false;       // output variable length HTMids
bool symbolic = true;


int searchlevel=10;
int buildlevel=9;
int maximum_nrGaps=100;

size_t i;

int main(int argc, char *argv[]) {    
    SpatialIndex spatialIndex(searchlevel,buildlevel);   
        
    HtmRange htmRangeCover;
    HtmRange htmRangeInterior;
    HtmRange htmRangeBoundary;
    RangeConvex convex;
    
    SpatialVector corner2(0.0347738288389, 0.0127308084721, 0.999314118455); 
    SpatialVector corner1(0.0515441635412, 0.0193857623262, 0.998482544376); 
    SpatialVector corner4(0.0330035382386, 0.0163594289767, 0.999321337482); 
    SpatialVector corner3(0.0489487014627, 0.0248000461452, 0.998493356180);
   
    convex = RangeConvex(&corner1, &corner2, &corner3, &corner4);
    
    convex.intersect(&spatialIndex, &htmRangeCover, varlength, &htmRangeInterior, &htmRangeBoundary);
    
    htmRangeCover.defrag();
    htmRangeCover.setSymbolic(symbolic);
    
    htmRangeInterior.defrag();
    htmRangeInterior.setSymbolic(symbolic);
        
    cout << htmRangeInterior <<endl;    
    HtmRangeIterator iter(&htmRangeCover);
    char buffer[80];
    while (iter.hasNext()) {                        
      cout << iter.nextSymbolic(buffer) << endl;
    }   
    
    
   
    
}
