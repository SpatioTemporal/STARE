#include "STARE.h"
#include "VarStr.h"
#include <getopt.h>


void usage(char *name) {
            std::cout 
            << "STARE temporal index lookup" << std::endl
            << "Usage: " << name << " [options] {--timestamp ±YYYYYYYYY-MM-DDThh:mm:ss.sss | year [month [day [hour [second [millisecond]]]]]}" << std::endl
			<< "Examples:" << std::endl			
			<< "  " << name << " --timezone UTC --timestamp=2018-09-03T20:13:01.312" << std::endl
			<< "  " << name << " --timezone TOI 2018 9 3 20 1 312" << std::endl
			<< std::endl
			<< "Mandatory arguments to long options are mandatory for short options too." << std::endl
			<< "Options:" << std::endl			
			<< "  " << " -h, --help                                      : print this help" << std::endl			
			<< "  " << " -v, --verbose                                   : verbose: print all" << std::endl			
			<< "  " << " -q, --quiet                                     : don't chat, just give back index" << std::endl			
			<< "  " << " -z, --timezone=UTC|TOI                          : Timezone to UTC or TOI. (Default: UTC)" << std::endl		
			<< "  " << " -r, --resolution=RESOLUTION                     : Resolution of index to return (Default: 23)" << std::endl
			<< "  " << " -t, --timestamp=YYYYYYYYY-MM-DDThh:mm:ss.sss    : Datetime passed as extended ISO 8601 date and time representation (signed 9 digits year)" << std::endl
			<< "  " << " year month date hour minute second millisecond  : Time representation" << std::endl
			<< std::endl;
	exit(0);
};


struct Arguments {
    bool verbose = true;
    bool quiet = false;    
    bool utc = true;
    bool toi = false;    
    string timestamp;
    int resolution = 23;    
    int datetime[7] = {0, 1, 1, 0, 0, 0, 0}; 
};


Arguments parseArguments(int argc, char *argv[]) {
    if (argc == 1)  usage(argv[0]);
    Arguments arguments;        
    static struct option long_options[] = {
        {"help",        no_argument,       0,  'h' },
        {"verbose",     no_argument,       0,  'v' },
        {"quiet",       no_argument,       0,  'q' },
        {"timezone",    required_argument, 0,  'z' },
        {"resolution",  required_argument, 0,  'r' },
        {"timestamp",   required_argument, 0,  't' },
        {0,             0,                 0,  0   }
    };
    int long_index = 0;
    int opt = 0;
    while ((opt = getopt_long(argc, argv, "hvqz:r:t:", long_options, &long_index)) != -1) {
        switch (opt) {
            case 'h': usage(argv[0]);
            case 'v': arguments.verbose = true; break;
            case 'q': arguments.quiet = true; break;
            case 'z': 
                if (strcmp(optarg, "UTC")==0) {
                    arguments.utc = true; 
                    arguments.toi = false;                    
                }
                else if (strcmp(optarg, "TOI")==0) { 
                    arguments.utc = false; 
                    arguments.toi = true;
                }                
            case 'r': {
                arguments.resolution = atoi(optarg); break;                
            }
            case 't': arguments.timestamp = optarg; break;
            
        }
    }   
    if (arguments.timestamp.length() == 0) {  
        int has_datetime_args = false;
        for(int n_datetime_args= 0; optind+n_datetime_args < argc; n_datetime_args++){   
            arguments.datetime[n_datetime_args] = atoi(argv[optind+n_datetime_args]);         
            has_datetime_args = true;        
        }     
        if (!has_datetime_args) usage(argv[0]);
    }
    return arguments;
}

    

STARE stare;

STARE_ArrayIndexTemporalValue makeIndexValue(string timestamp, int resolution) {        
    struct tm tm;    
    int type = 2;
    strptime(timestamp.data(), "%Y-%m-%dT%H:%M:%S", &tm);
    return stare.ValueFromUTC_tm(tm, resolution, type);
}

STARE_ArrayIndexTemporalValue makeIndexValue(int dt[7], int resolution) {
    int type = 2;
    return stare.ValueFromUTC(dt[0], dt[1], dt[2], dt[3], dt[4], dt[5], dt[6], resolution, type);            
}


int main(int argc, char *argv[]) {     
    Arguments arguments = parseArguments(argc, argv);        
    STARE_ArrayIndexTemporalValue indexValue;
    if (arguments.timestamp.length() > 0) {        
        indexValue = makeIndexValue(arguments.timestamp, arguments.resolution);        
    } else { 
        indexValue = makeIndexValue(arguments.datetime, arguments.resolution);   
    }
    std::cout << " " << indexValue << std::endl;    
}

