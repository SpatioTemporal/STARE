
/*

  Read a winter2010_MERRA2_ccl CSV file, construct STARE spatial and
  temporal indexes, and write to another file.

  Example:

    ./winter2010blizzard_csv_to_STARE --file example.txt --level 13

  copies the data in example.txt, adds STARE info, and writes example.txt.stare.

  mike@rilee.net
  2017-1121

 */

#include "SpatialIndex.h"
#include "BitShiftNameEncoding.h"
#include "EmbeddedLevelNameEncoding.h"
#include "TemporalIndex.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

/// https://ysonggit.github.io/coding/2014/12/16/split-a-string-using-c.html
vector<string> split(const string &s, const char &delim) {
  stringstream ss(s); string item; vector<string> tokens;
  while(getline(ss,item,delim)) tokens.push_back(item);
  return tokens; }

void usage(char *name) {
  cout << "usage:" << endl
       << name << "  --file <path-file> --level <level>" << endl
       << endl;
  exit(0);
}

int main(int argc, char *argv[]) {

  string sourceFile;
  int level      = 8;
  int buildlevel = 5;

  int arg = 2;  // # of required arguments
  int args = 0; // current argument of focus
  while(argc>0){
    if (strcmp(argv[args],"--file")==0) {
      argc--; // consume
      if(argc<=0)usage(argv[0]);
      sourceFile = argv[++args];
      arg--; // The argument is entered.
    }
    if (strcmp(argv[args],"--level")==0) {
      argc--; // consume
      if(argc<=0)usage(argv[0]);
      level = atoi(argv[++args]);
      arg--; // The argument is entered.
    }
    args++; // next arg
    argc--; // consume
  }
  if(arg > 0)usage(argv[0]);

  cout << "sourceFile: '" << sourceFile << "'" << endl;

  string targetFile = sourceFile+".stare";
  cout << "targetFile: '" << targetFile << "'" << endl;

  std::ifstream ifile(sourceFile);
  std::string   str;

  std::ofstream ofile(targetFile);

  TemporalIndex tIndex;
  

  std::getline(ifile,str);
  ofile << str << ",stare-spatial,stare-temporal" << endl;
  
  SpatialIndex index(level,buildlevel);
  while(std::getline(ifile,str)) {
    ofile << str << ",";
    vector<string> items = split(str,',');
    int yr, mo, dy, hr; double lat, lon;
    int j=4;
    yr  = stoi(items[++j]);
    mo  = stoi(items[++j]);
    dy  = stoi(items[++j]);
    hr  = stoi(items[++j]);
    lat = stod(items[++j]);
    lon = stod(items[++j]);
    tIndex.set_zero();
    tIndex.hackSetTraditionalDate(
				  yr,
				  mo-1, // 0..11
				  dy,   // 1..31
				  hr,
				  0,    // min
				  0,    // sec
				  0     // ms
				  );
    tIndex.set_resolutionLevel(0);

    uint64 idRJ = index.idByLatLon(lat,lon);
    BitShiftNameEncoding rj(idRJ);
    EmbeddedLevelNameEncoding lj(rj.leftJustifiedId_NoDepthBit());
    
    //     ofile << yr << mo << dy << hr << lat << lon;
    ofile << hex;
    ofile << "0x";
    ofile << lj.getSciDBLeftJustifiedFormat();
    ofile << ",";
    ofile << "0x";
    ofile << tIndex.scidbTemporalIndex();
    ofile << dec;
    ofile << endl;
    
  }
  ifile.close();
  ofile.close();
}
