#include <iostream>    
#include <stdint.h>
#include <iomanip>
#include <ios>
//dcdtmp #include <VarVecDef.h>
#include "VarStr.h"
#include "SpatialVector.h"
#include "SpatialInterface.h"
#include "EmbeddedLevelNameEncoding.h"

using namespace std;

int getCornersLatLon(htmInterface *htm, uint64 id) {
        SpatialVector v1,v2,v3;
        htm->index().nodeVertex(htm->index().nodeIndexFromId(id),v1,v2,v3);
        float64 lat1, lon1, lat2, lon2, lat3, lon3;
        v1.getLatLonDegrees(lat1,lon1);
        v2.getLatLonDegrees(lat2,lon2);
        v3.getLatLonDegrees(lat3,lon3);        
        cout.precision(18);
        cout << lat1 << " " << lon1 << endl
             << lat2 << " " << lon2 << endl
             << lat3 << " " << lon3 << endl;
    
}


int getCorners(htmInterface *htm, uint64 id) {
    SpatialVector v1,v2,v3;
    htm->index().nodeVertex(htm->index().nodeIndexFromId(id),v1,v2,v3);
    cout.precision(18);
    cout    << v1 << endl
			<< v2 << endl
			<< v3 << endl;
}
 
int getFromCoord(bool printCorners, int level) {
    // Declaring variables        
    float x;    
    float y; 
    float z;     
    float64 lat;
    float64 lon;
    uint64 id;
    htmInterface *htm;       
    VarStr cmd;   
    char str[200];     
    SpatialVector *v = NULL;    
    v = new SpatialVector();    
    htm = new htmInterface(12);  
    while (true){          
        cin >> lat;
        cin >> lon;        
        v->setLatLonDegrees(lat,lon); 
        x = v->x(); y = v->y(); z = v->z();        
        sprintf(str,"CARTESIAN %d %20.16f %20.16f %20.16f",level, x,y,z);    
        cmd = str;
        id = htm->lookupIDCmd(cmd.data());        
        cout << id<< endl;   
        if (printCorners) getCorners(htm, id);
    }
}



int main(int argc, char *argv[]) { 
    bool corner = false;
    int level;
    level = atoi(argv[1]);
    if (strcmp(argv[2], "--corners")==0) corner = true;        
    getFromCoord(corner, level);
}                           

