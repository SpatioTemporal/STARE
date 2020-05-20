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
#include <fstream>

#include <sys/types.h>
#include <dirent.h>
 
STARE stare;
 
void usage(char *name) {
  std::cout 
    << "STARE spatial index lookup. " << std::endl
    << "Usage: " << std::endl  << std::endl  
    << "  " << name << " [options] --resolution RESOLUTION (--file FILE | --folder FOLDER | --corners (lat1 lon1) (lat2 lon2) (lat3 lon3) {(latN lonN)}) " << std::endl << std::endl 
    << "Examples:" << std::endl 
    << "  " << name << " --resolution 12 -c 41.6 106.1 42.3 107.5 42.5 107.3"            << std::endl    
    << "  " << name << " --resolution 12 --folder csvs/"                                 << std::endl    
    << "  " << name << " --resolution 12 --file csvs/01.csv"                             << std::endl    
    << std::endl
    << "Mandatory arguments to long options are mandatory for short options too." << std::endl
    << "Options:" << std::endl			
    << "  " << " -h, --help                      : print this help"                                    << std::endl
    << "  " << " -v, --verbose                   : verbose: print all"                                 << std::endl
    << "  " << " -q, --quiet                     : don't chat, just give back index"                   << std::endl
    << "  " << " -x, --hex                       : print node id in hexadecimal (default: off)"        << std::endl
    << "  " << " -i, --integer                   : print node id as an integer (default: on)"          << std::endl
    << "  " << " -s, --symbol                    : print node id as a string symbol (default: off)"    << std::endl
    << "  " << " -r, --resolution RESOLUTION     : Force resolution of indices to return"              << std::endl
    << "  " << " -c, --corners (lat lon)         : specify corners of polygon to look up"              << std::endl
    << "  " << " -f, --file FILE                 : location of csv of corners of polygon to look up"   << std::endl    
    << "  " << " -l, --folder FOLDER             : location folder of csv of corners of polygon to look up"   << std::endl
    << std::endl;
  exit(0);
};


struct Arguments {
  bool verbose = false;
  bool quiet = false;    
  bool hex= true;
  bool integer = true;    
  bool symbol = false;    
  int resolution = -1;    
  int corners = false;
  bool file = false;
  bool folder = false;
  string filename = "";
  string foldername = "";
  LatLonDegrees64ValueVector cornerVector;
};

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


Arguments parseArguments(int argc, char *argv[]) {
    if (argc == 1) usage(argv[0]);
    Arguments arguments;        
    static struct option long_options[] = {
					 {"help",        no_argument,       0,  'h' },
					 {"verbose",     no_argument,       0,  'v' },
					 {"quiet",       no_argument,       0,  'q' },
					 {"hex",         no_argument,       0,  'x' },
					 {"integer",     no_argument,       0,  'i' },
					 {"symbol",      no_argument,       0,  's' },
					 {"resolution",  required_argument, 0,  'r' },
					 {"corners",     required_argument, 0,  'c' },
                     {"file",        required_argument, 0,  'f' },
                     {"folder",      required_argument, 0,  'l' },
					 {0,             0,                 0,  0   }
    };    
    
    int long_index = 0;
    int opt = 0;
    while ((opt = getopt_long(argc, argv, "hvqxisr:c:f:l:", long_options, &long_index)) != -1) {
        switch (opt) {
            case 'h': usage(argv[0]);
            case 'v': arguments.verbose = true; break;
            case 'q': arguments.quiet = true; break;
            case 'x': arguments.hex = true; break;
            case 'i': arguments.integer = true; break;
            case 's': arguments.symbol = true; break;
            case 'r': arguments.resolution = atoi(optarg); break;                
            case 'c': arguments.corners = true; break;
            case 'f': arguments.file = true; arguments.filename = optarg; break;
            case 'l': arguments.folder = true; arguments.foldername = optarg; break;
        }
    }   
    
    
    if (arguments.verbose) {
        arguments.hex = true;
        arguments.integer = true;
        arguments.symbol= true;
    } else if (arguments.quiet) {
        arguments.hex = false;
        arguments.integer = true;
        arguments.symbol= false;
    };
    
    if (arguments.corners && (argc-optind-1)%2==0 ) {
        int ilatlon = optind-1;
        while (ilatlon<argc) {
            float lon = atof(argv[ilatlon]);
            float lat = atof(argv[ilatlon+1]);
            arguments.cornerVector.push_back(LatLonDegrees64(lat, lon));
        ilatlon += 2;
        };
    } else if (arguments.file) {
        arguments.cornerVector = readCSV(arguments.filename);        
    } else if (arguments.folder) {
        vector<string> file_paths = listDirectory(arguments.foldername);
        for(auto const& file_path: file_paths) {
            std::cout << file_path << std::endl;  
            arguments.cornerVector.clear();
            arguments.cornerVector = readCSV(file_path);
            stare.ConvexHull(arguments.cornerVector, arguments.resolution);
        }
    } else usage(argv[0]);
   
    if (arguments.verbose) {
        for(auto const& corner: arguments.cornerVector) {
            std::cout << corner.lat << " "<< corner.lon << std::endl;
        }
    }
    return arguments;
};



int main(int argc, char *argv[]) {
  Arguments arguments = parseArguments(argc, argv);        

  // Getting index Value
  STARE_SpatialIntervals indexValues;
  indexValues = stare.ConvexHull(arguments.cornerVector, arguments.resolution);
    
  // Producing outputs
  if (!arguments.quiet) std::cout << "Integer Value of STARE index: "<< std::endl;
  for(auto const& value: indexValues) {
    if (arguments.integer) {
      std::cout << " " << value << std::endl;
    };
    if(arguments.symbol) {
      std::cout << " " << stare.getIndex().nameById(value) << std::endl;
    };
  };
};
    
    
    
