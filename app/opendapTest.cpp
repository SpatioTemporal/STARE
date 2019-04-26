#include "SpatialVector.h"
#include "SpatialIndex.h"
#include "SpatialInterface.h"
#include "RangeConvex.h"
#include "HtmRange.h"
#include "HtmRangeIterator.h"
#include "VarStr.h"

#include <D4Connect.h>
#include <Connect.h>
#include <Response.h>
#include <Array.h>


void findIndex(std::string dataUrl) {
	uint32 lat;
	uint32 lon;

	libdap::Connect *url = 0;
	url = new libdap::Connect(dataUrl);
	libdap::BaseTypeFactory factory;
	libdap::DataDDS dds(&factory);
	libdap::DAS das;

	url->request_data(dds, "");
	url->request_das(das);		//May not be necessary, used for attributes

	for (libdap::DDS::Vars_iter i = dds.var_begin(); i != dds.var_end(); i++) {

	}
}

int main(int argc, char *argv[]) {
    int args = 1;
    bool quiet = false;
    size_t level;
    size_t savelevel=10;

    while(args < argc) {
        if  (strcmp(argv[args], "--quiet")==0) quiet = true;
        else  {
            level = atoi(argv[args]);
            break;
        }
        args++;
    }

    SpatialVector* cnrs[4];
    float64 lat;
    float64 lon;
    args++;
    if (args<argc-4){
        // Get corners from arguments
        for (int c = 0 ; c < 4 ; c++ ) {
            lat = atof(argv[args]);
            lon = atof(argv[args+1]);
            cnrs[c] = new SpatialVector();
            cnrs[c]->setLatLonDegrees(lat, lon);
            args += 2;
        }
    }
    else {
        // Get the corners from stdin
        for (int c = 0 ; c < 4 ; c++ ) {
            if (!quiet) cout << "enter corner " << c << endl;
            cin >> lat;
            cin >> lon;
            cnrs[c] = new SpatialVector();
            cnrs[c]->setLatLonDegrees(lat, lon);
        }
    }


    SpatialIndex* si;
    RangeConvex* conv;
    HtmRange htmRange;
    uint64 intVal = 0;

    // create htm interface
    htmInterface htm(level,savelevel);

    // Create spatial index at according level
    //si = new SpatialIndex(level,savelevel);
    const SpatialIndex &index = htm.index();

    // create spatial domain
    SpatialDomain domain(&index);   // initialize empty domain

    // Create convex to represent the geometry
    conv = new RangeConvex(cnrs[0],cnrs[1],cnrs[2],cnrs[3]);

    // Add convex to domain
    domain.add(*conv);

    // Purge HTMRange
    htmRange.purge();

    // Do the intersect. The boolean is for varlength ids or not
    //conv->intersect(si,htmRange,false);
    domain.intersect(&index, &htmRange, false);	  // intersect with range


    // Iterate through the indeces
    HtmRangeIterator iter(&htmRange);
    char buffer[80];
    while (iter.hasNext()) {
        //intVal = iter.nextSymbolicInt(buffer);
    		iter.nextSymbolic(buffer);
        intVal = htmRange.encoding->idByName(buffer);
        cout << "int64: " << intVal << endl;
    }
    //htmRange.print(HtmRange::LOWS, cout, true);
    cout << "resolution level: " << index.getMaxlevel() << endl;
    cout << "done" << endl;

}

