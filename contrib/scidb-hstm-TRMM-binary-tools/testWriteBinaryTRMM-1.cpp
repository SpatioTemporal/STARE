

/*

  Process TRMM data for ingest into SciDB using a proto-STARE index.

  Originally written for DERECHOS in 2017.

  Michael Rilee
  Rilee Systems Technologies
  mike@rilee.net

 */

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>



// TODO #include the headers for the code you want to test
//#include "SpatialGeneral.h"
// #include "SpatialException.h"
#include "SpatialIndex.h"
#include "TemporalIndex.h"
//#include "SpatialVector.h"
//#include "SpatialInterface.h"
//#include "SpatiallyAdaptiveDataCover.h"
//
//#include "HtmRangeIterator.h"
//#include "BitShiftNameEncoding.h"
#include "EmbeddedLevelNameEncoding.h"
//
//#include "HtmRangeMultiLevel.h"
//#include "HtmRangeMultiLevelIterator.h"
//
//#include "HstmIndex.h"
//#include "HstmRange.h"


using namespace std;

int main( int argc, char *argv[] ) {
  cout << "hello, world!" << endl << flush;

  if(argc != 2) {
    cout << "usage:" << endl
	 << " " << argv[0] << " <fileNameBase> " << endl
	 << "e.g." << endl
	 << " " << argv[0] << " 2B31.20091201.68611.7" << endl;
    exit(0);
  }

  bool printFlag = true;
  printFlag = false;

  string fileNameBase = argv[1];
  string fileName;


  // TODO Simplify this interface
  // Spatial level
  int level = 11; // 5 km
  // int level = 10;
  // Temporal co-level
  int temporalCoLevel = 0; // "Maximum" resolution level.

  ///////////////////////////////////////////////////////////////////////////

  int nscan = 0;
  fileName = fileNameBase + ".nscan";
  std::ifstream nscan_ifs(fileName.c_str(), std::ios::binary );
  if(!nscan_ifs) {
    cout << fileName << " not found. Exiting." << endl << flush;
    exit(1);
  }
  nscan_ifs.read(reinterpret_cast<char*>(&nscan),sizeof(int));
  if(printFlag) cout << "nscan: " << nscan << endl << flush;
  nscan_ifs.close();

  int nray = 0;
  fileName = fileNameBase + ".nray";  
  std::ifstream nray_ifs(fileName.c_str(), std::ios::binary );
  if(!nray_ifs) {
    cout << fileName << " not found. Exiting." << endl << flush;
    exit(1);
  }
  nray_ifs.read(reinterpret_cast<char*>(&nray),sizeof(int));
  if(printFlag) cout << "nray: " << nray << endl << flush;
  nray_ifs.close();


  /* the following are nscan*nray 'ary */
  
  float lat_tmp = -999;
  fileName = fileNameBase + ".Latitude";
  std::ifstream lat_ifs(fileName.c_str(), std::ios::binary );

  float lon_tmp = -999;
  fileName = fileNameBase + ".Longitude";
  std::ifstream lon_ifs(fileName.c_str(), std::ios::binary );

  /* the following are nscan 'ary */

  short int year_tmp;
  fileName = fileNameBase + ".Year";
  std::ifstream year_ifs(fileName.c_str(), std::ios::binary );

  char month_tmp;
  fileName = fileNameBase + ".Month";
  std::ifstream month_ifs(fileName.c_str(), std::ios::binary );

  char day_tmp;
  fileName = fileNameBase + ".DayOfMonth";
  std::ifstream day_ifs(fileName.c_str(), std::ios::binary );

  char hour_tmp;
  fileName = fileNameBase + ".Hour";
  std::ifstream hour_ifs(fileName.c_str(), std::ios::binary );  

  char minute_tmp;
  fileName = fileNameBase + ".Minute";
  std::ifstream minute_ifs(fileName.c_str(), std::ios::binary );  
  
  char second_tmp;
  fileName = fileNameBase + ".Second";
  std::ifstream second_ifs(fileName.c_str(), std::ios::binary );  
  
  short int millisecond_tmp;
  fileName = fileNameBase + ".MilliSecond";
  std::ifstream millisecond_ifs(fileName.c_str(), std::ios::binary );  
  
  ///////////////////////////////////////////////////////////////////////////

  fileName = fileNameBase + ".TemporalIndex";
  std::ofstream temporal_index_ifs(fileName.c_str(), std::ios::binary );

  fileName = fileNameBase + ".SpatialIndex";
  std::ofstream spatial_index_ifs(fileName.c_str(), std::ios::binary );

  
  ///////////////////////////////////////////////////////////////////////////

  for(int iScan=0; iScan < nscan; ++iScan) {
  // for(int iScan=0; iScan < 2; ++iScan) {

    /* the following are nscan 'ary */

    year_ifs.read(reinterpret_cast<char*>(&year_tmp),sizeof(short int));
    if(printFlag) cout << "year: " << year_tmp << endl << flush;

    month_ifs.read(reinterpret_cast<char*>(&month_tmp),sizeof(char));
    if(printFlag) cout << "month: " << (int)month_tmp << endl << flush;

    day_ifs.read(reinterpret_cast<char*>(&day_tmp),sizeof(char));
    if(printFlag) cout << "day: " << (int)day_tmp << endl << flush;

    hour_ifs.read(reinterpret_cast<char*>(&hour_tmp),sizeof(char));
    if(printFlag) cout << "hour: " << (int)hour_tmp << endl << flush;

    minute_ifs.read(reinterpret_cast<char*>(&minute_tmp),sizeof(char));
    if(printFlag) cout << "minute: " << (int)minute_tmp << endl << flush;
  
    second_ifs.read(reinterpret_cast<char*>(&second_tmp),sizeof(char));
    if(printFlag) cout << "second: " << (int)second_tmp << endl << flush;

    millisecond_ifs.read(reinterpret_cast<char*>(&millisecond_tmp),sizeof(short int));
    if(printFlag) cout << "millisecond: " << (int)millisecond_tmp << endl << flush;

    TemporalIndex tIndex;

    try {
      tIndex.hackSetTraditionalDate(
				    year_tmp,
				    month_tmp-1, // Note months are 0..b.
				    day_tmp,
				    hour_tmp,
				    minute_tmp,
				    second_tmp,
				    millisecond_tmp
				    );
    } catch ( const SpatialException &e ) {
      cout << "tIndexhackSetTraditionalDate 100: " << e.what() << endl << flush;

      cout
	<< "   iScan: " << iScan << endl << flush
	<< "   year : " << year_tmp << endl << flush
	<< "   month: " << (int)month_tmp << endl << flush
	<< "   day  : " << (int)day_tmp << endl << flush
	<< "   hour : " << (int)hour_tmp << endl << flush
	<< "   min  : " << (int)minute_tmp << endl << flush
	<< "   sec  : " << (int)second_tmp << endl << flush
	<< "   msec : " << millisecond_tmp << endl << flush;
      
      exit(1);
    }
    if(printFlag) cout << "tIndex-trad:   " << tIndex.hackStringInTraditionalDate() << endl << flush;
    if(printFlag) cout << "tIndex-native: " << tIndex.stringInNativeDate() << endl << flush;
    if(printFlag) cout << "tIndex-scidb:  "
		       << hex << tIndex.scidbTemporalIndex() << " "
		       << dec << tIndex.scidbTemporalIndex()
		       << endl << flush;

    long int tIndex_tmp = tIndex.scidbTemporalIndex();
    

    /////

    for(int iRay=0; iRay < nray; ++ iRay){

      temporal_index_ifs.write(reinterpret_cast<char*>(&tIndex_tmp),sizeof(long int));
  
      /* the following are nscan*nray 'ary */

      lat_ifs.read(reinterpret_cast<char*>(&lat_tmp),sizeof(float));
      if(printFlag) cout << "lat: " << lat_tmp << endl << flush;

      lon_ifs.read(reinterpret_cast<char*>(&lon_tmp),sizeof(float));
      if(printFlag) cout << "lon: " << lon_tmp << endl << flush;

      // TODO Simplify this interface
      // int level = 11; // 5 km
      // int level = 10;
      SpatialIndex index(level);
      uint64 id_RightJustified = -999;
      try {
	id_RightJustified = index.idByLatLon(lat_tmp,lon_tmp);
      } catch ( const SpatialException &e ) {
	cout << "index.idByLatLon 200: " << e.what() << endl
	     << "lat-lon: " << lat_tmp << ", " << lon_tmp << endl
	     << flush;
	exit(1);
      }
      uint64 id_LeftJustified;

      BitShiftNameEncoding     rightJustified;
      rightJustified.setId(id_RightJustified);
      uint64 id_Right = rightJustified.getId();
      if(printFlag) cout << "spatial-right-id:       " << hex << "0x" << id_Right << dec << " " << id_Right << endl << flush;
      if(printFlag) cout << "spatial-right-id-level: " << rightJustified.getLevel() << endl << flush;

      EmbeddedLevelNameEncoding leftJustified;
      leftJustified.setId(rightJustified.leftJustifiedId());
      int64 id_Scidb = leftJustified.getSciDBLeftJustifiedFormat();
      if(printFlag) cout << "spatial-left-id-scidb: " << hex << "0x" << id_Scidb << dec << " " << id_Scidb << endl << flush;
      if(printFlag) cout << "spatial-left-id-level: " << leftJustified.getLevel() << endl << flush;
  
      long int sIndex_tmp = leftJustified.getSciDBLeftJustifiedFormat();
      spatial_index_ifs.write(reinterpret_cast<char*>(&sIndex_tmp),sizeof(long int));

    } // iRay
  } // iScan
  
  ///////////////////////////////////////////////////////////////////////////
  
  /* nscan*nray */
  lat_ifs.close();
  lon_ifs.close();

  /* nscan */
  year_ifs.close();
  month_ifs.close();
  day_ifs.close();
  hour_ifs.close();
  minute_ifs.close();
  second_ifs.close();
  millisecond_ifs.close();

  /* output */
  temporal_index_ifs.close();
  spatial_index_ifs.close();

  cout << "wrote (nscan, nray) = ( " << nscan << ", " << nray << " ) spatial and temporal indexes to "
       << fileNameBase << "." << endl << flush;
  cout << "Spatial level = " << level << ", " << "temporal co-level = " << temporalCoLevel << endl << flush;
  cout << "good bye!" << endl << flush;
}
