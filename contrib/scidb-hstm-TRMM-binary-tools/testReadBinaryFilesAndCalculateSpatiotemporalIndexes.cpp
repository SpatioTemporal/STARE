

/*

  Exploratory coding for processing TRMM data for ingest into SciDB using a proto-STARE index.

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

  bool printFlag = true;
  // printFlag = false;

  string fileNameBase = "2B31.20091201.68611.7";
  string fileName;

  int nscan;
  fileName = fileNameBase + ".nscan";
  std::ifstream nscan_ifs(fileName.c_str(), std::ios::binary );
  nscan_ifs.read(reinterpret_cast<char*>(&nscan),sizeof(int));
  if(printFlag) cout << "nscan: " << nscan << endl << flush;
  nscan_ifs.close();

  int nray;
  fileName = fileNameBase + ".nray";  
  std::ifstream nray_ifs(fileName.c_str(), std::ios::binary );
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

  float rrSurf_tmp = -999;
  fileName = fileNameBase +".rrSurf";
  std::ifstream rrSurf_ifs(fileName.c_str(), std::ios::binary );

  int sigmaRRsurf_tmp;
  fileName = fileNameBase +".sigmaRRsurf";
  std::ifstream sigmaRRsurf_ifs(fileName.c_str(), std::ios::binary );
  
  float prSurf_tmp;
  fileName = fileNameBase +".prSurf";
  std::ifstream prSurf_ifs(fileName.c_str(), std::ios::binary );

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

  for(int i=0; i < 2; ++i) {

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

  /////

  TemporalIndex tIndex;

  tIndex.hackSetTraditionalDate(
				year_tmp,
				month_tmp,
				day_tmp,
				hour_tmp,
				minute_tmp,
				second_tmp,
				millisecond_tmp
				);
  if(printFlag) cout << "tIndex-trad:   " << tIndex.hackStringInTraditionalDate() << endl << flush;
  if(printFlag) cout << "tIndex-native: " << tIndex.stringInNativeDate() << endl << flush;
  if(printFlag) cout << "tIndex-scidb:  " << tIndex.scidbTemporalIndex() << endl << flush;

  /* the following are nscan*nray 'ary */

  lat_ifs.read(reinterpret_cast<char*>(&lat_tmp),sizeof(float));
  if(printFlag) cout << "lat: " << lat_tmp << endl << flush;

  lon_ifs.read(reinterpret_cast<char*>(&lon_tmp),sizeof(float));
  if(printFlag) cout << "lon: " << lon_tmp << endl << flush;

  int level = 11; // 5 km
  // int level = 10;
  SpatialIndex index(level);
  uint64 id_RightJustified = index.idByLatLon(lat_tmp,lon_tmp);
  uint64 id_LeftJustified;

  BitShiftNameEncoding     rightJustified;
  rightJustified.setId(id_RightJustified);
  uint64 id_Right = rightJustified.getId();
  cout << "spatial-right-id:       " << hex << "0x" << id_Right << dec << " " << id_Right << endl << flush;
  cout << "spatial-right-id-level: " << rightJustified.getLevel() << endl << flush;

  EmbeddedLevelNameEncoding leftJustified;
  leftJustified.setId(rightJustified.leftJustifiedId());
  int64 id_Scidb = leftJustified.getSciDBLeftJustifiedFormat();
  cout << "spatial-left-id-scidb: " << hex << "0x" << id_Scidb << dec << " " << id_Scidb << endl << flush;
  cout << "spatial-left-id-level: " << leftJustified.getLevel() << endl << flush;
  
  

  rrSurf_ifs.read(reinterpret_cast<char*>(&rrSurf_tmp),sizeof(float));
  if(printFlag) cout << "rrSurf: " << rrSurf_tmp << endl << flush;

  sigmaRRsurf_ifs.read(reinterpret_cast<char*>(&sigmaRRsurf_tmp),sizeof(int));
  if(printFlag) cout << "sigmaRRsurf: " << sigmaRRsurf_tmp << endl << flush;

  prSurf_ifs.read(reinterpret_cast<char*>(&prSurf_tmp),sizeof(float));
  if(printFlag) cout << "prSurf: " << prSurf_tmp << endl << flush;

  }
  
  ///////////////////////////////////////////////////////////////////////////
  
  /* nscan*nray */
  lat_ifs.close();
  lon_ifs.close();
  rrSurf_ifs.close();
  sigmaRRsurf_ifs.close();
  prSurf_ifs.close();

  /* nscan */
  year_ifs.close();
  month_ifs.close();
  day_ifs.close();
  hour_ifs.close();
  minute_ifs.close();
  second_ifs.close();
  millisecond_ifs.close();
  
  if(printFlag) cout << "good bye!" << endl << flush;
}
