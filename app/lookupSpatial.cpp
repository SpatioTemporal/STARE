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
            << "Usage: " << name << " [options] --resolution=RESOLUTION {--latlon lat lon | --xyz x y z | --radenc ra dec }" << std::endl
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
            << "  " << " -a, --area                                      : print area of node (default: off)"                  << std::endl
            << "  " << " -c, --corner                                    : print node corners (default: off)"                  << std::endl
			<< "  " << " -r, --resolution=RESOLUTION                     : Resolution of index to return (Default: 23)"        << std::endl
			<< "  " << " -l, --latlon lat lon                            : specify latitude longitude of point to look up"     << std::endl
			<< "  " << " -m, --xyz x y z                                 : specify cartesian coordinate of point to look up"   << std::endl
			<< "  " << " -n, --radenc ra dec                             : specify J2000 coordinate of point to look up"       << std::endl
			<< std::endl;
	exit(0);
};


struct Arguments {
    bool verbose = false;
    bool quiet = false;    
    bool hex= true;
    bool integer = true;    
    bool symbol = false;    
    bool area = false;
    bool corner = false;                    
    int resolution = 23;    
    bool latlon = false;
    bool xyz = false;
    bool radenc = false;    
    double lat, lon;
    double ra, dec;
    double x, y, z;    
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
        {"area",        no_argument,       0,  'a' },
        {"corner",      no_argument,       0,  'c' },
		{"resolution",  required_argument, 0,  'r' },
        {"latlon",      no_argument,       0,  'l' },
        {"xyz",         no_argument,       0,  'm' },
        {"radenc",      no_argument,       0,  'n' },
        {0,             0,                 0,  0   }
    };
    
    int long_index = 0;
    int opt = 0;
    while ((opt = getopt_long(argc, argv, "hvqxdsacr:lmn", long_options, &long_index)) != -1) {
        switch (opt) {
            case 'h': usage(argv[0]);
            case 'v': arguments.verbose = true; break;
            case 'q': arguments.quiet = true; break;
            case 'x': arguments.hex = true; break;
            case 'i': arguments.integer = true; break;
            case 's': arguments.symbol = true; break;
            case 'a': arguments.area = true; break;
            case 'c': arguments.corner = true; break;
            case 'r': arguments.resolution = atoi(optarg); break;                
            case 'l': arguments.latlon=true; break; 
            case 'm': arguments.xyz= true; break;
            case 'n': arguments.radenc=true; break;                                                   
        }
    }   
    if (arguments.verbose) {
        arguments.hex = true;
        arguments.integer = true;
        arguments.symbol= true;
        arguments.area = true;
        arguments.corner = true;
    } else if (arguments.quiet) {
        arguments.hex = false;
        arguments.integer = true;
        arguments.symbol= false;
        arguments.area = false;
        arguments.corner = false;
    }    
    
    if (arguments.latlon && argc-optind == 2) {
        arguments.lat = atof(argv[optind]);
        arguments.lon = atof(argv[optind+1]);
    } else if (arguments.xyz && argc-optind == 3) {
        arguments.x = atof(argv[optind]);
        arguments.y = atof(argv[optind + 1]);
        arguments.z = atof(argv[optind + 2]);
    } else if (arguments.radenc && argc-optind == 2) {
        arguments.ra = atof(argv[optind]);
        arguments.dec = atof(argv[optind + 1]);        
    } else usage(argv[0]);    
        
    return arguments;
}

STARE stare;

int main(int argc, char *argv[]) {
    Arguments arguments = parseArguments(argc, argv);        
    STARE_ArrayIndexSpatialValue indexValue;
    
    // Getting index Value
    if (arguments.latlon) {
         indexValue = stare.ValueFromLatLonDegrees(arguments.lat, arguments.lon, arguments.resolution);
    } else if (arguments.xyz) {    
    } else if (arguments.radenc) {}
    
    // Producing outputs
    if (arguments.integer) {
        if (!arguments.quiet) std::cout << "Integer Value of STARE index: "<< std::endl;
        std::cout << " " << indexValue << std::endl;
    }
    if (arguments.hex) {};
    if (arguments.symbol) {};
    if (arguments.area) {
        float64 area = stare.AreaFromValue(indexValue, arguments.resolution);
        std::cout << "Area of Trixel: "<< std::endl;
        std::cout << " " << area << std::endl;
    }
    if (arguments.corner) {
        Triangle triangle;
        triangle = stare.TriangleFromValue(indexValue, arguments.resolution);
        std::cout << "Trixel Corners: "<< std::endl;
        std::cout << " " << triangle.vertices[0] << std::endl;
        std::cout << " " << triangle.vertices[1] << std::endl;
        std::cout << " " << triangle.vertices[2] << std::endl;
    }
    
    
    
}
