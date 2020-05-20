/*
 * STARE_Covers_test.cpp
 *
 *  Created on: Feb 20, 2020
 *      Author: mrilee
 *
 *  Copyright (C) 2020 Rilee Systems Technologies LLC
 */

#include <iostream>
#include "Test.h"
#include "SpatialInterface.h"
#include <dirent.h>

#include <string>
#include <cstdlib>

using namespace std;                            


vector<string> listDirectory(const std::string& folder_name) {
    vector<string> file_paths;
    DIR* dirp = opendir(folder_name.c_str());
    struct dirent * dp;
    std::string file_path;
    while ((dp = readdir(dirp)) != NULL) {
        string file_name = dp->d_name;
        if (file_name.find(".csv") != std::string::npos){
            file_path = folder_name + "/" + file_name;
            file_paths.push_back(file_path);
        }
    }
    closedir(dirp);
    return file_paths;
}


LatLonDegrees64ValueVector readCSV(string file_path) {
    LatLonDegrees64ValueVector cornerVector;
    std::ifstream file(file_path);
    std::string row;         
    while (std::getline(file, row)) {  
        float lon = stof(row.substr(0, row.find(",")));        
        float lat = stof(row.substr(row.find(",")+1));
        cornerVector.push_back(LatLonDegrees64(lat, lon));
    }
    file.close();    
    return cornerVector;
}


STARE stare;

void STARE_Covers_test() {

	string path_query    = "STARE_SRC_HOME";
	string base_dir;
	const char* query_result = getenv(path_query.c_str());
	if( query_result ) {
		base_dir = query_result;
		base_dir = base_dir+"/";
	} else {
		base_dir = "";
	}

    if (false) {        
        int n_test = 80;
        LatLonDegrees64ValueVector points1 = readCSV(base_dir+"tests/CUTE/polygons/0/0.csv");
        //LatLonDegrees64ValueVector points1 = makeCornerVector1(lons_lats1,n_test);
      
        LatLonDegrees64ValueVector points;
        for(int i=0; i<n_test/2; ++i) {
            points.push_back(points1[i]);
        }

        for(int i=0; i<n_test/2; ++i) {
            double dp2;
            dp2 = pow(points1[i].lat - points[i].lat, 2) + pow(points1[i].lon - points[i].lon, 2);
            if( dp2 > 1.0e-9) {
                cout << i << " i: p,p1,dp: "
					<< "( " << points[i].lat << " " << points[i].lon << " ) "
					<< "( " << points1[i].lat << " " << points1[i].lon << " ) "
					<< "( " << (points1[i].lat-points[i].lat) << " " << (points1[i].lon-points[i].lon) << " ) "
					<< endl << flush;
            }
        }

        int level = 16;
        STARE_SpatialIntervals cover = stare.ConvexHull(points, level);
    }
    

    // Testing country polygons of the geopandas lowres_world dataset, version 0.4
    if (true) {
        vector<string> file_paths = listDirectory(base_dir+"tests/CUTE/polygons/world_04/");
        for(auto const& file_path: file_paths) {
            LatLonDegrees64ValueVector cornerVector = readCSV(file_path);
            stare.ConvexHull(cornerVector, 8);
        }
    }
    
    
    // Testing country polygons of the geopandas lowres_world dataset, version 0.4
    if (true) {
        vector<string> file_paths = listDirectory(base_dir+"tests/CUTE/polygons/world_07/");
        for(auto const& file_path: file_paths) {
            LatLonDegrees64ValueVector cornerVector = readCSV(file_path);
            stare.ConvexHull(cornerVector, 8);
        }
    }
    
    // Testing shapes of caribbean outlines 
    if (false) {
        vector<string> file_paths = listDirectory(base_dir+"tests/CUTE/polygons/caribbean/");
        for(auto const& file_path: file_paths) {
            LatLonDegrees64ValueVector cornerVector = readCSV(file_path);
            stare.ConvexHull(cornerVector, 8);
        }
    }
    
    // Testing assortment of shapes
    if (false) {
        vector<string> file_paths = listDirectory(base_dir+"tests/CUTE/polygons/0/");
        for(auto const& file_path: file_paths) {
            LatLonDegrees64ValueVector cornerVector = readCSV(file_path);
            stare.ConvexHull(cornerVector, 8);
        }
    }
    
    
    if (false) {        
        LatLonDegrees64ValueVector cornerVector = readCSV(base_dir+"tests/CUTE/polygons/0/0.csv");
        int n_test = 80;
        int level = 16;
        STARE_SpatialIntervals cover = stare.ConvexHull(cornerVector, level);
    
        // REGRESSION TEST - NOT YET VALIDATED
        long long int cover_[ 45 ] = {
   			  0x3d7e69d220000010
			 ,0x3d7e69d228000010
			 ,0x3d7e69d238000010
			 ,0x3d7e69d288000010
			 ,0x3d7e69d2a0000010
			 ,0x3d7e69d2a8000010
			 ,0x3d7e69d2b0000010
			 ,0x3d7e69d2b8000010
			 ,0x3d7e69d2c0000010
			 ,0x3d7e69d2d0000010
			 ,0x3d7e69d2d8000010
			 ,0x3d7e69d2e0000010
			 ,0x3d7e69d2f0000010
			 ,0x3d7e69d2f8000010
			 ,0x3d7e69d380000010
			 ,0x3d7e69d388000010
			 ,0x3d7e69d390000010
			 ,0x3d7e69d398000010
			 ,0x3d7e69d3c0000010
			 ,0x3d7e69d3c8000010
			 ,0x3d7e69d3d0000010
			 ,0x3d7e69d3d8000010
			 ,0x3d7e69d3e0000010
			 ,0x3d7e69d3e8000010
			 ,0x3d7e69d3f0000010
			 ,0x3d7e69d3f8000010
			 ,0x3d7e69d540000010
			 ,0x3d7e69d548000010
			 ,0x3d7e69d558000010
			 ,0x3d7e69d580000010
			 ,0x3d7e69d590000010
			 ,0x3d7e69d598000010
			 ,0x3d7e69d610000010
			 ,0x3d7e69d620000010
			 ,0x3d7e69d628000010
			 ,0x3d7e69d630000010
			 ,0x3d7e69d638000010
			 ,0x3d7e69d640000010
			 ,0x3d7e69d648000010
			 ,0x3d7e69d650000010
			 ,0x3d7e69d658000010
			 ,0x3d7e69d660000010
			 ,0x3d7e69d668000010
			 ,0x3d7e69d670000010
			 ,0x3d7e69d678000010
        };

        for( int i = 0; i < cover.size(); ++i ) {
            ASSERT_EQUAL(cover_[i],cover[i]);
        }
    }
    
    if (false) {	
        LatLonDegrees64ValueVector cornerVector = readCSV(base_dir+"tests/polygons/0/0.csv");
		STARE_SpatialIntervals indexValues = stare.ConvexHull(cornerVector, 12);
		
        // A regression. Perhaps foolhardy.
		ASSERT_EQUAL(indexValues[   0], 4063372763795030021);
		ASSERT_EQUAL(indexValues[ 209], 4047214340913233929);
		ASSERT_EQUAL(indexValues[ 418], 4048304506692173834);
		ASSERT_EQUAL(indexValues[ 627], 4069240857352470538);
		ASSERT_EQUAL(indexValues[ 836], 4047805878168977419);
		ASSERT_EQUAL(indexValues[1045], 4062281498504462347);
		ASSERT_EQUAL(indexValues[1254], 4071136552837709835);
		ASSERT_EQUAL(indexValues[1463], 4037621170680365068);
		ASSERT_EQUAL(indexValues[1672], 4037929652411432972);
		ASSERT_EQUAL(indexValues[1881], 4046503437926400012);
		ASSERT_EQUAL(indexValues[2090], 4047784506411712524);
		ASSERT_EQUAL(indexValues[2299], 4048209742533754892);
		ASSERT_EQUAL(indexValues[2508], 4049064166147751948);
		ASSERT_EQUAL(indexValues[2717], 4050190581450670092);
		ASSERT_EQUAL(indexValues[2926], 4050638220122128396);
		ASSERT_EQUAL(indexValues[3135], 4051281090826993676);
		ASSERT_EQUAL(indexValues[3344], 4051436980959969292);
		ASSERT_EQUAL(indexValues[3553], 4062307233948499980);
		ASSERT_EQUAL(indexValues[3762], 4062741541041471500);
		ASSERT_EQUAL(indexValues[3971], 4063200552786329612);
		ASSERT_EQUAL(indexValues[4180], 4066811658209591308);
		ASSERT_EQUAL(indexValues[4389], 4068128048505946124);
		ASSERT_EQUAL(indexValues[4598], 4068353207871471628);
		ASSERT_EQUAL(indexValues[4807], 4070473512966422540);
		ASSERT_EQUAL(indexValues[5016], 4071135865642942476);
		ASSERT_EQUAL(indexValues[5225], 4165894663942701068);
		ASSERT_EQUAL(indexValues[5434], 4166328008962998284);
    }

}
