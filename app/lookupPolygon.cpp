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


void usage(char *name) {
            std::cout 
            << "STARE spatial index lookup. " << std::endl
            << "Usage: " << name << " [options] --resolution=RESOLUTION --corners (lat1 lon1) (lat2 lon2) (lat3 lon3) {(latN lonN)} " << std::endl
			<< "Examples:" << std::endl			
			<< "  " << name << " --resolution 12 41.6 106.1"            << std::endl
			<< "  " << name << " --hex --symbol --area --xyz 5 3 2"  << std::endl
			<< std::endl
			<< "Mandatory arguments to long options are mandatory for short options too." << std::endl
			<< "Options:" << std::endl			
			<< "  " << " -h, --help                                      : print this help"                                    << std::endl
			<< "  " << " -v, --verbose                                   : verbose: print all"                                 << std::endl
			<< "  " << " -q, --quiet                                     : don't chat, just give back index"                   << std::endl
			<< "  " << " -x, --hex                                       : print node id in hexadecimal (default: off)"        << std::endl
            << "  " << " -i, --integer                                   : print node id as an integer (default: on)"          << std::endl
            << "  " << " -s, --symbol                                    : print node id as a string symbol (default: off)"    << std::endl
			<< "  " << " -r, --resolution=RESOLUTION                     : Force resolution of indices to return"              << std::endl
			<< "  " << " -c, --corners (lat lon)                         : specify corners of polygon to look up"              << std::endl
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
    LatLonDegrees64ValueVector cornerVector;
};


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
        {0,             0,                 0,  0   }
    };
    
    int long_index = 0;
    int opt = 0;
    while ((opt = getopt_long(argc, argv, "hvqxisc:lmn", long_options, &long_index)) != -1) {
        switch (opt) {
            case 'h': usage(argv[0]);
            case 'v': arguments.verbose = true; break;
            case 'q': arguments.quiet = true; break;
            case 'x': arguments.hex = true; break;
            case 'i': arguments.integer = true; break;
            case 's': arguments.symbol = true; break;
            case 'r': arguments.resolution = atoi(optarg); break;                
            case 'c': arguments.corners = true; break;
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
    }
    if (arguments.corners && (argc-optind-1)%2==0 ) {
        int ilatlon = optind-1;
        while (ilatlon<argc) {
            float lon = atof(argv[ilatlon]);
            float lat = atof(argv[ilatlon+1]);
            arguments.cornerVector.push_back(LatLonDegrees64(lat, lon));
            ilatlon += 2;
        };
    } else usage(argv[0]);    
    return arguments;
};

STARE stare;

main(int argc, char *argv[]) {
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
    
    
    
