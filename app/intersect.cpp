//#     Filename:       intersect.cpp
//#
//#     Intersect a domain with the index, returning the full and partial nodes
//#
//#
//#     Author:         Peter Z. Kunszt, Gyorgy Fekete
//#
//#     Date:           October 15, 1999
//#                     November 11, 2002 Gyorgy Fekete
//#                     Fixed  a few bugs.
//#
//#
//# This program is free software; you can redistribute it and/or
//# modify it under the terms of the GNU General Public License
//# as published by the Free Software Foundation; either version 2
//# of the License, or (at your option) any later version.
//#
//# This program is distributed in the hope that it will be useful,
//# but WITHOUT ANY WARRANTY; without even the implied warranty of
//# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//# GNU General Public License for more details.
//#
//# You should have received a copy of the GNU General Public License
//# along with this program; if not, write to the Free Software
//# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//#
//#

#include <iostream>

#define _INC_IOSTREAM


#include <SpatialGeneral.h>
#include <SpatialIndex.h>
#include <SpatialConstraint.h>
#include <RangeConvex.h>
#include <SpatialDomain.h>
#include <sqlInterface.h>
#include <HtmRange.h>
#include <HtmRangeIterator.h>


#include <fstream>
#include <time.h>


bool visualize = false;
bool verbose = false;		// verbosity flag
bool compress = false;         // whether or not to compress ranges;
bool varlength = false;       // output variable length HTMids
bool expand = false;       // output all hids (could be long list!)
bool symbolic = false;

int arg = 2;			// number of required arguments

char *infile;			// infile name
int searchlevel,buildlevel=5;	// searchlevel and buildlevel
int maximum_nrGaps=100;
int olevel = -1;

char htmidfilename[1024];
char domainfilename[1024];

size_t i;

void
usage(char *name) {

	cerr << "usage: " << endl
			<< name << " [-build buildlevel] [-verbose] [-olevel olevel] level domainfile" << endl;
	cerr << " [-build buildlevel]   : store up to this level (default 2)" << endl
			<< " [-verbose]          : verbose" << endl
			<< " [-olevel out_level] : output HTMID level (must be >= level)" << endl
			<< " [-symbolic]         : output HTMID as a symbolic name, must be used with -varlength" << endl
			<< " [-expand]           : output all HTMIDs in the range (conflicts with -varlength)" << endl
			<< " [-varlength]        : output natural HTMID (conflicts with -olevel)" << endl
			<< " [-nranges nrages]   : keep number of ranges below this number" << endl
			//       << " [-visualize domainfile hidfile] : creates two files to be used with htmviz openGL application" << endl
 			<< " searchlevel         : Maximal spatialindex level " << endl
			<< " domainfile          : filename of domain " << endl
			<< endl;
	exit(1);
}

int init_all(int argc, char *argv[])
{

	int args = 1;			// start to parse argument no 1
	argc--;

	while(argc > 0) {
		if(strcmp(argv[args],"-verbose")==0) {
			verbose = true;
		} else if(strcmp(argv[args],"-visualize")==0) {
			visualize = true;
			if (argc > 2){
				strcpy(domainfilename, argv[++args]); argc--;
				strcpy(htmidfilename, argv[++args]); argc--;
			} else {
				usage(argv[0]);
			}
		} else if (strcmp(argv[args],"-expand")==0) {
			expand = true;
		} else if (strcmp(argv[args],"-symbolic")==0) {
			symbolic = true;
		} else if (strcmp(argv[args],"-varlength")==0) {
			varlength = true;
		} else if(strcmp(argv[args],"-build")==0) {
			buildlevel = atoi(argv[++args]); argc--;
			if(buildlevel < 1 || buildlevel > 8) {
				cerr << "buildlevel should be between 1 and 8, is " << buildlevel << endl;
			//	return -1;
			}
		} else if(strcmp(argv[args],"-olevel")==0) {
			olevel = atoi(argv[++args]); argc--;
		} else if(strcmp(argv[args],"-nranges")==0) {
			maximum_nrGaps = atoi(argv[++args]); argc--;
			compress = true;
		} else {
			if( *argv[args] == '-' ) usage(argv[0]);
			switch(arg) { // How many "bare" arguments are there?  2 are expected.
			case 2:
				searchlevel = atoi(argv[args]); // MLR searchlevel is really level, no?
				if(searchlevel < 1 || searchlevel > HTMMAXDEPTH) {
					cerr << "searchlevel should be between 1 and " << HTMMAXDEPTH << ", is " << searchlevel << endl;
					return -1;
				}
				break;
			case 1:
				infile = argv[args];
				break;
			default:
				usage(argv[0]); // If we have more bare arguments than expected.
			}
			arg--; // Got one more bare argument.
		}
		argc--;
		args++;
	} // while (argc > 0)

	if (olevel < searchlevel)
		olevel = searchlevel;

	//   if (symbolic && !varlength)
	//     usage(argv[0]);

	cerr << "searchlevel = " << searchlevel << ", output searchlevel = " << olevel << endl;
	return 0;
}
// intopends60.lib 


int main(int argc, char *argv[]) {

/*******************************************************
/
/ Initialization
/
******************************************************/
	Key gapsize;
	int rstat = init_all(argc, argv);
	if (rstat != 0){
		return rstat;
	}

	if(arg > 0)
		usage(argv[0]);

	ifstream in(infile);
	if(!in) {
		cout << "cannot open file " << infile << endl;
		return -1;
	}

	try
	{
        
       
    
		htmInterface htm(searchlevel,buildlevel);  
		const SpatialIndex &index = htm.index();
        
		HtmRange htmRange;
        HtmRange htmRangeInterior;
        HtmRange htmBoundary;

		
/*******************************************************
/
/ Create Domain
/
******************************************************/
  
  SpatialDomain domain(&index);   
    
  in >> domain;	      // read domainfile
  // //////////////////////////////////////////////
  // TODO What is the difference between olevel and searchlevel.
  // cf. above olevel < searchlevel => olevel = searchlevel
  // cf. what does it mean for olevel to be greater than searchlevel?
  // olevel is the output level -- but how does this differ from searchlevel in the index?
  // domain.setOlevel(olevel); TODO huh? Can olevel differ from the index searchlevel?
  if(verbose) {
    cout << domain;
  }

  if (visualize){
    // A file for hids, and a file for the domain
    // Write domain to a file
    ofstream outfile(domainfilename, ios::out);
      if (outfile){
        outfile << domain;
        outfile.close();
      }
  }

/*******************************************************
/
/ Intersection
/
******************************************************/
  // do intersection and time it
  time_t t0 = clock();		

  // cerr << "Normal intersect starts here" << endl;
  htmRange.purge();
  htmRangeInterior.purge();
  htmBoundary.purge();		
  domain.intersect(&index, &htmRange, varlength, &htmRangeInterior, &htmBoundary);	 

  time_t t1 = clock();
  if(verbose) {
    printf("Intersections done at a rate of %f intersections/sec\n",
          ((float)1)/(((double)(t1-t0)/(double)CLOCKS_PER_SEC)));
		}
        
/*******************************************************
/
/ Print result
/
******************************************************/
		if (varlength){
			// only print lows (should be same as highs)
			htmRange.print(HtmRange::LOWS, cout, symbolic);
			if (visualize) { 	// print hids to file too
				ofstream outfile(htmidfilename, ios::out);
				if (outfile){
					htmRange.print(HtmRange::LOWS, outfile, symbolic);
					outfile.close();
				}
			}
		} else {
			htmRange.defrag();
			if (compress){
				// find best gap, and merge (defrag) ranges
				gapsize = htmRange.bestgap(maximum_nrGaps);
				htmRange.defrag(gapsize);
				cerr << "Combining ranges with gaps <= " << gapsize;
				cerr << " with no more than " << maximum_nrGaps << endl;
			}
			if (expand){
				HtmRangeIterator iter(&htmRange);
				if (!symbolic){
					while (iter.hasNext()) {                           
						cout << iter.next() << endl;
                     
					}
				} else {
					char buffer[80];
					while (iter.hasNext()) {                        
						cout << iter.nextSymbolic(buffer) << endl;
					}
				}
			} else {
				htmRange.setSymbolic(symbolic);
				cout << htmRange;
			}
		}
	}
	catch (SpatialException &x)
	{
		printf("%s\n",x.what());
	}
	cout << "" << endl;
	return 0;
    
}
